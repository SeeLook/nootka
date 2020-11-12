/***************************************************************************
 *   Copyright (C) 2018-2020 by Tomasz Bojczuk                             *
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


TmelodyListView::TmelodyListView(QQuickItem* parent) :
  QQuickItem(parent)
{
}


TmelodyListView::~TmelodyListView()
{
  clearMelodyList();
}


void TmelodyListView::setMelodyModel(QObject* mm) {
  if (mm != m_melodyModel) {
    m_melodyModel = mm;
  }
}


/**
 * Load melodies with delay to give time for score creation,
 * otherwise score complains with warnings
 */
void TmelodyListView::setLevel(Tlevel* l) {
  m_level = l;
  QTimer::singleShot(50, [=]{ loadMelodies(); });
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
      m_melodies << TmelodyAtList(melody);
      m_melodies.last().delMelody = true;
      emit appendMelody();
      emit melodiesChanged();
      m_listWasChanged = true;
    }
  }
}


/**
 * @p m_emitWhenRemove is a switch determining when @p melodiesChanged() signal is emitted,
 * to inform level creator about level change. Normally it happens when user adds/removes a melody from the list.
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


void TmelodyListView::swapMelodies(int from, int to) {
  if (from > -1 && from < m_melodies.count() && to > -1 && to < m_melodies.count()) {
      m_melodies.move(from, to);
      m_listWasChanged = true;
      emit melodiesChanged();
  } else
      qDebug() << "[TmelodyListView] FIXME! Wrong melodies to swap";
}


Tmelody* TmelodyListView::getMelody(int melId) {
  return melId > -1 && melId < m_melodies.count() ? m_melodies[melId].melody : nullptr;
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TmelodyListView::loadMelodies() {
  QMetaObject::invokeMethod(m_melodyModel, "clear");
  clearMelodyList();
  for (int m = 0; m < m_level->melodySet.count(); ++m) {
    m_melodies << TmelodyAtList(&m_level->melodySet[m]);
    emit appendMelody();
  }
  m_listWasChanged = false;
}


void TmelodyListView::clearMelodyList() {
  int melCnt = m_melodies.count();
  for (int m = 0; m < melCnt; ++m) {
    if (m_melodies[m].delMelody) {
      delete m_melodies[m].melody;
    }
  }
  m_melodies.clear();
}
