/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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

#ifndef TMELODY_H
#define TMELODY_H


#include <QString>
#include <QList>
#include "tchunk.h"
#include "tkeysignature.h"

/** 
 * Class describing a musical melody - sequence of notes (Tchunk)
 * Also it is able to save/load a melody into/from MusicXML structure 
 * Default tempo of a melody is 120 bpm.
 */
class Tmelody
{

public:
	Tmelody(const QString& title = "", const TkeySignature& k = TkeySignature());
	
	QString title() {return m_title; }
	void setTitle(const QString& t) { m_title = t; }
	
	int length() { return m_notes.size(); } /** A length of the melody (notes number) */
	
	QList<Tchunk>& notes() { return m_notes; } /** A reference to list (QList) of melody notes */
	
	int tempo() { return m_tempo; }
	void setTempo(int tmp) { m_tempo = tmp; }
	
	TkeySignature key() { return m_key; }
	void setKey(const TkeySignature& k) { m_key = k; }
	
	void toXml(QXmlStreamWriter& xml);
	bool fromXml(QXmlStreamReader& xml);
	
private:
	QString						m_title;
	QList<Tchunk>			m_notes;
	int								m_tempo;
	TkeySignature			m_key;
};

#endif // TMELODY_H
