# Nootka
### ...to play scores#

***

! ! ! ! ! ! ! ! ! TODO LIST ! ! ! ! ! ! ! ! !
============================================================================
============================================================================

### Bugs to fix:
  - Actions after displaying certificate are messy
  - Exam summary may not fit the window.
  - Exams: For non guitars single note question "give name of:" has nothing more
  - playing with metronome doesn't work under Android
  - older Android versions (below 7.1, 4.4 for sure) doesn't detect/load default translation

### To test:
  - melodies with ties and multiple rests (either playing or writing)
  - single note exams: playing note by note (times between questions)
  - no rhythms melodies - wait for correct note option
  - no rhythms melodies - mixed with other melodies
  - levels with melodies to be played and written in one

### For incoming stable release version 2.0
  - rejecting pitch-detected note due to out of current ambitus causes rhythm gaps
  - clean correction code, key signature anim maybe
  - level creator:
     - fix storing melody list in temporary/edited level - use that level list instead of creating pointers
  - add more levels, in separate files, distributed with the installers
  - add info that reading grand staff from musicxml created out of Nootka is not supported
  - charts - show preview of entire melody, chart tip is not suitable for it
  - clean code:
    - when right mouse button is not used: TexamExacutor and TexecutorSupply

### For next stable Nootka version 2.2 and up
  - migrate on preferred 48000 sample rate (ogg files and audio methods). DO NOT forget to resize output samples length
  - MIDI input & revert MIDI out of course
  - storing custom tuning with user name. Import/export tuning in XML files
  - add rests option for random melodies in exams
  - implement repeating a melody that was heard
  - adding set of rhythm patterns for melodies generated from random notes (similar to melody list)
  - time stamps in question/answer units - to better track exam progress
  - music XML import dialog, to select voice or staff if there are more, and so on
  - read/save compressed music XML (*.mxl)
  - adding actually edited score melody to current level
  - editing melody from currently selected/composed level
  - add score helpers: transposition, folding ties
  - add ukulele support.
  - add violin support and revert pitch accuracy check.
    It has less usage for currently supported instruments, but for violin can be perfect
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
 - during adding note to score one by one it doesn't play occasionally
 - when exercise becomes exam tips are blended
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
# @@@@ IDEAS FOR FURTHER RELEASES @@@@
***

### SMALL NEW FEATURES/IMPROVEMENTS
  - add minimal zoom value depends on screen resolution
  - average effectiveness line in chart
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


