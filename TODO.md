#Nootka
###...to play scores#

***

# ! ! ! ! ! ! ! ! ! TODO LIST ! ! ! ! ! ! ! ! !

============================================================================

### For incoming stable release version 2.0
  - fix detecting last note duration in exercises/exams
  - add an option to skip rests (pitch detection gaps) and treat their duration as previous note was longer
  - manual adding/editing ties
  - finish tuner, functionality similar to Android one (show tuning, middle A freq)
  - level creator:
    - some pages make no sense for particular cases (i.e. for single note melody & rhythm page, etc.)
    - so implement disabling unused pages, colorize panel icons properly for that
    - question page icon might change depends on is there single note or a melody
    - those three ways of generating melodies (range, notes, list) might be exposed more clearly
      especially list of melodies imported from files which seems to be very important feature
  - music XML import dialog, to select voice or staff if there are more, and so
  - exam summary - give more valuable info, wear it nicely
  - charts suck. For exams with many questions it wastes a lot of RAM. Make them QML
  - migrate on preferred 48000 sample rate (ogg files and audio methods)

### For next stable Nootka version 2.2
  - MIDI input & revert MIDI out of course
  - adding actually edited score melody to current level
  - editing melody from currently selected/composed level
  - switch for selecting imported melodies from the list one by one or in random order
  - add violin support and revert pitch accuracy check.
    It has small usage for currently supported instruments, but for violin can be perfect
  - support for triplets
  - exams control melody tempo, either fixed value from melody or random from declared range (or increased every question)
  - viewing entire melody in stats, playing them and single notes as well
    so far, user can play whatever tempo he likes


============================================================================

### small glitches:
 - melody won't play in exercises/exams (first question OK but further is worst)
 - sometimes setting a note doesn't scale tie - probably due to note width is not refreshed
 - meter can be too close to key signature - probably related with clef change
 - chart tip for non guitars still displays string/fret with goofy values

### and crashes:
 - when level is saved and added to list, it crashes when it is selected and exercise is going to be started
 - sometimes app crash on exit - possible some note objects are note deleted in proper order

### Translation context changes:
 - TscoreKeySignature to KeySignature

============================================================================

## MISSING IN ACTION (comparing to previous major version) - do we really need them?
 - left hand guitar (do we really need this?)
 - question mark and note name over instrument during exam/exercises (remember about actual name style)
 - no intonation gradients

 - revert exam times (if we really need that):  
    next question delay  
    correction preview

***
***
# @@@@ IDEAS FOR FURTHER RELEASES @@@@
***

### SMALL NEW FEATURES/IMPROVEMENTS
  - NotaData of Tartini might be cleaned
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

  - unlock left-handed guitar then implement frets zoom to respect transforms


#### TEXAMEXECUTOR
  - think about hide octave in note name format when octaves are not required in answers.  
     Also for questions in "hint" - don't show octave when only name is required 


============================================================================
#### To consideration:
  - additional settings options [Android]:
      - disable screen locking (TRUE by default)
      - full screen mode (TRUE by default)
      - home button actions
  - In wizard: option for single note mode for very beginners


