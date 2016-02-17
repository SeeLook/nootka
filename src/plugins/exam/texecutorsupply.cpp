/***************************************************************************
 *   Copyright (C) 2012-2016 by Tomasz Bojczuk                             *
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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "texecutorsupply.h"
#include "tequalrand.h"
#include <exam/texam.h>
#include <exam/tlevel.h>
#include <exam/tattempt.h>
#include <music/ttune.h>
#include <music/tmelody.h>
#include <tinitcorelib.h>
#include <tscoreparams.h>
#include <widgets/tintonationview.h>
#include <gui/tstatuslabel.h>
#if defined (Q_OS_ANDROID)
  #include <ttouchmessage.h>
#else
  #include <QMouseEvent>
  #include <iostream>
#endif
#include <QtWidgets/qmessagebox.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qdebug.h>



#if !defined (Q_OS_ANDROID)
void debugMelody(Tmelody* mel) {
	for (int i = 0; i < mel->length(); ++i)
		std::cout << mel->note(i)->p().toText().toStdString() << "\t";
	std::cout << "\n";
}


void debugNotesStruct(QList<TnoteStruct>& st) {
	Tmelody mel;
	mel.fromNoteStruct(st);
	debugMelody(&mel);
}
#endif

/*static*/

bool TexecutorSupply::m_paramsMessage = false;
bool TexecutorSupply::m_playCorrections = true;


void TexecutorSupply::checkPlayCorrected(Tlevel* level) {
	m_playCorrections = true;
	if (level->instrument == e_noInstrument) {
		if (level->answerIsSound())
			if (Tcore::gl()->instrument != e_noInstrument)
				if (level->inScaleOf(Tcore::gl()->loString().chromatic(), Tcore::gl()->hiNote().chromatic()))
					m_playCorrections = false;
	} else
			m_playCorrections = false;
}


void TexecutorSupply::checkGuitarParamsChanged(Texam* exam) {
	checkPlayCorrected(exam->level());
	QString changesMessage;
	if (exam->level()->instrument != e_noInstrument) { // when instrument is guitar it has a matter
			if (exam->level()->instrument != Tcore::gl()->instrument)
					changesMessage = tr("Instrument type was changed!");
			Tcore::gl()->instrument = exam->level()->instrument;
	} // otherwise it reminds unchanged
	if ((exam->level()->canBeGuitar() || exam->level()->canBeSound()) && !m_playCorrections &&
		exam->tune() != *Tcore::gl()->Gtune() ) { // Is tune the same?
			if (!changesMessage.isEmpty())
						changesMessage += QLatin1String("<br>");
			Ttune tmpTune = exam->tune();
			Tcore::gl()->setTune(tmpTune);
			changesMessage += tr("Tuning of the guitar was changed to:") + QLatin1String(" <b> ") + Tcore::gl()->Gtune()->name + QLatin1String("!</b>");
	}
	if (exam->level()->canBeGuitar() && exam->level()->hiFret > Tcore::gl()->GfretsNumber) { // Are enough frets?
			if (!changesMessage.isEmpty())
					changesMessage += QLatin1String("<br>");
			changesMessage += tr("Guitar fret number was changed!");
			Tcore::gl()->GfretsNumber = exam->level()->hiFret;
	}
	if (!changesMessage.isEmpty()) {
#if defined (Q_OS_ANDROID)
      tMessage->setMessage(changesMessage, 0);
#else
      QColor c = Qt::red;
      c.setAlpha(50);
      STATUS->setBackground(c);
      STATUS->setMessage(changesMessage);
#endif
			m_paramsMessage = true;
	} else
			m_paramsMessage = false;
}


QColor& TexecutorSupply::answerColor(const TQAunit* answer) {
	return answerColor(answer->mistake());
}


QColor& TexecutorSupply::answerColor(quint32 mistake) {
	if (mistake == (quint32)TQAunit::e_correct)
    return Tcore::gl()->EanswerColor;
  else if (!(mistake & TQAunit::e_wrongPos) && !(mistake & TQAunit::e_wrongNote) && !(mistake & TQAunit::e_veryPoor))
    return Tcore::gl()->EnotBadColor;
  else
    return Tcore::gl()->EquestionColor;
}


/* end of static */

