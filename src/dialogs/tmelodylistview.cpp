/***************************************************************************
 *   Copyright (C) 2018-2021 by Tomasz Bojczuk                             *
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
#if !defined (Q_OS_ANDROID)
  #include <tmtr.h>
  #include <qtr.h>
  #include <QtWidgets/qfiledialog.h>
  #include <QtGui/qguiapplication.h>
  #include <QtGui/qfont.h>
#endif

#include <QtCore/qdebug.h>


TmelodyListView::TmelodyListView(QQuickItem* parent) :
  QQuickItem(parent)
{
}


TmelodyListView::~TmelodyListView() {}


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


void TmelodyListView::loadMelody() {
#if defined (Q_OS_ANDROID)
  QString musicXMLfile = NOO->getXmlToOpen();
  if (!musicXMLfile.isEmpty()) {
    m_level->melodySet << Tmelody();
    auto melody = &m_level->melodySet.last();
    if (melody->grabFromMusicXml(musicXMLfile)) {
        emit appendMelody();
        emit melodiesChanged();
    } else
        m_level->melodySet.removeLast();
  }
#else
  auto f = qApp->font();
  qApp->setFont(Tmtr::systemFont);
  auto names = QFileDialog::getOpenFileNames(nullptr, qTR("TmainScoreObject", "Open melody file"), GLOB->lastXmlDir(),
                                            qTR("TmainScoreObject", "MusicXML file") + QLatin1String(" (*.xml *.musicxml)"));
  qApp->setFont(f);
  if (names.isEmpty())
    return;

  for (auto musicXMLfile : names) {
    if (!musicXMLfile.isEmpty()) {
      m_level->melodySet << Tmelody();
      auto melody = &m_level->melodySet.last();
      if (melody->grabFromMusicXml(musicXMLfile)) {
          emit appendMelody();
          emit melodiesChanged();
      } else
          m_level->melodySet.removeLast();
    }
  }
#endif
}


void TmelodyListView::removeMelody(int id) {
  if (id >= 0 && id < m_level->melodySet.count()) {
    m_level->melodySet.removeAt(id);
    emit melodiesChanged();
  }
}


void TmelodyListView::swapMelodies(int from, int to) {
  if (from > -1 && from < m_level->melodySet.count() && to > -1 && to < m_level->melodySet.count()) {
      m_level->melodySet.move(from, to);
      emit melodiesChanged();
  } else
      qDebug() << "[TmelodyListView] FIXME! Wrong melodies to swap";
}


Tmelody* TmelodyListView::getMelody(int melId) {
  return melId > -1 && melId < m_level->melodySet.count() ? &m_level->melodySet[melId] : nullptr;
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TmelodyListView::loadMelodies() {
  QMetaObject::invokeMethod(m_melodyModel, "clear");
  for (int m = 0; m < m_level->melodySet.count(); ++m)
    emit appendMelody();
}
