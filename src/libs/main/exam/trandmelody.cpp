/***************************************************************************
 *   Copyright (C) 2014-2015 by Tomasz Bojczuk                             *
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

#include "trandmelody.h"
#include <music/tmelody.h>
#include <music/trhythm.h>
#include <QDebug>
#include <QDateTime>


void getRandomMelody(QList<TQAgroup>& qList, Tmelody* mel, int len, bool inKey, bool onTonic) {
  qsrand(QDateTime::currentDateTime().toTime_t());
	for (int i = 0; i < len; ++i) {
		int randVal = qrand() % qList.size();
		Tnote pitch = qList[randVal].note;
		TfingerPos fPos = qList[randVal].pos;
		if (inKey) { // note has to be in key signature declared in melody
			if (mel->key().inKey(pitch).note == 0) { // if it is not
				int tryCount = 0;
				int it = randVal;
				while (tryCount < qList.size()) { // find another one
					it++;
					if (it == qList.size())
						it = 0;
					Tnote tryNote = mel->key().inKey(qList[it].note);
					if (tryNote.note != 0) {
						pitch = tryNote;
						fPos = qList[it].pos;
						break;
					}
					tryCount++;
				}
			}
		}
		if (pitch.alter == 1 && mel->key().value() < 0) // flats key signature
			pitch = pitch.showWithFlat(); // so prefer flats
		else if (pitch.alter == -1 && mel->key().value() > 0) // sharps key signature
			pitch = pitch.showWithSharp(); // so prefer sharps
		Tchunk note(pitch, Trhythm(Trhythm::e_none), fPos);
		mel->addNote(note);
	}
	if (onTonic) {
		char tonicNoteNr;
		if (mel->key().isMinor())
			tonicNoteNr = TkeySignature::minorKeys[mel->key().value() + 7];
		else
			tonicNoteNr = TkeySignature::majorKeys[mel->key().value() + 7];
		int cnt = -1, i = (qrand() % qList.size()) - 1; // start iteration from random value
		while (cnt < qList.size()) {
			i++;
			if (i >= qList.size())
				i = 0;
			cnt++;
			Tnote tonic(tonicNoteNr + 1, 0, TkeySignature::scalesDefArr[mel->key().value() + 7][tonicNoteNr]);
			bool theSame = false;
			if (tonic.compareNotes(qList[i].note, true))
				theSame = true;
			else {
				Tnote tonicConverted;
				if (tonic.alter == 1)
					tonicConverted = tonic.showWithFlat();
				else if (tonic.alter == -1)
					tonicConverted = tonic.showWithSharp();
				else
					continue;
				if (tonicConverted.compareNotes(qList[i].note, true)) {
					theSame = true;
					tonic = tonicConverted;
				}
			}
			if (theSame) {
				tonic.octave = qList[i].note.octave;
				if (tonic.alter == 1 && mel->key().value() < 0) // flats key signature
					tonic = tonic.showWithFlat(); // so prefer flats
				mel->lastMeasure().lastNote().p() = tonic;
				mel->lastMeasure().lastNote().g() = qList[i].pos;
				break;
			}
		}
	if (cnt == qList.size())
		qDebug() << "Tonic note of" << mel->key().getName() << "was not found";
	}
}

