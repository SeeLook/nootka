/***************************************************************************
 *   Copyright (C) 2021 by Tomasz Bojczuk                                  *
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

#include "tnotesbaritem.h"
#include "tsound.h"
#include <music/tnotestruct.h>

#include <QtCore/qdebug.h>
#include <QtGui/qguiapplication.h>
#include <QtGui/qpainter.h>
#include <QtGui/qpalette.h>

TnotesBarItem::TnotesBarItem(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    connect(SOUND, &Tsound::noteStarted, this, &TnotesBarItem::noteStartedSlot);
    connect(SOUND, &Tsound::noteFinishedEntire, this, &TnotesBarItem::noteFinishedSlot);
}

void TnotesBarItem::setDetectedNote(const Tnote &detectedNote)
{
    if (m_detectedNote == detectedNote)
        return;

    m_detectedNote = detectedNote;
    emit detectedNoteChanged();
    update();
}

void TnotesBarItem::setExpectedNote(const Tnote &expectedNote)
{
    if (m_expectedNote == expectedNote)
        return;

    m_expectedNote = expectedNote;
    emit expectedNoteChanged();
    update();
}

qreal TnotesBarItem::expectedX() const
{
    qreal w = height() - qApp->font().pointSizeF() / 2.0;
    if (m_expectedNote.isValid()) {
        return qBound(0.0, (m_expectedNote.chromatic() - m_lowestNote.chromatic()) * (width() / m_notesSpan) - w / 2.0, width() - w);
    } else {
        return (width() - w) / 2.0;
    }
}

qreal TnotesBarItem::detectedX() const
{
    qreal w = height() - qApp->font().pointSizeF() / 2.0;
    if (m_detectedNote.isValid() && m_detectedNote.chromatic() != m_expectedNote.chromatic()) {
        qreal u = width() / m_notesSpan;
        int diff = m_expectedNote.chromatic() - m_detectedNote.chromatic();
        return qBound(0.0, (m_expectedNote.chromatic() - m_lowestNote.chromatic() - diff) * u - w / 2.0, width() - w);
    } else {
        return 0.0;
    }
}

qreal TnotesBarItem::notesDiff() const
{
    return static_cast<qreal>(m_detectedNote.chromatic() - m_expectedNote.chromatic());
}

void TnotesBarItem::setAmbitus(const Tnote &lo, const Tnote &hi)
{
    m_lowestNote = lo;
    m_highestNote = hi;
    m_notesSpan = m_highestNote.chromatic() - m_lowestNote.chromatic();
}

void TnotesBarItem::paint(QPainter *painter)
{
    if (m_expectedNote.isValid() && m_detectedNote.isValid() && m_expectedNote.chromatic() != m_detectedNote.chromatic()) {
        painter->setPen(QPen(qApp->palette().text().color(), qApp->font().pointSizeF() / 2.0, Qt::SolidLine, Qt::RoundCap));
        qreal u = width() / m_notesSpan;
        int startX = qMin(m_expectedNote.chromatic(), m_detectedNote.chromatic()) + 1 - m_lowestNote.chromatic();
        int endX = qMax(m_expectedNote.chromatic(), m_detectedNote.chromatic()) - m_lowestNote.chromatic();
        // draw lines (ticks) representing semitones
        for (int x = startX; x < endX; ++x)
            painter->drawLine(QPointF(x * u, height() * 0.35), QPointF(x * u, height() * 0.65));
    }
}

void TnotesBarItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    if (newGeometry.width() != oldGeometry.width() || newGeometry.height() != oldGeometry.height()) {
        m_notesSpan = m_highestNote.chromatic() - m_lowestNote.chromatic();
        emit expectedNoteChanged();
        emit detectedNoteChanged();
    }
}

// #################################################################################################
// ###################                PROTECTED         ############################################
// #################################################################################################

void TnotesBarItem::noteStartedSlot(const Tnote &n)
{
    if (n.isRest())
        return;

    setDetectedNote(n);
    m_isPlaying = true;
    emit isPlayingChanged();
}

void TnotesBarItem::noteFinishedSlot(const TnoteStruct &n)
{
    if (n.pitch.chromatic() != m_detectedNote.chromatic())
        setDetectedNote(n.pitch);
    m_isPlaying = false;
    emit isPlayingChanged();
}
