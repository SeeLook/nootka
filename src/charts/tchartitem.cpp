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

#include "tchartitem.h"
#include "tlinearchart.h"
#include "tbarchart.h"
#include "tcharttipitem.h"
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
  GLOB->config->beginGroup(QStringLiteral("Charts"));
  m_keepDrawerOpened = GLOB->config->value(QStringLiteral("keepDrawerOpened"), false).toBool();
  GLOB->config->endGroup();
  m_loadExamAct = new Taction(TexTrans::loadExamFileTxt(), QStringLiteral("nootka-exam"), this);
  connect(m_loadExamAct, &Taction::triggered, this, &TchartItem::getExamFileSlot);
  QQmlComponent c(NOO->qmlEngine(), this);
  m_loadExamAct->createQmlShortcut(&c, "StandardKey.Open");

  QString exerciseFile = QDir::toNativeSeparators(QFileInfo(GLOB->config->fileName()).absolutePath() + "/exercise2.noo");
  if (QFileInfo(exerciseFile).exists()) {
    Tlevel lev;
    Texam ex(&lev, QString());
    ex.loadFromFile(exerciseFile);
    m_exerciseAct = new Taction(qApp->translate("AnalyzeDialog", "Recent exercise on level")
                        + QLatin1String(": ") + lev.name, QStringLiteral("practice"), this);
    m_exerciseAct->setProperty("file", exerciseFile);
    connect(m_exerciseAct, &Taction::triggered, this, [=]{ loadExam(sender()->property("file").toString()); });
  }

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
    drawChart(false);
  });
  m_wrongSeparateAct = new Taction(qApp->translate("AnalyzeDialog", "show wrong answers separately"), QString(), this);
  m_wrongSeparateAct->setCheckable(true);
  m_wrongSeparateAct->setChecked(m_chartSetts.separateWrong);
  connect(m_wrongSeparateAct, &Taction::triggered, this, [=]{
    m_wrongSeparateAct->setChecked(!m_wrongSeparateAct->checked());
    m_chartSetts.separateWrong = m_wrongSeparateAct->checked();
    m_inclWrongAct->setEnabled(!m_chartSetts.separateWrong); // nothing to include when wrongs are separated
    drawChart(false);
  });
  m_miscSettModel << m_wrongSeparateAct << m_inclWrongAct;
  m_wrongSeparateAct->setEnabled(false);
  m_inclWrongAct->setEnabled(false);

  m_lineTip = new TtipInfo();
  m_lineTip->kind = TtipInfo::e_line;

  m_yValueActs << TexTrans::reactTimeTxt().toLower() << TexTrans::effectTxt().toLower() << prepareTimeTxt().toLower()
               << attemptsNumberTxt().toLower() << playedNumberTxt().toLower();
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
  GLOB->config->beginGroup(QStringLiteral("Charts"));
  GLOB->config->setValue(QStringLiteral("keepDrawerOpened"), m_keepDrawerOpened);
  GLOB->config->endGroup();
  if (m_wasExamCreated)
    delete m_exam;
  delete m_lineTip;
  delete m_level;
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
    drawChart(false); // do not rewind chart, just switch Y value
    emit yValueChanged();
  }
}


int TchartItem::xOrder() const { return static_cast<int>(m_chartSetts.order); }

