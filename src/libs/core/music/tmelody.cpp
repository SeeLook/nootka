/***************************************************************************
 *   Copyright (C) 2014-2021 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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

#include "tmelody.h"
#include "tnotestruct.h"
#include "tclef.h"
#include "tmeter.h"
#include "tchunk.h"
#include "nootkaconfig.h"
#include "timportscore.h"

#include "minizip/tzip.h"

#include <QtCore/qvariant.h>
#include <QtCore/qdebug.h>
#include <QtCore/qfile.h>
#include <QtCore/qdatetime.h>


/* local static */

/**
 * Prints warning message and sets given clef reference to current default clef type.
 */
void unsupportedClef(Tclef::EclefType& clefType) {
  if (!IMPORT_SCORE) // Do not warn during importing score
    qDebug() << "[Tmelody] Unsupported clef. Set to default" << Tclef(Tclef::defaultType).name();
  clefType = Tclef::defaultType;
}

/**
 * Stores technical note data obtained from <words> tag (text).
 * In such a cases it occurs before <note> tag
 */
static Ttechnical technical;

/**
 * Display warning only once
 */
static bool appendWarn = true;
/*******************************************************************************************/


Tmelody::Tmelody(const QString& title, const TkeySignature& k) :
  m_title(title),
  m_tempo(120),
  m_key(k),
  m_meter(new Tmeter),
  m_clef(Tclef::defaultType)
{
  appendWarn = true;
}


Tmelody::Tmelody(const Tmelody& other)
{
  appendWarn = true;
  m_title = other.title();
  m_composer = other.composer();
  m_tempo = other.tempo();
  m_clef = other.clef();
  m_key = other.key();
  m_meter = new Tmeter;
  setMeter(other.meter()->meter());
  m_beat = other.beat();
  for (int n = 0; n < other.length(); ++n)
    addNote(other.chunk(n));
}



Tmelody::~Tmelody()
{
  delete m_meter;
}

//####################################################################################################
//########################################## PUBLIC ##################################################
//####################################################################################################

void Tmelody::setMeter(int m) {
  m_meter->setMeter(static_cast<Tmeter::Emeter>(m));
}


/**
 * IT DOESN'T CHECK NOTES DURATION!!!!
 * all notes added to melody have to be nicely divided to fit every measure duration exactly.
 */
void Tmelody::addNote(const Tchunk& n) {
  if (p_measures.isEmpty() || lastMeasure().isFull())
    p_measures << Tmeasure(p_measures.count() + 1, m_meter->meter());

  lastMeasure().addNote(n);
  m_notes << &lastMeasure().lastNote();
}


void Tmelody::clear(bool withCredits, bool withKey) {
  m_notes.clear();
  p_measures.clear();
  if (withCredits) {
    m_title.clear();
    m_composer.clear();
  }
  if (withKey)
    m_key = TkeySignature();
}


void Tmelody::toList(QList<Tchunk>& chunks) {
  for (auto m : p_measures) {
    for (int n = 0; n < m.count(); ++n)
      chunks << m.note(n);
  }
}


void Tmelody::swapWithNotes(int noteNr, const QList<Tchunk>& notes) {
  int notesCnt = 0;
  Tmeasure* barToSwapIn = nullptr;
  for (Tmeasure& m : p_measures) {
    notesCnt += m.count();
    if (notesCnt > noteNr) {
      barToSwapIn = &m;
      break;
    }
  }
  int noteIdInBar = noteNr - (notesCnt - barToSwapIn->count());
  if (barToSwapIn)
    barToSwapIn->swapWithNotes(noteIdInBar, notes);
  for (int n = 1; n < notes.count(); ++n) {
    m_notes.insert(noteNr + n, &barToSwapIn->note(noteIdInBar + n));
  }
}


