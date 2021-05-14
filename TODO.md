# Nootka
### ...to play scores#

***

! ! ! ! ! ! ! ! ! TODO LIST ! ! ! ! ! ! ! ! !
============================================================================
============================================================================

### Bugs to fix:
  - Actions after displaying certificate are messy
  - level validation routines are optimized for guitars mostly, troubles are there
  - *play scores* level has description about guitar. Divide that text when all translations will be ready and adjust for instruments apparently

### To test:
  - melodies with ties and multiple rests (either playing or writing)
  - single note exams: playing note by note (times between questions)
  - no rhythms melodies - wait for correct note option
  - no rhythms melodies - mixed with other melodies
  - levels with melodies to be played and written in one

### For incoming stable release version 2.0
  - update links to online documentation (update doc in first place)
  - clean correction code, key signature anim maybe

### For next stable Nootka version 2.0.2 or above
  - music XML import dialog, to select voice or staff if there are more, and so on
  - ukulele support
  - increase versions of *.nel *.noo files to avoid problems with previous Nootka versions
  - read/save compressed music XML (*.mxl)
  - clean code if objections will not occur:
    - when right mouse button is not used: TexamExacutor and TexecutorSupply
  - score helpers also utilized for musicxml import:
    - transpose by semitones (intervals when implemented)  
    - fold (simplify) ties & rests
  - bring back option for displaying extra (remaining) accidentals

### ...and much more in the post versions
  - MIDI input & revert MIDI out of course
  - migrate on preferred 48000 sample rate (ogg files and audio methods). DO NOT forget to resize output samples length
  - storing custom tuning named by user. Import/export tuning in XML files
  - download particular instrument samples file on demand - to save installers size
  - add rests option for random melodies in exams
  - solfege as a kind of an instrument with banch of ear training exercises
  - playing with metronome under Android - possible migration to Oboe library
  - implement repeating a melody that was listened to
  - adding set of rhythm patterns for melodies generated from random notes (similar to melody list)
  - time stamps in question/answer units - to better track exam progress
  - adding actually edited score melody to current level
  - editing melody from currently selected/composed level
  - add score helpers: transposition, folding ties
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
 - sometimes setting a note doesn't scale tie - probably due to note width is not refreshed
 - space key for tapping tempo doesn't work well
 - TexamExacutor checkAnswerAct hint -> remove %1

### Translation context changes:
 - TscoreKeySignature to KeySignature

============================================================================

## MISSING IN ACTION (comparing to previous major version) - do we really need them?
 - left hand guitar (do we really need this?)
 - question mark during exam/exercises
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