TexecutorSupply::TexecutorSupply(Tlevel* level, QObject* parent) :
  QObject(parent),
  m_level(level),
  m_prevAccid(Tnote::e_Natural),
  m_isSolfege(false),
  m_dblAccidsCntr(0),
  m_eisCesCntr(0),
  m_wasFinished(false)
{
	m_loFret = m_level->loFret;
	m_hiFret = m_level->hiFret;
  calcQAPossibleCount();
	checkPlayCorrected(level);
	if (m_level->useKeySign && !m_level->isSingleKey)
		m_randKey = new TequalRand(m_level->hiKey.value() - m_level->loKey.value() + 1, m_level->loKey.value());
	else
		m_randKey = 0;
}


TexecutorSupply::~TexecutorSupply()
{
	if (m_randKey)
		delete m_randKey;
}


//##########################################################################################
//#######################     METHODS       ################################################
//##########################################################################################


void TexecutorSupply::createQuestionsList(QList<TQAgroup> &list) {
	char openStr[6];
//       for (int i = 0; i < 6; i++)
	for (int i = 0; i < Tcore::gl()->Gtune()->stringNr(); i++)
			openStr[i] = Tcore::gl()->Gtune()->str(i + 1).chromatic();
		
		/** FIXING MISTAKE RELATED WITH A NEW VALIDATION WAY DURING SAVING NEW LEVEL 
			* When there is no guitar in a level,
			* add to question list only the lowest position sounds. 
			* In this way question list contains proper number of questions. */
	if (!m_level->canBeGuitar() && !m_level->answerIsSound())  // adjust fret range
		m_level->onlyLowPos = true;

	if (!m_playCorrections || m_level->instrument != e_noInstrument || m_level->showStrNr || m_level->canBeGuitar()) {
// 		qDebug() << "Question list created fret by fret. Tune:" << Tcore::gl()->Gtune()->name << Tcore::gl()->Gtune()->stringNr();
		if (m_level->instrument == e_noInstrument && Tcore::gl()->instrument != e_noInstrument) {
// 			if (Tnote(Tcore::gl()->hiString().chromatic() + m_hiFret).chromatic() < m_level->hiNote.chromatic())
// 					m_hiFret = m_level->hiNote.chromatic() - Tcore::gl()->hiString().chromatic();
// 			if (Tnote(Tcore::gl()->loString().chromatic() + m_loFret).chromatic() > m_level->loNote.chromatic())
// 				m_loFret = Tcore::gl()->loString().chromatic() - m_level->loNote.chromatic();
			char hi = m_hiFret, lo = m_loFret;
			if (!m_level->adjustFretsToScale(lo, hi))
					qDebug() << "Cant adjust fret range. Corrections will be played!";
			m_loFret = lo; 
			m_hiFret = hi;
		}
		if (m_level->loFret != m_loFret || m_level->hiFret != m_hiFret)
				qDebug() << "Fret range of a level adjusted to current instrument [" << m_loFret << m_hiFret << "]";
		for(int s = 0; s < Tcore::gl()->Gtune()->stringNr(); s++) {
				if (m_level->usedStrings[Tcore::gl()->strOrder(s)])// check string by strOrder
						for (int f = m_loFret; f <= m_hiFret; f++) {
								Tnote n = Tnote(Tcore::gl()->Gtune()->str(Tcore::gl()->strOrder(s) + 1).chromatic() + f);
							if (n.chromatic() >= m_level->loNote.chromatic() &&
										n.chromatic() <= m_level->hiNote.chromatic()) {
								bool hope = true; // we still have hope that note is proper for the level
								if (m_level->onlyLowPos) {
									if (s > 0) {
											// we have to check when note is on the lowest positions.
											// Is it really lowest position when strOrder[s] is 0 - it is the highest sting
											char diff = openStr[Tcore::gl()->strOrder(s-1)] - openStr[Tcore::gl()->strOrder(s)];
											if( (f - diff) >= m_loFret && (f - diff) <= m_hiFret)
													hope = false; //There is the same note on highest string
											else
													hope = true;
									}
								}
								if (hope && m_level->useKeySign && m_level->onlyCurrKey)
									hope = isNoteInKey(n);
								if (hope) {
										if (n.alter && (!m_level->withFlats && !m_level->withSharps))
												continue;
										else {
											TfingerPos ff = TfingerPos(Tcore::gl()->strOrder(s) + 1, f);
											addToList(list, n, ff);
										}
								}
						}
				}
		}
	} else {
// 		qDebug() << "Question list created note by note";
		for (int nNr = m_level->loNote.chromatic(); nNr <= m_level->hiNote.chromatic(); nNr++) {
			Tnote n = Tnote(nNr);
			bool hope = true; // we still have hope that note is proper for the level
			if (hope && m_level->useKeySign && m_level->onlyCurrKey)
					hope = isNoteInKey(n);
			if (hope) {
				if (n.alter && (!m_level->withFlats && !m_level->withSharps))
						continue;
				else {
						TfingerPos ff = TfingerPos();
						addToList(list, n, ff);
				}
		}
		}
	}	
	
//    for (int i = 0; i < list.size(); i++)
//        qDebug() << i << (int)list[i].pos.str() << "f"
//                << (int)list[i].pos.fret() << " note: "
//                << QString::fromStdString(list[i].note.getName());

  // generate m_fretFretList if needed
    m_fretFretList.clear();
    if (m_level->questionAs.isFret() && m_level->answersAs[TQAtype::e_asFretPos].isFret()) {
      QList<TfingerPos> tmpSameList;
      for (int i = 0; i < list.size(); i++) {
        tmpSameList.clear();
        getTheSamePos(list[i].pos, tmpSameList);
          if (!tmpSameList.isEmpty())
            m_fretFretList << (quint16)i;
      }
//       for (int i = 0; i < m_fretFretList.size(); i++)
//        qDebug() << m_fretFretList.at(i) << (int)list[m_fretFretList.at(i)].pos.str() << "f"
//                << (int)list[m_fretFretList.at(i)].pos.fret() << " note: "
//                << QString::fromStdString(list[m_fretFretList.at(i)].note.getName());
//       qDebug() << "question list for only guitar created\nnumber:" << m_fretFretList.size() <<
//           " among:" << list.size();
    }
    
    qsrand(QDateTime::currentDateTime().toTime_t());
    
		if (m_level->canBeMelody())
			m_obligQuestNr = qBound(5, 250 / m_level->melodyLen, 30); // longer melody - less questions
		else
			m_obligQuestNr = qBound(20, list.size() * 4, 250);
    if (m_level->useKeySign && !m_level->isSingleKey)
        m_obligQuestNr = qMax(m_obligQuestNr, (m_level->hiKey.value() - m_level->loKey.value() + 1) * 5);
    m_obligQuestNr = qMax(qaPossibilities() * 4, m_obligQuestNr);
}


