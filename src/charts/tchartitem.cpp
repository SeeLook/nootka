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

#include "tchartitem.h"
#include "tlinearchart.h"
#include "tbarchart.h"
#include "tcharttipitem.h"
#include "tyaxis.h"
#include "dialogs/tlevelpreviewitem.h"
#include <tglobals.h>
#include <taction.h>
#include <exam/texam.h>
#include <exam/tlevel.h>
#include <exam/textrans.h>
#include <texamparams.h>
#include <tnootkaqml.h>
#include <Android/tfiledialog.h>

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
  m_xOrderActs << qApp->translate("TanalysDialog", "question number", "see comment in 'ordered by:' entry")
               << qApp->translate("TanalysDialog", "note pitch") << qApp->translate("TanalysDialog", "fret number")
               << qApp->translate("TanalysDialog", "accidentals") << qApp->translate("TanalysDialog", "key signature")
               << qApp->translate("TanalysDialog", "mistake") << qApp->translate("TanalysDialog", "question type");

  /** NOTE
   * Clazy tool blames emit call beneath that probably emit has no effect because it is made from constructor.
   * However clazy seems to be paranoid sometimes, cause it works fine.
   * To keep it quiet one may add 'no-incorrect-emit' to level 1 check when it is invoking.
   */
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
  setWidth(m_chart->width());
  setHeight(m_chart->height());
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


int TchartItem::yValue() const { return static_cast<int>(m_chartSetts.yValue); }

void TchartItem::setYValue(int yV) {
  if (yV != yValue()) {
    m_chartSetts.yValue = static_cast<TmainLine::EyValue>(yV);
    drawChart();
    emit yValueChanged();
  }
}


int TchartItem::xOrder() const { return static_cast<int>(m_chartSetts.order); }

void TchartItem::setXOrder(int xO) {
  if (xO != xOrder()) {
    m_chartSetts.order = static_cast<Tchart::EanswersOrder>(xO);
    drawChart();
    emit xOrderChanged();
  }
}



void TchartItem::setExam(Texam* e) {
  if (e != m_exam) {
    if (m_wasExamCreated) {
      delete m_exam;
      m_wasExamCreated = false;
    }
    if (singleOrMelodyChanged(e))
      resetChartSettings();
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


void TchartItem::setChartType(bool lin) {
  if (lin) {
      if (m_chartSetts.type != Tchart::e_linear) {
        m_chartSetts.type = Tchart::e_linear;
        drawChart();
      }
  } else {
      if (m_chartSetts.type != Tchart::e_bar) {
        m_chartSetts.type = Tchart::e_bar;
        if (m_chartSetts.order == Tchart::e_byNumber) {
          m_chartSetts.order = Tchart::e_byNote;
          emit xOrderChanged();
        }
        drawChart();
      }
  }
}


QString TchartItem::chartWindowTitle() const {
  if (m_exam)
    return QApplication::translate("AnalyzeDialog", m_exam->isExercise() ? "Analysis of exercise" : "Analysis of exam results");
  return QApplication::translate("AnalyzeDialog", "Analyze");
}


QString TchartItem::levelName() const {
  return m_exam ? m_exam->level()->name : QString();
}

void TchartItem::fillPreview(TlevelPreviewItem* lpi) {
  if (m_exam && lpi)
    lpi->setLevel(m_exam->level());
}


QString TchartItem::chartHelpText() const {
#if defined(Q_OS_MAC)
    auto modKey = QStringLiteral("CMD");
#else
    auto modKey = QStringLiteral("CTRL");
#endif
  return QApplication::translate("AnalyzeDialog", "Press %1 button to select an exam from a file.")
                        .arg(QLatin1String("<a href=\"charts\"> ") + NOO->pixToHtml("nootka-exam", NOO->fontSize() * 3) + QLatin1String("</a> "))
            + QLatin1String("<br>") + QApplication::translate("AnalyzeDialog", "Use %1 + mouse wheel or %2 buttons to zoom a chart.")
                        .arg(modKey, NOO->pixToHtml("zoom-in", NOO->fontSize() * 2) + QLatin1String(" ") + NOO->pixToHtml("zoom-out", NOO->fontSize() * 2))
            + QLatin1String("<br>") + QApplication::translate("AnalyzeDialog", "Click and Drag the cursor to move the chart.");
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TchartItem::hoverChangedSlot() {
  m_tipItem->setQuestion(m_chart->curQ() ? m_chart->curQ() : nullptr);
}


void TchartItem::getExamFileSlot() {
  QString fileName = TfileDialog::getOpenFileName(TexTrans::loadExamFileTxt(), GLOB->E->examsDir, TexTrans::examFilterTxt());
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
    resetChartSettings();
    drawChart();
    emit examChanged();
  }
}


void TchartItem::drawChart() {
  if (m_exam) {
    m_chart->deleteLater();
  CHECKTIME(
    TmainChart *newChart;
    if (m_chartSetts.type == Tchart::e_linear)
      newChart = new TlinearChart(this);
    else
      newChart = new TbarChart(this);
    newChart->setExam(m_exam);
    newChart->setChartSettings(m_chartSetts);
    newChart->init();
    m_chart = newChart;
    m_chart->setParentHeight(height());
    connect(m_chart, &Tchart::hoveredChanged, this, &TchartItem::hoverChangedSlot);
    m_tipItem->setExam(m_exam);
    setParentHeight(m_parentHeight);
  )
  }
}


bool TchartItem::singleOrMelodyChanged(Texam* e) {
  if (e && m_exam)
    return e->melodies() != m_exam->melodies();
  else
    return true;
}


void TchartItem::resetChartSettings() {
  m_chartSetts.type = Tchart::e_linear;
  m_chartSetts.yValue = TmainLine::e_questionTime;
  emit yValueChanged();
  m_chartSetts.order = Tchart::e_byNumber;
  emit xOrderChanged();
}
