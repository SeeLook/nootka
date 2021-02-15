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
    + br + NOO->onlineDocP(QStringLiteral("start-exam"));
}


QString TgotIt::getQuestionText() {
  return TexamHelp::getQuestionText();
}


QString TgotIt::confirmText() {
  return TexamHelp::confirmText();
}


QString TgotIt::practiceText() {
  return TexamHelp::practiceText();
}


QString TgotIt::practiceExplained() {
  return TexamHelp::practiceExplained();
}


QString TgotIt::examText() {
  return TexamHelp::examText();
}


QString TgotIt::examExplained() {
  return TexamHelp::examExplained();
}


QString TgotIt::optionsText() {
  return TexamHelp::optionsText();
}


QString TgotIt::onlineDocP(const QString& hash) {
  return NOO->onlineDocP(hash);
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