Tnote TexecutorSupply::determineAccid(Tnote n) {
    Tnote nA = n;
    bool notFound = true;
    if (m_level->withSharps || m_level->withFlats || m_level->withDblAcc) {
        if (m_level->withDblAcc) {
            m_dblAccidsCntr++;
            if (m_dblAccidsCntr == 4) { //double accid note occurs every 4-th question
                if ( (qrand() % 2) ) // randomize dblSharp or dblFlat
                    nA = n.showWithDoubleSharp();
                else
                    nA = n.showWithDoubleFlat();
                if (nA == n) // dbl accids are not possible
                    m_dblAccidsCntr--;
                else {
                    m_dblAccidsCntr = 0;
                    notFound = false;
                }
            }
        }
        if (notFound && m_prevAccid != Tnote::e_Flat && m_level->withFlats) {
          if ((n.note == 3 || n.note == 7) && n.alter == 0 ) { // increase counter for f and c notes
            m_eisCesCntr++;
            if (m_eisCesCntr == 3) { // fes or ces can occur every 3 e or b occurences
              m_eisCesCntr = 0;
              nA = n.showWithFlat();
              notFound = false;
            }
          } else { // other notes can be with flat allways
            nA = n.showWithFlat();
            notFound = false;
          }
        }
        if (notFound && m_prevAccid != Tnote::e_Sharp && m_level->withSharps) {
          if ((n.note == 4 || n.note == 1) && n.alter == 0) { // increase counter for f and c notes
            m_eisCesCntr++;
            if (m_eisCesCntr == 3) { // eis or bis can occur every 3 f or c occurences
              nA = n.showWithSharp();
              m_eisCesCntr = 0;
            }
          } else { // other notes can be with sharp allways
            nA = n.showWithSharp();
          }
        }
    }
    m_prevAccid = (Tnote::Ealter)nA.alter;
    return nA;
}


Tnote TexecutorSupply::forceEnharmAccid(Tnote n) {
    Tnote nX;
    char acc = m_prevAccid;
     for (int i = 0; i < 6; i++) { // to be sure that all cases are checked
        acc++;
        if (acc > 2) acc = -2;

        if (acc == Tnote::e_DoubleFlat && m_level->withDblAcc)
            nX = n.showWithDoubleFlat();
        if (acc == Tnote::e_Flat && m_level->withFlats)
            nX = n.showWithFlat();
        if (acc == Tnote::e_Natural)
            nX = n.showAsNatural();
        if (acc == Tnote::e_Sharp && m_level->withSharps)
            nX = n.showWithSharp();
        if (acc == Tnote::e_DoubleSharp && m_level->withDblAcc)
            nX = n.showWithDoubleSharp();
        if (nX.note && n != nX) break;
     }
    m_prevAccid = (Tnote::Ealter)acc;
    if (nX.note)
        return nX;
    else return n;
}


