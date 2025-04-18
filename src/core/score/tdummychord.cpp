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

#include "tdummychord.h"
#include "music/timportscore.h"
#include "music/tmelody.h"
#include "score/tnoteitem.h"
#include "score/tscoreobject.h"

#include <QtCore/qdebug.h>

TdummyChord::TdummyChord(QQuickItem *parent)
    : QQuickItem(parent)
{
}

TdummyChord::~TdummyChord()
{
}

int TdummyChord::chordModel() const
{
    return m_alaChord ? m_alaChord->notes()->length() : 0;
}

void TdummyChord::setParentItem(QQuickItem *pi)
{
    m_parentNote = qobject_cast<TnoteItem *>(pi);
    QQuickItem::setParentItem(pi);
    if (m_parentNote) {
        findHiLoPos();
        connect(m_parentNote->parent(), &QObject::destroyed, this, [=] {
            m_parentNote = nullptr;
        });
        emit chordChanged();
    }
}

void TdummyChord::setChord(TalaChord *c)
{
    m_alaChord = c;
    m_alaChord->setDummyChord(this);
    findHiLoPos();
    emit chordChanged();
    if (m_selected == -1)
        setSelected(0);
}

void TdummyChord::setSelected(int s)
{
    if (s != m_selected) {
        bool changeNote = m_selected > 0 || s > 0;
        m_selected = s;
        emit selectedChanged();
        if (changeNote) { // change the 'original' melody only when user selected other note from the chord
            Tnote &n = m_alaChord->part->melody()->note(m_alaChord->noteNr())->p();
            auto ch = m_alaChord->notes()->note(m_selected)->p();
            n.setNote(ch.note());
            n.setOctave(ch.octave());
            n.setAlter(ch.alter());
        }
    }
}

QString TdummyChord::noteHead() const
{
    return m_parentNote ? m_parentNote->getHeadText(m_parentNote->note()->rtm) : QString();
}

qreal TdummyChord::headPos(int id)
{
    return m_parentNote && m_alaChord ? m_parentNote->getHeadY(m_alaChord->notes()->note(id)->p()) : 0.0;
}

QString TdummyChord::alterText(int id)
{
    Tnote &n = m_alaChord->notes()->note(id)->p();
    QString a = TnoteItem::unicodeGlyphArray(n.alter());
    if (m_alaChord->part->score() && m_alaChord->part->score()->accidInKey(n.note() - 1)) {
        // when key signature has an accidental on this note
        if (n.alter() == 0) // show neutral if note has not any accidental
            a = QStringLiteral("\ue261");
        else
            a.clear();
    }
    return a;
}

QVariant TdummyChord::part()
{
    return QVariant::fromValue(m_alaChord->part);
}

bool TdummyChord::selectSingle() const
{
    return m_alaChord && m_alaChord->notes()->note(0)->p().rhythm() == Trhythm::NoRhythm;
}

void TdummyChord::setRhythm()
{
    m_alaChord->setRhythm();
    emit selectSingleChanged();
}

void TdummyChord::arpeggiateChord()
{
    m_alaChord->arpeggiateChord();
}

// #################################################################################################
// ###################                PROTECTED         ############################################
// #################################################################################################
void TdummyChord::findHiLoPos()
{
    if (m_loPosY < 1.0 && m_alaChord) { // yet unset
        for (int n = 0; n < m_alaChord->notes()->length(); ++n) {
            auto hp = headPos(n);
            m_hiPosY = qMin(m_hiPosY, hp);
            m_loPosY = qMax(m_loPosY, hp);
        }
    }
}
