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

#include "ttoucharea.h"
#include "ttouchproxy.h"
#include <tmtr.h>
#include <QtWidgets>


/*static*/
bool TtouchArea::touchEnabled() { return TtouchProxy::touchEnabled(); }


TtouchArea::TtouchArea(QWidget* parent) :
  QScrollArea(parent)
{
  setWidgetResizable(true);
#if defined (Q_OS_ANDROID) // hide scroll-bars for mobile
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#else
  setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
#endif
  setFrameShape(QFrame::NoFrame);
  setAttribute(Qt::WA_AcceptTouchEvents);
  setWidget(new QWidget);
  widget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################
bool TtouchArea::event(QEvent* event) {
  if (touchEnabled()) {
    if (event->type() == QEvent::TouchBegin || event->type() == QEvent::TouchUpdate || event->type() == QEvent::TouchEnd) {
      QTouchEvent *te = static_cast<QTouchEvent*>(event);
      if (te->touchPoints().first().pos().x() > Tmtr::fingerPixels() / 3) { // skip margin
        if (event->type() == QEvent::TouchBegin) {
          return true;
        } else if (event->type() == QEvent::TouchUpdate) {
            if (qAbs(te->touchPoints().first().pos().y() - te->touchPoints().first().startPos().y()) > Tmtr::fingerPixels() / 3)
              verticalScrollBar()->setValue(verticalScrollBar()->value() + (te->touchPoints()[0].lastPos().y() - te->touchPoints()[0].pos().y()));
            if (qAbs(te->touchPoints().first().pos().x() - te->touchPoints().first().startPos().x()) > Tmtr::fingerPixels() / 3)
              horizontalScrollBar()->setValue(horizontalScrollBar()->value() + (te->touchPoints()[0].lastPos().y() - te->touchPoints()[0].pos().y()));
        }
      }
    }
  }
  return QScrollArea::event(event);
}

