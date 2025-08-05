# MusicXML Exercise Segfault Fix

## Problem Description

When creating levels from imported MusicXML files and starting exercises with them, Nootka would crash with a segmentation fault. This prevented users from using the MusicXML import functionality for creating practice exercises.

## Root Cause Analysis

The segfault occurred in `src/core/exam/tlevel.cpp` at line 857 in the `useRhythms()` method:

```cpp
bool Tlevel::useRhythms() const
{
    return canBeMelody() && ((meters && (dotsRhythms || basicRhythms)) || 
           (isMelodySet() && melodySet.first().meter()->meter() != Tmeter::NoMeter));
    //                     ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    //                     This line caused the segfault
}
```

### Why the Crash Occurred

1. **Unsafe Memory Access**: The code accessed `melodySet.first().meter()->meter()` without proper null checking
2. **Race Condition**: Even though `isMelodySet()` checked if the melody set was not empty, there was a timing issue where the first melody might not be properly initialized
3. **MusicXML Import Edge Case**: MusicXML import could create melodies with invalid or null meter pointers

The existing code had a FIXME comment acknowledging this issue but it was never addressed.

## Solution Implemented

### Before (Unsafe):
```cpp
(isMelodySet() && melodySet.first().meter()->meter() != Tmeter::NoMeter)
```

### After (Safe):
```cpp
(isMelodySet() && !melodySet.isEmpty() && melodySet.first().meter() && 
 melodySet.first().meter()->meter() != Tmeter::NoMeter)
```

### Defensive Checks Added:
1. **`!melodySet.isEmpty()`** - Ensures the melody set actually contains melodies
2. **`melodySet.first().meter()`** - Verifies the meter pointer is not null
3. **Only then access `meter()->meter()`** - Safe to dereference after null checks

## Files Modified

- `src/core/exam/tlevel.cpp` (line 857): Added defensive null pointer checks
- `src/CMakeLists.txt` (line 137): Added missing `VERSION 1.0` parameter for Qt QML module

## Testing

### How to Reproduce the Original Bug:
1. Import a MusicXML file using Melody → Open
2. Create a level from the imported melody in Level Creator
3. Save the level as a `.nel` file
4. Load the level and click "Start Exercise"
5. **Before fix**: Segmentation fault
6. **After fix**: Exercise starts normally

### Verification:
The fix has been tested and confirmed to resolve the segfault while preserving all original functionality.

## Impact

- **Safety**: Eliminates crash when using MusicXML-created levels
- **Functionality**: Preserves all existing rhythm detection capabilities
- **Performance**: Minimal overhead from additional null checks
- **Compatibility**: No breaking changes to existing levels or functionality

## Build Information

- **Qt Version**: 6.4.2
- **Build System**: CMake
- **Architecture**: x86-64 Linux
- **Debug Symbols**: Included

This fix enables safe use of the MusicXML import functionality for creating practice exercises in Nootka.