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

#include "tnoteitem.h"
#include <tnoofont.h>
#include <music/trhythm.h>
#include <QtWidgets/qapplication.h>
#include <QtGui/qpalette.h>
#include <QtGui/qpainter.h>

#include <QtCore/qdebug.h>


TnoteItem::TnoteItem(TscoreScene* scene, Trhythm *r) :
  TscoreItem(scene),
  m_color(qApp->palette().text().color()),
  m_rhythm(r),
  m_stemUp(true)
{
  setAcceptHoverEvents(false);
  obtainNoteLetter();
}


void TnoteItem::setRhythm(Trhythm *r) {
//   if (r->rhythm() != m_rhythm->rhythm()) {
    m_rhythm = r;
    if (m_rhythm && m_rhythm->isRest())
      m_stemUp = true;
    obtainNoteLetter();
    update();
//   }
}


void TnoteItem::setStemUp(bool isUp) {
  if (m_stemUp != isUp) { // makes no sense to change stem when there is not - rhythm is 0
    if (m_rhythm && !m_rhythm->isRest() && m_rhythm->rhythm() > Trhythm::e_whole) {
      prepareGeometryChange();
      m_stemUp = isUp;
      obtainNoteLetter();
    }
  }
}


void TnoteItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  Q_UNUSED(option)
  Q_UNUSED(widget)

//   paintBackground(painter, Qt::blue);
  painter->setPen(QPen(m_color, 0.4));

  painter->setFont(TnooFont(8));
  painter->drawText(QPointF(0.0, m_stemUp ? 1.0 : 6.32), m_noteLetter);
  if (m_rhythm && m_rhythm->hasDot()) {
    painter->drawText(QPointF(m_rhythm->rhythm() == Trhythm::e_whole ? 3.5 : 2.7,
                              1.4 + (m_stemUp ? 1.0 : -1.0) * ((int)y() % 2)),
                      QStringLiteral("."));
  }
}

QRectF TnoteItem::boundingRect() const {
  return QRectF(0.0, m_stemUp ? -5.5 : -0.5, 4.4, 8.0);
}


void TnoteItem::obtainNoteLetter() {
  Trhythm::Erhythm v = Trhythm::e_none;
  bool rest = false;
  if (m_rhythm) {
    v = m_rhythm->rhythm();
    if (m_rhythm->isRest()) { // handle rest glyphs - for score they are just rectangles without leading line and getCharFromRhythm not works for it
      int charNr = 0;
      if (m_rhythm->rhythm() == Trhythm::e_whole)
        charNr = 0xe102;
      else if (m_rhythm->rhythm() == Trhythm::e_half)
        charNr = 0xe103;
      if (charNr) {
        m_noteLetter = QString(QChar(charNr));
        return;
      }
      rest = true;
    }
  }
  m_noteLetter = QString(QChar(TnooFont::getCharFromRhythm(v, m_stemUp, rest)));
}