void Tmelody::toXml(QXmlStreamWriter& xml, int trans) {
  for (int m = 0; m < p_measures.size(); ++m) {
    xml.writeStartElement(QStringLiteral("measure"));
      Tmeasure& bar = measure(m);
      xml.writeAttribute(QStringLiteral("number"), QVariant(bar.number()).toString());
      if (bar.number() == 1) {
        xml.writeStartElement(QStringLiteral("attributes"));
          xml.writeTextElement(QStringLiteral("divisions"), QString("%1").arg(Trhythm(Trhythm::Quarter).duration()));
          if (m_key.value() || m_key.isMinor())
            m_key.toXml(xml);
          m_meter->toXml(xml);
          if (m_clef == Tclef::PianoStaffClefs)
            xml.writeTextElement(QStringLiteral("staves"), QStringLiteral("2"));
          Tclef(m_clef).toXml(xml);
          if (trans) {
            xml.writeStartElement(QStringLiteral("transpose"));
              xml.writeTextElement(QStringLiteral("chromatic"), QString::number(trans % 12));
              xml.writeTextElement(QStringLiteral("octave-change"), QString::number(trans / 12));
            xml.writeEndElement(); // transpose
          }
        xml.writeEndElement(); // attributes
        xml.writeStartElement(QStringLiteral("direction"));
          xml.writeAttribute(QStringLiteral("placement"), QStringLiteral("above"));
          xml.writeStartElement(QStringLiteral("direction-type"));
            xml.writeStartElement(QStringLiteral("metronome"));
              QString beatUnitString = QStringLiteral("quarter");
              if (beat() == Tmeter::BeatEighth)
                beatUnitString = QStringLiteral("eighth");
              else if (beat() == Tmeter::BeatHalf)
                beatUnitString = QStringLiteral("half");
              xml.writeTextElement(QStringLiteral("beat-unit"), beatUnitString);
              if (beat() == Tmeter::BeatQuarterDot)
                xml.writeEmptyElement(QStringLiteral("beat-unit-dot"));
              xml.writeTextElement(QStringLiteral("per-minute"), QString::number(tempo()));
            xml.writeEndElement(); // metronome
          xml.writeEndElement(); // direction-type
        xml.writeEndElement(); // direction
      }
      int staffNr_1 = 1, staffNr_2 = 2;
      int *staffPtr = nullptr;
      for (int n = 0; n < bar.count(); ++n) {
        if (m_clef == Tclef::PianoStaffClefs) {
          if (bar.note(n).p().onUpperStaff())
            staffPtr = &staffNr_1;
          else
            staffPtr = &staffNr_2;
        }
        bar.note(n).toXml(xml, staffPtr);
      }
    xml.writeEndElement(); // measure
  }
}


