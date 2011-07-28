prefix=@PREFIXDIR@
exec_prefix=@EXEC_PREFIX@
libdir=@LIBDIR@
includedir=@INCLUDEDIR@

Name: unity-2d-private
Library
Description: Unity 2d private library
Version: @VERSION@
Libs: -L@LIBDIR@ -lunity-2d-private
Cflags: -I@INCLUDEDIR@/unity-2d-private
Requires: glib-2.0, gtk+-3.0, libwnck-3.0, pango