void TchartItem::setXOrder(int xO) {
  if (xO != xOrder()) {
    m_chartSetts.order = static_cast<Tchart::EanswersOrder>(xO);
    checkMiscOpts();
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
    if (!m_recentExamsActs.isEmpty())
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
  if (m_exam) {
    if (m_exam->isExercise())
      return QApplication::translate("AnalyzeDialog", "Analysis of exercise");
    else
      return QApplication::translate("AnalyzeDialog", "Analysis of exam results");
  }
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
                        .arg(QLatin1String("<a href=\"charts\"> ") + NOO->pixToHtml("chartSett", NOO->fontSize() * 3) + QLatin1String("</a> "))
            + QLatin1String("<br>") + QApplication::translate("AnalyzeDialog", "Use %1 + mouse wheel or %2 buttons to zoom a chart.")
                        .arg(modKey, NOO->pixToHtml("zoom-in", NOO->fontSize() * 2) + QLatin1String(" ") + NOO->pixToHtml("zoom-out", NOO->fontSize() * 2))
            + QLatin1String("<br>") + QApplication::translate("AnalyzeDialog", "Click and Drag the cursor to move the chart.");
}


int TchartItem::chartModel() const {
  return m_exam && m_chart ? m_chart->xCount() : 0;
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


QString TchartItem::timeFormated(qreal realTime) const {
  int t = static_cast<int>(realTime);
  QString hh, mm, ss, ms, u;
  int dig = 0;
  if (m_chart->maxValue() > 3599.0 && t / 3600) {
    hh = QString("%1").arg(t / 3600);
    dig = 2;
  }
  int dig2 = 0;
  if (m_chart->maxValue() > 59.0 && (t % 3600) / 60) {
    mm = QString("%1").arg((t % 3600) / 60, dig, 'i', 0, '0');
    dig2 = 2;
  }
  ss = QString("%1").arg((t % 3600) % 60, m_chart->maxValue() > 59.0 && realTime > 9.0 ? 2 : 1, 'i', 0, '0');
  if (realTime - static_cast<qreal>(t))
    ms = QLatin1String(".") + QString("%1").arg((int)((realTime - static_cast<qreal>(t)) * 10.0));
  if (m_chart->maxValue() < 60.0)
    u = QStringLiteral("s");
  return (hh.isEmpty() ? QString() : hh + QLatin1String(":")) + (mm.isEmpty() ? QString() : mm + QLatin1String(":")) + ss + ms + u;
}


qreal TchartItem::averageTime() const {
  return m_exam ? static_cast<qreal>(m_exam->averageReactonTime()) / 10.0 : 0.0;
}


QString TchartItem::yAxisTickText(int id) {
  if (m_chart && id > -1 && id < m_chart->yTickList().size()) {
    if ((m_chartSetts.yValue == Tchart::e_YquestionTime || m_chartSetts.yValue == Tchart::e_YprepareTime)
        && m_chartSetts.order != Tchart::e_byMistake)
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


/**
 * Overloaded method called from QML
 * It invokes @p tipEntered(question) when the question info exists
 */
void TchartItem::tipEntered() {
  if (m_tipItem && m_tipItem->question())
    tipEntered(m_tipItem->question());
}


void TchartItem::tipEntered(TtipInfo* ti) {
  m_enterTimer->start(300);
  m_leaveTimer->stop();
  m_hoveredItem = ti;
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
  delete m_level;
  m_level = new Tlevel();
  m_exam = new Texam(m_level, QString());
  if (m_exam->loadFromFile(examFile) == Texam::e_file_OK) {
      m_wasExamCreated = true; // delete exam in destructor
      resetChartSettings();
      drawChart();
      if (sender() != m_exerciseAct) {
          bool foundTheSame = false;
          for (int e = 0; e < m_recentExamsActs.size(); e++) {
            if (m_recentExamsActs[e]->property("file").toString() == examFile) {
              m_selectedFileId = e;
              foundTheSame = true;
              break;
            }
          }
          if (!foundTheSame) {
            QFileInfo fi(examFile);
            auto act = new Taction(fi.fileName(), QString(), this);
            act->setProperty("file", examFile);
            m_recentExamsActs.prepend(act);
            connect(act, &Taction::triggered, this, [=]{ loadExam(sender()->property("file").toString()); });
            emit recentExamsChanged();
            m_selectedFileId = 0;
          }
      } else {
          m_selectedFileId = -1;
      }
  } else {
      delete m_exam;
      m_exam = nullptr;
      emit chartModelChanged();
      emit loadExamFailed(QLatin1String("<h4>")
                + qTR("TexamExecutor", "File: %1 \n is not valid exam file!").arg(examFile)
                                                .replace(QLatin1String("\n"), QLatin1String("<br>"))
                                                .replace(QLatin1String(":"), QLatin1String(":<br>"))
                + QLatin1String("</h4>"));
      m_selectedFileId = -1;
  }
  emit selectedFileIdChanged();
}


void TchartItem::drawChart(bool resetModel) {
  if (m_exam) {
    delete m_chart;
    m_chart = nullptr;
    if (resetModel)
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
    m_ignoreSignalExamChanged = resetModel;
    emit examChanged();
    m_ignoreSignalExamChanged = false;
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
  checkMiscOpts();
  if (m_exam) {
    emit lockXorderList(static_cast<int>(Tchart::e_byNumber), false);
    emit lockXorderList(static_cast<int>(Tchart::e_byNote), m_exam->melodies());
    emit lockXorderList(static_cast<int>(Tchart::e_byFret), m_exam->melodies() || !Tinstrument(m_exam->level()->instrument).isGuitar());
    emit lockXorderList(static_cast<int>(Tchart::e_byAccid),
                        m_exam->melodies() || !(m_exam->level()->withSharps || m_exam->level()->withFlats || m_exam->level()->withDblAcc));
    emit lockXorderList(static_cast<int>(Tchart::e_byKey),
                        m_exam->melodies() || !(m_exam->level()->useKeySign && !m_exam->level()->isSingleKey));
    emit lockXorderList(static_cast<int>(Tchart::e_byMistake), m_exam->melodies());
    emit lockXorderList(static_cast<int>(Tchart::e_byQuestAndAnsw), m_exam->melodies());
  }
  emit resetChartPos();
}


void TchartItem::checkMiscOpts() {
  if (m_chartSetts.order == Tchart::e_byNumber) {
      if (!isMelody()) {
        m_wrongSeparateAct->setEnabled(false);
        m_inclWrongAct->setEnabled(false);
      }
  } else if (m_chartSetts.order == Tchart::e_byMistake || m_chartSetts.order == Tchart::e_byQuestAndAnsw) {
      m_wrongSeparateAct->setChecked(false); // with this kind of sorting separating wrongs makes no sense.
      m_chartSetts.separateWrong = false;  // Uncheck when necessary
      m_wrongSeparateAct->setEnabled(false);
      m_inclWrongAct->setEnabled(true);
  } else {
      m_wrongSeparateAct->setEnabled(!isMelody() && m_chartSetts.type != Tchart::e_bar);
      m_inclWrongAct->setEnabled(!m_chartSetts.separateWrong);
  }  
}
