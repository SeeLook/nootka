# Nootka Build and Launch Instructions

## Prerequisites

### System Requirements
- **OS**: Linux (tested on Ubuntu/Debian-based systems)
- **Architecture**: x86-64
- **Qt Version**: 6.4.2 or compatible
- **CMake**: 3.16 or higher
- **Compiler**: GCC with C++17 support

### Dependencies
Install the following packages:

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    qt6-base-dev \
    qt6-qml-dev \
    qt6-declarative-dev \
    libqt6multimedia6-dev \
    libfftw3-dev \
    libvorbis-dev \
    libasound2-dev \
    libjack-jackd2-dev \
    libsoundtouch-dev \
    zlib1g-dev
```

## Building Nootka

### 1. Clone and Setup
```bash
# Navigate to your project directory
cd /path/to/your/nootka_project

# Ensure you're in the nootka subdirectory
cd nootka
```

### 2. Configure Build
```bash
# Create and enter build directory
mkdir -p build
cd build

# Configure with CMake
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

### 3. Build Application
```bash
# Build with parallel compilation (uses all CPU cores)
make -j$(nproc)

# Set up runtime environment
make runinplace
```

### Expected Output
- **Build Progress**: Should reach 100% without errors
- **Final Binary**: `src/nootka` (approximately 50+ MB)
- **Build Time**: 2-5 minutes depending on system

## Launching Nootka

### From Build Directory
```bash
# Ensure you're in the build directory
cd /path/to/your/nootka_project/nootka/build

# Launch Nootka
./src/nootka
```

### Expected Startup
```
QML debugging is enabled. Only use this in a safe environment.
Nootka launch time: ~500-600 [ms]
Audio data will be resampled to 48000
"ALSA" IN: "ALSA default" samplerate: 48000 , buffer size: 512
"ALSA" OUT: "ALSA default" samplerate: 48000 , buffer size: 512
```

## Testing MusicXML Functionality

### 1. Import MusicXML File
1. Launch Nootka: `./src/nootka`
2. Click **"Melody"** button in toolbar
3. Select **"Open"** from dropdown menu
4. Choose a MusicXML file (`.xml`, `.musicxml`, or `.mxl`)
5. If multiple parts detected, select desired parts in import dialog

### 2. Create Level from Imported Melody
1. Click **"Level"** button in toolbar (Level Creator)
2. Configure your level settings in the various tabs:
   - **Levels**: General level settings
   - **Questions**: What to ask students  
   - **Melody**: Melody-specific settings
   - **Rhythms**: Rhythm detection settings
   - **Range**: Note range settings
3. Click **"Save"** button in Levels tab
4. Enter level name and description
5. Choose save location (`.nel` file format)

### 3. Test Exercise (Verifying Fix)
1. In Level Creator, select your saved level
2. Click **"Start Exercise"** 
3. **Before fix**: Would crash with segmentation fault
4. **After fix**: Exercise starts normally without crash

## Troubleshooting

### Common Issues

#### "Cannot load Nootka fonts!"
**Solution**: Run `make runinplace` from the build directory

#### Missing Qt6 Dependencies
**Solution**: Install Qt6 development packages:
```bash
sudo apt install qt6-base-dev qt6-qml-dev qt6-declarative-dev
```

#### Audio Buffer Underflow Warnings
**Status**: Normal operational messages, not errors

#### CMake QML Module Version Error
**Solution**: Already fixed in this version with `VERSION 1.0` parameter

### Build Verification
```bash
# Check if executable was created
ls -la src/nootka

# Verify dependencies
ldd src/nootka | head -10

# Test basic functionality
./src/nootka --help
```

## Performance Notes

- **Launch Time**: ~500-600ms (normal)
- **Memory Usage**: ~50-100MB typical
- **Audio Latency**: 512 samples buffer (low latency)
- **CPU Usage**: Low during idle, moderate during audio processing

## Files Created During Build

- `src/nootka` - Main executable
- `lang/*.qm` - Compiled translation files
- `share/nootka/` - Runtime resources (linked via `make runinplace`)
- Various build artifacts in CMake cache

This build includes the critical segfault fix for MusicXML-created exercises.