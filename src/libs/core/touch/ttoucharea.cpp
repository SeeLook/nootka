/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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
  setContentsMargins(0, 0, 0, Tmtr::fingerPixels()); // space at the bottom to better touch lowest widgets
#else
  setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
#endif
  setFrameShape(QFrame::NoFrame);
  setAttribute(Qt::WA_AcceptTouchEvents);
  setWidget(new QWidget);
  widget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  QScroller::grabGesture(viewport(), QScroller::LeftMouseButtonGesture);
}


#if defined (Q_OS_ANDROID)
QLabel* getLabelFromStatus(QWidget* w, bool indent, bool wordWrap) {
  QLabel *l = new QLabel(w->statusTip() + QStringLiteral("<br>"), w);
  QFont f = w->font();
  f.setPixelSize(qBound(Tmtr::fingerPixels() / 4, Tmtr::shortScreenSide() / 30, w->fontMetrics().height() - 4));
  // minimal text size for eye (ca 2.5 mm) or screen height divided by 30 but not bigger than system font (- 4px)
  l->setFont(f);
  l->setWordWrap(wordWrap);
  w->setStatusTip("");
  if (indent)
    l->setIndent(Tmtr::fingerPixels() / 2);
  return l;
}
#endif





