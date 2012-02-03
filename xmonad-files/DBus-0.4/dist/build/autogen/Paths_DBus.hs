module Paths_DBus (
    version,
    getBinDir, getLibDir, getDataDir, getLibexecDir,
    getDataFileName
  ) where

import Data.Version (Version(..))
import System.Environment (getEnv)

version :: Version
version = Version {versionBranch = [0,4], versionTags = []}

bindir, libdir, datadir, libexecdir :: FilePath

bindir     = "/home/ham/.cabal/bin"
libdir     = "/home/ham/.cabal/lib/DBus-0.4/ghc-7.0.3"
datadir    = "/home/ham/.cabal/share/DBus-0.4"
libexecdir = "/home/ham/.cabal/libexec"

getBinDir, getLibDir, getDataDir, getLibexecDir :: IO FilePath
getBinDir = catch (getEnv "DBus_bindir") (\_ -> return bindir)
getLibDir = catch (getEnv "DBus_libdir") (\_ -> return libdir)
getDataDir = catch (getEnv "DBus_datadir") (\_ -> return datadir)
getLibexecDir = catch (getEnv "DBus_libexecdir") (\_ -> return libexecdir)

getDataFileName :: FilePath -> IO FilePath
getDataFileName name = do
  dir <- getDataDir
  return (dir ++ "/" ++ name)
