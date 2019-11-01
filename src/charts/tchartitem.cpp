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
#include <qtr.h>
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

  m_inclWrongAct = new Taction(qApp->translate("AnalyzeDialog", "include time of wrong answers to average"), QString(), this);
  m_inclWrongAct->setCheckable(true);
  connect(m_inclWrongAct, &Taction::triggered, this, [=]{
    m_inclWrongAct->setChecked(!m_inclWrongAct->checked());
    m_chartSetts.inclWrongAnsw = m_inclWrongAct->checked();
    drawChart();
  });
  m_miscSettModel << m_inclWrongAct;
  m_wrongSeparateAct = new Taction(qApp->translate("AnalyzeDialog", "show wrong answers separately"), QString(), this);
  m_wrongSeparateAct->setCheckable(true);
  m_wrongSeparateAct->setChecked(m_chartSetts.separateWrong);
  connect(m_wrongSeparateAct, &Taction::triggered, this, [=]{
    m_wrongSeparateAct->setChecked(!m_wrongSeparateAct->checked());
    m_chartSetts.separateWrong = m_wrongSeparateAct->checked();
    drawChart();
  });
  m_miscSettModel << m_wrongSeparateAct;

  m_lineTip = new TtipInfo();
  m_lineTip->kind = TtipInfo::e_line;

  m_yValueActs << TexTrans::reactTimeTxt().toLower() << TexTrans::effectTxt().toLower() << TYaxis::prepareTimeTxt().toLower()
               << TYaxis::attemptsNumberTxt().toLower() << TYaxis::playedNumberTxt().toLower();
  m_xOrderActs << qApp->translate("AnalyzeDialog", "question number", "see comment in 'ordered by:' entry")
               << qApp->translate("AnalyzeDialog", "note pitch") << qApp->translate("AnalyzeDialog", "fret number")
               << qApp->translate("AnalyzeDialog", "accidentals") << qApp->translate("AnalyzeDialog", "key signature")
               << qApp->translate("AnalyzeDialog", "mistake") << qApp->translate("AnalyzeDialog", "question type");

  /** NOTE
   * Clazy tool blames emit call beneath that probably this emitting has no effect because it is made from constructor.
   * However clazy seems to be paranoid sometimes, cause it works fine.
   * To keep it quiet one may add 'no-incorrect-emit' to level 1 check when it is invoking.
   */
  emit recentExamsChanged();
  emit xOrderActionsChanged();

  m_enterTimer = new QTimer(this);
  connect(m_enterTimer, &QTimer::timeout, this, &TchartItem::enterTimeOut);
  m_leaveTimer = new QTimer(this);
  connect(m_leaveTimer, &QTimer::timeout, this, &TchartItem::leaveTimeOut);

  m_chart = new Tchart(this); // default "empty" chart
}


