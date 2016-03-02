/***************************************************************************
 *   Copyright (C) 2016 by Tomasz Bojczuk                                  *
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

#ifndef TSCOREMETER_H
#define TSCOREMETER_H

#include "nootkacoreglobal.h"
#include "tscoreitem.h"
#include <music/tmeter.h>


/**
 * Graphical representation of time signature on a staff (first one)
 */
class NOOTKACORE_EXPORT TscoreMeter : public TscoreItem
{

  Q_OBJECT

public:
  TscoreMeter(TscoreScene* scene, TscoreStaff* staff);

  void setMeter(const Tmeter& meter);
  Tmeter meter() const { return m_meter; }

  void setReadOnly(bool readOnly) { m_isReadOnly = readOnly; }
  bool isReadOnly() { return m_isReadOnly; }

      /** This is only way to inform const boundingRect about staff type changed */
  void setPianoStaff(bool piano) { m_pianoStaff = piano; }

  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
  virtual QRectF boundingRect() const;

  qreal width() { return m_width; }

signals:
  void meterChanged();

protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);

private:
  Tmeter            m_meter;
  bool              m_isReadOnly, m_pianoStaff;
  QString           m_upperDigit, m_lowerDigit;
  qreal             m_width;
};


#include <QtWidgets/qmenu.h>

class QButtonGroup;

/**
 * A menu to select a time signature.
 * It works as ordinary @class QMenu,
 * just returns @class Tmeter.
 */
class NOOTKACORE_EXPORT TselectMeter : public QMenu {

  Q_OBJECT

public:
  explicit TselectMeter(const Tmeter& meter, QWidget* parent = 0);

  Tmeter exec(const QPoint& pos);

private:
  QButtonGroup        *m_group;
};

#endif // TSCOREMETER_H
