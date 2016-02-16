/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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


#ifndef TTIPHANDLER_H
#define TTIPHANDLER_H

#include <qgraphicsitem.h>
#include <QPointer>

class QTimer;
class TgroupedQAunit;
class TgraphicsTextTip;


/** 
 * This class manages a tip on a chart.
 * It displays it and removes after appropirate time. 
 * This is base class for chart itmes witch display tips.
 * Top level class inherited from @class TtipHandler has to:
 * - reimplement hoverEnterEvent(QGraphicsSceneHoverEvent *event)
 * and prepare there TgraphicsTextTip *tip
 * - then call handleTip(event->scenePos()) inside 
 * Static QTimer and TgraphicsTextTip art to have exactly only one instance
 * of them for all chart elements. 
 * m_initObject keeps an object that invoked a tip.
 * It is used to delete prevoius tip when another object gets enterEvent.
 */

class TtipHandler : public QGraphicsObject
{  
  Q_OBJECT
  
public:
  TtipHandler();
  virtual ~TtipHandler();
  
      /** Simply deletes tip. 
        * It is static to give possibility remove TgraphicsTextTip outside of a class. 
        * It returns true when tip existed and was deleted. */
  static bool deleteTip();

protected:
      /** This method should be called after creating a tip instance
        * in delivered class in its hoverEnterEvent.
        * @param scenePos is position of an event in a scene taken from event->scenePos()*/
  void handleTip(QPointF scenePos);
  
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent*);
  
  static QPointer<TgraphicsTextTip> tip;
  
      /** Returns pointer to graphics object that created this tip. */
  static const QGraphicsObject* initObject() { return m_initObject; }
  
private:
  static QPointer<QTimer>         m_hideTimer, m_showTimer;
  static QGraphicsObject         *m_initObject;
  bool                            m_entered;
  static bool                     m_mouseOverTip;

private slots:
  void hideTip();
  void showTip();
  void tipEnteredSlot();
  void tipLeavedSlot();
  
  
    
};

#endif // TTIPHANDLER_H
