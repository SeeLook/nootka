/***************************************************************************
 *   Copyright (C) 2015-2016 by Tomasz Bojczuk                             *
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

#ifndef TMELODYITEM_H
#define TMELODYITEM_H

#include "nootkasoundglobal.h"
#include <QtWidgets/qgraphicsitem.h>


class QGraphicsEllipseItem;
class QAction;
class QTimer;
class TflyItem;

/**
 * This is a "three-points-button" on touchable screens to
 * handle touch (sends signal usually associated with main menu)
 * and manages flying icons-buttons with actions.
 * TmelodyItem is available as a static @p instance() method.
 * @class TmelMan and @class TpitchView use it to manage playing/sniffing/recording
 */
class NOOTKASOUND_EXPORT TmelodyItem : public QGraphicsObject
{
  Q_OBJECT

public:
  TmelodyItem();
  virtual ~TmelodyItem();

      /** Instance of @class TmelodyItem or 0 */
  static TmelodyItem* instance() { return m_instance; }

      /** Signalizes about playing process  */
  void setPlaying(bool isPlay);
  void playingStarted() { setPlaying(true); }
  void playingStopped() { setPlaying(false); }

      /** Signalizes about listening (sniffing) process  */
  void setListening(bool isListen);
  void listeningStarted() { setListening(true); }
  void listeningStopped() { setListening(false); }

  void setRecording(bool isRecord);

      /** Signalizes about recording process  */
  void recordingStarted() { setRecording(true); }
  void recordingStopped() { setRecording(false); }

  bool isTouched() { return m_touched; }

      /** @p True when audio input (sniffing) is enabled  */
  bool audioInEnabled();

      /** @p True when audio output (listening) is enabled  */
  bool audioOutEnabled();

      /** Performs initial helping animation how to use the button  */
  void initialAnim();

      /** List of actions from which side icons are created
       * They are drawn in order of the list
       * o-------------1
       * |\_
       * |  \_
       * |    \_
       * |       2
       * 3
       * Use @p insert() to add some action between,
       * and @p removeOne(QAction) to take it from the list.
       * Disabled actions are not displayed but spare space remains.
       */
  QList<QAction*>* actions() { return &m_actions; }

  virtual QRectF boundingRect() const;

signals:
      /** Emitted when item was touched and released */
  void menuSignal();

      /** Emitted when item was just touched */
  void touched();

      /** Emitted when user clicked 'Got It' button during preview */
  void previewFinished();

protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent*);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

  void setDotColor(QGraphicsEllipseItem* dot, const QColor& c);
  QGraphicsEllipseItem* createDot(int dotNr);
  void createFlyActions();

private:
  static TmelodyItem          *m_instance;
  QGraphicsEllipseItem        *m_playDot, *m_recDot, *m_snifDot;
  bool                         m_touched;
  QList<TflyItem*>             m_flyList;
  QList<QAction*>              m_actions;
  QTimer                      *m_timer; /**< Counts time when to display fly-icons menu */
  QAction                     *m_selectedAction;
  bool                         m_locked; /**< @p TRUE when preview mode */
  TflyItem                    *m_currentIt;
};

#endif // TMELODYITEM_H