bool Tmelody::fromXml(QXmlStreamReader& xml, bool madeWithNootka, int partId) {
  bool ok = true;
  int prevTie = -1; // -1 -> no tie, otherwise this number points to previous note that had a tie
  m_notes.clear();
  p_measures.clear();
  m_meter->setMeter(Tmeter::NoMeter);
  setTempo(0); // reset tempo, try to read from XML
  int barNr = 0;
  QStringList clefSuppList;
  int fixTransposition = 0;
  while (xml.readNextStartElement()) {
/** [measure] */
    if (xml.name() == QLatin1String("measure")) {
      int tmpBarNr = xml.attributes().value(QStringLiteral("number")).toInt();
      barNr++;
      if (tmpBarNr != barNr) {
        qDebug() << "[Tmelody] Something wrong with measure numbers!" << barNr << "was expected, but" << tmpBarNr << "was read.\n"
                 << "Better check integrity of this music XML file!";
      }
      while (xml.readNextStartElement()) {
/** [attributes] */
        if (xml.name() == QLatin1String("attributes")) {
            Tclef::EclefType clef1 = Tclef::NoClef, clef2 = Tclef::NoClef;
            bool clefChanged = false;
            int staffCnt = 1;
            while (xml.readNextStartElement()) {
              if (xml.name() == QLatin1String("staves")) {
                  staffCnt = xml.readElementText().toInt();
                  if (staffCnt > 2) {
                    qDebug() << "[Tmelody] Read from more staves is unsupported";
                    staffCnt = 2;
                  }
              } else if (xml.name() == QLatin1String("clef")) {
                  Tclef cl;
                  QString unsuppClefTxt;
                  cl.fromXml(xml, IMPORT_SCORE ? &unsuppClefTxt : nullptr);
                  clefSuppList << unsuppClefTxt;
                  Tclef::EclefType tmpClef = cl.type();
                  if (tmpClef == Tclef::NoClef)
                    unsupportedClef(tmpClef);
                  if (clef1 == Tclef::NoClef) // detecting piano staff
                    clef1 = tmpClef;
                  else if (clef2 == Tclef::NoClef)
                    clef2 = tmpClef;
                  if (barNr > 1)
                    clefChanged = true;
/** [key signature] */
              } else if (xml.name() == QLatin1String("key")) {
                  if (barNr == 1)
                      m_key.fromXml(xml);
                  else {
                      if (IMPORT_SCORE) {
                          TkeySignature newKey;
                          newKey.fromXml(xml);
                          if (newKey.value() != m_key.value())
                            IMPORT_SCORE->keyChanged(newKey);
                      } else {
                          xml.skipCurrentElement();
                          qDebug() << "[Tmelody] Change key signature in the middle of a melody is not supported!";
                      }
                  }
/** [meter (time signature)] */
              } else if (xml.name() == QLatin1String("time")) {
                  if (barNr == 1)
                      m_meter->fromXml(xml);
                  else {
                      if (IMPORT_SCORE) {
                          Tmeter newMeter;
                          newMeter.fromXml(xml);
                          if (newMeter.meter() != m_meter->meter())
                            IMPORT_SCORE->meterChanged(newMeter);
                      } else {
                          xml.skipCurrentElement();
                          qDebug() << "[Tmelody] Change time signature (meter) in the middle of a melody is not supported!";
                      }
                  }
              } else
                  xml.skipCurrentElement();
            }
            if (barNr == 1) {
                if (staffCnt == 2) {
                    if (clef1 == Tclef::Treble_G && clef2 == Tclef::Bass_F)
                      m_clef = Tclef::PianoStaffClefs;
                    else
                      unsupportedClef(m_clef);
                } else
                    m_clef = clef1;
            } else {
                if (clefChanged) {
                  // WARNING: clef change (split staff) will work for all clefs except piano staff
                  if (IMPORT_SCORE)
                    IMPORT_SCORE->clefChanged(clef1);
                  else
                    qDebug() << "[Tmelody] Change clef in the middle of a melody is not supported!";
                }
            }
/** [note] */
        } else if (xml.name() == QLatin1String("note")) {
            int staffNr = 0, voiceNr = 0;
            int *staffPtr = nullptr, *voicePtr = nullptr;
            if (m_clef == Tclef::PianoStaffClefs || IMPORT_SCORE)
              staffPtr = &staffNr;
            if (!madeWithNootka && IMPORT_SCORE)
              voicePtr = &voiceNr;
            Tchunk ch;
            Tchunk* dblDotCh = nullptr;
            auto chunkOk = ch.fromXml(xml, staffPtr, voicePtr);
            if ((!madeWithNootka && IMPORT_SCORE) || !(chunkOk & Tchunk::e_xmlUnsupported)) {
              if (ch.p().isRest() && ch.p().rhythm() == Trhythm::NoRhythm) { // Fix rest duration - if it is undefined - means entire measure
                ch.p().setRhythm(m_meter->duration()); // it will reset 'rest' attribute
                ch.p().setRest(true);
              }
              if (!(staffPtr && !ch.p().isValid() && ch.p().rhythm() == Trhythm::NoRhythm)) {
                // Nootka is not able to import from grand staff of real score (XML)
                // and above condition avoids it, but allows to import piano staves created by Nootka itself

                if (m_clef == Tclef::PianoStaffClefs)
                  ch.p().setOnUpperStaff(staffNr < 2);
                if (prevTie > -1) {
                  // check and fix tie, Nootka supports them only between the same notes
                  // scoring app may set tie between different ones, but seems like in such case there is no 'stop' tag used (musescore)
                  Tnote& prevNote = m_notes[prevTie]->p();
                  short prevChromatic = prevNote.chromatic(), currChromatic = ch.p().chromatic();
                  if ((prevChromatic == currChromatic && ch.p().rtm.tie() == Trhythm::e_noTie) || prevChromatic != currChromatic) {
                    if (prevNote.rtm.tie() == Trhythm::e_tieCont)
                      prevNote.rtm.setTie(Trhythm::e_tieEnd);
                    else if (prevNote.rtm.tie() == Trhythm::e_tieStart)
                      prevNote.rtm.setTie(Trhythm::e_noTie);
                  }
                }
                prevTie = ch.p().rtm.tie() ? m_notes.count() : -1;
                if (!technical.isEmpty()) {
                  // technical data was read before note, so approve it now. But data from <technical> have priority
                  if (!ch.bowing() && technical.bowing())
                    ch.setBowing(technical.bowing());
                  if (ch.finger() == -1 && technical.finger() != -1)
                    ch.setFinger(technical.finger());
                  technical.reset(); // reset for the next note
                }
                if (m_meter->meter() == Tmeter::NoMeter && ch.p().rtm.isValid())
                  ch.p().setRhythm(Trhythm::NoRhythm);
                bool dblDot = false, tieCont = false;
                if (ch.p().rtm.isValid() && chunkOk & Tchunk::e_xmlHasTwoDots) {
                  tieCont = ch.p().rtm.tie() == Trhythm::e_tieStart || ch.p().rtm.tie() == Trhythm::e_tieCont;
                  ch.p().rtm.setTie(ch.p().rtm.tie() > Trhythm::e_tieStart ? Trhythm::e_tieCont : Trhythm::e_tieStart);
                  dblDot = true;
                }
                addNote(ch);
                if (dblDot) {
                  // XML import disallows eights with double dots, so divide rhythmic value easily
                  Trhythm r(static_cast<Trhythm::Erhythm>(ch.p().rhythm() + 2));
                  r.setTie(tieCont ? Trhythm::e_tieCont : Trhythm::e_tieEnd);
                  dblDotCh = new Tchunk(Tnote(ch.p(), r), ch.t());
                  addNote(*dblDotCh);
                }
              }
            }
            if (!madeWithNootka && IMPORT_SCORE) {
              // NOTE: This is safe as long as it occurs only during import.
              // Old Nootka versions used Dropped bass clef (levels, exams)
              // but TlevelSelector, Texam handle that - converts all level stuff into ordinary bass clef
              if (m_clef == Tclef::Bass_F_8down) {
                fixTransposition = 12;
                m_clef = Tclef::Bass_F;
              }
              if (fixTransposition)
                ch.p().transpose(fixTransposition);
              bool skip = !clefSuppList[staffNr - 1].isEmpty();
              if (chunkOk & Tchunk::e_xmlIsChord) {
                  if (!skip) {
                    IMPORT_SCORE->addChordNote(ch);
                    // TODO: double dots note
                  }
              } else if (chunkOk & Tchunk::e_xmlIsGrace) {
                // TODO: grace note if any - so far skipping
              } else {
                  IMPORT_SCORE->addNote(partId, staffNr, voiceNr, ch, skip);
                  if (dblDotCh && !skip) {
                    if (fixTransposition)
                      dblDotCh->p().transpose(fixTransposition);
                    IMPORT_SCORE->addNote(partId, staffNr, voiceNr, *dblDotCh);
                  }
                  if (chunkOk & Tchunk::e_xmlIsStrangeRtm || chunkOk & Tchunk::e_xmlIsTupletStart|| chunkOk & Tchunk::e_xmlIsTupletStop)
                    IMPORT_SCORE->setUnsupported(partId, staffNr, voiceNr, chunkOk);

              }
            }
            if (dblDotCh)
              delete dblDotCh;
/** [direction] with bowing/bellow and fingering detected from texts below/above note */
        } else if (xml.name() == QLatin1String("direction")) {
            while (xml.readNextStartElement()) {
              if (xml.name() == QLatin1String("direction-type")) {
                  while (xml.readNextStartElement()) {
                    if (xml.name() == QLatin1String("words")) { // fingering and bowing can be read this way
                        auto words = xml.readElementText();
                        if (words == QLatin1String("(A)"))
                            technical.setBowing(Ttechnical::BowDown);
                        else if (words == QLatin1String("(C)"))
                            technical.setBowing(Ttechnical::BowUp);
                        else {
                          bool isNumber = false;
                          int finger = words.toInt(&isNumber);
                          if (isNumber)
                            technical.setFinger(finger);
                        }
                    } else if (xml.name() == QLatin1String("metronome")) {
                        int tempoWillBe = 120;
                        auto beatWillBe = Tmeter::EbeatUnit::BeatQuarter;
                        bool dotWillBe = false;
                        while (xml.readNextStartElement()) {
                          if (xml.name() == QLatin1String("beat-unit")) {
                              auto beatUnit = xml.readElementText();
                              if (beatUnit == QLatin1String("quarter"))
                                beatWillBe = Tmeter::BeatQuarter;
                              else if (beatUnit == QLatin1String("eighth"))
                                beatWillBe = Tmeter::BeatEighth;
                              else if (beatUnit == QLatin1String("half"))
                                beatWillBe = Tmeter::BeatHalf;
                              else
                                qDebug() << "[Tmelody] Unknown 'beat-unit' value. Stay with 'quarter' beat.";
                          } else if (xml.name() == QLatin1String("beat-unit-dot")) {
                              dotWillBe = true;
                              xml.skipCurrentElement();
                          } else if (xml.name() == QLatin1String("per-minute")) {
                              tempoWillBe = xml.readElementText().toInt();
                          } else
                              xml.skipCurrentElement();
                        }
                        if (dotWillBe) {
                          if (beatWillBe == Tmeter::BeatQuarter)
                            beatWillBe = Tmeter::BeatQuarterDot;
                          else
                            qDebug() << "[Tmelody] Metronome beat with dot only supports quarter. Ignore dot then!";
                        }
                        int quarterTempo = Tmeter::quarterTempo(tempoWillBe, beatWillBe);
                        if (barNr == 1 || tempo() == 0) { // read metronome tempo but only for 1st bar or if not yet set
                          if (quarterTempo >= 40 && quarterTempo <= 180) {
                              setTempo(tempoWillBe);
                              setBeat(beatWillBe);
                          } else // too fast or too slow
                              qDebug() << "[Tmelody]" << beatWillBe << "for tempo" << tempoWillBe << "is not supported. (Too fast or too slow)";
                        }
                    } else
                        xml.skipCurrentElement();
                  }
              } else if (xml.name() == QLatin1String("sound")) {
                  int t = xml.attributes().value(QStringLiteral("tempo")).toInt();
                  if (m_tempo == 0) { // always prefer 'metronome' tag, so skip this, if tempo has been set already
                    if (t >= 40 && t <= 180) // but keep tempo in Nootka supported boundaries
                      setTempo(t);
                    else if (t > 180)
                      setTempo(180);
                  }
                  xml.skipCurrentElement();
              } else
                  xml.skipCurrentElement();
            }
        }
        else
            xml.skipCurrentElement();
      }
    } else
        xml.skipCurrentElement();

  }
  if (tempo() == 0) {
    setBeat(m_meter->optimalBeat());
    setTempo(qRound(60.0 * Tmeter::beatTempoFactor(m_beat)));
//     qDebug() << "[Tmelody] Tempo was not read from this melody file. Set it to" << m_tempo << "with beat" << m_beat;
  }
  return ok;
}


