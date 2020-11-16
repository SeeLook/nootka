#!/bin/sh

VERSION=$1
SRC=$2
BUILD=$(git -C $SRC rev-list HEAD --count)

cat <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
        <key>CFBundleDevelopmentRegion</key>
        <string>English</string>
        <key>CFBundleDocumentTypes</key>
        <array>
                <dict>
                        <key>CFBundleTypeExtensions</key>
                        <array>
                                <string>nel</string>
                        </array>
                        <key>CFBundleTypeIconFile</key>
                        <string>nootka-levels.icns</string>
                        <key>CFBundleTypeMIMETypes</key>
                        <array>
                                <string>application/x-nootka</string>
                        </array>
                        <key>CFBundleTypeName</key>
                        <string>Nootka level file</string>
                        <key>CFBundleTypeRole</key>
                        <string>Editor</string>
                        <key>LSTypeIsPackage</key>
                        <false/>
                        <key>LSIsAppleDefaultForType</key>
                        <true/>
                </dict>
                <dict>
                        <key>CFBundleTypeExtensions</key>
                        <array>
                                <string>noo</string>
                        </array>
                        <key>CFBundleTypeIconFile</key>
                        <string>nootka-exam.icns</string>
                        <key>CFBundleTypeMIMETypes</key>
                        <array>
                                <string>application/x-nootka</string>
                        </array>
                        <key>CFBundleTypeName</key>
                        <string>Nootka exam file</string>
                        <key>CFBundleTypeRole</key>
                        <string>Editor</string>
                        <key>LSTypeIsPackage</key>
                        <false/>
                        <key>LSIsAppleDefaultForType</key>
                        <true/>
                </dict>
        </array>
        <key>CFBundleExecutable</key>
        <string>nootka</string>
        <key>CFBundleGetInfoString</key>
        <string>Nootka;release;for;Mac;OSX</string>
        <key>CFBundleIconFile</key>
        <string>nootka.icns</string>
        <key>CFBundleIdentifier</key>
        <string>net.sourceforge.nootka</string>
        <key>CFBundleInfoDictionaryVersion</key>
        <string>6.0</string>
        <key>CFBundleLongVersionString</key>
        <string>$VERSION</string>
        <key>CFBundleName</key>
        <string>Nootka</string>
        <key>CFBundlePackageType</key>
        <string>APPL</string>
        <key>CFBundleShortVersionString</key>
        <string>$VERSION</string>
        <key>CFBundleSignature</key>
        <string>????</string>
        <key>CFBundleVersion</key>
        <string>$BUILD</string>
        <key>CSResourcesFileMapped</key>
        <true/>
        <key>LSRequiresCarbon</key>
        <true/>
        <key>NSHumanReadableCopyright</key>
        <string>nootka.sf.net</string>
</dict>
</plist>

EOF
