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


TnoteItem::TnoteItem(TscoreScene* scene, const Trhythm& r) :
  TscoreItem(scene),
  m_color(qApp->palette().text().color()),
  m_paintFlags(false),
  m_upsideDown(false)
{
  m_rhythm = new Trhythm(r);
  setAcceptHoverEvents(false);
  obtainNoteLetter();
}


TnoteItem::~TnoteItem()
{
  delete m_rhythm;
}


void TnoteItem::setRhythm(const Trhythm& r) {
  bool up = false;
  bool upDown = upsideDown(r);
  if (r.stemDown() != m_rhythm->stemDown() || upDown != upsideDown(*m_rhythm)) {
    if (r.stemDown() != m_rhythm->stemDown() && r == *m_rhythm && r.stemDown() != upDown) {
      if (!m_paintFlags)
        qDebug() << "Ignore geometry change";
      m_rhythm->setRhythm(r);
    } else {
      if (!m_paintFlags)
        qDebug() << "stem direction requires geometry change";
      prepareGeometryChange();
      up = true;
    }
  }
  if (up || *m_rhythm != r || (r.beam() && !m_rhythm->beam()) || (!r.beam() && m_rhythm->beam())) {
    m_rhythm->setRhythm(r);
    m_upsideDown = upDown;
    obtainNoteLetter();
    update();
  }
}


void TnoteItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  Q_UNUSED(option)
  Q_UNUSED(widget)

//   paintBackground(painter, Qt::blue);
  painter->setPen(m_color);

  painter->setFont(TnooFont(8));
  // place note head into bounding rectangle that it Y = 0 is always inside the head
  painter->drawText(QPointF(0.0, m_upsideDown ? 6.32 : 1.0), m_noteLetter);
  if (m_rhythm->hasDot()) {
    painter->drawText(QPointF(m_rhythm->rhythm() == Trhythm::e_whole ? 3.5 : 2.7,
                              1.4 + (m_upsideDown ? -1.0 : 1.0) * ((int)y() % 2)),
                      QStringLiteral("."));
  }
}

QRectF TnoteItem::boundingRect() const {
  return QRectF(0.0, m_upsideDown ? -0.5 : -5.5, 4.4, 8.0);
}


void TnoteItem::obtainNoteLetter() {
    quint16 v = m_rhythm->weight();
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
    } else
        if (!m_paintFlags && m_rhythm->weight() > 4 && m_rhythm->beam())
            v = 0; // paint head only - stems and beams are handled outside
  m_noteLetter = QString(QChar(TnooFont::getCharFromRhythm(v, !m_rhythm->stemDown(), m_rhythm->isRest())));
}


/**
 * Determine whether bounding rectangle is different (notes with stems down)
 * + stem down flag is set but it is performed only when:
 *   -- there is no beam and rhythm is higher than whole (whole has no stem)
 *   -- or there is a beam, so half and quarter notes are up-side-down
 */
bool TnoteItem::upsideDown(const Trhythm& r) {
  return r.stemDown() && ((!r.beam() && r.weight() > 1) || (r.beam() && r.weight() > 1 && r.weight() < 8));
}


