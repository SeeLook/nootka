# Nootka
### ...to play scores#

***

! ! ! ! ! ! ! ! ! TODO LIST ! ! ! ! ! ! ! ! !
============================================================================
============================================================================

### For next stable Nootka version 2.0.2 or above
  - music XML import dialog, to select voice or staff if there are more, and so on
    - fill partial measure with rests at the beginning
  - improve ukulele
  - clean code if objections will not occur:
    - when right mouse button is not used: TexamExacutor and TexecutorSupply
  - score helpers also utilized for musicxml import:
    - fold (simplify) ties & rests

### ...and much more in the post versions
  - **MIDI** input & revert MIDI out of course
  - migrate on preferred **48000 Hz** sample rate (ogg files and audio methods). DO NOT forget to resize output samples length
  - storing **custom tuning** named by user. Import/export tuning in XML files
  - **download particular instrument samples** file on demand - to save installers size
  - add **rests** option for random melodies in exams
  - **solfege** as a kind of an instrument with bunch of ear training exercises
  - playing with metronome under Android - possible migration to **Oboe** library
  - implement repeating a melody that was listened to
  - adding set of **rhythm patterns** for melodies generated from random notes (similar to melody list)
  - **time stamps** in question/answer units - to better track exam progress
  - adding actually edited score melody to current level
  - editing melody from currently selected/composed level
  - add **violin** support and revert **pitch accuracy** check.
    It has less usage for currently supported instruments, but for violin can be perfect
  - support for triplets
  - support for **pickup (anacrusis)** measures
  - exams control melody tempo, either fixed value from melody or random from declared range (or increased every question)..
    so far, user can play whatever tempo he likes
  - playing melodies in charts and single notes as well
  - option for **randomizing timbre** (instrument sound) in every dictation melody
  - add QML control for displaying single note using Bravura texts/glyphs system (to use in chart and question tips)
  - add **Italian names** of tempo to tempo menu
  - read-only **levels - password protected**, when teacher doesn't want user to see melodies inside


============================================================================

### small glitches:
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



