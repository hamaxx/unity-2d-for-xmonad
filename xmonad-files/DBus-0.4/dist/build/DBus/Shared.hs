{-# OPTIONS_GHC -optc-DDBUS_API_SUBJECT_TO_CHANGE #-}
{-# LINE 1 "DBus/Shared.hsc" #-}
-- HDBus -- Haskell bindings for D-Bus.
{-# LINE 2 "DBus/Shared.hsc" #-}
-- Copyright (C) 2006 Evan Martin <martine@danga.com>

-- tell Haddock not to doc this module:
-- #hide


{-# LINE 8 "DBus/Shared.hsc" #-}

{-# LINE 9 "DBus/Shared.hsc" #-}

module DBus.Shared (
  -- * Constants
  -- |Well-known service, path, and interface names.
  ServiceName, PathName, InterfaceName,
  serviceDBus,
  pathDBus, pathLocal,
  interfaceDBus, interfaceIntrospectable, interfaceLocal,
  
) where

import Foreign
import Foreign.C.String

type ServiceName = String
type PathName = FilePath
type InterfaceName = String
serviceDBus :: ServiceName
pathDBus, pathLocal :: PathName
interfaceDBus, interfaceIntrospectable, interfaceLocal :: InterfaceName
serviceDBus = "org.freedesktop.DBus"
{-# LINE 30 "DBus/Shared.hsc" #-}
pathDBus    = "/org/freedesktop/DBus"
{-# LINE 31 "DBus/Shared.hsc" #-}
pathLocal   = "/org/freedesktop/DBus/Local"
{-# LINE 32 "DBus/Shared.hsc" #-}
interfaceDBus = "org.freedesktop.DBus"
{-# LINE 33 "DBus/Shared.hsc" #-}
interfaceIntrospectable = "org.freedesktop.DBus.Introspectable"
{-# LINE 34 "DBus/Shared.hsc" #-}
interfaceLocal = "org.freedesktop.DBus.Local"
{-# LINE 35 "DBus/Shared.hsc" #-}

-- vim: set ts=2 sw=2 tw=72 et ft=haskell :
