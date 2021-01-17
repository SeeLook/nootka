#Nootka
###...to play scores#

***

# ! ! ! ! ! ! ! ! ! TODO LIST ! ! ! ! ! ! ! ! !

============================================================================
### Bugs to fix:
  - 'out of instrument' message for sax doesn't fit a size
  - Exams: For non guitars single note question "give name of:" has nothing more
  - certificate for i.e. piano has strange size - probably font size/DPI scaling related issue
  - for melody set level selection accidentals to use makes no sense
  - do not allow selecting rests to start playing from them
  - jump to first tied note when playing position is changed
  - playing with metronome doesn't work under Android

### To test:
  - melodies with ties and multiple rests (either playing or writing)
  - single note exams: playing note by note (times between questions)
  - no rhythms melodies - wait for correct note option
  - no rhythms melodies - mixed with other melodies
  - levels with melodies to be played and written in one

### For incoming stable release version 2.0
  - Default font size - initial value is taken wrongly, so probably all adjustments have to be reviewed
  - (Settings) When tuning changes clef, set it for global and vice versa - change tuning clef when score one changes
  - level creator:
     - fix storing melody list in temporary/edited level - use that level list instead of creating pointers
  - add more levels, in separate files, distributed with the installers
  - "Got It!" clues in some crucial parts: Android quick menu, pitch detection and so on
  - add info that reading grand staff from musicxml created out of Nootka is not supported
  - exam summary - give more valuable info, display it nicely
  - charts - show preview of entire melody, chart tip is not suitable for it
  - migrate on preferred 48000 sample rate (ogg files and audio methods). DO NOT forget to resize output samples length

### For next stable Nootka version 2.2 and up
  - MIDI input & revert MIDI out of course
  - add rests option for random melodies in exams
  - implement repeating a melody that was heard
  - adding set of rhythm patterns for melodies generated from random notes (similar to melody list)
  - time stamps in question/answer units - to better track exam progress
  - music XML import dialog, to select voice or staff if there are more, and so
  - adding actually edited score melody to current level
  - editing melody from currently selected/composed level
  - add score helpers: transposition, folding ties
  - add violin support and revert pitch accuracy check.
    It has small usage for currently supported instruments, but for violin can be perfect
  - support for triplets
  - exams control melody tempo, either fixed value from melody or random from declared range (or increased every question)
  - viewing entire melody in stats, playing them and single notes as well
    so far, user can play whatever tempo he likes
  - option for randomizing timbre (instrument sound) in every dictation melody
  - add QML control for displaying single note using Bravura texts/glyphs system (to use in chart and question tips)
  - add Italian tempo names to tempo menu
  - read-only levels, password protected, when teacher doesn't want user to see melodies inside


============================================================================

### small glitches:
 - when exercise becomes exam tips are blended
 - melody won't play in exercises/exams (first question OK but further is worst)
 - sometimes setting a note doesn't scale tie - probably due to note width is not refreshed
 - space key for tapping tempo doesn't work well

### Translation context changes:
 - TscoreKeySignature to KeySignature

============================================================================

## MISSING IN ACTION (comparing to previous major version) - do we really need them?
 - left hand guitar (do we really need this?)
 - question mark and note name over instrument during exam/exercises (remember about actual name style)
 - no intonation gradients
 - There is no right mouse button to check/ask question - do we need that???

 - revert exam times (if we really need that):  
    next question delay  
    correction preview

***
***
# @@@@ IDEAS FOR FURTHER RELEASES @@@@
***

### SMALL NEW FEATURES/IMPROVEMENTS
  - add minimal zoom value depends on screen resolution
  - average effectiveness line in chart
  - certificates would have some additional data about melodies
  - bring back isVoice() audio param - average pitch of all chunks or min duration will depend on it

  - settings for keyboard shortcuts
  - intervals: creating melody with set of available intervals only
  - intervals: questions related to them in exam/exercises
  - recognizing melodies from *.ogg and *.wav files
  - support for (octavation marks) - octave drop/rise (8-va, 15-va)
  - preview of questions in Level Creator (list, on the guitar, in the score)
  - comparing exams (charts)
  - keep exam file in /tmp and restore it when crash or so
  - add sound during wizard and About dialog (???)
  - options for adjusting pitch detection range to  1.score, 2. instrument scale (???)


#### TEXAMEXECUTOR
  - think about hide octave in note name format when octaves are not required in answers.  
     Also for questions in "hint" - don't show octave when only name is required 


============================================================================
#### To consideration:
  - In wizard: option for single note mode for very beginners


