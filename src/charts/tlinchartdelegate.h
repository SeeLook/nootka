/***************************************************************************
 *   Copyright (C) 2019 by Tomasz Bojczuk                                  *
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

#ifndef TLINCHARTDELEGATE_H
#define TLINCHARTDELEGATE_H


#include <QtQuick/qquickpainteditem.h>
#include <QtGui/qpainter.h>


class TchartItem;
class TQAunit;
class TtipInfo;


/**
 * C++ logic for linear delegate of QML Nootka chart.
 * Exposes data of singe question to chart (QML ListView)
 * But it paints staff lines and average progress using @p QPainter
 */
class TlinChartDelegate : public QQuickPaintedItem {

  Q_OBJECT

  Q_PROPERTY(TchartItem* chart READ chart WRITE setChart NOTIFY examChanged)
  Q_PROPERTY(int chartNr READ chartNr WRITE setChartNr NOTIFY nrChanged)
  Q_PROPERTY(int groupNr READ groupNr NOTIFY nrChanged)
  Q_PROPERTY(qreal pointY READ pointY NOTIFY pointYChanged)
  Q_PROPERTY(QColor pointColor READ pointColor NOTIFY examChanged)
  Q_PROPERTY(QString pointSymbol READ pointSymbol NOTIFY examChanged)
  Q_PROPERTY(qreal averageY READ averageY NOTIFY examChanged)
    // X axis text column
  Q_PROPERTY(QString nrText READ nrText NOTIFY examChanged)
  Q_PROPERTY(QString noteText READ noteText NOTIFY examChanged)
  Q_PROPERTY(QString posText READ posText NOTIFY examChanged)
  Q_PROPERTY(QString keyText READ keyText NOTIFY examChanged)

public:
  explicit TlinChartDelegate(QQuickItem* parent = nullptr);
  ~TlinChartDelegate() override;

  void paint(QPainter* painter) override;

  TchartItem* chart() { return m_chart; }
  void setChart(TchartItem* ch);

  int chartNr() const { return m_nrInchart; }
  void setChartNr(int n);
  int groupNr() const;

  qreal pointY() const;
  QColor pointColor() const;
  QString pointSymbol() const;

  // X axis text column
  QString nrText() const;
  QString noteText() const;
  QString posText() const;
  QString keyText() const;

      /**
       * For ordered by number it is average reaction time of entire exam
       * and for the rest linear sorting order is average value of a group delegate belongs to
       * @p 0.0 should be ignored
       */
  qreal averageY() const;

  Q_INVOKABLE void pointEntered();
  Q_INVOKABLE void pointExited();
  Q_INVOKABLE void lineEntered(qreal posX, qreal posY);
  Q_INVOKABLE void lineExited();

      /**
       * 
       */
  Q_INVOKABLE QString getHintText();

signals:
  void nrChanged();
  void pointYChanged();
  void examChanged();

protected:
  TQAunit* getUnit(int qNr) const;

      /**
       * Progress line is fancy information about changes of average time for every next question.
       * Values are taken from @p TchartItem::TmainChart::averChunk()
       * @p m_prevAverY and @p m_thisAverY hold Y values to paint that line
       * and this methods calculates those values which depend on height as well.
       * They are also used to catch mouse hover over that line
       */
  void calcProgressRangeY();

  void setProgressHoverred(bool ph);

  void hoverEnterEvent (QHoverEvent* e) override;
  void hoverMoveEvent (QHoverEvent* e) override;
  void hoverLeaveEvent (QHoverEvent*) override;

private:
  TchartItem              *m_chart = nullptr;
  TtipInfo                *m_qInf;
  int                      m_nrInchart = -1;
  qreal                    m_prevAverY = 0.0, m_thisAverY = 0.0;
  bool                     m_progressHoverred = false;
};

#endif // TLINCHARTDELEGATE_H
