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

#ifndef TSCOREMETRUM_H
#define TSCOREMETRUM_H

#include "nootkacoreglobal.h"
#include "tscoreitem.h"
#include <music/tmetrum.h>


/**
 * Graphical representation of time signature on a staff (first one)
 */
class NOOTKACORE_EXPORT TscoreMetrum : public TscoreItem
{

  Q_OBJECT

public:
  TscoreMetrum(TscoreScene* scene, TscoreStaff* staff);

  void setMetrum(const Tmetrum& metrum);
  Tmetrum metrum() const { return m_metrum; }

  void setReadOnly(bool readOnly) { m_isReadOnly = readOnly; }
  bool isReadOnly() { return m_isReadOnly; }

      /** This is only way to inform const boundingRect about staff type changed */
  void setPianoStaff(bool piano) { m_pianoStaff = piano; }

  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
  virtual QRectF boundingRect() const;

  qreal width() { return m_width; }

signals:
  void metrumChanged();

protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);

private:
  Tmetrum           m_metrum;
  bool              m_isReadOnly, m_pianoStaff;
  QString           m_upperDigit, m_lowerDigit;
  qreal             m_width;
};


#include <QtWidgets/qmenu.h>

class QButtonGroup;

/**
 * Inner part of menu to select a time signature
 */
class NOOTKACORE_EXPORT TselectMetrum : public QMenu {

  Q_OBJECT

public:
  explicit TselectMetrum(const Tmetrum& metrum, QWidget* parent = 0);

  Tmetrum exec(const QPoint& pos);

private:
  QButtonGroup        *m_group;
};

#endif // TSCOREMETRUM_H
