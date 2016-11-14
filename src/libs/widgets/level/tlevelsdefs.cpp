/***************************************************************************
 *   Copyright (C) 2014-2016 by Tomasz Bojczuk                             *
 *   tomaszbojczuk@gmail.com                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tlevelsdefs.h"
#include <exam/tlevel.h>
#include <music/ttune.h>
#include <tinitcorelib.h>
#include <QtWidgets/qapplication.h>


void getExampleLevels(QList<Tlevel>& llist) {
  Tlevel l = Tlevel();
  int octaveOffset = 0; // depends on guitar type and for bass drops range octave down
  if (Tcore::gl()->instrument == e_bassGuitar)
    octaveOffset = -1;
  else if (Tcore::gl()->instrument == e_noInstrument)
    octaveOffset = 1;
  //----------------------------------------------------------------------------
  l.name = QObject::tr("open strings");
  l.desc = QObject::tr("The simplest. No key signatures, no double accidentals and no sound.<br>Automatically adjusted to current tune.");
  l.questionAs.setAsSound(false);
  l.answersAs[0] = TQAtype(false, true, true, false); // q: score -- a: name, guitar
  l.answersAs[1] = TQAtype(true, false, true, false); // q: name -- a: score, guitar
  l.answersAs[2] = TQAtype(true, true, false, false); // q: guitar -- a: score, name
  l.answersAs[3] = TQAtype(false, false, false, false);
  l.withSharps = false;
  l.withFlats = false;
  l.withDblAcc = false;
  l.useKeySign = false;
  l.loKey = 0;
  l.hiKey = 0;
  l.forceAccids = false;
  l.requireOctave = false;
  l.requireStyle = false;
  l.showStrNr = false;
  l.intonation = 0; // do not check
  //clef default, user/tune defined
  //instrument default - selected by user
  l.hiNote = Tnote(Tcore::gl()->hiString().chromatic());
  //loNote is lowest by constructor
  l.hiFret = 0;// loFret is 0 by constuctor
  for (int i = 1; i < 7; i++) { //accids will be used if current tune requires it
    if (Tcore::gl()->Gtune()->str(i).alter == 1)
      l.withSharps = true;
    if (Tcore::gl()->Gtune()->str(i).alter == -1)
      l.withFlats = true;
  }
  if (l.instrument == e_noInstrument) // force instrument when not defined
    l.instrument = e_classicalGuitar;
  l.melodyLen = 1;
  //     l.requireInTempo = false;
  //     l.endsOnTonic = fasle;
  llist << l;
  //----------------------------------------------------------------------------
  l.name = QObject::tr("C-major scale");
  l.desc = QObject::tr("In first position. No accidentals, no sound.<br>Octave required.");
  l.questionAs.setAsSound(false);
  bool isGuitar = true;
  if (Tcore::gl()->instrument == e_noInstrument) {
    isGuitar = false;
    l.desc = QObject::tr("Give note name in C-major scale or show note on the staff knowing its name.");
  }
  l.instrument = Tcore::gl()->instrument;
  l.questionAs.setAsFret(isGuitar);
  l.answersAs[0] = TQAtype(false, true, isGuitar, false);
  l.answersAs[1] = TQAtype(true, false, isGuitar, false);
  l.answersAs[2] = TQAtype(isGuitar, isGuitar, false, false);
  l.answersAs[3] = TQAtype(false, false, false, false);
  l.withSharps = false;
  l.withFlats = false;
  l.withDblAcc = false;
  l.useKeySign = false;
  l.manualKey = false;
  l.loKey = 0;
  l.hiKey = 0;
  l.forceAccids = false;
  l.requireOctave = true;
  l.requireStyle = false;
  l.showStrNr = false;
  l.loNote = Tnote(1, 0 + octaveOffset, 0);
  l.hiNote = Tnote(1, 1 + octaveOffset, 0);
  l.hiFret = 3; // loFret is 0 by constructor
  l.intonation = 0; // do not check
  llist << l;
  //----------------------------------------------------------------------------
  l.name = QObject::tr("All to V fret");
  l.desc = QObject::tr("All chromatic notes till V-th fret, no sound.<br>Without double accidentals. Key signatures: C, G, D, A");
  l.questionAs.setAsSound(false);
  l.answersAs[0] = TQAtype(false, true, true, false);
  l.answersAs[1] = TQAtype(true, false, true, false);
  l.answersAs[3] = TQAtype(false, false, false, false);
  l.withSharps = true;
  l.withFlats = true;
  l.withDblAcc = false;
  l.useKeySign = true;
  l.manualKey = true;
  l.loKey = 0;
  l.hiKey = 3;
  l.forceAccids = true;
  l.requireOctave = true;
  l.requireStyle = false;
  l.showStrNr = true;
  l.loNote = Tcore::gl()->loString();
  l.hiNote = Tnote(Tcore::gl()->hiString().chromatic() + 5);
  l.hiFret = 5;// loFret is 0 by constructor
  l.intonation = 0; // do not check
  if (Tcore::gl()->instrument == e_noInstrument) // force instrument when not defined
    l.instrument = e_classicalGuitar;
  l.melodyLen = 1;
  llist << l;
  //----------------------------------------------------------------------------
  if (Tcore::gl()->loNote().chromatic() <= Tnote(5, 0).chromatic()) {
    // This level makes sense only when lowest note instrument is lowest or equal than g small note
    l = Tlevel();
    l.name = QApplication::translate("Tlevel", "Grand staff");
    l.desc = QApplication::translate("Tlevel", "Guess notes from grand staff in different keys");
    l.clef = Tclef(Tclef::e_pianoStaff);
    l.instrument = Tcore::gl()->instrument;
    l.questionAs.setAsSound(false);
    l.questionAs.setAsFret(isGuitar);
    l.answersAs[0] = TQAtype(false, true, isGuitar, false);
    l.answersAs[1] = TQAtype(true, false, false, false);
    l.answersAs[2] = TQAtype(isGuitar, false, false, false);
    l.answersAs[3] = TQAtype(false, false, false, false);
    l.withSharps = true;
    l.withFlats = true;
    l.withDblAcc = false;
    l.useKeySign = true;
    l.manualKey = true;
    l.loKey = -3;
    l.hiKey = 3;
    l.forceAccids = true;
    l.requireOctave = true;
    l.requireStyle = false;
    l.showStrNr = isGuitar;
    if (isGuitar) { // till 12 fret on guitar
      if (l.hiFret >= 12) { // adjust highest note to 12th fret
        l.hiNote = Tnote(Tcore::gl()->hiString().chromatic() + 12);
        l.hiFret = 12;
      }
    } else { // for non guitar whole scale
      // set scale to C great -> a2
      Tnote C_great(1, -1);
      if (Tcore::gl()->loNote().chromatic() > C_great.chromatic()) { // lowest note is higher
        if (Tcore::gl()->loNote().note > 1) { // if lowest isn't c note
          if (Tcore::gl()->loNote().octave <= -1) // check is near c note lays on bottom staff
            l.loNote = Tnote(1, Tcore::gl()->loNote().octave + 1); // and start from next c note in instrument scale
        }
        // or the lowest level note is the same like lowest scale one
      } else
        l.loNote = C_great;
      Tnote a_2(6, 2);
      if (Tcore::gl()->hiNote().chromatic() > a_2.chromatic()) // if highest note is high than a2
        l.hiNote = a_2; // set end to a2
      // or keep highest note the same as highest in the scale
    }
    l.intonation = 0; // do not check
    l.melodyLen = 1;
    llist << l;
  }
  //----------------------------------------------------------------------------
  if (Tclef::defaultType != Tclef::e_bass_F && Tclef::defaultType != Tclef::e_bass_F_8down
      && Tcore::gl()->loNote().chromatic() < Tnote(1, 1).chromatic()) {
    // makes no sense if lowest note is above c 1 (i.e. flute)
    l = Tlevel();
    l.name = QApplication::translate("Tlevel", "Bass clef");
    l.desc = QApplication::translate("Tlevel", "Play a short and simple melody in bass clef");
    l.clef = Tclef(Tclef::e_bass_F);
    l.questionAs.setAsFret(false); // no guitar
    l.questionAs.setAsName(false); // no names
    l.questionAs.setAsSound(false); // don't play
    l.answersAs[0] = TQAtype(false, false, false, true); // score only
    l.answersAs[1] = TQAtype(false, false, false,false);
    l.answersAs[2] = TQAtype(false, false, false,false);
    l.answersAs[3] = TQAtype(false, false, false,false);
    l.withSharps = false;
    l.withFlats = false;
    l.withDblAcc = false;
    l.useKeySign = false;
    l.manualKey = false;
    l.onlyCurrKey = true;
    l.endsOnTonic = true;
    l.loKey = 0;
    l.hiKey = 0;
  //  l.instrument = Tcore::gl()->instrument;
    l.forceAccids = false;
    l.requireOctave = true;
    l.requireStyle = false;
    l.showStrNr = false;
    l.loNote = Tcore::gl()->loNote();
    if (isGuitar) {
      if (l.hiFret >= 12) { // adjust highest note to 12th fret
        l.hiNote = Tnote(Tcore::gl()->hiString().chromatic() + 5);
        l.hiFret = 5;
      }
    } else { // find note 3 octaves up from lowest one or keep highest one ot the instrument
      Tnote up3octaves(l.loNote.note, l.loNote.octave + 3, l.loNote.alter);
      if (up3octaves.chromatic() < l.hiNote.chromatic())
        l.hiNote = up3octaves;
    }
    //     l.hiFret by constructor
    //     l.intonation = Tcore::gl()->A->intonation; // user preferences (in constructor)
    l.onlyLowPos = true;
    l.melodyLen = 7;
    llist << l;
  }
  //----------------------------------------------------------------------------
  l = Tlevel();
  l.name = QObject::tr("Ear training");
  l.desc = QObject::tr("Listen to a sound and show it on the staff.<br>Guitar, note names and key signatures are not used.<br>Scale a - a<sup>2</sup>.");
  l.questionAs.setAsFret(false); // no guitar
  l.questionAs.setAsName(false); // no names
  l.questionAs.setAsNote(false); // score only as an question
  l.answersAs[0] = TQAtype(false, false, false, false);
  l.answersAs[1] = TQAtype(false, false, false, false);
  l.answersAs[2] = TQAtype(false, false, false, false);
  l.answersAs[3] = TQAtype(true, false, false, false); // score only
  l.withSharps = true;
  l.withFlats = true;
  l.withDblAcc = false;
  l.useKeySign = false;
  l.manualKey = false;
  l.loKey = 0;
  l.hiKey = 0;
  l.forceAccids = false;
  l.requireOctave = true;
  l.requireStyle = false;
  l.showStrNr = false;
  l.clef = Tclef(Tclef::e_treble_G);
  l.instrument = e_noInstrument;
  l.intonation = 0; // do not check
  l.loNote = Tnote(6, 0); // a
  l.hiNote = Tnote(6, 2); // a2
  l.hiFret = 19;// loFret is 0 by constructor
  l.melodyLen = 10;
  llist << l;
  //----------------------------------------------------------------------------
  if (Tcore::gl()->instrument == e_classicalGuitar || Tcore::gl()->instrument == e_electricGuitar) {
    l = Tlevel();
    l.name = QObject::tr("Melody on trebles");
    l.desc = QObject::tr("Play short melody on treble strings with notes just on first three frets. Sharps keys only.");
    l.questionAs.setAsFret(false); // no guitar
    l.questionAs.setAsName(false); // no names
    l.questionAs.setAsSound(false); // don't play
    l.answersAs[0] = TQAtype(false, false, false, true); // score only
    l.answersAs[1] = TQAtype(false, false, false,false);
    l.answersAs[2] = TQAtype(false, false, false,false);
    l.answersAs[3] = TQAtype(false, false, false,false);
    l.withSharps = true;
    l.withFlats = false;
    l.withDblAcc = false;
    l.useKeySign = true;
    l.manualKey = false;
    l.loKey = 0;
    l.hiKey = 3;
    l.forceAccids = false;
    l.requireOctave = true;
    l.requireStyle = false;
    l.showStrNr = false;
    //clef default, user/tune defined
    //instrument default - selected by user
    l.loNote = Tcore::gl()->Gtune()->str(3);
    l.hiNote = Tnote(Tcore::gl()->hiString().chromatic() + 3);
    l.hiFret = 3;
    //     l.intonation = Tcore::gl()->A->intonation; // user preferences (in constructor)
    l.onlyLowPos = true;
    //     if (Tcore::gl()->instrument == e_noInstrument) // force instrument when not defined
    //       l.instrument = Tcore::gl()->instrument;
    l.melodyLen = 5;
    l.endsOnTonic = true;
    l.onlyCurrKey = true;
    llist << l;
  }
  //----------------------------------------------------------------------------
  l = Tlevel();
  l.name = QObject::tr("Play scores");
  l.desc = QObject::tr("Take your instrument and just play...<br>No note names, no fretboard. Keys till 4 accids and double accids enabled! Scale of whole guitar without positions.");
  l.questionAs.setAsFret(false); // no guitar
  l.questionAs.setAsName(false); // no names
  l.questionAs.setAsSound(false); // don't play
  l.answersAs[0] = TQAtype(false, false, false, true); // score only
  l.answersAs[1] = TQAtype(false, false, false,false);
  l.answersAs[2] = TQAtype(false, false, false,false);
  l.answersAs[3] = TQAtype(false, false, false,false);
  l.withSharps = true;
  l.withFlats = true;
  l.withDblAcc = true;
  l.useKeySign = true;
  l.manualKey = false;
  l.loKey = -4;
  l.hiKey = 4;
  l.forceAccids = false;
  l.requireOctave = true;
  l.requireStyle = false;
  l.showStrNr = false;
  //clef default, user/tune defined
  //instrument default - selected by user
  //     l.loNote & l.hiNote from constructor
  //     l.hiFret by constructor
  //     l.intonation = Tcore::gl()->A->intonation; // user preferences (in constructor)
  l.onlyLowPos = true;
  //     if (Tcore::gl()->instrument == e_noInstrument) // force instrument when not defined
  //       l.instrument = Tcore::gl()->instrument;
  l.melodyLen = 10;
  llist << l;
  //----------------------------------------------------------------------------
  l = Tlevel();
  l.name = QObject::tr("Sing scores");
  l.desc = QObject::tr("Just sing a score...<br>No note names, no fretboard, no keys and double accids. Scale doesn't matter because octaves are ignored - you can transpose.");
  l.questionAs.setAsFret(false); // no guitar
  l.questionAs.setAsName(false); // no names
  l.questionAs.setAsSound(false); // don't play
  l.answersAs[0] = TQAtype(false, false, false, true); // score only
  l.answersAs[1] = TQAtype(false, false, false,false);
  l.answersAs[2] = TQAtype(false, false, false,false);
  l.answersAs[3] = TQAtype(false, false, false,false);
  l.withSharps = true;
  l.withFlats = true;
  l.withDblAcc = false;
  l.useKeySign = false;
  l.manualKey = false;
  l.clef = Tclef(Tclef::e_treble_G);
  l.loKey = 0;
  l.hiKey = 0;
  l.forceAccids = false;
  l.requireOctave = false;
  l.requireStyle = false;
  l.showStrNr = false;
  l.loNote = Tnote(5, 0); // G contra
  l.hiNote = Tnote(6, 2); // a one-line
  //     l.hiFret by constructor
  //     l.intonation = Tcore::gl()->A->intonation; // user preferences (in constructor)
  l.instrument = e_noInstrument;
  l.onlyLowPos = true;
  l.melodyLen = 5;
  llist << l;
  //----------------------------------------------------------------------------
  l = Tlevel();
  l.name = QApplication::translate("Tlevel", "Play grand staff");
  l.desc = QApplication::translate("Tlevel", "Play random melodies from two staves of piano notation. Keys up to four accidentals.");
  l.questionAs.setAsFret(false); // no guitar
  l.questionAs.setAsName(false); // no names
  l.questionAs.setAsSound(false); // don't play
  l.answersAs[0] = TQAtype(false, false, false, true); // score only
  l.answersAs[1] = TQAtype(false, false, false,false);
  l.answersAs[2] = TQAtype(false, false, false,false);
  l.answersAs[3] = TQAtype(false, false, false,false);
  l.withSharps = true;
  l.withFlats = true;
  l.withDblAcc = false;
  l.useKeySign = true;
  l.manualKey = false;
  l.onlyCurrKey = true;
  l.endsOnTonic = true;
  l.loKey = -4;
  l.hiKey = 4;
  l.clef = Tclef(Tclef::e_pianoStaff);
//  l.instrument = Tcore::gl()->instrument;
  l.forceAccids = false;
  l.requireOctave = true;
  l.requireStyle = false;
  l.showStrNr = isGuitar;
  l.loNote = Tcore::gl()->loNote();
  if (isGuitar) {
    if (l.hiFret >= 12) { // adjust highest note to 12th fret
      l.hiNote = Tnote(Tcore::gl()->hiString().chromatic() + 12);
      l.hiFret = 12;
    }
  } else { // find note 4 octaves up from lowest one or keep highest one ot the instrument
    Tnote up4octaves(l.loNote.note, l.loNote.octave + 4, l.loNote.alter);
    if (up4octaves.chromatic() < l.hiNote.chromatic())
      l.hiNote = up4octaves;
  }
  //     l.hiFret by constructor
  //     l.intonation = Tcore::gl()->A->intonation; // user preferences (in constructor)
  l.onlyLowPos = false;
  l.melodyLen = 15;
  llist << l;
//----------------------------------------------------------------------------
  l = Tlevel();
  l.name = QObject::tr("Pentatonic");
  l.desc = QObject::tr("Play melody composed from pentatonic scale, every time in different key signature.");
  l.questionAs.setAsFret(false); // no guitar
  l.questionAs.setAsName(false); // no names
  l.questionAs.setAsSound(false); // don't play
  l.answersAs[0] = TQAtype(false, false, false, true); // score only
  l.answersAs[1] = TQAtype(false, false, false,false);
  l.answersAs[2] = TQAtype(false, false, false,false);
  l.answersAs[3] = TQAtype(false, false, false,false);
  l.withSharps = true;
  l.withFlats = true;
  l.withDblAcc = false;
  l.useKeySign = true;
  l.manualKey = false;
  l.loKey = -1; // F-major
  l.hiKey = 4; // E-major
  l.forceAccids = false;
  l.requireOctave = true;
  l.requireStyle = false;
  l.showStrNr = false;
  //clef default, user/tune defined
  //instrument default - selected by user
  l.loNote = Tcore::gl()->loNote();
  l.hiNote = Tcore::gl()->hiNote();
  // l.hiFret = ; user defined
  //     l.intonation = Tcore::gl()->A->intonation; // user preferences (in constructor)
  l.onlyLowPos = true;
  //     if (Tcore::gl()->instrument == e_noInstrument) // force instrument when not defined
  //       l.instrument = Tcore::gl()->instrument;
  l.melodyLen = 10;
  l.endsOnTonic = false;
  l.onlyCurrKey = true;
  l.randMelody = Tlevel::e_randFromList;
//   l.keyOfrandList; // C-major by default
  quint8 pentaArr[5] = {1, 2, 3, 5, 6};
  qint8 firstOctave = l.loNote.octave + (l.loNote.note == 1 ? 0 : 1); // increase octave if instr. scale doesn't start from note C
  for (int n = 0; n < 10; ++n) {
    l.notesList << Tnote(pentaArr[n % 5], firstOctave, 0);
    if (n == 4)
      firstOctave++;
  }
  llist << l;
//----------------------------------------------------------------------------
/*  if (Tcore::gl()->instrument == e_classicalGuitar || Tcore::gl()->instrument == e_electricGuitar) {
    l = Tlevel();
    l.name = QObject::tr("Melody and open bass strings");
    l.desc = QObject::tr("");
    l.questionAs.setAsFret(false); // no guitar
    l.questionAs.setAsName(false); // no names
    l.questionAs.setAsSound(false); // don't play
    l.answersAs[0] = TQAtype(false, false, false, true); // score only
    l.answersAs[1] = TQAtype(false, false, false,false);
    l.answersAs[2] = TQAtype(false, false, false,false);
    l.answersAs[3] = TQAtype(false, false, false,false);
    l.withSharps = true;
    l.withFlats = false;
    l.withDblAcc = false;
    l.useKeySign = true;
    l.manualKey = false;
    l.loKey = 0;
    l.hiKey = 3;
    l.forceAccids = false;
    l.requireOctave = true;
    l.requireStyle = false;
    l.showStrNr = false;
    //clef default, user/tune defined
    //instrument default - selected by user
    l.loNote = Tcore::gl()->Gtune()->str(3);
    l.hiNote = Tnote(Tcore::gl()->hiString().chromatic() + 3);
    l.hiFret = 3;
    //     l.intonation = Tcore::gl()->A->intonation; // user preferences (in constructor)
    l.onlyLowPos = true;
    //     if (Tcore::gl()->instrument == e_noInstrument) // force instrument when not defined
    //       l.instrument = Tcore::gl()->instrument;
    l.melodyLen = 5;
    l.endsOnTonic = true;
    l.onlyCurrKey = true;
    llist << l;
  }*/
}
