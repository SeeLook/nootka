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

#include "tchartitem.h"
#include "tlinearchart.h"
#include "tcharttipitem.h"
#include "tyaxis.h"
#include <tglobals.h>
#include <taction.h>
#include <exam/texam.h>
#include <exam/tlevel.h>
#include <exam/textrans.h>
#include <texamparams.h>

#include <QtWidgets/qfiledialog.h>
#include <QtQml/qqmlengine.h>
#include <QtQuick/qquickitem.h>
#include <QtCore/qsettings.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qtimer.h>
#include <QtCore/qdebug.h>
#include "checktime.h"


TchartItem::TchartItem(QQuickItem* parent) :
  QQuickItem(parent),
  m_level(new Tlevel)
{
  auto loadExamAct = new Taction(TexTrans::loadExamFileTxt(), QStringLiteral("nootka-exam"), this);
  connect(loadExamAct, &Taction::triggered, this, &TchartItem::getExamFileSlot);
  //TODO create common method in Taction for creating shortcut
  QQmlEngine e;
  QQmlComponent c(&e, this);
  c.setData("import QtQuick 2.9; Shortcut { sequence: StandardKey.Open }", QUrl());
  auto openShort = c.create();
  openShort->setParent(this);
  loadExamAct->setShortcut(openShort);
  m_recentExamsActs << loadExamAct;
  QStringList recentExams = GLOB->config->value(QStringLiteral("recentExams")).toStringList();
  for (int i = 0; i < recentExams.size(); i++) {
    QFileInfo fi(recentExams[i]);
    if (fi.exists()) {
      auto act = new Taction(fi.fileName(), QString(), this);
      act->setProperty("file", recentExams[i]);
      m_recentExamsActs << act;
      connect(act, &Taction::triggered, this, [=]{ loadExam(sender()->property("file").toString()); });
    }
  }

  m_yValueActs << TexTrans::reactTimeTxt().toLower() << TexTrans::effectTxt().toLower() << TYaxis::prepareTimeTxt().toLower()
               << TYaxis::attemptsNumberTxt().toLower() << TYaxis::playedNumberTxt().toLower();

  emit actionsPrepared();

  m_chart = new Tchart(this); // default "empty" chart
}


TchartItem::~TchartItem() {
  if (m_wasExamCreated)
    delete m_exam;
}


int TchartItem::questionNr() const {
  return m_chart->curQ() ? m_chart->curQ()->nr() : 0;
}


void TchartItem::setTipItem(TchartTipItem* ti) {
  if (ti != m_tipItem) {
    m_tipItem = ti;
  }
}


void TchartItem::setParentHeight(qreal ph) {
  m_parentHeight = ph;
  m_chart->setParentHeight(ph);
  setSize(m_chart->size());
}


QString TchartItem::userName() const {
  return m_exam ? m_exam->userName() : QString();
}


QString TchartItem::questionCount() const {
  return m_exam ? qApp->translate("AnalyzeDialog", "Question number") + QString(": <b><font size=\"4\">%1</font></b>").arg(m_exam->count()) : QString();
}


QString TchartItem::effectiveness() const {
  return m_exam ? TexTrans::effectTxt() + QString(": <b><font size=\"4\">%1%</font></b>").arg(m_exam->effectiveness(), 0, 'f', 1, '0') : QString();
}


void TchartItem::changeChartYvalue(int val) {
  m_chartSetts.yValue = static_cast<TmainLine::EyValue>(val);
  drawChart();
}


void TchartItem::setExam(Texam* e) {
  if (e != m_exam) {
    if (m_wasExamCreated) {
      delete m_exam;
      m_wasExamCreated = false;
    }
    m_exam = e;
    drawChart();
    emit examChanged();
  }
}


void TchartItem::setAllowOpen(bool ao) {
  if (ao != m_allowOpen) {
    m_allowOpen = ao;
    emit allowOpenChanged();
    qobject_cast<Taction*>(m_recentExamsActs.first())->setEnabled(m_allowOpen);
  }
}


bool TchartItem::isMelody() const {
  return m_exam && m_exam->melodies();
}


void TchartItem::zoom(bool in) {
  if (m_chart) {
    setParentHeight(m_parentHeight * (in ? 1.125 : 0.888889));
  }
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TchartItem::hoverChangedSlot() {
  m_tipItem->setQuestion(m_chart->curQ() ? m_chart->curQ() : nullptr);
}


void TchartItem::getExamFileSlot() {
  QString fileName = QFileDialog::getOpenFileName(nullptr, TexTrans::loadExamFileTxt(), GLOB->E->examsDir, TexTrans::examFilterTxt());
  if (!fileName.isEmpty())
    loadExam(fileName);
}


void TchartItem::wheelEvent(QWheelEvent* event) {
  if (event->modifiers() == Qt::ControlModifier) {
      if  (event->angleDelta().y() > 0)
        zoom(true);
      else if  (event->angleDelta().y() < 0)
        zoom(false);
  } else // normal wheel behavior - scrolling a chart
      QQuickItem::wheelEvent(event);
}


//#################################################################################################
//###################              PRIVATE             ############################################
//#################################################################################################

void TchartItem::loadExam(const QString& examFile) {
  if (m_exam)
    delete m_exam;
  m_exam = new Texam(m_level, QString());
  m_wasExamCreated = true; // delete exam in destructor
  if (m_exam->loadFromFile(examFile) == Texam::e_file_OK) {
    drawChart();
    emit examChanged();
  }
}


void TchartItem::drawChart() {
  if (m_exam) {
    qDebug() << "[TanalyzeObject] preparing new chart of exam" << m_exam->userName() << m_exam->count();
//     auto chartParent = m_chart->parentItem();
    m_chart->deleteLater();
//     if (chartSett.type == Tchart::e_linear)
  CHECKTIME(
    auto newChart = new TlinearChart(this);
    newChart->setExam(m_exam);
    newChart->setChartSettings(m_chartSetts);
    newChart->init();
    m_chart = newChart;
    m_chart->setParentHeight(height());
    connect(m_chart, &Tchart::hoveredChanged, this, &TchartItem::hoverChangedSlot);
    m_tipItem->setExam(m_exam);
    setParentHeight(m_parentHeight);
  )
//     else
//       m_chart = new TbarChart(m_exam, m_chartSetts, this);
  }
}
