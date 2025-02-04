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

#include "tmelodyimportitem.h"
#include <music/timportscore.h>
#include <music/tmelody.h>
#include <score/tdummychord.h>
#include <score/tnoteitem.h>
#include <score/tscoreobject.h>
#include <tglobals.h>

#include <QtCore/qdebug.h>
#include <QtQml/qqmlengine.h>

TmelodyImportItem::TmelodyImportItem(QQuickItem *parent)
    : QQuickItem(parent)
{
    connect(IMPORT_SCORE, &TimportScore::xmlWasRead, this, &TmelodyImportItem::partsModelChanged);
}

TmelodyImportItem::~TmelodyImportItem()
{
    if (IMPORT_SCORE)
        IMPORT_SCORE->deleteLater();
}

QString TmelodyImportItem::title() const
{
    if (IMPORT_SCORE && IMPORT_SCORE->mainMelody())
        return IMPORT_SCORE->mainMelody()->composer() + QLatin1String(" - ") + IMPORT_SCORE->mainMelody()->title();

    return QString();
}

QList<QObject *> TmelodyImportItem::partsModel() const
{
    if (IMPORT_SCORE && IMPORT_SCORE->xmlReadFinished())
        return IMPORT_SCORE->model();

    return QList<QObject *>();
}

void TmelodyImportItem::emitImport()
{
    if (IMPORT_SCORE)
        emit IMPORT_SCORE->importReady();
}

int TmelodyImportItem::globalSplitNr() const
{
    return TimportScore::splitEveryBarNr();
}

void TmelodyImportItem::setGlobalSplitNr(int gsn)
{
    if (gsn != TimportScore::splitEveryBarNr()) {
        TimportScore::setSplitBarNr(gsn);
        emit globalSplitNrChanged();
    }
}

void TmelodyImportItem::transpose(int semis, bool outScaleToRes, bool inInstrScale, TmelodyPart *part)
{
    if (part) { // transpose single melody
        for (auto p : part->parts) {
            if (p->melody()) {
                p->melody()->transpose(semis,
                                       outScaleToRes,
                                       inInstrScale ? GLOB->loNote() : p->score()->lowestNote(),
                                       inInstrScale ? GLOB->hiNote() : p->score()->highestNote());
                p->score()->setMelody(p->melody());
                if (!p->chords.isEmpty()) { // also transpose dummy chords
                    for (auto ch : p->chords) {
                        ch->notes()->setKey(p->melody()->key());
                        ch->notes()->transpose(semis,
                                               outScaleToRes,
                                               inInstrScale ? GLOB->loNote() : p->score()->lowestNote(),
                                               inInstrScale ? GLOB->hiNote() : p->score()->highestNote());
                        // reset parent note item
                        ch->dummyChord()->setParentItem(p->score()->note(ch->noteNr()));
                        emit ch->dummyChord()->chordChanged();
                    }
                }
            }
        }
    } else { // transpose all parts
        // NOTE: there is no option to invoke such a case due to it has less sense.
        // Musical score used to have parts with different key signatures,
        // so importing them to Nootka exercising purposes requires rather individual approach for each every part
    }
}

bool TmelodyImportItem::multiSelect() const
{
    return IMPORT_SCORE ? IMPORT_SCORE->multiSelect() : false;
}

void TmelodyImportItem::setMultiSelect(bool ms)
{
    if (IMPORT_SCORE)
        IMPORT_SCORE->setMultiSelect(ms);
}

void TmelodyImportItem::selectNoteInChords(int noteNr, bool fromTop)
{
    if (IMPORT_SCORE)
        IMPORT_SCORE->selectNoteInChords(noteNr, fromTop);
}

void TmelodyImportItem::arpeggiateChords()
{
    if (IMPORT_SCORE)
        IMPORT_SCORE->arpeggiateChords();
}

void TmelodyImportItem::importWindowReady()
{
    IMPORT_SCORE->setContextObject(this->parent());
}