Tnote::EnameStyle TexecutorSupply::randomNameStyle(int style) {
    if (style != -1) {
        if ((Tnote::EnameStyle)style == Tnote::e_italiano_Si || (Tnote::EnameStyle)style == Tnote::e_russian_Ci)
            m_isSolfege = true;
        else
            m_isSolfege = false;
		}
    if (m_isSolfege) {
        m_isSolfege = false;
        if (qrand() % 2) { // full name like cis, gisis
            if (Tcore::gl()->S->seventhIs_B)
                return Tnote::e_nederl_Bis;
            else
                return Tnote::e_deutsch_His;
        } else { // name and sign like c#, gx
            if (Tcore::gl()->S->seventhIs_B)
                return Tnote::e_english_Bb;
            else
                return Tnote::e_norsk_Hb;
        }
    } else {
        m_isSolfege = true;
        return Tnote::e_italiano_Si;
    }
}


void TexecutorSupply::getTheSamePos(TfingerPos& fingerPos, QList<TfingerPos>& posList, bool strCheck, bool order) {
  int chStr = Tcore::gl()->Gtune()->str(strNr(fingerPos.str() - 1, order) + 1).chromatic();
  for (int i = 0; i < Tcore::gl()->Gtune()->stringNr(); i++)
    if (i != strNr(fingerPos.str() - 1, order)) { 
      if (strCheck && !m_level->usedStrings[i])
          continue; // skip unavailable strings when strCheck is true
      int fret = chStr + fingerPos.fret() - Tcore::gl()->Gtune()->str(strNr(i, order) + 1).chromatic();
      if (fret >= m_level->loFret && fret <= m_level->hiFret) {
        posList << TfingerPos(strNr(i, order) + 1, fret);
      }
    }
}


void TexecutorSupply::getTheSamePosNoOrder(TfingerPos& fingerPos, QList<TfingerPos>& posList, bool strCheck) {
	getTheSamePos(fingerPos, posList, strCheck, false);
}


void TexecutorSupply::calcQAPossibleCount() {
  m_qaPossib = 0;
  if (m_level->questionAs.isNote()) {
    if (m_level->answersAs[TQAtype::e_asNote].isNote())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asNote].isName())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asNote].isFret())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asNote].isSound())
      m_qaPossib++;
  }
  if (m_level->questionAs.isName()) {
    if (m_level->answersAs[TQAtype::e_asName].isNote())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asName].isName())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asName].isFret())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asName].isSound())
      m_qaPossib++;
  }
  if (m_level->questionAs.isFret()) {
    if (m_level->answersAs[TQAtype::e_asFretPos].isNote())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asFretPos].isName())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asFretPos].isFret())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asFretPos].isSound())
      m_qaPossib++;
  }
  if (m_level->questionAs.isSound()) {
    if (m_level->answersAs[TQAtype::e_asSound].isNote())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asSound].isName())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asSound].isFret())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asSound].isSound())
      m_qaPossib++;
  }
}


void TexecutorSupply::checkNotes(TQAunit* curQ, Tnote& expectedNote, Tnote& userNote, bool reqOctave, bool reqAccid) {
	Tnote exN = expectedNote, retN = userNote;
	if (retN.note) {
		Tnote nE = exN.showAsNatural();
		Tnote nR = retN.showAsNatural();
		if (exN != retN) {
			if (reqOctave) {
					if (nE.note == nR.note && nE.alter == nR.alter) {
							if (nE.octave != nR.octave)
								curQ->setMistake(TQAunit::e_wrongOctave);
					} else {
							curQ->setMistake(TQAunit::e_wrongNote);
					}
			}
			if (!curQ->wrongNote()) { // There is still something to check
				if (exN.note != retN.note || exN.alter != retN.alter) {// if they are equal it means that only octaves were wrong
						exN = exN.showAsNatural();
						retN = retN.showAsNatural();
						if (reqAccid) {
								if (exN.note == retN.note && exN.alter == retN.alter)
										curQ->setMistake(TQAunit::e_wrongAccid);
								else
										curQ->setMistake(TQAunit::e_wrongNote);
						} else {
								if (exN.note != retN.note || exN.alter != retN.alter)
									curQ->setMistake(TQAunit::e_wrongNote);
						}
				}
			}
		}
	} else
			curQ->setMistake(TQAunit::e_wrongNote);
}


