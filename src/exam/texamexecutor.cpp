/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#include "texamexecutor.h"
#include "tglobals.h"
#include "tstartexamdlg.h"
#include "tquestionaswdg.h"
#include "mainwindow.h"
#include <QDebug>

extern Tglobals *gl;

/*static*/
const qint32 TexamExecutor::examVersion = 0x95121702;

TexamExecutor::TexamExecutor(MainWindow *mainW)
{
    mW = mainW;
    m_examFile = "";

    TstartExamDlg *startDlg = new TstartExamDlg(mW);
    QString resultText;
    TstartExamDlg::Eactions userAct = startDlg->showDialog(resultText, m_level);
    delete startDlg;
    m_glStore.tune = gl->Gtune();
    if (userAct == TstartExamDlg::e_newLevel) {
        m_userName = resultText;
        mW->examResults->startExam();

    } else 
        if (userAct == TstartExamDlg::e_continue) {
            QFile file(resultText);
            quint32 totalTime;
            quint16 questNr, mistNr, tmpAverTime, averTime = 0;
            if (file.open(QIODevice::ReadOnly)) {
                 QDataStream in(&file);
                 in.setVersion(QDataStream::Qt_4_7);
                 quint32 ev; //exam template version
                 in >> ev;
                 if (ev != examVersion) {
                     QMessageBox::critical(mW, "",
                                 tr("File: %1 \n is not valid exam file !!!").arg(file.fileName()));
                     mW->clearAfterExam();
                     return;
                 }
                 in >> m_userName;
                 getLevelFromStream(in, m_level);
                 Ttune tmpTune;
                 in >> tmpTune;
                 in >> totalTime;
                 in >> questNr >> tmpAverTime>> mistNr;
                 while (!in.atEnd()) {
                     TQAunit qaUnit; /** @todo do something with corrupted answers*/
                     getTQAunitFromStream(in, qaUnit);
                     m_answList << qaUnit;
                     averTime += qaUnit.time;
                     /** @todo count mistakes here*/
                 }
                 m_examFile = resultText;
                 if (tmpTune != gl->Gtune() ) {
                     gl->setTune(tmpTune);
                     QMessageBox::critical(mW, "",
			tr("Tune of guitar was changed in this exam !!.<br>Now it is:<br><b>%1</b>").arg(gl->Gtune().name));
                 }
             } else {
                 QMessageBox::critical(mW, "", tr("Cannot open file\n %1 \n for reading\n%2 ").arg(file.fileName()).arg(qPrintable(file.errorString())));
                 /** @todo the same text is in TlevelSelector. make it common */
                 mW->clearAfterExam();
                 return;
             }
            mW->examResults->startExam(totalTime, m_answList.size(), averTime/m_answList.size(), mistNr);
    } else {
        mW->clearAfterExam();
        return;
    }

    prepareToExam();
    createQuestionsList();

    m_isSolfege = false;
    m_prevAccid = Tnote::e_Natural;
    m_dblAccidsCntr = 0;
    m_level.questionAs.randNext(); // Randomize question and answer type
    for (int i = 0; i < 4; i++)
        m_level.answersAs[i].randNext();

    nextQuestAct = new QAction(tr("next question (space)"), this);
    nextQuestAct->setStatusTip(nextQuestAct->text());
    nextQuestAct->setIcon(QIcon(gl->path+"picts/nextQuest.png"));
    nextQuestAct->setShortcut(QKeySequence(Qt::Key_Space));
    connect(nextQuestAct, SIGNAL(triggered()), this, SLOT(askQuestion()));
    mW->nootBar->addAction(nextQuestAct);

    prevQuestAct = new QAction(tr("repeat prevoius question (backspace)"), this);
    prevQuestAct->setStatusTip(prevQuestAct->text());
    prevQuestAct->setIcon(QIcon(gl->path+"picts/prevQuest.png"));
    prevQuestAct->setShortcut(QKeySequence(Qt::Key_Backspace));
    connect(prevQuestAct, SIGNAL(triggered()), this, SLOT(repeatQuestion()));

    checkAct = new QAction(tr("check answer (enter)"), this);
    checkAct->setStatusTip(checkAct->text());
    checkAct->setIcon(QIcon(gl->path+"picts/check.png"));
    checkAct->setShortcut(QKeySequence(Qt::Key_Return));
    // shortcuts
    connect(checkAct, SIGNAL(triggered()), this, SLOT(checkAnswer()));

    if (m_questList.size() == 0) {
        QMessageBox::critical(mW, "", tr("Level <b>%1<b><br>has no sense till there is no any possible question to ask.<br>It can be unadjusted to current tune.<br>Repair it in Level Creator and try again.").arg(m_level.name));
        restoreAfterExam();
        return;
    }

}


