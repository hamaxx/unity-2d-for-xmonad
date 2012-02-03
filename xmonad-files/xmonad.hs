import XMonad
import XMonad.Config.Gnome

import Control.OldException(catchDyn,try)
import Control.Concurrent
import DBus
import DBus.Connection
import DBus.Message
import System.Cmd
import XMonad.Hooks.DynamicLog


myManageHook = composeAll (
  [ manageHook gnomeConfig
  , className =? "Unity-2d-panel" --> doIgnore
  , className =? "Unity-2d-launcher" --> doIgnore
  , className =? "Unity-2d-places" --> doFloat
  , className =? "Do" --> doIgnore
  ])

main = withConnection Session $ \ dbus -> do
  xmonad gnomeConfig {
    manageHook = myManageHook
  , logHook    = logHook gnomeConfig >> dynamicLogWithPP (myPrettyPrinter dbus)
  }

myPrettyPrinter dbus = defaultPP {
    ppOutput  = outputThroughDBus dbus
  , ppTitle   = returnBlank
  , ppCurrent = pangoColor "#ffffff" . wrap "[" "]" . pangoSanitize
  , ppVisible = pangoColor "#dddddd" . wrap "(" ")" . pangoSanitize
  , ppHidden  = wrap " " " "
  , ppUrgent  = pangoColor "red"
  }

-- This retry is really awkward, but sometimes DBus won't let us get our
-- name unless we retry a couple times.
getWellKnownName :: Connection -> IO ()
getWellKnownName dbus = tryGetName `catchDyn` (\ (DBus.Error _ _) ->
                                                getWellKnownName dbus)
 where
  tryGetName = do
    namereq <- newMethodCall serviceDBus pathDBus interfaceDBus "RequestName"
    addArgs namereq [String "org.xmonad.Log", Word32 5]
    sendWithReplyAndBlock dbus namereq 0
    return ()

outputThroughDBus :: Connection -> String -> IO ()
outputThroughDBus dbus str = do
  let str' = "<font color=\"#dddddd\">" ++ str ++ "</span>"
  msg <- newSignal "/org/xmonad/Log" "org.xmonad.Log" "Update"
  addArgs msg [String str']
  send dbus msg 0 `catchDyn` (\ (DBus.Error _ _ ) -> return 0)
  return ()

returnBlank :: String -> String
returnBlank x = ""

pangoColor :: String -> String -> String
pangoColor fg = wrap left right
 where
  left  = "<font color=\"" ++ fg ++ "\">"
  right = "</font>"

pangoSanitize :: String -> String
pangoSanitize = foldr sanitize ""
 where
  sanitize '>'  acc = "&gt;" ++ acc
  sanitize '<'  acc = "&lt;" ++ acc
  sanitize '\"' acc = "&quot;" ++ acc
  sanitize '&'  acc = "&amp;" ++ acc
  sanitize x    acc = x:acc
