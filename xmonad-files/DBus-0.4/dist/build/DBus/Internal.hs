{-# OPTIONS_GHC -optc-DDBUS_API_SUBJECT_TO_CHANGE #-}
{-# LINE 1 "DBus/Internal.hsc" #-}
-- HDBus -- Haskell bindings for D-Bus.
{-# LINE 2 "DBus/Internal.hsc" #-}
-- Copyright (C) 2006 Evan Martin <martine@danga.com>

-- tell Haddock not to doc this module:
-- #hide


{-# LINE 8 "DBus/Internal.hsc" #-}

{-# LINE 9 "DBus/Internal.hsc" #-}

module DBus.Internal where

import Control.OldException (throwDyn)
import Control.Monad (when)
import DBus (Error(..))
import Foreign
import Foreign.C.String

type MessageTag = ()
type MessageP = Ptr MessageTag
type ConnectionTag = ()
type ConnectionP = Ptr ConnectionTag

allocInit :: Int -> (Ptr a -> IO ()) -> (Ptr a -> IO b) -> IO b
allocInit size init cont = allocaBytes size $ \obj -> do init obj; cont obj

-- Sometimes functions claim they can only fail in oom conditions.
-- What else can we do but die?
catchOom :: IO Bool -> IO ()
catchOom action = throwIf_ (== False) (const "Out of memory") action

foreign import ccall unsafe "dbus_message_ref"
  message_ref :: MessageP -> IO ()
foreign import ccall unsafe "&dbus_message_unref"
  message_unref :: FunPtr (MessageP -> IO ())
messagePToMessage msg addref = do
  throwIfNull "null message" (return msg)
  when addref $ message_ref msg
  newForeignPtr message_unref msg

type ErrorTag = ()
type ErrorP = Ptr ErrorTag

foreign import ccall unsafe "dbus_error_init"
  error_init :: ErrorP -> IO ()
foreign import ccall unsafe "dbus_error_is_set"
  error_is_set :: ErrorP -> IO Bool
foreign import ccall unsafe "dbus_error_free"
  error_free :: ErrorP -> IO ()

withErrorP :: (ErrorP -> IO a) -> IO a
withErrorP f =
  allocInit (32) error_init $ \err -> do
{-# LINE 53 "DBus/Internal.hsc" #-}
    ret <- f err
    has_err <- error_is_set err
    if not has_err
      then return ret
      else do name <- (\hsc_ptr -> peekByteOff hsc_ptr 0) err >>= peekCString
{-# LINE 58 "DBus/Internal.hsc" #-}
              msg  <- (\hsc_ptr -> peekByteOff hsc_ptr 8) err >>= peekCString
{-# LINE 59 "DBus/Internal.hsc" #-}
              error_free err
              throwDyn $ Error name msg

-- vim: set ts=2 sw=2 tw=72 et ft=haskell :