void TexamExecutor::createQuestionsList() {
    char openStr[6];
    for (int i=0; i<6; i++)
        openStr[i] = gl->Gtune()[i+1].getChromaticNrOfNote();

// 1. searching all frets in range, string by string
    for(int s = 0; s < 6; s++) {
        if (m_level.usedStrings[gl->strOrder(s)])// check string by strOrder
            for (int f = m_level.loFret; f <= m_level.hiFret; f++) {
                Tnote n = Tnote(gl->Gtune()[gl->strOrder(s)+1].getChromaticNrOfNote() + f);
            if (n.getChromaticNrOfNote() >= m_level.loNote.getChromaticNrOfNote() &&
                n.getChromaticNrOfNote() <= m_level.hiNote.getChromaticNrOfNote()) {
                bool hope = true; // we stil have hope that note is for an exam
                if (m_level.onlyLowPos) {
                    if (s > 0) {
                       // we have to check when note is on the lowest positions
                       // is it realy lowest pos
                       // when strOrder[s] is 0 - it is the highest sting
                        char diff = openStr[gl->strOrder(s-1)] - openStr[gl->strOrder(s)];
                       if( (f-diff) >= m_level.loFret && (f-diff) <= m_level.hiFret) {
                           hope = false; //There is the same note on highest string
                       }
                       else {
                           hope = true;
                       }
                    }
                }
                if (hope && m_level.useKeySign && m_level.onlyCurrKey) {
                  hope = false;
                  if (m_level.isSingleKey) {
                    if(m_level.loKey.inKey(n).note != 0)
                        hope = true;
                    } else {
                        for (int k = m_level.loKey.value(); k <= m_level.hiKey.value(); k++) {
                          if (TkeySignature::inKey(TkeySignature(k), n).note != 0) {
                            hope = true;
                            break;
                          }
                        }
                    }
                }
                if (hope) {
                    if (n.acidental && (!m_level.withFlats && !m_level.withSharps))
                        continue;
                    else {
                        TQAunit::TQAgroup g;
                        g.note = n; g.pos = TfingerPos(gl->strOrder(s)+1, f);
                        m_questList << g;
                    }
                }
            }
        }
    }


//    for (int i = 0; i < m_questList.size(); i++)
//        qDebug() << i << (int)m_questList[i].pos.str() << "f"
//                << (int)m_questList[i].pos.fret() << " note: "
//                << QString::fromStdString(m_questList[i].note.getName());
}