bool Tmelody::saveToMusicXml(const QString& xmlFileName, int transposition) {
    // if compressed file then invoke its compression/deflation
    if (xmlFileName.endsWith(QStringLiteral(".mxl")))
       return saveToMXL(xmlFileName, transposition);

    QFile file(xmlFileName);
    if (file.open(QIODevice::WriteOnly)) {
        QXmlStreamWriter xml(&file);
        writeXmlStream(xml, transposition);
        file.close();
        return true;
    } else
        return false;
}


void Tmelody::writeXmlStream(QXmlStreamWriter &xml, int transposition) {
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);
    xml.writeStartDocument();
    xml.writeDTD(QStringLiteral("<!DOCTYPE score-partwise PUBLIC \"-//Recordare//DTD MusicXML 3.1 Partwise//EN\" \"http://www.musicxml.org/dtds/partwise.dtd\">"));
    xml.writeStartElement(QStringLiteral("score-partwise"));
      xml.writeStartElement(QStringLiteral("work"));
        xml.writeTextElement(QStringLiteral("work-title"), title());
      xml.writeEndElement(); // work
      xml.writeStartElement(QStringLiteral("identification"));
        xml.writeStartElement(QStringLiteral("creator"));
          xml.writeAttribute(QStringLiteral("type"), QStringLiteral("composer"));
          xml.writeCharacters(composer());
        xml.writeEndElement(); // creator composer
        xml.writeStartElement(QStringLiteral("encoding"));
          xml.writeTextElement(QStringLiteral("software"), QLatin1String("Nootka ") + QString(NOOTKA_VERSION));
          xml.writeTextElement(QStringLiteral("encoding-date"), QDate::currentDate().toString(Qt::ISODate));
        xml.writeEndElement(); // encoding
      xml.writeEndElement(); // identification
      xml.writeStartElement(QStringLiteral("part-list"));
        xml.writeStartElement(QStringLiteral("score-part"));
          xml.writeAttribute(QStringLiteral("id"), QStringLiteral("P1"));
          xml.writeTextElement(QStringLiteral("part-name"), QStringLiteral("Nootka"));
