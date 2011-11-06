#configuration
CONFIG +=  release stl qt_no_framework
QT_ARCH = i386
QT_EDITION = OpenSource
QT_CONFIG +=  qt3support accessibility opengl minimal-config small-config medium-config large-config full-config reduce_exports ipv6 getaddrinfo ipv6ifname getifaddrs system-jpeg system-mng system-png png no-gif system-freetype system-zlib nis cups iconv glib x11sm xshape xinerama xcursor xfixes xrandr xrender fontconfig tablet xkb release

#versioning
QT_VERSION = 4.2.3
QT_MAJOR_VERSION = 4
QT_MINOR_VERSION = 2
QT_PATCH_VERSION = 3

QMAKE_RPATHDIR += "/usr/local/Trolltech/Qt-4.2.3/lib"
