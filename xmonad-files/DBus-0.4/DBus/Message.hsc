{-# LANGUAGE PatternSignatures, TypeSynonymInstances, FlexibleInstances #-}
{-# OPTIONS -fglasgow-exts #-}
-- HDBus -- Haskell bindings for D-Bus.
-- Copyright (C) 2006 Evan Martin <martine@danga.com>

#define DBUS_API_SUBJECT_TO_CHANGE
#include "dbus/dbus.h"

module DBus.Message (
  -- * Messages
  Message,
  newSignal, newMethodCall,
  -- * Accessors
  Type(..),
  getType, getSignature,
  getPath, getInterface, getMember, getErrorName,
  getDestination, getSender,

  -- * Arguments
  Arg(..), args, addArgs,
  signature, stringSig, variantSig
{-
  Arg(..),
  args, addArgs,
  -- ** Dictionaries
  -- | D-Bus functions that expect a dictionary must be passed a 'Dict',
  -- which is trivially constructable from an appropriate list.
  Dict, dict,
  -- ** Variants
  -- | Some D-Bus functions allow variants, which are similar to
  -- 'Data.Dynamic' dynamics but restricted to D-Bus data types.
  Variant, variant
-}
) where

import Control.Monad (when)
import Data.Int
import Data.Word
import Data.Char
import Data.Dynamic
import Foreign
import Foreign.C.String
import Foreign.C.Types (CInt)
import System.IO.Unsafe

import DBus.Internal
import DBus.Shared

import qualified Data.ByteString as B
import qualified Data.ByteString.Internal as B

type Message = ForeignPtr MessageTag

foreign import ccall unsafe "dbus_message_new_signal"
  message_new_signal :: CString -> CString -> CString -> IO MessageP
foreign import ccall unsafe "dbus_message_new_method_call"
  message_new_method_call :: CString -> CString -> CString -> CString -> IO MessageP

newSignal :: PathName      -- ^Path.
          -> InterfaceName -- ^Interface.
          -> String        -- ^Method name.
          -> IO Message
newSignal path iface name =
  withCString path $ \cpath ->
    withCString iface $ \ciface ->
      withCString name $ \cname -> do
        msg <- message_new_signal cpath ciface cname
        messagePToMessage msg False

newMethodCall :: ServiceName   -- ^Bus name.
              -> PathName      -- ^Path.
              -> InterfaceName -- ^Interface.
              -> String        -- ^Method name.
              -> IO Message
newMethodCall busname path iface method =
  withCString busname $ \cbusname ->
    withCString path $ \cpath ->
      withCString iface $ \ciface ->
        withCString method $ \cmethod -> do
          msg <- message_new_method_call cbusname cpath ciface cmethod
          messagePToMessage msg False

data Type = MethodCall | MethodReturn | Error | Signal
          | Other Int deriving Show
instance Enum Type where
  toEnum #{const DBUS_MESSAGE_TYPE_METHOD_CALL}   = MethodCall
  toEnum #{const DBUS_MESSAGE_TYPE_METHOD_RETURN} = MethodReturn
  toEnum #{const DBUS_MESSAGE_TYPE_ERROR}         = Error
  toEnum #{const DBUS_MESSAGE_TYPE_SIGNAL}        = Signal
  toEnum x = Other x
  fromEnum = error "not implemented"
foreign import ccall unsafe "dbus_message_get_type"
  message_get_type :: MessageP -> IO Int
getType :: Message -> IO Type
getType msg = withForeignPtr msg message_get_type >>= return . toEnum

getOptionalString :: (MessageP -> IO CString) -> Message -> IO (Maybe String)
getOptionalString getter msg =
  withForeignPtr msg getter >>= maybePeek peekCString

foreign import ccall unsafe dbus_message_get_path :: MessageP -> IO CString
foreign import ccall unsafe dbus_message_get_interface :: MessageP -> IO CString
foreign import ccall unsafe dbus_message_get_member :: MessageP -> IO CString
foreign import ccall unsafe dbus_message_get_error_name :: MessageP -> IO CString
foreign import ccall unsafe dbus_message_get_sender :: MessageP -> IO CString
foreign import ccall unsafe dbus_message_get_signature :: MessageP -> IO CString
foreign import ccall unsafe dbus_message_get_destination :: MessageP -> IO CString
getPath, getInterface, getMember, getErrorName, getDestination, getSender
  :: Message -> IO (Maybe String)
getPath        = getOptionalString dbus_message_get_path
getInterface   = getOptionalString dbus_message_get_interface
getMember      = getOptionalString dbus_message_get_member
getErrorName   = getOptionalString dbus_message_get_error_name
getDestination = getOptionalString dbus_message_get_destination
getSender      = getOptionalString dbus_message_get_sender
getSignature :: Message -> IO String
getSignature msg =
  withForeignPtr msg dbus_message_get_signature >>= peekCString

type IterTag = ()
type Iter = Ptr IterTag


data Arg = Byte Word8 | Boolean Bool | Int16 Int16 | Word16 Word16 | Int32 Int32 | Word32 Word32
         | Int64 Int64 | Word64 Word64 | Double Double | String String | ObjectPath {- ? -}
         | TypeSignature {- ? -} | Array String [Arg] | Variant Arg | Struct [Arg] | DictEntry Arg Arg
         | ByteString B.ByteString | Invalid
           deriving (Show,Read,Typeable)

stringSig :: String
stringSig = signature String{}

variantSig = signature Variant{}

signature Byte{} = #{const_str DBUS_TYPE_BYTE_AS_STRING}
signature Boolean{} = #{const_str DBUS_TYPE_BOOLEAN_AS_STRING}
signature Int32{} = #{const_str DBUS_TYPE_INT32_AS_STRING}
signature (Array sig _) = #{const_str DBUS_TYPE_ARRAY_AS_STRING} ++ sig
signature ByteString{} = signature (Array (signature (Byte 0)) [])
signature (DictEntry argA argB) = "{"++signature argA ++ signature argB ++ "}"
signature Variant{} = #{const_str DBUS_TYPE_VARIANT_AS_STRING}
signature String{} = #{const_str DBUS_TYPE_STRING_AS_STRING}
signature (Struct args) = "{" ++ concatMap signature args ++ "}"
signature arg = error $ "DBus.Message.signature: " ++ show arg

decodeArgs iter
    = let loop = unsafeInterleaveIO $
                 do arg <- decodeArg iter
                    has_next <- message_iter_next iter
                    if has_next then fmap (arg:) loop
                                else return [arg]
      in loop

decodeArg iter
    = do arg_type <- message_iter_get_arg_type iter
         case arg_type of
           #{const DBUS_TYPE_INT32}  -> fmap Int32 getBasic
           #{const DBUS_TYPE_UINT32} -> fmap Word32 getBasic
           #{const DBUS_TYPE_STRING} -> fmap String (getBasic >>= peekCString)
           #{const DBUS_TYPE_ARRAY}  -> decodeArray iter
           #{const DBUS_TYPE_BOOLEAN}-> fmap Boolean getBasic
           #{const DBUS_TYPE_INVALID}-> return Invalid
           #{const DBUS_TYPE_STRUCT} -> decodeStruct iter
           _ -> error $ "Unknown argument type: " ++ show arg_type ++ " ("++show (chr arg_type)++")"
    where getBasic :: Storable a => IO a
          getBasic = alloca $ \ptr -> do
                       message_iter_get_basic iter ptr
                       peek ptr

decodeArray iter
    = withIter $ \sub ->
      do message_iter_recurse iter sub
         len <- message_iter_get_array_len iter
         elt_type <- message_iter_get_element_type iter
         case elt_type of
           #{const DBUS_TYPE_BYTE} -> if len > 0 then decodeByteArray iter
                                                 else return $ ByteString B.empty
           _other -> withIter $ \sub ->
                     do message_iter_recurse iter sub
                        sig <- getIterSignature sub
                        lst <- if len > 0 then getArray sub else return []
                        return $ Array sig lst
    where getArray sub = do x <- decodeArg sub
                            has_next <- message_iter_next sub
                            if has_next
                              then do xs <- getArray sub
                                      return (x:xs)
                              else return [x]

decodeByteArray iter
    = alloca $ \elts_ptr ->
      alloca $ \n_ptr ->
      do message_iter_get_fixed_array iter elts_ptr n_ptr
         byte_ptr <- peek elts_ptr
         len <- peek n_ptr
         fptr <- newForeignPtr_ byte_ptr
         return $ ByteString $ B.fromForeignPtr fptr 0 (fromIntegral len)

decodeStruct iter
    = withIter $ \sub ->
      do message_iter_recurse iter sub
         let fetch = do arg <- decodeArg sub
                        has_next <- message_iter_next sub
                        if has_next then fmap (arg:) fetch
                                    else return [arg]
         fmap Struct fetch

encodeArg iter arg
    = case arg of
        Int32 i32  -> putSimple #{const DBUS_TYPE_INT32} i32
        Word32 u32 -> putSimple #{const DBUS_TYPE_UINT32} u32
        String str -> withCString str $ \cstr -> putSimple #{const DBUS_TYPE_STRING} cstr
        Array sig lst -> encodeArray iter sig lst
        DictEntry argA argB -> encodeDict iter argA argB
        _ -> error $ "Can't encode argument: " ++ show arg
    where putSimple ty val = with val $ putBasic iter ty

encodeArray iter sig lst
    = withCString sig $ \csig ->
        withContainer iter #{const DBUS_TYPE_ARRAY} csig $ \sub ->
          mapM_ (encodeArg sub) lst

encodeDict iter argA argB
    = withContainer iter #{const DBUS_TYPE_DICT_ENTRY} nullPtr $ \sub ->
      do message_iter_recurse iter sub
         encodeArg sub argA
         encodeArg sub argB




{-
class Show a => Arg a where
  toIter :: a -> Iter -> IO ()
  fromIter :: Iter -> IO a
  signature :: a -> String

  -- for collection types, this does from/to inside the collection's iter
  toIterInternal   :: a -> Iter -> IO ()
  toIterInternal   = toIter
  fromIterInternal :: Iter -> IO a
  fromIterInternal = fromIter

assertArgType iter expected_type = do
  arg_type <- message_iter_get_arg_type iter
  when (arg_type /= expected_type) $
    fail $ "Expected arg type " ++ show expected_type ++
           " but got " ++ show arg_type
-}
putBasic iter typ val = catchOom (message_iter_append_basic iter typ val)

withContainer iter typ sig f =
  withIter $ \sub -> do
    catchOom $ (message_iter_open_container iter typ sig sub)
    f sub
    catchOom $ (message_iter_close_container iter sub)
{-
instance Arg () where
  toIter _ _ = return ()
  fromIter _ = return ()
  signature _ = "()" -- not really correct, but we should never need this.

instance Arg String where
  fromIter iter = do
    assertArgType iter #{const DBUS_TYPE_STRING}
    alloca $ \str -> do
      message_iter_get_basic iter str
      peek str >>= peekCString
  toIter arg iter =
    -- we need a pointer to a CString (which itself is Ptr CChar)
    withCString arg $ \cstr ->
      with cstr $ putBasic iter #{const DBUS_TYPE_STRING}
  signature _ = "s"

instance Arg Int32 where
  fromIter iter = do
    assertArgType iter #{const DBUS_TYPE_INT32}
    alloca $ \int -> do
      message_iter_get_basic iter int
      peek int
  toIter arg iter = with arg $ putBasic iter #{const DBUS_TYPE_INT32}
  signature _ = "i"
instance Arg Word32 where
  fromIter iter = do
    assertArgType iter #{const DBUS_TYPE_UINT32}
    alloca $ \int -> do
      message_iter_get_basic iter int
      peek int
  toIter arg iter = with arg $ putBasic iter #{const DBUS_TYPE_UINT32}
  signature _ = "u"

data Variant = forall a. Arg a => Variant a
variant :: Arg a => a -> Variant
variant = Variant
instance Show Variant where
  show (Variant a) = "[variant]" ++ show a
instance Arg Variant where
  fromIter iter = do
    assertArgType iter #{const DBUS_TYPE_VARIANT}
    elem_type <- message_iter_get_element_type iter
    withIter $ \sub -> do
      message_iter_recurse iter sub
      variantFromIterType sub elem_type where
    -- XXX there ought to be a more clever way to do this.
    variantFromIterType iter #{const DBUS_TYPE_STRING} = do
      (v :: String) <- fromIter iter; return $ Variant v
    variantFromIterType iter #{const DBUS_TYPE_INT32} = do
      (v :: Int32)  <- fromIter iter; return $ Variant v
    variantFromIterType iter #{const DBUS_TYPE_UINT32} = do
      (v :: Word32) <- fromIter iter; return $ Variant v
  toIter (Variant arg) iter = do
    withIter $ \sub ->
      withCString (signature arg) $ \sig ->
        withContainer iter #{const DBUS_TYPE_VARIANT} sig $ \sub ->
          toIter arg sub where
  signature _ = "v"

instance Arg a => Arg [a] where
  fromIter iter = do
    assertArgType iter #{const DBUS_TYPE_ARRAY}
    len <- message_iter_get_array_len iter
    if len > 0
      then withIter $ \sub -> do
             message_iter_recurse iter sub
             array <- getArray sub
             return array
      else return [] where
    getArray sub = do x <- fromIter sub
                      has_next <- message_iter_next sub
                      if has_next
                        then do xs <- getArray sub
                                return (x:xs)
                        else return [x]
  toIter arg iter =
    withIter $ \sub ->
      withCString (signature (undefined :: a)) $ \sig ->
        withContainer iter #{const DBUS_TYPE_ARRAY} sig $ \sub ->
          mapM_ (\v -> toIter v sub) arg
  signature a = "a" ++ signature (undefined :: a)

--instance (Show a,Show b,Show c,Show d,Show e,Show f,Show g,Show h) => Show (a,b,c,d,e,f,g,h) where
--  show _ = "[show not implemented]"

newtype DictEntry a b = DictEntry (a,b)
instance (Show a, Show b) => Show (DictEntry a b) where
  show (DictEntry pair) = "[DictEntry] " ++ show pair
instance (Arg a, Arg b) => Arg (DictEntry a b) where
  toIter (DictEntry pair) iter =
    withContainer iter #{const DBUS_TYPE_DICT_ENTRY} nullPtr $ \sub -> do
      toIterInternal pair sub
  fromIter iter = do
    pair <- fromIter iter
    return (DictEntry pair)
  signature _ =
    "{" ++ signature (undefined :: a) ++ signature (undefined :: b) ++ "}"

type Dict a b = [DictEntry a b]
dict :: (Arg a, Arg b) => [(a, b)] -> Dict a b
dict = map DictEntry

-- tuple instances generated by codegen/TupleInstances.hs
instance (Arg a,Arg b) => Arg (a,b) where
  toIter arg iter =
    withContainer iter #{const DBUS_TYPE_STRUCT} nullPtr $ \sub -> do
    toIterInternal arg sub
  fromIter iter =
    withIter $ \sub -> do
      message_iter_recurse iter sub
      fromIterInternal sub
  signature _ =
    "{" ++ signature (undefined :: a) ++ signature (undefined :: b) ++ "}"
  toIterInternal (a,b) iter = do
    toIter a iter; toIter b iter
    return ()
  fromIterInternal iter = do
    a <- fromIter iter; next <- message_iter_next iter
    b <- fromIter iter; next <- message_iter_next iter
    return (a,b)

instance (Arg a,Arg b,Arg c,Arg d,Arg e,Arg f,Arg g,Arg h) => Arg (a,b,c,d,e,f,g,h) where
  toIter arg iter =
    withContainer iter #{const DBUS_TYPE_STRUCT} nullPtr $ \sub -> do
    toIterInternal arg sub
  fromIter iter =
    withIter $ \sub -> do
      message_iter_recurse iter sub
      fromIterInternal sub
  signature _ =
    "{" ++ signature (undefined :: a) ++ signature (undefined :: b) ++ signature (undefined :: c) ++ signature (undefined :: d) ++ signature (undefined :: e) ++ signature (undefined :: f) ++ signature (undefined :: g) ++ signature (undefined :: h) ++ "}"
  toIterInternal (a,b,c,d,e,f,g,h) iter = do
    toIter a iter; toIter b iter; toIter c iter; toIter d iter; toIter e iter; toIter f iter; toIter g iter; toIter h iter
    return ()
  fromIterInternal iter = do
    a <- fromIter iter; next <- message_iter_next iter
    b <- fromIter iter; next <- message_iter_next iter
    c <- fromIter iter; next <- message_iter_next iter
    d <- fromIter iter; next <- message_iter_next iter
    e <- fromIter iter; next <- message_iter_next iter
    f <- fromIter iter; next <- message_iter_next iter
    g <- fromIter iter; next <- message_iter_next iter
    h <- fromIter iter; next <- message_iter_next iter
    return (a,b,c,d,e,f,g,h)

dynTag :: Dynamic -> Int32
dynTag x =
  case fromDynamic x of
    Just (i :: Int32) -> #{const DBUS_TYPE_INT32}
    Nothing ->
      case fromDynamic x of
        Just (word :: Word32) -> #{const DBUS_TYPE_UINT32}
        Nothing ->
          case fromDynamic x of
            Just (s :: String) -> #{const DBUS_TYPE_STRING}
            Nothing ->
              case fromDynamic x of
                Just (arr :: [Dynamic]) -> #{const DBUS_TYPE_ARRAY}
                Nothing ->
                  case fromDynamic x of
                    Just (a :: [Dynamic], b :: [Dynamic]) -> #{const DBUS_TYPE_DICT_ENTRY}
                    Nothing ->
                      case fromDynamic x of
                        Just (b :: Bool) -> #{const DBUS_TYPE_BOOLEAN}
                        Nothing -> #{const DBUS_TYPE_INVALID}

instance Arg [Dynamic] where
  fromIter iter =
    let loop list False = return (reverse list)
        loop list True = do
          argt <- message_iter_get_arg_type iter
          let next x = do {
            valid <- message_iter_next iter;
            loop (x:list) valid }
          -- putStr $ "argt is " ++ show argt ++ "\n"
          case argt of
            #{const DBUS_TYPE_INVALID} -> return (reverse list)
            #{const DBUS_TYPE_UINT32} -> do
              (word :: Word32) <- fromIter iter
              next (toDyn word)
            #{const DBUS_TYPE_INT32} -> do
              (i :: Int32) <- fromIter iter
              next (toDyn i)
            #{const DBUS_TYPE_STRING} -> do
              (s :: String) <- fromIter iter
              next (toDyn s)
            #{const DBUS_TYPE_ARRAY} -> do
              (arr :: [[Dynamic]]) <- fromIter iter -- this could be better
              next (toDyn arr)
            #{const DBUS_TYPE_DICT_ENTRY} -> do
              (DictEntry (a :: [Dynamic], b :: [Dynamic])) <- fromIter iter
              next (toDyn (a,b))
            #{const DBUS_TYPE_STRUCT} -> do
              withIter $ \sub -> do
                message_iter_recurse iter sub
                (inner :: [Dynamic]) <- fromIter sub
                next (toDyn inner)
            #{const DBUS_TYPE_BOOLEAN} -> do
              (b :: Bool) <- alloca $ \int -> do {
                               message_iter_get_basic iter int;
                               peek int }
              next (toDyn b)
            #{const DBUS_TYPE_VARIANT} -> do
              withIter $ \sub -> do
                message_iter_recurse iter sub
                (inner :: [Dynamic]) <- fromIter sub
                next (toDyn inner)
    in loop [] True
  toIter list iter =
    let loop [] iter = return ()
        loop (x:xs) iter = do
          case dynTag x of
            #{const DBUS_TYPE_INVALID} -> fail $ "Unsupported dynamic value: " ++ show x
            #{const DBUS_TYPE_UINT32} -> toIter (fromDyn x (0 :: Word32)) iter
            #{const DBUS_TYPE_INT32} -> toIter (fromDyn x (0 :: Int32)) iter
            #{const DBUS_TYPE_STRING} -> toIter (fromDyn x ("" :: String)) iter
          loop xs iter
    in loop list iter
  signature _ = "d"
-}
foreign import ccall unsafe "dbus_message_iter_init"
  message_iter_init :: MessageP -> Iter -> IO Bool
foreign import ccall unsafe "dbus_message_iter_init_append"
  message_iter_init_append :: MessageP -> Iter -> IO ()
foreign import ccall unsafe "dbus_message_iter_get_arg_type"
  message_iter_get_arg_type :: Iter -> IO Int
foreign import ccall unsafe "dbus_message_iter_get_element_type"
  message_iter_get_element_type :: Iter -> IO Int
foreign import ccall unsafe "dbus_message_iter_get_fixed_array"
  message_iter_get_fixed_array :: Iter -> Ptr a -> Ptr CInt -> IO ()
foreign import ccall unsafe "dbus_message_iter_get_signature"
  message_iter_get_signature :: Iter -> IO CString
foreign import ccall unsafe "dbus_message_iter_get_basic"
  message_iter_get_basic :: Iter -> Ptr a -> IO ()
foreign import ccall unsafe "dbus_message_iter_get_array_len"
  message_iter_get_array_len :: Iter -> IO CInt
foreign import ccall unsafe "dbus_message_iter_recurse"
  message_iter_recurse :: Iter -> Iter -> IO ()
foreign import ccall unsafe "dbus_message_iter_next"
  message_iter_next :: Iter -> IO Bool
foreign import ccall unsafe "dbus_message_iter_append_basic"
  message_iter_append_basic :: Iter -> CInt -> Ptr a -> IO Bool
foreign import ccall unsafe "dbus_message_iter_open_container"
  message_iter_open_container :: Iter -> CInt -> CString -> Iter -> IO Bool
foreign import ccall unsafe "dbus_message_iter_close_container"
  message_iter_close_container :: Iter -> Iter -> IO Bool

getIterSignature :: Iter -> IO String
getIterSignature iter = message_iter_get_signature iter >>= peekCString

withIter = allocaBytes #{size DBusMessageIter}

-- |Retrieve the arguments from a message.
args :: Message -> [Arg]
args msg = unsafePerformIO $
  withForeignPtr msg $ \msg -> do
    withIter $ \iter -> do
      has_args <- message_iter_init msg iter
      if has_args then decodeArgs iter
                  else return []

-- |Add arguments to a message.
addArgs :: Message -> [Arg] -> IO ()
addArgs msg args =
  withForeignPtr msg $ \msg ->
    allocInit #{size DBusMessageIter} (message_iter_init_append msg) $ \iter ->
      mapM_ (encodeArg iter) args

-- vim: set ts=2 sw=2 tw=72 et ft=haskell :
