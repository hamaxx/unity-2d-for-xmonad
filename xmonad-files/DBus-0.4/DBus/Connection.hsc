-- HDBus -- Haskell bindings for D-Bus.
-- Copyright (C) 2006 Evan Martin <martine@danga.com>

#define DBUS_API_SUBJECT_TO_CHANGE
#include "dbus/dbus.h"

module DBus.Connection (
  -- * Connection Basics
  Connection,
  BusType(..),
  busGet, busConnectionUnref,
  send, sendWithReplyAndBlock,
  flush, close,
  withConnection,

  -- * Main Loop Management
  readWriteDispatch, addFilter, addMatch,

  RequestNameReply(..),
  busRequestName,
) where

import Control.Exception (bracket)
import Control.Monad (when)
import Foreign
import Foreign.C.String
import Foreign.C.Types (CInt)

import DBus.Internal
import DBus.Message
import DBus.Shared

type Connection = ForeignPtr ConnectionTag

-- |Multiple buses may be active simultaneously on a single system.
--  The BusType indicates which one to use.
data BusType = Session -- ^The session bus is restricted to the user's current
                       --  GNOME session.
             | System  -- ^This bus is system-wide.
             | Starter -- ^The bus that started us, if any.

foreign import ccall unsafe "&dbus_connection_unref"
  connection_unref :: FunPtr (ConnectionP -> IO ())

connectionPTOConnection conn = do
  when (conn == nullPtr) $ fail "null connection"
  newForeignPtr connection_unref conn

-- |Force the dereference of a connection. Note that this is usually not
-- necessary since the connections are garbage collected automatically.
busConnectionUnref :: Connection -> IO ()
busConnectionUnref = finalizeForeignPtr


foreign import ccall unsafe "dbus_bus_get"
  bus_get :: CInt -> ErrorP -> IO ConnectionP
-- |Connect to a standard bus.
busGet :: BusType -> IO Connection
busGet bt = withErrorP (bus_get (toInt bt)) >>= connectionPTOConnection where
  toInt Session = #{const DBUS_BUS_SESSION}
  toInt System  = #{const DBUS_BUS_SYSTEM}
  toInt Starter = #{const DBUS_BUS_STARTER}

data RequestNameReply = PrimaryOwner | InQueue | Exists | AlreadyOwner
foreign import ccall unsafe "dbus_bus_request_name"
  bus_request_name :: ConnectionP -> CString -> CInt -> ErrorP -> IO CInt
busRequestName :: Connection -> String -> [Int] -> IO RequestNameReply
busRequestName conn name flags =
  withForeignPtr conn $ \conn -> do
    withCString name $ \cname -> do
      ret <- withErrorP (bus_request_name conn cname 2)
      return $ fromInt ret where
  fromInt #{const DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER} = PrimaryOwner
  fromInt #{const DBUS_REQUEST_NAME_REPLY_IN_QUEUE}      = InQueue
  fromInt #{const DBUS_REQUEST_NAME_REPLY_EXISTS}        = Exists
  fromInt #{const DBUS_REQUEST_NAME_REPLY_ALREADY_OWNER} = AlreadyOwner

-- |Close a connection.  A connection must be closed before its last
-- reference disappears.
-- You may not close a connection created with @busGet@.
foreign import ccall unsafe "dbus_connection_close"
  connection_close :: ConnectionP -> IO ()
close :: Connection -> IO ()
close conn = withForeignPtr conn connection_close

-- |Open a connection and run an IO action, ensuring it is properly closed when
-- you're done.
withConnection :: BusType -> (Connection -> IO a) -> IO a
withConnection bt = bracket (busGet bt) busConnectionUnref

foreign import ccall unsafe "dbus_connection_send"
  connection_send :: ConnectionP -> MessageP -> Ptr Word32 -> IO Bool

-- |Adds a 'Message' to the outgoing message queue.
send :: Connection -> Message
     -> Word32    -- ^Serial.
     -> IO Word32 -- ^Returned serial.
send conn msg serial =
  withForeignPtr conn $ \conn -> do
    withForeignPtr msg $ \msg -> do
      with serial $ \serial -> do
        catchOom $ connection_send conn msg serial
        peek serial

type PendingCallTag = ()
type PendingCallP = Ptr PendingCallTag
type PendingCall = ForeignPtr PendingCallTag

foreign import ccall unsafe "dbus_connection_send_with_reply"
  connection_send_with_reply :: ConnectionP -> MessageP
                             -> Ptr PendingCallP -> IO Bool
foreign import ccall unsafe "&dbus_pending_call_unref"
  pending_call_unref :: FunPtr (PendingCallP -> IO ())

sendWithReply :: Connection -> Message
              -> Maybe Int -- ^Optional timeout in milliseconds.
              -> IO PendingCall