//           xml.writeTextElement("part-name", "instrument");
        xml.writeEndElement(); // score-part
      xml.writeEndElement(); //part-list
      xml.writeStartElement(QStringLiteral("part"));
        xml.writeAttribute(QStringLiteral("id"), QStringLiteral("P1"));
        toXml(xml, transposition);
      xml.writeEndElement(); // part
    xml.writeEndElement(); // score-partwise
    xml.writeEndDocument();
}


bool Tmelody::saveToMXL(const QString& xmlFileName, int transposition) {
  QByteArray xmlData;
  QXmlStreamWriter xml(&xmlData);
  writeXmlStream(xml, transposition);
  return Tzip::zipMusicXml(xmlFileName, &xmlData);
}


bool Tmelody::grabFromMusicXml(const QString& xmlFileName) {
  QFile file(xmlFileName);
  bool ok = true;

  // if compressed file then invoke its extraction
  if (xmlFileName.endsWith(QStringLiteral(".mxl")))
    return grabFromMXL(xmlFileName);

  if (file.open(QIODevice::ReadOnly)) {
    QXmlStreamReader xml(&file);
    ok = processXMLData(xml);
    file.close();
  };

  return ok;
}


bool Tmelody::grabFromMXL(const QString& xmlFileName) {
  bool ok = true;

  QByteArray xmlData;
  Tzip::getXmlFromZip(xmlFileName, &xmlData);
  if (xmlData.size() > 0) {
      QXmlStreamReader xmlScore(xmlData);
      if (xmlScore.error() != QXmlStreamReader::NoError)
        ok = false;
      else
        ok = processXMLData(xmlScore);
  } else
      ok = false;

  return ok;
}


