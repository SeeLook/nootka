# Introduction
This project aims to add functionality to Nootka where it can read in MusicXML melodies and listen to accuracy of playing the scores

**UPDATE**: Investigation revealed that MusicXML import functionality already exists in Nootka but had a critical segfault bug when creating exercises from imported melodies. This has been fixed.

# Commands: Git source for nootka
Since we will be modifying the nootka application code. I use
```sh
git submodule add https://github.com/SeeLook/nootka.git
```

# Progress and Findings

## MusicXML Functionality Discovery
- **Status**: ✅ **FOUND - Already implemented**
- **Location**: Melody toolbar → "Open" option
- **Supported formats**: `.xml`, `.musicxml`, `.mxl` (compressed MusicXML)
- **Access path**: Melody button → Open → Select MusicXML file → Import dialog (if multiple parts)

## Critical Bug Fixed: Exercise Segfault
- **Issue**: Starting exercises with MusicXML-created levels caused segmentation fault
- **Root cause**: Unsafe memory access in `src/core/exam/tlevel.cpp:857`
- **Fix applied**: Added defensive null pointer checks in `useRhythms()` method
- **Status**: ✅ **FIXED and tested**

## Build System Issues Resolved
- **CMake QML Module**: Fixed missing `VERSION 1.0` parameter
- **Runtime setup**: Documented need for `make runinplace` 
- **Status**: ✅ **Building successfully**

# Key Files Modified
1. `src/core/exam/tlevel.cpp` - Fixed segfault in melody rhythm detection
2. `src/CMakeLists.txt` - Added missing QML module version
3. `MUSICXML_SEGFAULT_FIX.md` - Comprehensive technical documentation
4. `BUILD_INSTRUCTIONS.md` - Complete build and usage guide

# How to Use MusicXML Import (Now Fixed)
1. **Import**: Melody → Open → Select MusicXML file
2. **Create Level**: Level Creator → Configure → Save as `.nel`
3. **Exercise**: Load level → Start Exercise (no longer crashes!)

# Build and Test
```sh
cd nootka/build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
make runinplace
./src/nootka  # Test the fixed application
```
