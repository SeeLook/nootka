# Test Files for Nootka MusicXML Import

This directory contains test MusicXML files for verifying the MusicXML import functionality and the segfault fix.

## Test Files

### hjlesson_July26_2025.xml
- **Purpose**: Test file for MusicXML import and exercise creation
- **Creator**: Alex Prigalu 
- **Software**: MuseScore 3.2.3
- **Format**: MusicXML 3.1 Partwise
- **Size**: ~13KB
- **Use Case**: Tests the complete workflow from MusicXML import to exercise creation

## How to Test

### Import Test
1. Launch Nootka: `./src/nootka`
2. Click **Melody** → **Open**
3. Select `test_files/hjlesson_July26_2025.xml`
4. Verify successful import without errors

### Exercise Creation Test (Segfault Fix Verification)
1. After importing the XML file
2. Open **Level Creator** (Level button in toolbar)
3. Configure level settings as desired
4. **Save** the level as a `.nel` file
5. **Start Exercise** with the created level
6. **Expected**: Exercise starts normally (no segfault)
7. **Before Fix**: Would crash with segmentation fault

## Technical Notes

This test file specifically exercises the code path that was causing segfaults in:
- `src/core/exam/tlevel.cpp:857` - `useRhythms()` method
- Melody set rhythm detection for imported MusicXML files

The fix adds defensive null pointer checks to prevent crashes when accessing melody meter information from imported MusicXML files.