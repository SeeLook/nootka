#! /bin/bash

echo "-- copying GCC libraries"

cp /usr/x86_64-w64-mingw32/bin/libgobject-2.0-0.dll installs/
cp /usr/x86_64-w64-mingw32/bin/libstdc++-6.dll installs/
cp /usr/x86_64-w64-mingw32/bin/libwinpthread-1.dll installs/
cp /usr/x86_64-w64-mingw32/bin/libgcc_s_seh-1.dll installs/
cp /usr/x86_64-w64-mingw32/bin/libiconv-2.dll installs/
cp /usr/x86_64-w64-mingw32/bin/libharfbuzz*.dll installs/
cp /usr/x86_64-w64-mingw32/bin/libpng*.dll installs/
cp /usr/x86_64-w64-mingw32/bin/zlib1.dll installs/
cp /usr/x86_64-w64-mingw32/bin/libpcre*.dll installs/
cp /usr/x86_64-w64-mingw32/bin/libgraphite2.dll installs/
cp /usr/x86_64-w64-mingw32/bin/libbz2-1.dll installs/
cp /usr/x86_64-w64-mingw32/bin/libfreetype-6.dll installs/
cp /usr/x86_64-w64-mingw32/bin/libglib-2.0-0.dll installs/
cp /usr/x86_64-w64-mingw32/bin/libintl-8.dll installs/
cp /usr/x86_64-w64-mingw32/bin/libssp-0.dll installs/
cp /usr/x86_64-w64-mingw32/bin/libjpeg-8.dll installs/

echo "-- copying Qt5 libraries"
QT_LIBS=(Qt5Core  Qt5Gui  Qt5Network  Qt5PrintSupport  Qt5Qml  Qt5Quick  Qt5QuickControls2  Qt5QuickTemplates2  Qt5Widgets Qt5QmlModels Qt5Svg Qt5QmlWorkerScript)
for lib in ${QT_LIBS[*]}
do
  echo $lib.dll
  cp /usr/x86_64-w64-mingw32/bin/$lib.dll installs/
done

echo "-- copying Qt5 plugins"
QT_PLUGS=(bearer imageformats platforms qmltooling)
for plug in ${QT_PLUGS[*]}
do
  echo $plug
  cp -r /usr/x86_64-w64-mingw32/lib/qt/plugins/$plug installs/
done

echo "-- copying QML modules"
QML_MODS=(Qt QtGraphicalEffects QtQml QtQuick QtQuick.2)
for mod in ${QML_MODS[*]}
do
  echo $mod
  cp -r /usr/x86_64-w64-mingw32/lib/qt/qml/$mod installs/
done

echo "-- remove unused parts of QML modules"
rm -r installs/Qt/labs/location
rm -r installs/QtQuick/Particles.2
rm -r installs/QtQuick/Shapes
rm installs/platforms/qdirect2d.dll
rm installs/platforms/qminimal.dll
rm installs/platforms/qoffscreen.dll
