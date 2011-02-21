file(GLOB PO_FILES *.po)
gettext_create_translations(unity-2d.pot ALL ${PO_FILES})