-- XXX a NULL PendingCall lets us track timeout errors
sendWithReply conn msg timeout = do
  withForeignPtr conn $ \conn -> do
    withForeignPtr msg $ \msg -> do
      with (nullPtr :: PendingCallP) $ \ppcp -> do
          catchOom $ connection_send_with_reply conn msg ppcp
          throwIfNull "null PPendingCall" (return ppcp)
          pcp <- peek ppcp
          throwIfNull "null PendingCall" (return pcp)
          newForeignPtr pending_call_unref pcp

foreign import ccall unsafe "dbus_connection_send_with_reply_and_block"
  connection_send_with_reply_and_block :: ConnectionP -> MessageP -> Int -> ErrorP -> IO MessageP

sendWithReplyAndBlock :: Connection -> Message
                      -> Int -- ^Timeout in milliseconds.
                      -> IO Message
sendWithReplyAndBlock conn msg timeout =
  withForeignPtr conn $ \conn -> do
    withForeignPtr msg $ \msg -> do
      ret <- withErrorP $ connection_send_with_reply_and_block conn msg timeout
      messagePToMessage ret False

foreign import ccall unsafe "dbus_connection_flush"
  connection_flush :: ConnectionP -> IO ()
-- |Block until all pending messages have been sent.
flush :: Connection -> IO ()
flush conn = withForeignPtr conn connection_flush

foreign import ccall "dbus_connection_read_write_dispatch"
  connection_read_write_dispatch :: ConnectionP -> Int -> IO Bool
-- |Block until a message is read or written, then return True unless a
-- disconnect message is received.
readWriteDispatch :: Connection
                  -> Int -- ^Timeout, in milliseconds.
                  -> IO Bool
readWriteDispatch conn timeout = do
  withForeignPtr conn $ \conn ->
    connection_read_write_dispatch conn timeout

-- "a" here is the type of the callback function.
data FreeClosure a = FreeClosure { fcCallback :: FunPtr a,
                                   fcFree :: FunPtr (FreeFunction a) }
type FreeFunction a = StablePtr (FreeClosure a) -> IO ()
foreign import ccall "wrapper"
  wrapFreeFunction :: FreeFunction a -> IO (FunPtr (FreeFunction a))

mkFreeClosure :: FunPtr a -> IO (FreeClosure a)
mkFreeClosure callback = do
  freef <- wrapFreeFunction freeFunction
  return $ FreeClosure callback freef
  where
    freeFunction :: FreeFunction a
    freeFunction sptr = do
      (FreeClosure cb freef) <- deRefStablePtr sptr
      freeStablePtr sptr
      freeHaskellFunPtr cb
      freeHaskellFunPtr freef  -- XXX we are freeing ourselves.
                               -- XXX this is officially not ok,
                               -- XXX but it seems like it'll do.

type HandleMessageFunction = ConnectionP -> MessageP -> Ptr () -> IO CInt

foreign import ccall "wrapper"
  wrapHandleMessageFunction :: HandleMessageFunction
                            -> IO (FunPtr HandleMessageFunction)

foreign import ccall "dbus_connection_add_filter"
  connection_add_filter :: ConnectionP
                        -> FunPtr HandleMessageFunction -> StablePtr a
                        -> FunPtr (StablePtr a -> IO ()) -> IO Bool
addFilter :: Connection
          -> (Message -> IO Bool) -- ^A callback that returns True if
                                  --  the message has been handled.
          -> IO ()
addFilter conn callback = do
  withForeignPtr conn $ \conn -> do
    hmf <- wrapHandleMessageFunction handleMessageFunction
    closure <- mkFreeClosure hmf
    pclosure <- newStablePtr closure
    catchOom $ connection_add_filter conn hmf pclosure (fcFree closure)
    where
    handleMessageFunction :: HandleMessageFunction
    handleMessageFunction connp messagep datap = do
      message <- messagePToMessage messagep True
      res <- callback message
      if res then return #{const DBUS_HANDLER_RESULT_HANDLED}
             else return #{const DBUS_HANDLER_RESULT_NOT_YET_HANDLED}


foreign import ccall "dbus_bus_add_match"
  bus_add_match :: ConnectionP -> CString -> ErrorP -> IO ()
addMatch :: Connection
         -> Bool -- ^Whether to block waiting for a response, allowing
                 -- us to raise an exception if a response never comes.
         -> String -> IO ()
addMatch conn block rule =
  withForeignPtr conn $ \conn ->
    withCString rule $ \rule -> do
      if block
        then withErrorP $ bus_add_match conn rule
        else bus_add_match conn rule nullPtr

-- vim: set ts=2 sw=2 tw=72 et ft=haskell :
