/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
 *   tomaszbojczuk@gmail.com                                               *
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

#ifndef TMELODYITEM_H
#define TMELODYITEM_H

#include "nootkasoundglobal.h"
#include <QGraphicsObject>


class QGraphicsEllipseItem;
class QAction;

/**
 * This is a "three-points-button on touchable screens to:
 * - start/stop playing
 * - enter into record mode
 * - start/stop sniffing (pitch detection)
 * It has got the actions for score menu and main menu
 * TmelodyItem is available as a static @p instance() method.
 * @class TmelMan and @class TpitchView use it to manage playing/sniffing/recording
 */
class NOOTKASOUND_EXPORT TmelodyItem : public QGraphicsObject
{
  Q_OBJECT

public:
  TmelodyItem(QAction* playAction, QAction* recordAction, QAction* listenAction);
  virtual ~TmelodyItem();

  static TmelodyItem* instance() { return m_instance; } /** Instance of @class TmelodyItem or 0 */

  void setPlaying(bool isPlay); /** Signalizes about playing process  */
  void playingStarted() { setPlaying(true); }
  void playingStopped() { setPlaying(false); }

  void setListening(bool isListen); /** Signalizes about listening (sniffing) process  */
  void listeningStarted() { setListening(true); }
  void listeningStopped() { setListening(false); }

  void setRecording(bool isRecord);
  void recordingStarted() { setRecording(true); } /** Signalizes about recording process  */
  void recordingStopped() { setRecording(false); }

signals:
  void scoreMenuSignal();
  void mainMenuSignal();
  void menuSignal();
  void touched();

protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent*);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent*);

  virtual QRectF boundingRect() const;
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

  void setDotColor(QGraphicsEllipseItem* dot, const QColor& c);
  QGraphicsEllipseItem* createDot(int dotNr);

private:
  static TmelodyItem          *m_instance;
  QAction                     *m_playAct, *m_recAct, *m_sniffAct, *m_scoreMenuAct, *m_mainMenuAct;
  QGraphicsEllipseItem        *m_playDot, *m_recDot, *m_snifDot;
  bool                         m_touched;

};

#endif // TMELODYITEM_H