TchartItem::~TchartItem() {
  if (m_wasExamCreated)
    delete m_exam;
  delete m_lineTip;
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
    m_chartSetts.yValue = static_cast<Tchart::EyValue>(yV);
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


void TchartItem::setChartType(int chT) {
  auto chType = static_cast<Tchart::EchartType>(chT);
  if (chType != m_chartSetts.type) {
    m_chartSetts.type = chType;
    if (chType == Tchart::e_bar) {
        if (m_chartSetts.order == Tchart::e_byNumber) {
          m_chartSetts.order = Tchart::e_byNote;
          emit xOrderChanged();
        }
        emit lockXorderList(static_cast<int>(Tchart::e_byNumber), true); // disable 'order by number' from bar chart - it makes no sense there
    } else {
        emit lockXorderList(static_cast<int>(Tchart::e_byNumber), false); // restore 'order by number'
    }
    drawChart();
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


int TchartItem::chartModel() const {
  return m_chart ? m_chart->xCount() : 0;
}


QString TchartItem::yAxisLabel() const {
  return m_chart ? m_chart->yAxisLabel() : QString();
}


QList<qreal> TchartItem::yAxisGridModel() const {
  return m_chart ? m_chart->yTickList() : QList<qreal>();
}


qreal TchartItem::maxYValue() const {
  return m_chart ? m_chart->maxValue() : 1.0;
}


QString TchartItem::timeFormated(qreal realTime, bool halfAllowed) const {
  int t = static_cast<int>(realTime);
  QString hh, mm = halfAllowed ? QString() : QStringLiteral("0"), ss, ms;
  int dig = 0;
  if (t / 3600) {
    hh = QString("%1").arg(t / 3600);
    dig = 2;
  }
  int dig2 = 0;
  if ((t % 3600) / 60) {
    mm = QString("%1").arg((t % 3600) / 60, dig, 'i', 0, '0');
    dig2 = 2;
  }
  ss = QString("%1").arg((t % 3600) % 60, 2, 'i', 0, '0');
  if (realTime - (qreal)t)
    ms = QLatin1String(".") + QString("%1").arg((int)((realTime - (qreal)t) * 10));
  return (hh.isEmpty() ? QString() : hh + QLatin1String(":")) + (mm.isEmpty() ? QString() : mm + QLatin1String(":")) + ss + ms;
}


qreal TchartItem::averageTime() const {
  return m_exam ? static_cast<qreal>(m_exam->averageReactonTime()) / 10.0 : 0.0;
}


QString TchartItem::yAxisTickText(int id) {
  if (m_chart && id > -1 && id < m_chart->yTickList().size()) {
    if (m_chartSetts.yValue == Tchart::e_YquestionTime)
      return timeFormated(m_chart->yTickList()[id]);
    return QString::number(m_chart->yTickList()[id])
                  + (m_chartSetts.yValue == Tchart::e_Yeffectiveness ? QLatin1String("%") : QString());
  }
  return QString();
}


//#################################################################################################
//#############  Properties of a tip with question/line/bar info ##################################
//#################################################################################################

void TchartItem::setTipItem(TchartTipItem* ti) {
  if (ti != m_tipItem) {
    m_tipItem = ti;
  }
}


void TchartItem::tipEntered(TtipInfo* ti) {
//   if (m_hoveredItem != ti) {
    m_enterTimer->start(300);
    m_leaveTimer->stop();
    m_hoveredItem = ti;
//   }
}


void TchartItem::tipExited() {
  m_enterTimer->stop();
  m_leaveTimer->start(500);
}


void TchartItem::setAverLineGroup(int averGr) {
  if (averGr != m_averLineGr) {
    m_averLineGr = averGr;
    emit averLineGroupChanged();
  }
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TchartItem::getExamFileSlot() {
  QString fileName = TfileDialog::getOpenFileName(TexTrans::loadExamFileTxt(), GLOB->E->examsDir, TexTrans::examFilterTxt());
  if (!fileName.isEmpty())
    loadExam(fileName);
}


void TchartItem::enterTimeOut() {
  m_enterTimer->stop();
  if (m_hoveredItem)
    m_tipItem->setQuestion(m_hoveredItem);
  else
    qDebug() << "[TmainChart] FIXME! No hovered item!";
}


void TchartItem::leaveTimeOut() {
  m_leaveTimer->stop();
  m_hoveredItem = nullptr;
  m_tipItem->setQuestion(nullptr);
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
  }
}


void TchartItem::drawChart() {
  if (m_exam) {
    m_chart->deleteLater();
  m_chart = nullptr;
  emit chartModelChanged();
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
    m_tipItem->setExam(m_exam);
    emit chartModelChanged();
    emit examChanged();
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
  m_chartSetts.yValue = Tchart::e_YquestionTime;
  emit yValueChanged();
  m_chartSetts.order = Tchart::e_byNumber;
  emit xOrderChanged();
  if (m_exam) {
    emit lockXorderList(static_cast<int>(Tchart::e_byNumber), false);
    emit lockXorderList(static_cast<int>(Tchart::e_byNote), m_exam->melodies());
    emit lockXorderList(static_cast<int>(Tchart::e_byFret), m_exam->melodies());
    emit lockXorderList(static_cast<int>(Tchart::e_byAccid), m_exam->melodies());
    emit lockXorderList(static_cast<int>(Tchart::e_byKey), m_exam->melodies());
    emit lockXorderList(static_cast<int>(Tchart::e_byMistake), m_exam->melodies());
    emit lockXorderList(static_cast<int>(Tchart::e_byQuestAndAnsw), m_exam->melodies());
  }
  emit resetChartPos();
}
