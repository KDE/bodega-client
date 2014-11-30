#! /bin/sh
$XGETTEXT_QT `find -name "*cpp" | grep -v '/tests/'` -o $podir/bodega_qt.pot

