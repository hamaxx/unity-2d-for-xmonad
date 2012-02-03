{-# OPTIONS_GHC -optc-DDBUS_API_SUBJECT_TO_CHANGE #-}
{-# LINE 1 "DBus.hsc" #-}
-- HDBus -- Haskell bindings for D-Bus.
{-# LINE 2 "DBus.hsc" #-}
-- Copyright (C) 2006 Evan Martin <martine@danga.com>


{-# LINE 5 "DBus.hsc" #-}

{-# LINE 6 "DBus.hsc" #-}

module DBus (
  module DBus.Shared,

  -- * Error Handling
  -- | Some D-Bus functions can only fail on out-of-memory conditions.
  -- I don't think there is much we can do in those cases.
  --
  -- Other D-Bus functions can fail with other sorts of errors, which are
  -- raised as dynamic exceptions.  Errors can be caught with
  -- 'Control.Exception.catchDyn', like this:
  --
  -- > do conn <- DBus.busGet DBus.System
  -- >    doSomethingWith conn
  -- > `catchDyn` (\(DBus.Error name msg) -> putStrLn ("D-Bus error! " ++ msg))
  --
  Error(..),
) where

import DBus.Shared
import Data.Typeable (Typeable(..), mkTyConApp, mkTyCon)
--import Control.Exception(Exception)

-- |'Error's carry a name (like \"org.freedesktop.dbus.Foo\") and a
-- message (like \"connection failed\").
data Error = Error String String
instance Typeable Error where
  typeOf _ = mkTyConApp (mkTyCon "DBus.Error") []
instance Show Error where
  show (Error name message) = "D-Bus Error (" ++ name ++ "): " ++ message
--instance Exception Error

-- vim: set ts=2 sw=2 tw=72 et ft=haskell :