void TexamExecutor::askQuestion() {
    gl->NnameStyleInNoteName = m_prevStyle;
    mW->noteName->setNoteNamesOnButt(m_prevStyle);

    clearWidgets();
    mW->setStatusMessage("");
    mW->startExamAct->setDisabled(true);
    mW->setMessageBg(gl->EquestionColor);
    m_answRequire.octave = true;
    m_answRequire.accid = true;
    m_answRequire.key = false;
//    curQ.qa_2.note = Tnote(0,0,0);

    TQAunit curQ = TQAunit(); // current question
    curQ.qa = m_questList[qrand() % m_questList.size()];
    curQ.questionAs = m_level.questionAs.next();
    curQ.answerAs = m_level.answersAs[curQ.questionAs].next();

    if (curQ.questionAs == TQAtype::e_asNote || curQ.answerAs == TQAtype::e_asNote) {
        if (m_level.useKeySign) {
            Tnote tmpNote = curQ.qa.note;
            if (m_level.isSingleKey) { //for single key
                curQ.key = m_level.loKey;
                tmpNote = m_level.loKey.inKey(curQ.qa.note);
            } else { // for multi keys
                curQ.key = TkeySignature((qrand() % (m_level.hiKey.value() - m_level.loKey.value() + 1)) +
                                         m_level.loKey.value());
                if (m_level.onlyCurrKey) { // if hote is in current key only
                    int keyRangeWidth = m_level.hiKey.value() - m_level.loKey.value();
                    int patience = 0; // we are lookimg for suitable key
                    char keyOff = curQ.key.value() - m_level.loKey.value();
                    tmpNote = curQ.key.inKey(curQ.qa.note);
                    while(tmpNote.note == 0 && patience < keyRangeWidth) {
                        keyOff++;
                        if (keyOff > keyRangeWidth) keyOff = 0;
                        curQ.key = TkeySignature(m_level.loKey.value() + keyOff);
                        patience++;
                        tmpNote = curQ.key.inKey(curQ.qa.note);
                        if (patience >= keyRangeWidth) {
                            qDebug() << "Oops!! It should never happend. I can not find key signature!!";
                            break;
                        }
                    }
                }
            }
            curQ.qa.note = tmpNote;
        }
        if ( !m_level.onlyCurrKey) // if key dosen't determine accidentals, we do this
            curQ.qa.note = determineAccid(curQ.qa.note);
    }

//    qDebug() << QString::fromStdString(curQ.qa.note.getName()) << "Q" << (int)curQ.questionAs
//            << "A" << (int)curQ.answerAs << curQ.key.getMajorName()
//            << (int)curQ.qa.pos.str() << (int)curQ.qa.pos.fret();

  // ASKING QUESIONS
    QString questText = QString("<b>%1. </b>").arg(m_answList.size()+1); //question number
    if (curQ.questionAs == TQAtype::e_asNote) {
        questText += tr("Given note show ");
        char strNr = 0;
        if ( curQ.answerAs == TQAtype::e_asFretPos && !m_level.onlyLowPos && m_level.showStrNr)
            strNr = curQ.qa.pos.str(); //show string nr or not
        if (m_level.useKeySign && curQ.answerAs != TQAtype::e_asNote)
            // when answer is also asNote we determine key in preparing answer part
            mW->score->askQuestion(curQ.qa.note, curQ.key, strNr);
        else mW->score->askQuestion(curQ.qa.note, strNr);
    }

    if (curQ.questionAs == TQAtype::e_asName) {
        if (curQ.answerAs == TQAtype::e_asFretPos && m_level.showStrNr)
            mW->noteName->askQuestion(curQ.qa.note, curQ.qa.pos.str());
        else
            mW->noteName->askQuestion(curQ.qa.note);
        questText += tr("Given note name show ");
    }

    if (curQ.questionAs == TQAtype::e_asFretPos) {
        mW->guitar->askQuestion(curQ.qa.pos);
        questText += tr("Given position show ");
        if (!m_level.forceAccids)
            m_answRequire.accid = false;
    }

// PREPARING ANSWERS
    if (curQ.answerAs == TQAtype::e_asNote) {
        questText += TquestionAsWdg::asNoteTxt;
        if (m_level.useKeySign) {
            if (m_level.manualKey) { // user have to manually secect a key
                mW->score->setKeySignature( // we randomize some key to cover this expected one
                        (qrand() % (m_level.hiKey.value() - m_level.loKey.value() + 1)) +
                        m_level.loKey.value());
                mW->score->setKeyViewBg(gl->EanswerColor);
                QString keyTxt;
                if (qrand() % 2) // randomize: ask for minor or major key ?
                    keyTxt = curQ.key.getMajorName();
                else
                    keyTxt = curQ.key.getMinorName();
                questText += tr(" <b>in %1 key.</b>", "in key signature").arg(keyTxt);
                m_answRequire.key = true;

            } else {
                mW->score->setKeySignature(curQ.key);
                mW->score->setKeyViewBg(gl->EquestionColor);
            }
        }
        if (curQ.questionAs == TQAtype::e_asNote) {// note has to be another than question
            curQ.qa_2.note = forceEnharmAccid(curQ.qa.note); // curQ.qa_2.note is expected note
            if (curQ.qa_2.note == curQ.qa.note) {
                qDebug() << "Blind question";
                //                    askQuestion();
            }
            questText += getTextHowAccid((Tnote::Eacidentals)curQ.qa_2.note.acidental);
            mW->score->forceAccidental((Tnote::Eacidentals)curQ.qa_2.note.acidental);
        }
        if (curQ.questionAs == TQAtype::e_asFretPos) {
            if (m_level.forceAccids)
                questText += getTextHowAccid((Tnote::Eacidentals)curQ.qa.note.acidental);
            mW->score->forceAccidental((Tnote::Eacidentals)curQ.qa.note.acidental);
        }
        mW->score->unLockScore();
        mW->score->setNoteViewBg(0, gl->EanswerColor);
    }

    if (curQ.answerAs == TQAtype::e_asName) {
        questText += TquestionAsWdg::asNameTxt;
        if (curQ.questionAs == TQAtype::e_asName) {
//             qDebug() << "as name";
            m_prevStyle = gl->NnameStyleInNoteName;
            Tnote::EnameStyle tmpStyle = m_prevStyle;
            if (m_isSolfege) {
                m_isSolfege = false;
                if (qrand() % 2) { // full name like cis, gisis
                    if (gl->seventhIs_B) tmpStyle = Tnote::e_nederl_Bis;
                    else tmpStyle = Tnote::e_deutsch_His;
                } else { // name and sign like c#, gx
                    if (gl->seventhIs_B) tmpStyle = Tnote::e_english_Bb;
                    else tmpStyle = Tnote::e_norsk_Hb;
                }
            } else {
                m_isSolfege = true;
                tmpStyle = Tnote::e_italiano_Si;
            }
            curQ.qa_2.note = forceEnharmAccid(curQ.qa.note); // force other name of note
            questText = QString("<b>%1. </b>").arg(m_answList.size()+1) +
                        tr("Give name of <span style=\"color: %1; font-size: %2px;\">").arg(
                                gl->EquestionColor.name()).arg(mW->getFontSize()*2) +
                        TnoteName::noteToRichText(curQ.qa.note) + ". </span>" +
                        getTextHowAccid((Tnote::Eacidentals)curQ.qa_2.note.acidental);
            mW->noteName->setNoteNamesOnButt(tmpStyle);
            gl->NnameStyleInNoteName = tmpStyle;
        }
        if (!m_level.requireOctave) m_answRequire.octave = false;

        if (curQ.questionAs == TQAtype::e_asFretPos) {
            if (m_level.forceAccids) {
                questText += getTextHowAccid((Tnote::Eacidentals)curQ.qa.note.acidental);
                curQ.qa_2.note = Tnote(1, 0, curQ.qa.note.acidental); // to show which accid on TnoteName
                /** @todo but don't use curQ.qa.note for this because it will be saved to
                    results' file. Create some temp variable*/
            }
        }
        mW->noteName->prepAnswer(curQ.qa_2.note);
    }

    if (curQ.answerAs == TQAtype::e_asFretPos) {
        questText += TquestionAsWdg::asFretPosTxt;
        if (curQ.questionAs == TQAtype::e_asName && m_level.showStrNr)
            questText += "<b>" + tr(" on <span style=\"font-family: nootka; font-size:%1px;\">%2</span> string.").arg(qRound(mW->getFontSize()*1.5)).arg((int)curQ.qa.pos.str()) + "</b>";

        mW->guitar->setMouseTracking(true);
    }
    m_answList << curQ;
    mW->setStatusMessage(questText);

    mW->nootBar->removeAction(nextQuestAct);
    mW->nootBar->removeAction(prevQuestAct);
    mW->nootBar->addAction(checkAct);
    mW->examResults->questionStart();

}

