/***************************************************************************
 *   Copyright (C) 2018-2019 by Tomasz Bojczuk                             *
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

#include "tmelodylistview.h"
#include <music/tmelody.h>
#include <score/tscoreobject.h>
#include <exam/tlevel.h>
#include <tglobals.h>
#include <tnootkaqml.h>

#include <QtQml/qqmlengine.h>
#include <QtCore/qtimer.h>
#if defined (Q_OS_ANDROID)
  #include "Android/tfiledialog.h"
#else
  #include <QtWidgets/qfiledialog.h>
#endif

#include <QtCore/qdebug.h>
#include "checktime.h"


#define MELODY_LENGHT (15)


TmelodyListView::TmelodyListView(QQuickItem* parent) :
  QQuickItem(parent)
{
}


TmelodyListView::~TmelodyListView()
{
  clearMelodyList();
}


/**
 * Load melodies with delay to give time for score creation,
 * otherwise score complains with warnings
 */
void TmelodyListView::setLevel(Tlevel* l) {
//   if (m_level != l) {
    m_level = l;
    QTimer::singleShot(50, [=]{ loadMelodies(); });
//   }
}


/**
 * First create copy list @p tempMelodies due to @p QList::clear() will reset all @p Tmelody pointers
 * stored in @p m_melodies list
 */
void TmelodyListView::save() {
  if (m_listWasChanged) {
    QList<Tmelody> tempMelodies;
    for (int m = 0; m < m_melodies.count(); ++m)
      tempMelodies << *m_melodies[m].melody;
    m_level->melodySet.clear();
    m_level->melodySet.append(tempMelodies);
    m_listWasChanged = false;
  }
}


void TmelodyListView::loadMelody() {
  QString musicXMLfile = NOO->getXmlToOpen();
  if (!musicXMLfile.isEmpty()) {
    auto melody = new Tmelody();
    if (melody->grabFromMusicXml(musicXMLfile)) {
      m_melodies << TscoreMelody(nullptr, melody);
      m_melodies.last().delMelody = true;
      emit addScore();
      emit melodiesChanged();
      m_listWasChanged = true;
    }
  }
}


/**
 * @p m_emitWhenRemove is a switch determining when @p melodiesChanged() signal is emitted,
 * to inform level creator about level change. Normally it happens when user adds/removes a melody from the list.
 * But when other level was set and it has less number of melodies than current one
 * @p loadMelodies() emits @p removeScore() signal and QML invokes @p removeMelody().
 * We don't want emitting @p melodiesChanged() then, so @p m_emitWhenRemove is set to false
 */
void TmelodyListView::removeMelody(int id) {
  if (id >= 0 && id < m_melodies.count()) {
    if (m_melodies[id].delMelody)
      delete m_melodies[id].melody;
    m_melodies.removeAt(id);
    if (m_emitWhenRemove)
      emit melodiesChanged();
    m_emitWhenRemove = true;
    m_listWasChanged = true;
  }
}


void TmelodyListView::setScore(int id, TscoreObject* score) {
  if (id >= 0 && id < m_melodies.count()) {
    if (m_melodies[id].score == nullptr) {
        m_melodies[id].score = score;
        if (m_melodies[id].melody)
          score->setMelody(m_melodies[id].melody, false, MELODY_LENGHT);
    } else
        qDebug() << "[TmelodyListView] score already set for item" << id;
  }
}

QString TmelodyListView::title(int melId) {
  if (melId > -1 && melId < m_melodies.count() && m_melodies[melId].melody)
    return m_melodies[melId].melody->title();
  else
    return QString();
}


QString TmelodyListView::composer(int melId) {
  if (melId > -1 && melId < m_melodies.count() && m_melodies[melId].melody)
    return m_melodies[melId].melody->composer();
  else
    return QString();
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TmelodyListView::loadMelodies() {
CHECKTIME(
  for (int m = 0; m < m_level->melodySet.count(); ++m) {
    if (m < m_melodies.count()) {
        if (m_melodies[m].delMelody) {
          delete m_melodies[m].melody;
          m_melodies[m].delMelody = false;
        }
        m_melodies[m].melody = &m_level->melodySet[m];
        m_melodies[m].score->setMelody(m_melodies[m].melody, false, MELODY_LENGHT);
    } else {
        m_melodies << TscoreMelody(nullptr, &m_level->melodySet[m]);
        emit addScore();
    }
  }
  if (m_level->melodySet.count() < m_melodies.count()) {
    int nrToRemove = m_melodies.count() - m_level->melodySet.count();
    for (int r = 0; r < nrToRemove; ++r) {
      m_emitWhenRemove = false;
      emit removeScore(m_level->melodySet.count() + r);
    }
  }
  m_listWasChanged = false;
)
}


void TmelodyListView::clearMelodyList() {
  for (int m = 0; m < m_melodies.count(); ++m) {
    if (m_melodies[m].delMelody) {
      delete m_melodies[m].melody;
      m_melodies[m].delMelody = false;
    }
  }
  m_melodies.clear();
}