void Tmelody::appendMelody(Tmelody* otherM) {
  if (!otherM)
    return;
  if (!lastMeasure().isFull() && appendWarn) {
    qDebug() << "[Tmelody] appending melody but the last measure is not finished. Notes in appended measures will be shifted!";
    appendWarn = false;
  }
  for (int n = 0; n < otherM->length(); ++n)
    addNote(*otherM->note(n));
}


void Tmelody::split(int byEveryBar, QList<Tmelody*>& parts) {
  if (m_meter->meter() == Tmeter::NoMeter) { // only one measure containing all bareheaded notes of a melody
      Tmeasure& firstBar = p_measures.first();
      if (firstBar.count() > byEveryBar) {
        for (int n = byEveryBar; n < firstBar.count(); ++n) {
          Tmelody* m;
          if (n % byEveryBar == 0) { // create new Tmelody instance every byEveryBar number
              m = new Tmelody(title(), key());
              m->setMeter(meter()->meter());
              m->setTempo(tempo());
              m->setClef(clef());
              parts << m;
          } else { // or just melody created before
              m = parts.last();
          }
          // then add to it byEveryBar number of notes
          m->addNote(firstBar.note(n));
        }
        // remove notes moved to another melodies from the list
        int nCnt = firstBar.count() - 1;
        for (int n = nCnt; n > byEveryBar - 1; --n) {
          firstBar.removeLastNote();
          m_notes.removeLast();
        }
      }
  } else {
      if (measuresCount() > byEveryBar) {
        auto lastNote = &p_measures[byEveryBar - 1].lastNote();
        if (lastNote->p().rtm.tie()) {
          if (lastNote->p().rtm.tie() == Trhythm::e_tieStart)
            lastNote->p().rtm.setTie(Trhythm::e_noTie);
          else
            lastNote->p().rtm.setTie(Trhythm::e_tieEnd);
        }
        for (int b = byEveryBar; b < measuresCount(); ++b) {
          Tmelody* m;
          if (b % byEveryBar == 0) { // create new Tmelody instance every byEveryBar number
              m = new Tmelody(title(), key());
              m->setMeter(meter()->meter());
              m->setTempo(tempo());
              m->setClef(clef());
              parts << m;
          } else { // or just melody created before
              m = parts.last();
          }
          // then add add to it all notes from the current measure
          Tmeasure& bar = p_measures[b];
          for (int n = 0; n < bar.count(); ++n) {
            Tchunk& ch = bar.note(n);
            if (n == 0) {
                if (ch.p().rtm.tie()) {
                  if (ch.p().rtm.tie() == Trhythm::e_tieEnd)
                    ch.p().rtm.setTie(Trhythm::e_noTie);
                  else
                    ch.p().rtm.setTie(Trhythm::e_tieStart);
                }
            } else if (n == bar.count() - 1) {
                if (ch.p().rtm.tie()) {
                  if (ch.p().rtm.tie() == Trhythm::e_tieStart)
                    ch.p().rtm.setTie(Trhythm::e_noTie);
                  else
                    ch.p().rtm.setTie(Trhythm::e_tieEnd);
              }
            }
            m->addNote(ch);
          }
        }
        // remove notes moved to another melodies from the list
        int l = length();
        bool oneToRemoveFound = false;
        for (int n = 0; n < l; ++n) {
          if (oneToRemoveFound)
            m_notes.removeLast();
          else if (note(n) == lastNote)
            oneToRemoveFound = true;
        }
        // remove moved measures either
        int mc = measuresCount();
        for (int b = byEveryBar; b < mc; ++b)
          p_measures.removeLast();
      }
  }
}


