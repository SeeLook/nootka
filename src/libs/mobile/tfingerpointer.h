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

#ifndef TFINGERPOINTER_H
#define TFINGERPOINTER_H


#include <QtWidgets/qgraphicsitem.h>
#include <QtGui/qpainter.h>

class TmovedAnim;

/**
 * Movable finger pointer used to explain how to use Nootka GUI
 * It moves finger pixmap of every set @p addMove sequence
 * with start and stop points coordinates
 */
class TfingerPointer : public QGraphicsObject
{

  Q_OBJECT

public:

          /** @class TmoveSeq describes single move of finger pointer  */
      class TmoveSeq {
        public:
          TmoveSeq(const QPointF& s, const QPointF& e, quint32 dur, quint32 del = 0) :
            start(s), stop(e), duration(dur), delay(del) {}
          QPointF start;       /**< start point of a move */
          QPointF stop;        /**< end point */
          quint32 duration;    /**< duration of a move */
          quint32 delay;       /**< delay after achieving end point */
      };

  TfingerPointer(QGraphicsItem* parent);

  virtual ~TfingerPointer();

      /** Adds single move sequence */
  void addMove(const QPointF& start, const QPointF& stop, quint32 duration, quint32 delay = 0) {
    m_moves << TmoveSeq(start, stop, duration, delay);
  }

      /** Starts performing animation */
  void start();

      /** Stops animation immediately */
  void stop();

      /** Number of currently performing sequence, -1 when not moving */
  int sequence() { return m_seqNr; }


  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
    painter->drawPixmap(0, 0, m_finger);
  }

  virtual QRectF boundingRect() const override { return m_finger.rect(); }


signals:
      /** Signal emitted after achieving every and point of a move
       * Emitted parameter is a number of finished moving sequence */
  void moved(int);

      /** Emitted when the last sequence was done */
  void finished();

protected:
      /** Starts moving of current sequence number */
  void moveFinger();

      /** Performed when current sequence is finished. Starts timer with sequence delay */
  void seqFinishedSlot();

      /** Starts next sequence or emits @p finished() if the last one */
  void timerSlot();

private:
  int                 m_seqNr;
  QPixmap             m_finger;
  TmovedAnim         *m_anim;
  QTimer             *m_timer;
  QList<TmoveSeq>     m_moves;
};

#endif // TFINGERPOINTER_H
