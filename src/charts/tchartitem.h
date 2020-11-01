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

#ifndef TCHARTITEM_H
#define TCHARTITEM_H


#include "tmainchart.h"
#include <QtQuick/qquickitem.h>
#include <QtGui/qguiapplication.h>


class Texam;
class Tlevel;
class Taction;
class Tchart;
class TchartTipItem;
class QQuickItem;
class TlevelPreviewItem;
class QTimer;


#define NO_AVER_GR (-100)


/**
 * @class TchartItem is the QML chart control with C++ logic.
 * It exposes recent exams to QML from @p QSettings through @p Taction list @p recentExamsActions.
 * Has @p loadExamAct and loads file from system and adds that file to the above list.
 * Keeps @p selectedFileId number to inform QML what item should be highlighted to user.
 *
 * Displays @p xOrderActions and @p yValueActions which determines exam data sorting.
 * It manages creation of appropriate chart type @p chartType, 
 * prepares exam data to expose in suitable model through @p chartModel
 * and exposes other exam features to QML:
 * - Y axis label: @p yAxisLabel
 * - Y axis grid lines: @p yAxisGridModel
 * Most of that data are tied common signal @p examChanged()
 *
 * It handles information about average line(s) of linear chart.
 * Tracks which group is actually highlighted @p averLineGroup
 *
 * Also it redirects from chart view delegate to QML what chart item is currently hovered
 * and sends information what has to be displayed in corresponding tip.
 */
class TchartItem : public QQuickItem
{

  Q_OBJECT

      /** Properties of analysis window */
  Q_PROPERTY(QList<QObject*> recentExamsActions READ recentExamsActions NOTIFY recentExamsChanged)
  Q_PROPERTY(int selectedFileId READ selectedFileId NOTIFY selectedFileIdChanged)
  Q_PROPERTY(QList<QObject*> miscSettModel READ miscSettModel NOTIFY recentExamsChanged)
  Q_PROPERTY(QStringList yValueActions READ yValueActions NOTIFY yValueActionsChanged)
  Q_PROPERTY(QStringList xOrderActions READ xOrderActions NOTIFY xOrderActionsChanged)
  Q_PROPERTY(QString userName READ userName NOTIFY examChanged)
  Q_PROPERTY(QString questionCount READ questionCount NOTIFY examChanged)
  Q_PROPERTY(QString effectiveness READ effectiveness NOTIFY examChanged)
  Q_PROPERTY(Texam* exam READ exam WRITE setExam)
  Q_PROPERTY(bool allowOpen READ allowOpen WRITE setAllowOpen NOTIFY allowOpenChanged)
  Q_PROPERTY(bool isMelody READ isMelody NOTIFY examChanged)
  Q_PROPERTY(int yValue READ yValue WRITE setYValue NOTIFY yValueChanged)
  Q_PROPERTY(int xOrder READ xOrder WRITE setXOrder NOTIFY xOrderChanged)
  Q_PROPERTY(QString chartWindowTitle READ chartWindowTitle NOTIFY examChanged)
  Q_PROPERTY(QString levelName READ levelName NOTIFY examChanged)

      /** Properties of chart (list view) and its delegate */
  Q_PROPERTY(int chartType READ chartType WRITE setChartType NOTIFY examChanged)
  Q_PROPERTY(int chartModel READ chartModel NOTIFY chartModelChanged)
  Q_PROPERTY(QString yAxisLabel READ yAxisLabel NOTIFY examChanged)
  Q_PROPERTY(QList<qreal> yAxisGridModel READ yAxisGridModel NOTIFY examChanged)
  Q_PROPERTY(qreal averageTime READ averageTime NOTIFY examChanged)
  Q_PROPERTY(int averLineGroup READ averLineGroup NOTIFY averLineGroupChanged)

  Q_PROPERTY(TchartTipItem* tipItem READ tipItem WRITE setTipItem)

public:
  explicit TchartItem(QQuickItem* parent = nullptr);
  ~TchartItem() override;

// Properties of analysis window
  QList<QObject*> recentExamsActions() { return m_recentExamsActs; }

      /**
       * Number of actually loaded file matching position on the recent exams list
       * or @p -1 if none
       */
  int selectedFileId() const { return m_selectedFileId; }
  QList<QObject*> miscSettModel() { return m_miscSettModel; }
  QStringList yValueActions() { return m_yValueActs; }
  QStringList xOrderActions() { return m_xOrderActs; }
  Q_INVOKABLE Taction* loadExamAct() { return m_loadExamAct; }
  Q_INVOKABLE Taction* exerciseAct() { return m_exerciseAct; }

  QString userName() const;
  QString questionCount() const;
  QString effectiveness() const;

  Texam* exam() { return m_exam; }
  void setExam(Texam* e);

  bool allowOpen() const { return m_allowOpen; }
  void setAllowOpen(bool ao);

  bool isMelody() const;

  int yValue() const;
  void setYValue(int yV);

  int xOrder() const;
  void setXOrder(int xO);

  int chartType() const { return static_cast<int>(m_chartSetts.type); }
  void setChartType(int chT = 0); // linear by default

  QString chartWindowTitle() const;
  QString levelName() const;

  Q_INVOKABLE void fillPreview(TlevelPreviewItem* lpi);

