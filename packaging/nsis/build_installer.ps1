# Powershell

echo "--- Installing WGET & NSIS"
Invoke-Expression (New-Object System.Net.WebClient).DownloadString('https://get.scoop.sh')
scoop bucket add extras
scoop install wget
scoop install nsis

echo "--- Getting ZLIB"
wget "https://www.opencode.net/seelook/nootka-build/-/raw/master/3rdParty/zlib.zip"
Expand-Archive -Path zlib.zip -DestinationPath zlib
Move-Item -Path .\zlib\zlib\*.* -Destination .\zlib\

echo "--- Getting 3rd party libraries (FFTW, OGG, VORBIS)"
wget "https://www.opencode.net/seelook/nootka-build/-/raw/master/3rdParty/win32-fftw-ogg-vorbis.tar.gz"
tar -xvzf .\win32-fftw-ogg-vorbis.tar.gz

mkdir build
cd build

# cmake -DCMAKE_C_COMPILER=C:/Qt/Tools/mingw810_32/bin/gcc.exe -DCMAKE_CXX_COMPILER=C:/Qt/Tools/mingw810_32/bin/g++.exe -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=installs ../

cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=installs ../

make -j2

mkdir installs
make install
make deploy

echo "--- Building installer"
makensis installs/nootka-utf16.nsi

Move-Item -Path .\installs\Nootka-*-Windows-Installer.exe -Destination ..\Nootka-Installer.exe

cd ..
ls
