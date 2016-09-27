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
#include <music/trhythm.h>
#include <QtWidgets/qapplication.h>
#include <QtGui/qpalette.h>
#include <QtGui/qpainter.h>

#include <QtCore/qdebug.h>


#define DOT (0xe1e7)
#define WHOLE_REST (0xe4e2)
#define BASE_HEAD (0xe0a1)

#define STEM_HEIGHT (5.8)
#define HALF_STEM (0.1)
#define HEAD_WIDTH (2.35938) // width of black note head glyph for default font size of 6
#define FONT_SIZE (6)

#define STEM_DOWN_Y (1.2)
#define STEM_UP_Y (0.79)
#define MID_LINE (20.0)
#define FLAG_GLYPH (0xe238)


TnoteItem::TnoteItem(TscoreScene* scene, const Trhythm& r) :
  TscoreItem(scene)
{
  m_rhythm = new Trhythm(r);
  m_stem = new QGraphicsLineItem(this);
  m_stem->hide();
  m_flag = new QGraphicsSimpleTextItem(this);
  m_flag->setFont(QFont(QStringLiteral("Scorek"), 5));
  m_flag->hide();
  setColor(qApp->palette().text().color());
  setAcceptHoverEvents(false);
}


TnoteItem::~TnoteItem()
{
  delete m_rhythm;
}


void TnoteItem::setItAsCursor() {
  delete m_flag;
  m_flag = nullptr;
  delete m_stem;
  m_stem = nullptr;
}


/**
 * Except setting Y coordinate,
 * this method takes care about stem length when note is much above or below staff.
 * Stem is prolonged until middle staff line (as professional scores have)
 *
 * TODO: WHAT ABOUT PIANO STAFF WHERE ARE TWO MIDDLE LINES
 */
void TnoteItem::setY(qreal yPos) {
  QGraphicsItem::setY(yPos);
  if (m_stem) {
    if (m_stem->isVisible()) {
      if (m_rhythm->stemDown())
        m_stem->setLine(HALF_STEM, STEM_DOWN_Y, HALF_STEM, qMax(STEM_HEIGHT + STEM_DOWN_Y, MID_LINE - yPos));
      else
        m_stem->setLine(HEAD_WIDTH - HALF_STEM, STEM_UP_Y, HEAD_WIDTH - HALF_STEM,
                qMin(-(STEM_HEIGHT + (m_rhythm->rhythm() > Trhythm::e_quarter && m_rhythm->hasDot() ? 1.0 : 0.0) - STEM_UP_Y), MID_LINE - yPos));
    }
    if (m_flag->isVisible()) {
      if (m_rhythm->stemDown())
        m_flag->setPos(HALF_STEM, m_stem->line().length() - STEM_DOWN_Y - 12.5);
      else
        m_flag->setPos(HEAD_WIDTH, STEM_UP_Y - m_stem->line().length() - 15.0);
    }
  }
}


/**
 * It occurs only when stem is managed by @p TscoreBeam class, so there is no flag
 * and no need to set flag position again
 */
void TnoteItem::setStemLength(qreal len) {
  qreal y2 = m_stem->line().y1() + len;
  if (m_rhythm->stemDown())
    y2 = qMax(y2, MID_LINE - y());
  else
    y2 = qMin(y2, MID_LINE - y());
  m_stem->setLine(m_stem->line().x1(), m_stem->line().y1(), m_stem->line().x2(), y2);
}



void TnoteItem::setColor(const QColor& c) {
  m_color = c;
  if (m_stem) {
    m_stem->setPen(QPen(m_color, 2 * HALF_STEM));
    m_flag->setBrush(m_color);
  }
}


void TnoteItem::setRhythm(const Trhythm& r) {
  if (r != *m_rhythm || r.stemDown() != m_rhythm->stemDown() || r.beam() != m_rhythm->beam()) {
    *m_rhythm = r;
    int v = BASE_HEAD + qMin(static_cast<int>(m_rhythm->rhythm()), 3);
    if (m_stem) {
      if (!m_rhythm->isRest() && m_rhythm->rhythm() > Trhythm::e_whole) {
            m_stem->show();
          if (m_rhythm->beam() == Trhythm::e_noBeam && m_rhythm->rhythm() > Trhythm::e_quarter) {
              m_flag->setText(QString(QChar(FLAG_GLYPH + m_rhythm->rhythm() * 2 + (m_rhythm->stemDown() ? 1 : 0))));
                m_flag->show();
          } else
              m_flag->hide();
          setY(y()); // updates positions of stem and flag
      } else {
          m_stem->hide();
          m_flag->hide();
      }
    }
    if (m_rhythm->isRest())
      v = WHOLE_REST + m_rhythm->rhythm();
    m_noteLetter = QString(QChar(v));
  }
}


void TnoteItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  Q_UNUSED(option)
  Q_UNUSED(widget)

//   paintBackground(painter, Qt::blue);
  painter->setPen(m_color);

  painter->setFont(QFont(QStringLiteral("Scorek"), FONT_SIZE));
  painter->drawText(QPointF(0.0, 1.0), m_noteLetter);
  if (m_rhythm->hasDot()) {
    painter->drawText(QPointF(m_rhythm->rhythm() == Trhythm::e_whole ? 3.5 : 2.9, 1.0 - static_cast<qreal>(static_cast<int>(y()) % 2)),
                      QString(QChar(DOT)));
  }
}


QRectF TnoteItem::boundingRect() const {
    return QRectF(0.0, -2.5, 3.0, 8.0); // it covers note heads and all rests
}


