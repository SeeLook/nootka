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


class Tmeter;


#define MIN_GAP (1.0) /**< Minimal distance between notes of the shortest duration in a staff */


/**
 * Graphical representation of time signature on a staff (first one)
 */
class NOOTKACORE_EXPORT TscoreMeter : public TscoreItem
{

  Q_OBJECT

public:
  TscoreMeter(TscoreScene* scene, TscoreStaff* staff);
  virtual ~TscoreMeter();

  void setMeter(const Tmeter& meter, bool emitSignal = true);
  Tmeter* meter() const { return m_meter; }

  void setReadOnly(bool readOnly) { m_isReadOnly = readOnly; }
  bool isReadOnly() { return m_isReadOnly; }

      /** This is only way to inform const boundingRect about staff type changed */
  void setPianoStaff(bool piano) { m_pianoStaff = piano; }
  bool isPianoStaff() { return m_pianoStaff; }

      /**
       * Returns duration of given @p grNr group starting from measure beginning
       * Describes grouping (beaming - beam connections) of notes in a single measure for current meter.
       * This is a group of a few int values - each representing duration of the one group:
       * - for 3/8 it is only single 36 value - whole measure under one beam
       * - for 3/4 it is 24, 48, 72) - three groups
       */
  quint8 groupPos(int grNr) { return m_groups[grNr]; }

      /** Number of beaming groups for this meter  */
  int groupCount() { return m_groups.count(); }

  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
  virtual QRectF boundingRect() const;

  qreal width() { return m_width; }

      /**
       * @p optimalGap is summary of note gaps for
       * optimal measure content, i.e.:
       * 2/4  is | 4 4 | so two gaps of quarter note
       * 5/8  is | 4. 4 | so gap of quarter with dot and just quarter
       * 12/8 is | 4. 4. 4. 4. | so four gaps of quarter and dot
       * It is used to determine has a staff space for a new measure
       */
  qreal optimalGap() { return m_optGap; }

signals:
  void meterChanged();

protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);

private:
  void updateOptGap();

private:
  Tmeter           *m_meter;
  bool              m_isReadOnly, m_pianoStaff;
  QString           m_upperDigit, m_lowerDigit;
  qreal             m_width;
  QList<quint8>     m_groups;
  qreal             m_optGap;
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
