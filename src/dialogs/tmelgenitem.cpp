/***************************************************************************
 *   Copyright (C) 2018 by Tomasz Bojczuk                                  *
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

#include "tmelgenitem.h"
#include "trtmselectoritem.h"
#include "main/tmainscoreobject.h"
#include "main/trandmelody.h"
#include <music/tmelody.h>
#include <QtCore/qsettings.h>
#include <QtGui/qguiapplication.h>

#include <QtCore/qdebug.h>


TmelGenItem::TmelGenItem(QQuickItem* parent) :
  QQuickItem(parent)
{
#if defined(Q_OS_WIN32) || defined(Q_OS_MAC) // I hate mess in Win registry
  m_settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, qApp->applicationName(), qApp->applicationName());
#else
  m_settings = new QSettings();
#endif
  m_settings->beginGroup(QLatin1String("Melody"));
  m_basicMask = m_settings->value(QLatin1String("basic"), BASIC_MASK).toUInt();
  m_dotsMask = m_settings->value(QLatin1String("dots"), DOTS_MASK).toUInt();
  m_rtmDiversity = qBound(1, m_settings->value(QLatin1String("rtmDiversity"), 5).toInt(), 10);
  m_barNumber = qBound(2, m_settings->value(QLatin1String("barNumber"), 4).toInt(), 32) ;
  m_endsOnTonic = m_settings->value(QLatin1String("endsOnTonic"), true).toBool();
  m_onlyCurrKey = m_settings->value(QLatin1String("onlyCurrKey"), true).toBool();
  m_maxStep = qBound(2, m_settings->value(QLatin1String("maxStep"), 7).toInt(), 36);
  m_settings->endGroup();
}

TmelGenItem::~TmelGenItem()
{
  delete m_settings;
}


void TmelGenItem::setRhythmSelector(TrtmSelectorItem* rtmS) {
  if (m_rtmSelector == nullptr) {
    m_rtmSelector = rtmS;
    m_rtmSelector->setBasicMask(m_basicMask);
    m_rtmSelector->setDotsMask(m_dotsMask);
  }
}


void TmelGenItem::setBarNumber(int bars) {
  if (bars != m_barNumber) {
    m_barNumber = bars;
    emit barNumberChanged();
  }
}


void TmelGenItem::setRhythmDiversity(int diverity) {
  if (diverity != m_rtmDiversity) {
    m_rtmDiversity = diverity;
    emit rhythmDiversityChanged();
  }
}


void TmelGenItem::setEndsOnTonic(bool eot) {
  if (eot != m_endsOnTonic) {
    m_endsOnTonic = eot;
    emit endsOnTonicChanged();
  }
}


void TmelGenItem::setOnlyCurrKey(bool only) {
  if (only != m_onlyCurrKey) {
    m_onlyCurrKey = only;
    emit onlyCurrKeyChanged();
  }
}


void TmelGenItem::setMaxStep(int max) {
  if (max != m_maxStep) {
    m_maxStep = max;
    emit maxStepChanged();
  }
}


void TmelGenItem::generate() {
  int meter = MAIN_SCORE->meter();
  TrhythmList rhythms = getRandomRhythm(meter, m_barNumber, m_rtmSelector->basicMask(), m_rtmSelector->dotsMask(), m_rtmDiversity);
  if (rhythms.isEmpty()) {
    qDebug() << "[TmelGenItem] no rhythms from current parameters. Skipping melody generate!";
    return;
  }

  QList<TQAgroup> ql;
  short ambit = 15; //highestNote().chromatic() - lowestNote().chromatic();
  short first = 1;
  auto clef = static_cast<Tclef::EclefType>(MAIN_SCORE->clefType());
  if (clef == Tclef::Bass_F_8down)
    first = -20;
  else if (clef == Tclef::Bass_F)
    first = -9;
  else if (clef == Tclef::Treble_G)
    first = 11;
  for (short i = 0; i < ambit; i++) {
    TQAgroup qa;
    qa.note = Tnote(first + i);
    ql << qa;
  }
  auto mel = new Tmelody(QString(), TkeySignature(MAIN_SCORE->keySignatureValue()));
  mel->setMeter(meter);
  mel->setClef(clef);
  getRandomMelodyNG(ql, mel, rhythms.count(), m_onlyCurrKey, m_endsOnTonic, m_maxStep);
// merge rhythm and melody
  mergeRhythmAndMelody(rhythms, mel);

  MAIN_SCORE->setMelody(mel);
  delete mel;
// store settings only when melody is generated
  m_settings->beginGroup(QLatin1String("Melody"));
  m_settings->setValue(QLatin1String("basic"), m_rtmSelector->basicMask());
  m_settings->setValue(QLatin1String("dots"), m_rtmSelector->dotsMask());
  m_settings->setValue(QLatin1String("rtmDiversity"), m_rtmDiversity);
  m_settings->setValue(QLatin1String("barNumber"), m_barNumber);
  m_settings->setValue(QLatin1String("endsOnTonic"), m_endsOnTonic);
  m_settings->setValue(QLatin1String("onlyCurrKey"), m_onlyCurrKey);
  m_settings->setValue(QLatin1String("maxStep"), m_maxStep);
  m_settings->endGroup();
}
