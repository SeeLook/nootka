
# Contributing to Nootka

Any help is more than appreciated and not only on the programming realm.  
And it could be just single, small thing or a few more or for a while or for a bit longer...  
Below some possibilities of a help are explained but if something other will come up - do not hesitate, just write.



1. [Developing Nootka code](#develop)

2. [Translating Nootka](https://www.opencode.net/seelook/nootka/blob/master/lang/how-to-translate.md)

3. [Recording audio samples](#record)

4. [Instrument specific help](#instrument)

5. [Writing/recording tutorials](#tutorials)


-----------------------------------
## 1. Developing Nootka code <a name="develop"></a>
  There is [TODO aka Road map](https://www.opencode.net/seelook/nootka/blob/master/TODO.md) list with some ideas what is going on now and what is planed in the next releases.

  First of all - fork git repository of Nootka project. Send any changes through pull request, please, but not as a patch file.  
  There are things which have to be eventually done but they don't require much integration with main Nootka code during development - in other words they can be done quite in parallel. But other things better to discus before start.  
  For visual part Nootka uses Qt `QML` language a top of `C++` logic with all `QObject` entanglements, but there are parts of code in `C++` only. So programming tasks can be divided by technology (language) to use.

  Here are some tasks waiting for their queue:

  * **Running diagnostic tests**  
    If You have any experience in testing QML/Qt/C++ application code then running some diagnostic tools on it might help to find potential weaknesses even without knowing what Nootka/code really does.

  * **Using QML - Qt Meta Language**  

    - **Triplet support in QML Score editor**  
      Seems like detecting note duration works well, so it can bear more tiny divisions like triplets. This is planed for next stable Nootka release (like 2.2).

    - **Score in general**  
      Even if Nootka is not an editing-score-application the musical score functionality is very important. The code of it is a mix of QML and C++ but it might utilize QML declarativeness much more to make code simpler and easier for further developing.

  * **Using C++ with Qt**
    - **Downloading audio file on demand**  
      Nootka package size ballooned recently due to files with audio samples. And it will grow even more when samples will get better quality.
      But there is no need to keep them all in distributed package (Android *.apk especially) when given user will utilize just one file with samples according to selected instrument.
      So the file could be downloaded from remote location after the instrument was selected.
      This way we might even get rid of ogg dependencies and downloading raw/wav audio samples - 
      bigger size but one file and fetched just once - stored locally,
      so next application updates will use it as well.
      Of course one, fallback file (piano) shipped with the app.

    - **Midi in/out support**  
      Playing (midi output) and rhythm detection (input) can be done with `C++` only - [RtMidi](https://www.music.mcgill.ca/~gary/rtmidi/) library preferred. However, at the top of that, integration with Nootka requires Qt stuff. There is some [old code](https://www.opencode.net/seelook/nootka/blob/master/src/libs/sound/tmidiout.h) but for playing only and without rhythms support.


  * **Using pure C++ or just C**  
    - **Algorithm for generating random melodies**  
        With configuration for rhythm complexity, intervals to use and so.  

    - **Comparing melodies function**  
      In spite of this algorithm has to compare melodies it is only data comparison.  
      A melody is expressed as a sequence of two numbers: (pitch, duration). One melody/sequence is Nootka generated (displayed or played) and the other melody is what user was played or written. It is easy to compare when all matches exactly or some notes don't. But how to determine similarity when user notes are just shifted towards Nootka notes.

    - **[Android] Utilize 'Oboe' sound library**  
       Nootka needs low level audio to combine audio samples with metronome ticks or even to change sample pitch in real time.
       Duplex callback is necessary under Android to implement metronome ticking during pitch detection and [Oboe C++ library](https://github.com/google/oboe) is what might help to achieve that.

    - **Pitch detection code needs modernization or migration to external library**

  * **Help with CI/CD integration**  
    - **Mac Os builds using GitHub Actions**

-----------------------------------
## 2. Translating Nootka

  Take a look at [separate article](https://www.opencode.net/seelook/nootka/blob/master/lang/how-to-translate.md)

-----------------------------------
## 3. Recording audio samples (urgent!) <a name="record"></a>
  Nootka is going to use real audio sounds and those have to be recorded in some special way. But for now, only classical and electric guitar samples are recorded that way. Unfortunately the rest of sounds are converted from midi.  
  Just take a look at (to be more precise: listen to) [any sound file (*.ogg)](https://www.opencode.net/seelook/nootka/tree/master/sounds) and that *special way of recording* should be revealed itself.
  Then consider to record such a thing. Tempo is exactly 120 bpm, note duration takes three beats (1.5 sec) then one beet rest follows (0.5 sec) - exactly 2 seconds per each note through entire chromatic scale of an instrument.
  Of course it has to be played with a metronome in an ear, but a metronome device or an app is not precise enough.
  Better play with this prepared [metronome backing track](https://www.opencode.net/seelook/nootka/raw/master/spare_parts/scales/metronome-for-Nootka-sound-samples.ogg) - it also can be used further to adjust recorded sample file with it.

-----------------------------------
## 4. Instrument specific help <a name="instrument"></a>
  For now, saxophones are abandoned completely.  
  Someone who plays any kind of saxophone would help with advices and testing the app.

  Ukulele support will come in next stable release.

-----------------------------------
## 5. Writing/recording tutorials <a name="tutorials"></a>
  On the Nootka web site there are a few help articles:  
  [https://nootka.sourceforge.io/index.php/help/#getting-started](https://nootka.sourceforge.io/index.php/help/#getting-started).  
  But images are obsolete (from old version).

  Also it could be more:

  - **interesting use cases**  

  - **video tutorials in different native languages**  

  - **more helping articles**  
    If you know already something what was difficult in Nootka at the beginning - write about it or just give us the hint.