void Tmelody::transpose(int semis, bool outScaleToRest, const Tnote& loNote, const Tnote& hiNote) {
  if (semis == 0 || length() == 0)
    return; // nothing to transpose

  bool doInScaleCheck = loNote.isValid() && hiNote.isValid();
  auto lo = doInScaleCheck ? loNote.chromatic() : 0;
  auto hi = doInScaleCheck ? hiNote.chromatic() : 0;

  for (int n = 0; n < length(); ++n) {
    Tnote& noteSeg = m_notes[n]->p();
    int transOff = 0;
    Trhythm transRtm(noteSeg.rtm);
    auto transChrom = noteSeg.chromatic() + semis;
    if (doInScaleCheck) {
      if (outScaleToRest) {
          if (transChrom > hi || transChrom < lo) {
            transRtm.setRest(true);
            transRtm.setTie(Trhythm::e_noTie);
            transRtm.setBeam(Trhythm::e_noBeam);
          }
      } else {
          if (transChrom > hi)
            transOff = -12; // when too high drop octave down
          else if (transChrom < lo)
            transOff = 12; // when too low raise octave up
      }
    }

    Tnote transposed(noteSeg, transRtm);
    if (transRtm.isRest())
      transposed.setNote(0);
    else
      transposed.transpose(semis + transOff);

    auto inKeyNote = m_key.inKey(transposed);
    if (inKeyNote.isValid()) {
      transposed.setNote(inKeyNote.note());
      transposed.setOctave(inKeyNote.octave());
      transposed.setAlter(inKeyNote.alter());
    }

    noteSeg = transposed;
  }
}