Tnote TexamExecutor::determineAccid(Tnote n) {
    Tnote nA = n;
    bool notFound = true;
    if (m_level.withSharps || m_level.withFlats || m_level.withDblAcc) {
        if (m_level.withDblAcc) {
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
        if (notFound && m_prevAccid != Tnote::e_Flat && m_level.withFlats) {
            nA = n.showWithFlat();
            notFound = false;
        }
        if (m_prevAccid != Tnote::e_Sharp && m_level.withSharps) {
            nA = n.showWithSharp();
        }
    }
    m_prevAccid = (Tnote::Eacidentals)nA.acidental;
    return nA;
}

Tnote TexamExecutor::forceEnharmAccid(Tnote n) {
    Tnote nX;
    char acc = m_prevAccid;
    int cnt;
    do {
        acc++;
        if (acc > 2) acc = -2;

        if (acc == Tnote::e_DoubleFlat && m_level.withDblAcc)
            nX = n.showWithDoubleFlat();
        if (acc == Tnote::e_Flat && m_level.withFlats)
            nX = n.showWithFlat();
        if (acc == Tnote::e_Natural)
            nX = n.showAsNatural();
        if (acc == Tnote::e_Sharp && m_level.withSharps)
            nX = n.showWithSharp();
        if (acc == Tnote::e_DoubleSharp && m_level.withDblAcc)
            nX = n.showWithDoubleSharp();
        cnt++;
    } while (n == nX || cnt < 6);
    m_prevAccid = (Tnote::Eacidentals)acc;
    if (nX.note)
        return nX;
    else return n;
}

void TexamExecutor::checkAnswer(bool showResults) {
    TQAunit curQ = m_answList[m_answList.size() - 1];
    curQ.time = mW->examResults->questionStop();
    mW->nootBar->removeAction(checkAct);
    mW->startExamAct->setDisabled(false);
// Let's check
    Tnote exN, retN; // example note & returned note
    // First we determine what have to be checked
    exN = curQ.qa.note;
    if (curQ.answerAs == TQAtype::e_asNote) {
        if (m_level.manualKey) {
            if (mW->score->keySignature().value() != curQ.key.value())
                curQ.setMistake(TQAunit::e_wrongKey);
        }
        if (curQ.questionAs == TQAtype::e_asNote)
            exN = curQ.qa_2.note;
        retN = mW->score->getNote(0);
    }

    if (curQ.answerAs == TQAtype::e_asName) {
        if (curQ.questionAs == TQAtype::e_asName)
            exN = curQ.qa_2.note;
        retN = mW->noteName->getNoteName();
    }
    if (curQ.answerAs == TQAtype::e_asFretPos) {
        if (curQ.qa.pos != mW->guitar->getfingerPos())
            curQ.setMistake(TQAunit::e_wrongPos);
    } else { // we check are the notes the same
      if (retN.note) {
        if (exN != retN) {
            if (m_answRequire.octave) {
                Tnote nE = exN.showAsNatural();
                Tnote nR = retN.showAsNatural();
                if (nE.note == nR.note && nE.acidental == nR.acidental) {
                    if (nE.octave != nR.octave)
                        curQ.setMistake(TQAunit::e_wrongOctave);
                } else
                    curQ.setMistake(TQAunit::e_wrongNote);
            }
            if (!curQ.wrongNote()) { // There is stil something to check
                exN.octave = 1;
                retN.octave = 1;//octaves are checed so we are reseting them
                if (exN != retN) {// if they are equal it means that only octaves were wrong
                    if (m_answRequire.accid) {
                        if(exN.showAsNatural() == retN.showAsNatural())
                            curQ.setMistake(TQAunit::e_wrongAccid);
                        else
                            curQ.setMistake(TQAunit::e_wrongNote);
                    } else
                        if (exN.showAsNatural() != retN.showAsNatural())
                            curQ.setMistake(TQAunit::e_wrongNote);
                }
            }
        }
      } else
          curQ.setMistake(TQAunit::e_wrongNote);
    }

    if (showResults) {
        QString answTxt;
        if (curQ.correct()) { // CORRECT
            answTxt = QString("<center><span style=\"color: %1; font-size:%2px; %3\">").arg(gl->EanswerColor.name()).arg(mW->getFontSize()).arg(gl->getBGcolorText(gl->EanswerColor));
            answTxt += tr("Exelent !!");
        } else { // WRONG
            answTxt = QString("<center><span style=\"color: %1; font-size:%2px; %3\">").arg(gl->EquestionColor.name()).arg(mW->getFontSize()).arg(gl->getBGcolorText(gl->EquestionColor));
            if (curQ.wrongNote())
                answTxt += tr("Wrong note.");
            if (curQ.wrongKey())
                answTxt += tr(" Wrong key signature.");
            if (curQ.wrongAccid())
                answTxt += tr(" Wrong accidental.");
            if (curQ.wrongPos())
                answTxt += tr(" Wrong position.");
            if (curQ.wrongOctave())
                answTxt += tr("<br>Wrong octave.");
        }
        answTxt += "</span><br>";
        if (gl->hintsEnabled) {
            answTxt += tr("<hr>Click <img src=\"%1\"> buton<br>or press <b>space</b> for next question.").arg(gl->path+"picts/next-icon.png");
            if (!curQ.correct())
                answTxt += tr("<br>Click <img src=\"%1\"> buton<br>or press <b>backspace</b> to correct question.").arg(gl->path+"picts/prev-icon.png");
        }
        answTxt += "</center>";
        QWhatsThis::showText(QPoint(mW->pos().x() + qRound(mW->centralWidget()->width()*0.75),
                                    mW->pos().y() + qRound(mW->centralWidget()->height()*0.5)),
                             answTxt);
//        mW->examResults->setAnswer(curQ.correct());
        if (!curQ.correct())
            mW->nootBar->addAction(prevQuestAct);
        mW->nootBar->addAction(nextQuestAct);
        disableWidgets();
    }
    mW->examResults->setAnswer(curQ.correct());
    m_answList[m_answList.size()-1] = curQ;
}

void TexamExecutor::repeatQuestion() {
    TQAunit curQ = m_answList[m_answList.size() - 1];
    QString m = mW->statusMessage();
    m.replace(0, m.indexOf("</b>"), QString("<b>%1.").arg(m_answList.size()+1));
    mW->setStatusMessage(m);
    curQ.setMistake(TQAunit::e_correct);
    if (curQ.answerAs == TQAtype::e_asNote)
        mW->score->unLockScore();
    if (curQ.answerAs == TQAtype::e_asName)
        mW->noteName->setNameDisabled(false);
    if (curQ.answerAs == TQAtype::e_asFretPos)
        mW->guitar->setMouseTracking(true);

    m_answList << curQ;

    mW->startExamAct->setDisabled(true);
    mW->nootBar->removeAction(nextQuestAct);
    mW->nootBar->removeAction(prevQuestAct);
    mW->nootBar->addAction(checkAct);
    mW->examResults->questionStart();
}

void TexamExecutor::prepareToExam() {
    mW->setWindowTitle(tr("EXAM!!") + " " + m_userName + " - " + m_level.name);
    mW->setStatusMessage("exam started on level:<br><b>" + m_level.name + "</b>");

    mW->settingsAct->setDisabled(true);
    mW->levelCreatorAct->setDisabled(true);
    mW->startExamAct->setIcon(QIcon(gl->path+"picts/stopExam.png"));
    mW->startExamAct->setText(tr("stop the exam"));
    mW->startExamAct->setStatusTip(mW->startExamAct->text());
//    mW->startExamAct->setDisabled(true);

    disableWidgets();

    disconnect(mW->score, SIGNAL(noteChanged(int,Tnote)), mW, SLOT(noteWasClicked(int,Tnote)));
    disconnect(mW->noteName, SIGNAL(noteNameWasChanged(Tnote)), mW, SLOT(noteNameWasChanged(Tnote)));
    disconnect(mW->guitar, SIGNAL(guitarClicked(Tnote)), mW, SLOT(guitarWasClicked(Tnote)));
    disconnect(mW->startExamAct, SIGNAL(triggered()), mW, SLOT(startExamSlot()));
    connect(mW->startExamAct, SIGNAL(triggered()), this, SLOT(stopExamSlot()));

    m_prevStyle = gl->NnameStyleInNoteName;
    m_glStore.nameStyleInNoteName = gl->NnameStyleInNoteName;
    m_glStore.showEnharmNotes = gl->showEnharmNotes;
    m_glStore.showKeySignName = gl->SshowKeySignName;
    m_glStore.showOtherPos = gl->GshowOtherPos;
    m_glStore.useDblAccids = gl->doubleAccidentalsEnabled;
    m_glStore.useKeySign = gl->SkeySignatureEnabled;
    m_glStore.octaveInName = gl->NoctaveInNoteNameFormat;

    gl->showEnharmNotes = false;
    gl->SshowKeySignName = false;
    gl->GshowOtherPos = false;
    gl->doubleAccidentalsEnabled = m_level.withDblAcc;
    gl->SkeySignatureEnabled = m_level.useKeySign;
    gl->NoctaveInNoteNameFormat = true;

    mW->score->acceptSettings();
    mW->noteName->setEnabledEnharmNotes(false);
    mW->guitar->acceptSettings();
  // clearing all views/widgets
    clearWidgets();

    if(gl->hintsEnabled)
        mW->setStatusMessage(tr("<img src=\"%1\"> or <b>space</b> to get next question.").arg(gl->path+"picts/next-icon.png"), 5000);

}

void TexamExecutor::restoreAfterExam() {
    mW->setWindowTitle(qApp->applicationName());
    mW->nootBar->removeAction(nextQuestAct);
    mW->examResults->clearResults();

    gl->NnameStyleInNoteName = m_glStore.nameStyleInNoteName;
    gl->showEnharmNotes = m_glStore.showEnharmNotes;
    gl->SshowKeySignName = m_glStore.showKeySignName;
    gl->GshowOtherPos = m_glStore.showOtherPos;
    gl->doubleAccidentalsEnabled  = m_glStore.useDblAccids;
    gl->SkeySignatureEnabled = m_glStore.useKeySign;
    gl->setTune(m_glStore.tune);
    gl->NoctaveInNoteNameFormat = m_glStore.octaveInName;

    mW->score->acceptSettings();
    mW->noteName->setEnabledEnharmNotes(false);
    mW->guitar->acceptSettings();

    mW->settingsAct->setDisabled(false);
    mW->levelCreatorAct->setDisabled(false);
    mW->startExamAct->setDisabled(false);
    mW->noteName->setNameDisabled(false);
    mW->guitar->setMouseTracking(true);

    connect(mW->score, SIGNAL(noteChanged(int,Tnote)), mW, SLOT(noteWasClicked(int,Tnote)));
    connect(mW->noteName, SIGNAL(noteNameWasChanged(Tnote)), mW, SLOT(noteNameWasChanged(Tnote)));
    connect(mW->guitar, SIGNAL(guitarClicked(Tnote)), mW, SLOT(guitarWasClicked(Tnote)));
    disconnect(mW->startExamAct, SIGNAL(triggered()), this, SLOT(stopExamSlot()));
    connect(mW->startExamAct, SIGNAL(triggered()), mW, SLOT(startExamSlot()));
    mW->score->isExamExecuting(false);
    mW->score->unLockScore();
    mW->clearAfterExam();
    
}

void TexamExecutor::disableWidgets() {
    mW->noteName->setNameDisabled(true);
    mW->score->isExamExecuting(true);
    mW->score->setScoreDisabled(true);
    mW->guitar->setMouseTracking(false);
}

void TexamExecutor::clearWidgets() {
    mW->score->clearScore();
    mW->noteName->clearNoteName();
    mW->guitar->clearFingerBoard();
}

void TexamExecutor::stopExamSlot() {
    mW->examResults->stopExam();
    if (m_examFile == "" && m_answList.size())
        m_examFile = saveExamToFile();
    if (m_examFile != "") {
        QFile file(m_examFile);
        if (file.open(QIODevice::WriteOnly)) {
            QDataStream out(&file);
            out.setVersion(QDataStream::Qt_4_7);
            out << examVersion;
            out << m_userName << m_level << gl->Gtune();
            out << mW->examResults->getTotalTime(); // elapsed exam time (quint32)
            // data for file preview
            out << (quint16)m_answList.size(); // number of questions
            out << mW->examResults->getAverageTime(); // average time of answer (quint16)
            // that's all
            out << mW->examResults->getMistakesNumber(); // number of mistakes (quint16)
            for (int i = 0; i < m_answList.size(); i++)
                out << m_answList[i]; // and obviously answers

#if defined(Q_OS_WIN32)
            QSettings sett(QSettings::IniFormat, QSettings::UserScope, "Nootka", "Nootka");
#else
            QSettings sett;
#endif
            QStringList recentExams = sett.value("recentExams").toStringList();
            recentExams.removeAll(m_examFile);
            recentExams.prepend(m_examFile);
            sett.setValue("recentExams", recentExams);

        } else
            QMessageBox::critical(mW, "",
                                  tr("Cannot save exam file:\n%1").arg(qPrintable(file.errorString())));
    }

    mW->setMessageBg(-1);
    mW->setStatusMessage("");
    mW->setStatusMessage("so a pity", 5000);

    clearWidgets();
    restoreAfterExam();
}

QString TexamExecutor::getTextHowAccid(Tnote::Eacidentals accid) {
    QString S = QString("<br><span style=\"color: %1\">").arg(gl->GfingerColor.name());
    if (accid) S += tr("Use %1").arg(QString::fromStdString(signsAcid[accid + 2]));
    else S += tr(" Don't use accidentals!");
    S +=  "</span>";
    return S;

}

bool TexamExecutor::closeNootka() {
    QMessageBox::StandardButton retMessage = QMessageBox::warning(mW, "", tr("Psssst... Exam is going.<br><br><b>Retry</b> - to continue<br><b>Save - </b> to check(if neded), save and exit<br>"),
                       QMessageBox::Save | QMessageBox::Retry, QMessageBox::Save);
    if (retMessage == QMessageBox::Retry) {
        return false;
    } else {
        if (mW->startExamAct->isEnabled()) {
            stopExamSlot();
            return true;
        } else {
            checkAnswer(false);
            stopExamSlot();
            return true;
        }
    }
}

QString TexamExecutor::saveExamToFile() {
    QString fileName = QFileDialog::getSaveFileName(mW, tr("Save exam's results as:"),
                         QDir::toNativeSeparators(QDir::homePath()+"/"+m_userName+"-"+m_level.name),
                                   TstartExamDlg::examFilterTxt);
    if (fileName == "")
        if (QMessageBox::warning(mW, "", tr("If You don't save to file<br>all results are lost !!"),
                             QMessageBox::Save | QMessageBox::Discard, QMessageBox::Save)
            == QMessageBox::Save)
                fileName = saveExamToFile();
    return fileName;

}

