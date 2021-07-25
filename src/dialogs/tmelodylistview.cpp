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
#include "tmelodywrapper.h"
#include <music/tmelody.h>
#include <score/tscoreobject.h>
#include <exam/tlevel.h>
#include <tglobals.h>
#include <tnootkaqml.h>
#include <music/timportscore.h>

#include <QtQml/qqmlapplicationengine.h>
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
 * otherwise score complains with warnings.
 * But for score itself 50ms is sufficient,
 * 250ms delay is for displaying 'entering melody page' animation,
 * then melodies show off.
 */
void TmelodyListView::setLevel(Tlevel* l) {
  QMetaObject::invokeMethod(m_melodyModel, "clear");
  m_lastMelodyId = -1; // reset
  m_level = l;
  QTimer::singleShot(250, this, [=]{ loadMelodies(); });
}


void TmelodyListView::loadMelody() {
#if defined (Q_OS_ANDROID)
  QString musicXMLfile = NOO->getXmlToOpen();
  if (musicXMLfile.isEmpty())
    return;
  else
    m_xmlFiles << musicXMLfile;
#else
  auto f = qApp->font();
  qApp->setFont(Tmtr::systemFont);
  m_xmlFiles = QFileDialog::getOpenFileNames(nullptr, qTR("TmainScoreObject", "Open melody file"), GLOB->lastXmlDir(),
                                             qTR("TmainScoreObject", "MusicXML file") + QLatin1String(" (*.xml *.musicxml *.mxl)"));
  qApp->setFont(f);
  if (m_xmlFiles.isEmpty())
    return;
#endif
  processNextXmlFile();
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


QVariant TmelodyListView::getMelody(int melId) {
  return QVariant::fromValue(melId > -1 && melId < m_level->melodySet.count() ? &m_level->melodySet[melId] : nullptr);
}


void TmelodyListView::divideMelody(int melId, int bars) {
  if (melId > -1 && melId < m_level->melodySet.count()) {
    QList<Tmelody*> parts;
    m_level->melodySet[melId].split(bars, parts);
    if (!parts.isEmpty()) {
      int i = 1;
      for (auto m : parts) {
        m_level->melodySet.insert(melId + i, *m);
        emit insertMelody(melId + i);
        i++;
      }
    }
  }
}


void TmelodyListView::transpose(int semis, bool outScaleToRes, bool inInstrScale, TmelodyWrapper* wrapp) {
  if (wrapp && wrapp->melody() && wrapp->score()) {
    wrapp->melody()->transpose(semis, outScaleToRes,
                               inInstrScale ? GLOB->loNote() : wrapp->score()->lowestNote(),
                               inInstrScale ? GLOB->hiNote() : wrapp->score()->highestNote());
    wrapp->reload();
  }
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TmelodyListView::loadMelodies() {
  m_lastMelodyId++;
  for (int m = m_lastMelodyId; m < m_level->melodySet.count(); ++m)
    emit appendMelody();
  m_lastMelodyId = m_level->melodySet.count() - 1;
}


void TmelodyListView::processNextXmlFile() {
  if (m_xmlFiles.isEmpty())
    return;

  auto musicXMLfile = m_xmlFiles.takeFirst();
  m_level->melodySet << Tmelody();
  auto m = &m_level->melodySet.last();
  auto melImport = new TimportScore(musicXMLfile, m);
  melImport->setMultiSelect(true);
  connect(melImport, &TimportScore::wantDialog, this, [=]{
    auto nootWin = qobject_cast<QQmlApplicationEngine*>(NOO->qmlEngine())->rootObjects().first();
    if (nootWin && QString(nootWin->metaObject()->className()).contains("MainWindow_QMLTYPE")) {
      QMetaObject::invokeMethod(nootWin, "showDialog", Q_ARG(QVariant, TnootkaQML::ScoreImport));
      connect(melImport, &TimportScore::importReady, this, &TmelodyListView::melodyImportSlot);
    }
  });
  connect(melImport, &TimportScore::xmlWasRead, this,[=]{
    if (IMPORT_SCORE->hasMoreParts()) {
        m_level->melodySet.removeLast();
    } else {
        IMPORT_SCORE->deleteLater();
        if (m_xmlFiles.isEmpty()) {
            QTimer::singleShot(100, this, [=]{ loadMelodies(); });
            emit melodiesChanged();
        } else
            QTimer::singleShot(100, this, [=]{ processNextXmlFile(); });
    }
  });
  melImport->runXmlThread();
}


void TmelodyListView::melodyImportSlot() {
  for (auto mi : IMPORT_SCORE->model()) {
    auto voicePart = qobject_cast<TmelodyPart*>(mi);
    if (voicePart && !voicePart->parts.isEmpty()) {
      for (auto snip : voicePart->parts) {
        if (snip->selected()) // append all selected parts
          m_level->melodySet << *snip->melody();
      }
    }
  }
  if (m_xmlFiles.isEmpty()) {
      QTimer::singleShot(100, this, [=]{ loadMelodies(); });
      emit melodiesChanged();
  } else
      QTimer::singleShot(100, this, [=]{ processNextXmlFile(); });
}