void Tmelody::fromNoteStruct(QList<TnoteStruct>& ns) {
  for (int i = 0; i < ns.size(); ++i)
    addNote(Tchunk(ns[i].pitch));
}

//#################################################################################################
//###################                PROTECTED         ############################################
//#################################################################################################

bool Tmelody::processXMLData(QXmlStreamReader& xml) {
  bool ok = true;
  if (xml.error() != QXmlStreamReader::NoError) {
    qDebug() << "[Tmelody] XML stream error:" << xml.error();
    return false;
  }

  // DTD is ignored, only <score-partwise> key is required as the main
  if (xml.readNextStartElement()) {
    if (xml.name() != QLatin1String("score-partwise")) {
      qDebug() << "[Tmelody] File is not MusicXML format.";
      return false;
    }
  }

  bool madeWithNootka = false;
  int partId = 0;
  while (xml.readNextStartElement()) {
    if (xml.name() == QLatin1String("movement-title")) {
        m_title = xml.readElementText();
    } else if (xml.name() == QLatin1String("work")) {
        while (xml.readNextStartElement()) {
          if (xml.name() == QLatin1String("work-title"))
            m_title = xml.readElementText();
          else
            xml.skipCurrentElement();
        }
    } else if (xml.name() == QLatin1String("identification")) {
        while (xml.readNextStartElement()) {
          if (xml.name() == QLatin1String("creator")) {
              if (xml.attributes().value("type").toString() == QLatin1String("composer"))
                m_composer = xml.readElementText();
              else
                xml.skipCurrentElement();
          } else if (xml.name() == QLatin1String("encoding")) {
              while (xml.readNextStartElement()) {
                if (xml.name() == QLatin1String("software")) {
                    if (xml.readElementText().startsWith(QLatin1String("Nootka")))
                      madeWithNootka = true;
                } else
                    xml.skipCurrentElement();
              }
          } else
              xml.skipCurrentElement();
        }
    } else if (xml.name() == QLatin1String("part-list")) {
        while (xml.readNextStartElement()) {
          if (xml.name() == QLatin1String("score-part")) {
              while (xml.readNextStartElement()) {
                if (xml.name() == QLatin1String("part-name")) {
                    if (IMPORT_SCORE)
                      IMPORT_SCORE->addPartName(xml.readElementText());
                    else
                      xml.skipCurrentElement();
                } else
                    xml.skipCurrentElement();
              }
          } else
              xml.skipCurrentElement();
        }
    } else if (xml.name() == QLatin1String("part")) {
        partId++;
        if (!fromXml(xml, madeWithNootka, partId))
          ok = false;
    } else
        xml.skipCurrentElement();
  }

  if (IMPORT_SCORE)
    IMPORT_SCORE->sumarize();

  return ok;
}