void TexecutorSupply::compareMelodies(Tmelody* q, Tmelody* a, Tattempt* att) {
	int notesCount = qMax(q->length(), a->length());
	for (int i = 0; i < notesCount; ++i) {
		TQAunit tmpUnit;
		if (a->length() > i && q->length() > i)
			checkNotes(&tmpUnit, q->note(i)->p(), a->note(i)->p(), m_level->requireOctave, m_level->forceAccids);
		else
			tmpUnit.setMistake(TQAunit::e_wrongNote);
		att->add(tmpUnit.mistake()); // times are ignored in that type of answer/attempt
	}
	att->updateEffectiveness();
}


void TexecutorSupply::compareMelodies(Tmelody* q, QList<TnoteStruct>& a, Tattempt* att) {
// 	debugMelody(q);
// 	debugNotesStruct(a);
	int notesCount = qMax(q->length(), a.size());
	for (int i = 0; i < notesCount; ++i) {
		TQAunit tmpUnit;
		if (a.size() > i && q->length() > i) {
			checkNotes(&tmpUnit, q->note(i)->p(), a[i].pitch, m_level->requireOctave, m_level->forceAccids);
			if (!tmpUnit.isWrong() && m_level->intonation != TintonationView::e_noCheck) {
				if (!TnoteStruct(Tnote(), a[i].pitchF).inTune(TintonationView::getThreshold(m_level->intonation)))
					tmpUnit.setMistake(TQAunit::e_wrongIntonation);
			}
		} else
				tmpUnit.setMistake(TQAunit::e_wrongNote);
		att->add(tmpUnit.mistake());
	}
	att->updateEffectiveness();
}


TkeySignature TexecutorSupply::getKey(Tnote& note) {
  Tnote tmpNote = note;
  TkeySignature key; // C-major by default
  if (m_level->isSingleKey) { //for single key
    key = m_level->loKey;
      if (m_level->onlyCurrKey) {
          tmpNote = m_level->loKey.inKey(note);
          if (!tmpNote.isValid())
            qDebug() << "There is no" << tmpNote.toText() << "in level with single key:" << m_level->loKey.getName() <<
                  "It should never happened!";
      }
  } else { // for many key signatures
      if (m_randKey)
          key = TkeySignature(m_randKey->get());
      if (m_level->onlyCurrKey && !m_level->canBeMelody()) { // if note is in current key only
          int keyRangeWidth = m_level->hiKey.value() - m_level->loKey.value();
          int patience = 0; // we are looking for suitable key
          char keyOff = key.value() - m_level->loKey.value();
          tmpNote = key.inKey(note);
          while(tmpNote.note == 0 && patience <= keyRangeWidth) {
              keyOff++;
              if (keyOff > keyRangeWidth) 
                keyOff = 0;
              key = TkeySignature(m_level->loKey.value() + keyOff);
              patience++;
              tmpNote = key.inKey(note);
              if (patience > keyRangeWidth) {
                  qDebug() << "Oops! It should never happened. Can not find key signature for" << note.toText();
                  break;
              }
          }
      }
  }
  note = tmpNote;
return key;
}


void TexecutorSupply::resetKeyRandom() {
  if (m_randKey)
    m_randKey->reset();
}



//##########################################################################################
//#######################     PRIVATE        ###############################################
//##########################################################################################

bool TexecutorSupply::isNoteInKey(Tnote& n) {
	if (m_level->isSingleKey) {
		if(m_level->loKey.inKey(n).note != 0)
				return true;
		} else {
				for (int k = m_level->loKey.value(); k <= m_level->hiKey.value(); k++) {
					if (TkeySignature::inKey(TkeySignature(k), n).note != 0)
						return true;
				}
		}
	return false;
}


void TexecutorSupply::addToList(QList<TQAgroup>& list, Tnote& n, TfingerPos& f) {
		TQAgroup g;
		g.note = n; 
		g.pos = f;
		list << g;
}


quint8 TexecutorSupply::strNr(quint8 str0to6, bool ordered) {
	if (ordered)
		return Tcore::gl()->strOrder((char)str0to6);
	else
		return str0to6;
}

//##########################################################################################
//#######################     EVENTS        ################################################
//##########################################################################################

#if !defined (Q_OS_ANDROID)
bool TexecutorSupply::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() == QEvent::MouseButtonPress) {
    QMouseEvent *me = static_cast<QMouseEvent *>(event);
    if (me->button() == Qt::RightButton) {
        emit rightButtonClicked();
        return true;
    }
  }
  return QObject::eventFilter(obj, event);
}
#endif




