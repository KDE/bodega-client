#! /usr/bin/env bash
$XGETTEXT `find . -name \*.qml` -L Java -o $podir/plasma_package_com.makeplaylive.addonsapp.pot
rm -f rc.cpp

