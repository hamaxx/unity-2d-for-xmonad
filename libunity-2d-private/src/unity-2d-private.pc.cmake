prefix=@PREFIXDIR@
exec_prefix=@EXEC_PREFIX@
libdir=${exec_prefix}/@CMAKE_INSTALL_LIBDIR@
includedir=${prefix}/@CMAKE_INSTALL_INCLUDEDIR@

Name: unity-2d-private
Description: Unity 2d private library
Version: 4.12.0

Requires.private: QtCore, QtDBus, QtDeclarative, QtGui, glib-2.0, gtk+-3.0, libwnck-3.0, pango
Libs: -L${libdir} -lunity-2d-private
Cflags: -I${includedir}/unity-2d-private

