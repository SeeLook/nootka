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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#ifndef TBGPIXMAP_H
#define TBGPIXMAP_H


#include <QtGui/qpixmap.h>


#define   BG_PIX    TbgPixmap::instance()

/**
 * QPixmap that contains background image of guitar body.
 * There is only single instance for whole application
 * If instrument is kind of other - pixmap is null
 */
class TbgPixmap : public QPixmap
{

public:

  TbgPixmap();
  ~TbgPixmap();

  static TbgPixmap* instance() { return m_instance; }

      /**
       * This is a heart of the class, here, the pixmap is loaded and scaled,
       * according to given @p windowSize of main Nootka Window,
       * @p instr - an instrument type
       * @p guitarHeight - height of guitar widget
       * @p XIIfretXpos - position of XII fret in the guitar and 
       * @p fingerboardX - X coordinate of starting point of fingerboard rectangle
       */
  void update(const QSize& windowSize, int instr, int guitarHeight, int XIIfretXpos, int fingerboardX, bool rightHanded);

      /** Position of the background image in Nootka main window coordinates */
  QPoint globalPos() const { return m_pos; }
  void setGlobalPos(const QPoint& p) { m_pos = p; }

      /** Size of main Nootka window */
  QSize windowSize() const { return m_windowSize; }

      /** @p TRUE if guitar layout is for right-handed users */
  bool rightHandedGuitar() { return m_rightHanded; }

      /** Rectangle representing position and size of guitar pickup, or empty if none */
  QRect pickUpRect() const { return m_pickRect; }
  void setPickUpRect(const QRect& rect) { m_pickRect = rect; }

private:
  static TbgPixmap            *m_instance;
  QPoint                       m_pos;
  QRect                        m_pickRect;
  QSize                        m_windowSize;
  int                          m_instr;
  bool                         m_rightHanded;
};

#endif // TBGPIXMAP_H