  Q_INVOKABLE QString chartHelpText() const;
  Q_INVOKABLE void openExam() { getExamFileSlot(); }


// Properties of chart (list view) and its delegate
  int chartModel() const;

  TmainChart* mainChart() { return static_cast<TmainChart*>(m_chart); }

  QString yAxisLabel() const;

      /**
       * List of y values for help grid lines
       */
  QList<qreal> yAxisGridModel() const;
  Q_INVOKABLE qreal maxYValue() const;
  QString timeFormated(qreal realTime) const;
  qreal averageTime() const;

      /**
       * Text of Y axis grid line value formatted apparently to Y unit
       */
  Q_INVOKABLE QString yAxisTickText(int id);

  Tchart::Tsettings* settings() { return &m_chartSetts; }

// Properties of a tip with question/line/bar info

      /**
       * QML instance of @p TchartTipItem
       */  
  TchartTipItem* tipItem() { return m_tipItem; }
  void setTipItem(TchartTipItem* ti);

  void tipEntered(TtipInfo* ti);
  Q_INVOKABLE void tipEntered();
  Q_INVOKABLE void tipExited();

  TtipInfo* lineTip() { return m_lineTip; }

      /**
       * @p averLineGroup defines group of what average line cursor was entered.
       * By default it is none: NO_AVER_GR (-100)
       * Average line consists of chunk lines of every delegate, 
       * so it allows do discriminate which delegates have to highlight chunk-lines apparently
       */
  int averLineGroup() const { return m_averLineGr; }
  void setAverLineGroup(int averGr);

      /**
       * Questions number
       */
  static QString questionsNumberTxt() { return QGuiApplication::translate("AnalyzeDialog", "Questions number"); }

      /**
       * Attempts number
       */
  static QString attemptsNumberTxt() { return QGuiApplication::translate("AnalyzeDialog", "Attempts number"); }

      /**
       * Preparation time
       */
  static QString prepareTimeTxt() { return QGuiApplication::translate("AnalyzeDialog", "Preparation time"); }

      /**
       * Played number
       */
  static QString playedNumberTxt() { return QGuiApplication::translate("AnalyzeDialog", "Played number"); }

      /**
       * Every chart delegate instances (either for bar or linear chart) reacts on @p examChanged() signal
       * to update its state.
       * But when chart model is reset (it becomes 0 for a while), QML chart view has to destroy all delegates,
       * so corresponding routines have to be ignored and this method signalizes it.
       * So delegate corresponding slot to @p examChanged() has to check @p ignoreSignalExamChanged() state
       * before it will refer to exam data.
       */
  bool ignoreSignalExamChanged() const { return m_ignoreSignalExamChanged; }

  Q_INVOKABLE bool keepDrawerOpened() const { return m_keepDrawerOpened; }
  Q_INVOKABLE void setKeepDrawer(bool k) { m_keepDrawerOpened = k; }

signals:
  void recentExamsChanged();
  void selectedFileIdChanged();
  void yValueActionsChanged();
  void xOrderActionsChanged();
  void questionChanged();
  void chartModelChanged();
  void examChanged();
  void allowOpenChanged();
  void yValueChanged();
  void xOrderChanged();
  void averLineGroupChanged();
  void lockXorderList(int itNr, bool state);
  void resetChartPos();

      /**
       * Emits error message when loading exam fails
       */
  void loadExamFailed(const QString& message);

protected:
  void getExamFileSlot();

  void enterTimeOut();
  void leaveTimeOut();

private:
      /**
       * Loads exam from given file name
       */
  void loadExam(const QString &examFile);

      /**
       * Using @p m_exam initializes chart depending on settings
       * Deletes previous chart object, and sends @p chartModelChanged()
       * but only when @p resetModel is @p true. When @p false chart view remains where it is.
       * Then it creates new chart object (bar or linear) and sends @p examChanged() to inform QML controls to update
       */
  void drawChart(bool resetModel = true);

      /**
       * @p TRUE when given exam melody mode is different than current
       */
  bool singleOrMelodyChanged(Texam* e);

  void resetChartSettings();
  void checkMiscOpts();

private:
  QList<QObject*>                 m_recentExamsActs;
  int                             m_selectedFileId = -1;
  QList<QObject*>                 m_miscSettModel;
  Taction                        *m_loadExamAct, *m_exerciseAct = nullptr, *m_inclWrongAct, *m_wrongSeparateAct;
  QStringList                     m_yValueActs;
  QStringList                     m_xOrderActs;
  Tchart                         *m_chart = nullptr;
  Texam                          *m_exam = nullptr;
  Tlevel                         *m_level;
  bool                            m_wasExamCreated = false;
  Tchart::Tsettings               m_chartSetts;
  TchartTipItem                  *m_tipItem = nullptr;
  bool                            m_allowOpen = true;
  TtipInfo                       *m_hoveredItem = nullptr;
  TtipInfo                       *m_currentItem = nullptr;
  TtipInfo                       *m_lineTip; /**< Instance of @p TtipInfo representing kind of TtipInfo::e_line */
  QTimer                         *m_enterTimer, *m_leaveTimer;
  int                             m_averLineGr = NO_AVER_GR;
  bool                            m_ignoreSignalExamChanged = false;
  bool                            m_keepDrawerOpened;
};

#endif // TCHARTITEM_H
