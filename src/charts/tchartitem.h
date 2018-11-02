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

#ifndef TCHARTITEM_H
#define TCHARTITEM_H


#include "tchart.h"
#include <QtQuick/qquickitem.h>


class Texam;
class Tlevel;
class Taction;
class Tchart;
class TchartTipItem;
class QQuickItem;


/**
 * @class TchartItem is the QML chart control with C++ logic.
 * It exposes recent exams to QML from @p QSettings through @p Taction list @p recentExamsActions
 * and loads appropriate file
 */
class TchartItem : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(QList<QObject*> recentExamsActions READ recentExamsActions NOTIFY actionsPrepared)
  Q_PROPERTY(QStringList yValueActions READ yValueActions NOTIFY actionsPrepared)
  Q_PROPERTY(QStringList xOrderActions READ xOrderActions NOTIFY actionsPrepared)
  Q_PROPERTY(int questionNr READ questionNr NOTIFY questionChanged)
  Q_PROPERTY(TchartTipItem* tipItem READ tipItem WRITE setTipItem)
  Q_PROPERTY(qreal parentHeight READ parentHeight WRITE setParentHeight)
  Q_PROPERTY(QString userName READ userName NOTIFY examChanged)
  Q_PROPERTY(QString questionCount READ questionCount NOTIFY examChanged)
  Q_PROPERTY(QString effectiveness READ effectiveness NOTIFY examChanged)
  Q_PROPERTY(Texam* exam READ exam WRITE setExam)
  Q_PROPERTY(bool allowOpen READ allowOpen WRITE setAllowOpen NOTIFY allowOpenChanged)
  Q_PROPERTY(bool isMelody READ isMelody NOTIFY examChanged)
  Q_PROPERTY(int yValue READ yValue WRITE setYValue NOTIFY yValueChanged)
  Q_PROPERTY(int xOrder READ xOrder WRITE setXOrder NOTIFY xOrderChanged)

public:
  explicit TchartItem(QQuickItem* parent = nullptr);
  ~TchartItem() override;

  QList<QObject*> recentExamsActions() { return m_recentExamsActs; }
  QStringList yValueActions() { return m_yValueActs; }
  QStringList xOrderActions() { return m_xOrderActs; }

  int questionNr() const;

  TchartTipItem* tipItem() { return m_tipItem; }
  void setTipItem(TchartTipItem* ti);

  qreal parentHeight() const { return m_parentHeight; }
  void setParentHeight(qreal ph);

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

      /**
       * zoom chart, by default zoom in but when false zoom out
       */
  Q_INVOKABLE void zoom(bool in);

signals:
  void actionsPrepared();
  void questionChanged();
  void examChanged();
  void allowOpenChanged();
  void yValueChanged();
  void xOrderChanged();

protected:
  void hoverChangedSlot();
  void getExamFileSlot();

  void wheelEvent(QWheelEvent* event) override;

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
  qreal                           m_parentHeight = 0.0;
  bool                            m_allowOpen = true;
};

#endif // TCHARTITEM_H
