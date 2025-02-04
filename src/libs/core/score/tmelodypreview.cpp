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

#include "tmelodypreview.h"
#include "music/tmelody.h"
#include "tscoreobject.h"

TmelodyPreview::TmelodyPreview(QQuickItem *parent)
    : QQuickItem(parent)
{
}

QString TmelodyPreview::composer() const
{
    return m_melody ? m_melody->composer() : QString();
}

QString TmelodyPreview::title() const
{
    return m_melody ? m_melody->title() : QString();
}

void TmelodyPreview::setScore(TscoreObject *sc)
{
    m_score = sc;
    setSelectReadOnly(m_selectReadOnly);
}

QVariant TmelodyPreview::melody()
{
    return QVariant::fromValue(m_melody);
}

void TmelodyPreview::setMelody(QVariant v)
{
    auto m = qvariant_cast<Tmelody *>(v);
    if (m != m_melody) {
        m_melody = m;
        if (m_melody && m_score)
            m_score->setMelody(m_melody);
        emit melodyChanged();
    }
}

bool TmelodyPreview::selectReadOnly() const
{
    return m_score ? m_score->selectInReadOnly() : false;
}

void TmelodyPreview::setSelectReadOnly(bool selRO)
{
    if (selRO != m_selectReadOnly || (m_score && m_score->selectInReadOnly() != selRO)) {
        m_selectReadOnly = selRO;
        if (m_score) {
            m_score->setSelectInReadOnly(selRO);
            if (selRO)
                connect(m_score, &TscoreObject::readOnlyNoteClicked, this, &TmelodyPreview::readOnlyNoteClicked, Qt::UniqueConnection);
            emit selectReadOnlyChanged();
        }
    }
}

void TmelodyPreview::reload()
{
    if (m_melody && m_score)
        m_score->setMelody(m_melody);
}
