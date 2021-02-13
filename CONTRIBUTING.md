
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
    - **Porting update checker to QML**  

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

    - **Read/save compressed music XML files(*.mxl)**
       [Here are specs](https://www.musicxml.com/tutorial/compressed-mxl-files/).
       Seems like it requires to use external library for zip/unzip dir structures and [Zlib](https://www.zlib.net) seems to be good choose for that, but if there is simpler way - just to compile simple code into the app - it is more appreciated.

    - **Pitch detection code needs modernization or migration to external library**
  
  * **Help with CI/CD integration**  

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

-----------------------------------
## 5. Writing/recording tutorials <a name="tutorials"></a>




