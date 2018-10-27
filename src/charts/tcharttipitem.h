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

#ifndef TCHARTTIPITEM_H
#define TCHARTTIPITEM_H


#include <QtQuick/qquickitem.h>


class TtipInfo;
class TscoreObject;
class Texam;


/**
 * C++ logic of a chart tip.
 * @p setQuestion(@p TtipInfo) is called from outside.
 * Depends of TtipInfo::Ekind a bunch of signals is emitted and QML ChartItem is set.
 * @p questionUpdated signal is general type for all kinds of tips (to update tip color and position)
 * @p questionTipUpdated and @p lineTipUpdated() are apparently for question tip and line tip
 * @p showChanged is emitted to inform QML about tip visibility
 */
class TchartTipItem : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(int number READ number NOTIFY questionTipUpdated)
  Q_PROPERTY(bool show READ show NOTIFY showChanged)
  Q_PROPERTY(QColor color READ color NOTIFY questionUpdated)
  Q_PROPERTY(QPointF pos READ pos NOTIFY questionUpdated)
  Q_PROPERTY(QString qaText READ qaText NOTIFY questionTipUpdated)
  Q_PROPERTY(bool isMelody READ isMelody NOTIFY examChanged)
  Q_PROPERTY(bool hasSecondScore READ hasSecondScore NOTIFY examChanged)
  Q_PROPERTY(TscoreObject* leftScore READ leftScore WRITE setLeftScore)
  Q_PROPERTY(TscoreObject* secondScore READ secondScore WRITE setSecondScore)
  Q_PROPERTY(QString resultText READ resultText NOTIFY questionTipUpdated)
  Q_PROPERTY(QString timeText READ timeText NOTIFY questionTipUpdated)
  Q_PROPERTY(int tipType READ tipType NOTIFY tipTypeChanged)
  Q_PROPERTY(QString tipText READ tipText NOTIFY lineTipUpdated)

public:
  explicit TchartTipItem(QQuickItem* parent = nullptr);
  ~TchartTipItem();

  int number() const;
  bool show() const { return m_question != nullptr; }
  QColor color() const;
  QPointF pos() const;
  QString qaText() const;
  bool isMelody() const;
  bool hasSecondScore() const;

  TscoreObject* leftScore() { return m_leftScore; }
  void setLeftScore(TscoreObject* ls);

  TscoreObject* secondScore() { return m_secondScore; }
  void setSecondScore(TscoreObject* ss);

  QString resultText() const;
  QString timeText() const;
  QString tipText() const;

  void setQuestion(TtipInfo* q);

  void setExam(Texam* e);

  int tipType() const;

signals:
  void examChanged();
  void questionUpdated();
  void questionTipUpdated();
  void lineTipUpdated();
  void tipTypeChanged();
  void showChanged();

private:
  TtipInfo                *m_question = nullptr;
  TscoreObject            *m_leftScore = nullptr;
  TscoreObject            *m_secondScore = nullptr;
  Texam                   *m_exam = nullptr;
  int                      m_kindOfTip = -1;
};

#endif // TCHARTTIPITEM_H
