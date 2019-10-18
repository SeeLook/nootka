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

#ifndef TCHARTITEM_H
#define TCHARTITEM_H


#include "tmainchart.h"
#include <QtQuick/qquickitem.h>


class Texam;
class Tlevel;
class Taction;
class Tchart;
class TchartTipItem;
class QQuickItem;
class TlevelPreviewItem;
class QTimer;


/**
 * @class TchartItem is the QML chart control with C++ logic.
 * It exposes recent exams to QML from @p QSettings through @p Taction list @p recentExamsActions
 * and loads appropriate file
 */
class TchartItem : public QQuickItem
{

  Q_OBJECT

      /** Properties of analysis window */
  Q_PROPERTY(QList<QObject*> recentExamsActions READ recentExamsActions NOTIFY actionsPrepared)
  Q_PROPERTY(QStringList yValueActions READ yValueActions NOTIFY actionsPrepared)
  Q_PROPERTY(QStringList xOrderActions READ xOrderActions NOTIFY actionsPrepared)
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
  Q_PROPERTY(int chartModel READ chartModel NOTIFY examChanged)
  Q_PROPERTY(QString yAxisLabel READ yAxisLabel NOTIFY examChanged)
  Q_PROPERTY(QList<qreal> yAxisGridModel READ yAxisGridModel NOTIFY examChanged)
  Q_PROPERTY(qreal averageTime READ averageTime NOTIFY examChanged)

  Q_PROPERTY(TchartTipItem* tipItem READ tipItem WRITE setTipItem)

public:
  explicit TchartItem(QQuickItem* parent = nullptr);
  ~TchartItem() override;

// Properties of analysis window
  QList<QObject*> recentExamsActions() { return m_recentExamsActs; }
  QStringList yValueActions() { return m_yValueActs; }
  QStringList xOrderActions() { return m_xOrderActs; }

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

  Q_INVOKABLE void setChartType(bool lin = true);

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
  Q_INVOKABLE QString timeFormated(qreal realTime, bool halfAllowed = false) const;
  qreal averageTime() const;

  Tchart::Tsettings* settings() { return &m_chartSetts; }

// Properties of a tip with question/line/bar info

      /**
       * QML instance of @p TchartTipItem
       */  
  TchartTipItem* tipItem() { return m_tipItem; }
  void setTipItem(TchartTipItem* ti);

  Q_INVOKABLE void tipEntered(TtipInfo* ti);
  Q_INVOKABLE void tipExited();

signals:
  void actionsPrepared();
  void questionChanged();
  void examChanged();
  void allowOpenChanged();
  void yValueChanged();
  void xOrderChanged();

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
       */
  void drawChart();

      /**
       * @p TRUE when given exam melody mode is different than current
       */
  bool singleOrMelodyChanged(Texam* e);

  void resetChartSettings();

private:
  QList<QObject*>                 m_recentExamsActs;
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
  QTimer                         *m_enterTimer, *m_leaveTimer;
};

#endif // TCHARTITEM_H
