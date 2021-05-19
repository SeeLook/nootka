/***************************************************************************
 *   Copyright (C) 2021 by Tomasz Bojczuk                                  *
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

#include "tgotit.h"
#include <tsound.h>
#include <score/tscoreobject.h>
#include <music/tnotestruct.h>
#include <tcolor.h>
#include <tglobals.h>
#include <tnootkaqml.h>
#include "main/tstartexamitem.h"
#include "help/texamhelp.h"
#include "help/tmainhelp.h"

#include <QtCore/qtimer.h>
// #include <QtCore/qdebug.h>


TgotIt::TgotIt(QObject *parent) :
  QObject(parent)
{
}


TgotIt::~TgotIt() {
  if (m_gotItType == GotSoundInfo) {
    SOUND->stopListen();
    SOUND->setTunerMode(false);
    SOUND->startListen();
  }
}


void TgotIt::setGotItType(TgotIt::EgotItType gt) {
  if (gt != m_gotItType) {
    switch (gt) {
      case GotSoundInfo:
        SOUND->stopListen();
        SOUND->setTunerMode(true);
        connect(SOUND, &Tsound::noteStarted, this, &TgotIt::noteStartedSlot);
        connect(SOUND, &Tsound::noteFinishedEntire, this, &TgotIt::noteFinishedSlot);
        m_soundTimer = new QTimer(this);
        connect(m_soundTimer, &QTimer::timeout, this, [=]{ m_maxVolume = qMax(m_maxVolume, SOUND->inputVol()); });
        m_soundTimer->start(100);
        SOUND->startListen();
        break;
      case GotNoteSelected:
        QTimer::singleShot(10, this, [=]{ SOUND->stop(); }); // HACK Playing has to be started first, stop it after delay
      default:
        break;
    }
    m_gotItType = gt;
    emit gotItTypeChanged();
  }
}


void TgotIt::setScore(TscoreObject *sc) {
  m_score = sc;
}


void TgotIt::setCursorAlter(int curAlt) {
  if (m_score)
    m_score->setCursorAlter(curAlt);
}


void TgotIt::setWorkRtmValue(int rtmV) {
  if (m_score)
    m_score->setWorkRtmValue(rtmV);
}


QString TgotIt::exerOrExamHelpTxt() {
  QLatin1String br("<br>");
  QLatin1String sp("&nbsp;");
  return br + TmainHelp::youWillLearnText() + br + br
    + QLatin1String("<table><tr><td valign=\"middle\">") + NOO->pix("practice", NOO->factor() * 4)
    + QLatin1String(" </td><td valign=\"middle\"> ") + TmainHelp::duringExercisingText()
    + br + TexamHelp::exerciseFeaturesText()+ QLatin1String("</td></tr>")
    + QLatin1String("<tr><td><br></td><td><br></td></tr>")
    + QLatin1String("<tr><td valign=\"middle\">") + NOO->pix("exam", NOO->factor() * 4)
    + QLatin1String(" </td><td valign=\"middle\"> ") + TmainHelp::duringExamsText()
    + br + TexamHelp::examFeaturesText()+ QLatin1String("</td></tr></table>")
    + br + NOO->getOnlineDoc(QStringLiteral("practicing"));
}


QString TgotIt::getQuestionText() const {
  return TexamHelp::getQuestionText();
}


QString TgotIt::confirmText() const {
  return TexamHelp::confirmText();
}


QString TgotIt::practiceText() const {
  return TexamHelp::practiceText();
}


QString TgotIt::practiceExplained() const {
  return TexamHelp::practiceExplained();
}


QString TgotIt::examText() const {
  return TexamHelp::examText();
}


QString TgotIt::examExplained() const {
  return TexamHelp::examExplained();
}


QString TgotIt::optionsText() const {
  return TexamHelp::optionsText();
}


QString TgotIt::onlineDocP(const QString& hash) const {
  return NOO->onlineDocP(hash);
}


QString TgotIt::noteCursorText() const {
  return tr("note cursor", "it might be just 'cursor' as well").replace(QLatin1String(" "), QLatin1String("\n"));
}


QString TgotIt::ifSelectedText() const {
  return tr("Note becomes selected (highlighted) when it was edited or clicked with right mouse button or tapped shortly.")
        + QLatin1String("<br>")
        + tr("If any note on the score is selected, only this particular one is changed by playing or clicking on an instrument. "
             "And only pitch of that note changes - rhythmic value remains the same. ")
             .replace(QLatin1String(". "), QLatin1String(".<br>"));
}


QString TgotIt::ifNotSelectedText() const {
  return tr("When no note is chosen on the score, note cursor is highlighted and blinks. "
            "Notes are added one by one, "
            "either by playing real instrument or by selecting a sound on instrument displayed by the application.")
            .replace(QLatin1String(". "), QLatin1String(".<br>"));
}


QString TgotIt::unselectText() const {
  return tr("To deselect the note just click or tap some free part of the score sheet or use arrow keys to navigate to the note cursor.");
}


//#################################################################################################
//###################                PROTECTED         ############################################
//#################################################################################################

void TgotIt::noteStartedSlot(const Tnote& n) {
  m_noteName = n.styledName();
  emit noteNameChanged();
}


void TgotIt::noteFinishedSlot(TnoteStruct) {
  m_noteName.clear();
  emit noteNameChanged();
  emit maxVolumeChanged();
  m_maxVolume = 0.0;
}
