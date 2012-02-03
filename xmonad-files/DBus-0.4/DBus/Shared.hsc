-- HDBus -- Haskell bindings for D-Bus.
-- Copyright (C) 2006 Evan Martin <martine@danga.com>

-- tell Haddock not to doc this module:
-- #hide

#define DBUS_API_SUBJECT_TO_CHANGE
#include "dbus/dbus.h"

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
serviceDBus = #{const_str DBUS_SERVICE_DBUS}
pathDBus    = #{const_str DBUS_PATH_DBUS}
pathLocal   = #{const_str DBUS_PATH_LOCAL}
interfaceDBus = #{const_str DBUS_INTERFACE_DBUS}
interfaceIntrospectable = #{const_str DBUS_INTERFACE_INTROSPECTABLE}
interfaceLocal = #{const_str DBUS_INTERFACE_LOCAL}

-- vim: set ts=2 sw=2 tw=72 et ft=haskell :
