# Powershell

# Invoke-Expression (New-Object System.Net.WebClient).DownloadString('https://get.scoop.sh')
iex "& {$(irm get.scoop.sh)} -RunAsAdmin"
scoop bucket add extras
scoop install nsis
scoop install wget

echo "--- Getting ZLIB"
wget -q "https://www.opencode.net/seelook/nootka-build/-/raw/master/3rdParty/zlib.zip"
Expand-Archive -Path "zlib.zip" -DestinationPath "zlibExtract"
rm -fo zlib.zip
Move-Item -Path ".\zlibExtract\zlib\*.*" -Destination ".\zlibExtract\"
Move-Item -Path "zlibExtract" -Destination "zlib"

echo "--- Getting 3rd party libraries (FFTW, OGG, VORBIS)"
wget -q "https://www.opencode.net/seelook/nootka-build/-/raw/master/3rdParty/win32-fftw-ogg-vorbis.tar.gz"
tar -xzf .\win32-fftw-ogg-vorbis.tar.gz

$mainDir = Get-Location
$mainDir -replace "\","\\\"
echo "--- MainDir: $mainDir"

mkdir build
cd build

cmake -G "MinGW Makefiles" -DZLIB_LIBRARY="$mainDir\\\zlib\\\zlib1.dll" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=installs ../

mingw32-make -j2

mkdir installs
mingw32-make install
mingw32-make deploy

echo "--- Building installer"
makensis installs/nootka-utf16.nsi

cd ..
