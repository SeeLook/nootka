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

#ifndef TBARCHARTDELEGATE_H
#define TBARCHARTDELEGATE_H


#include <QtQuick/qquickpainteditem.h>
#include <QtGui/qpainter.h>


class TchartItem;
class TgroupedQAunit;
class TtipInfo;


/**
 * @todo write docs
 */
class TbarChartDelegate : public QQuickPaintedItem {

  Q_OBJECT

  Q_PROPERTY(TchartItem* chart READ chart WRITE setChart NOTIFY examChanged)
  Q_PROPERTY(int groupNr READ groupNr WRITE setGroupNr NOTIFY groupNrChanged)
  Q_PROPERTY(bool hovered READ hovered WRITE setHovered NOTIFY hoveredChanged)
  Q_PROPERTY(qreal yValue READ yValue NOTIFY examChanged)
  Q_PROPERTY(QString xText READ xText NOTIFY examChanged)

public:
  explicit TbarChartDelegate(QQuickItem* parent = nullptr);
  ~TbarChartDelegate() override;

  void paint(QPainter* painter) override;

  TchartItem* chart() { return m_chart; }
  void setChart(TchartItem* ch);

  int groupNr() const { return m_groupNr; }
  void setGroupNr(int gr);

      /**
       * Due to @p TbarChartDelegate item is wrapped by DropShadow effect it is invisible,
       * so it doesn't handle hover events. They have to be redirected here from QML DropShadow
       */
  bool hovered() const { return m_hovered; }
  void setHovered(bool h);

  qreal yValue() const;
  QString xText() const;

signals:
  void hoveredChanged();
  void groupNrChanged();
  void examChanged();


private:
  TchartItem          *m_chart = nullptr;
  bool                 m_hovered = false;
  int                  m_groupNr = -1; /**< In fact, it is index number of delegate, but we grab @p TgroupedUnit by this number */
  TgroupedQAunit      *m_qaGroup = nullptr;
  qreal                m_wrongAt, m_notBadAt; /**< Keeps position of color gradient for mistakes */
  TtipInfo            *m_barInfo = nullptr;
};

#endif // TBARCHARTDELEGATE_H
