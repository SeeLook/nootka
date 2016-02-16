/***************************************************************************
 *   Copyright (C) 2006-2016 by Tomasz Bojczuk  				                   *
 *   seelook@gmail.com   	      					                                 *
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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "ttune.h"
#include <QtCore/qvariant.h>
#include <QtCore/qdebug.h>
#include <QtWidgets/qapplication.h>


/*static*/
Ttune Ttune::stdTune = Ttune(QString(), Tnote(3,1,0), Tnote(7,0,0),
                                          Tnote(5,0,0), Tnote(2,0,0), Tnote(6,-1,0),Tnote(3,-1,0));

Ttune Ttune::tunes[4];
Ttune Ttune::bassTunes[4];


void Ttune::prepareDefinedTunes() {
	stdTune = Ttune(QApplication::translate("Ttune", "Standard: E A D G B E "), Tnote(3, 1, 0), Tnote(7, 0, 0),
					Tnote(5, 0, 0), Tnote(2, 0, 0), Tnote(6, -1, 0), Tnote(3, -1, 0));

	tunes[0] = Ttune(QApplication::translate("Ttune", "Dropped D: D A D G B E "), Tnote(3, 1, 0), Tnote(7, 0, 0),
									 Tnote(5, 0, 0), Tnote(2, 0, 0), Tnote(6, -1, 0),Tnote(2, -1, 0));
	tunes[1] = Ttune(QApplication::translate("Ttune", "Dummy Lute: D A D F# B E "), Tnote(3, 1, 0), Tnote(7, 0, 0),
									 Tnote(4, 0, 1), Tnote(2, 0, 0), Tnote(6, -1, 0),Tnote(2, -1, 0));
	tunes[2] = Ttune(QApplication::translate("Ttune", "Open: D A D G A D "), Tnote(2, 1, 0), Tnote(6, 0, 0),
									 Tnote(5, 0, 0), Tnote(2, 0, 0), Tnote(6, -1, 0),Tnote(2, -1, 0));
	tunes[3] = Ttune(QApplication::translate("Ttune", "Kouyanbaba: D A D A D F "), Tnote(4, 1, 0), Tnote(2, 1, 0),
									 Tnote(6, 0, 0), Tnote(2, 0, 0), Tnote(6, -1, 0),Tnote(2, -1, 0));
	
	bassTunes[0] = Ttune(QApplication::translate("Ttune", "Standard bass: E A D G"), Tnote(5, -1, 0), Tnote(2, -1, 0),
												Tnote(6, -2, 0), Tnote(3, -2, 0), Tnote(0, 0, 0), Tnote(0, 0, 0));
	bassTunes[1] = Ttune(QApplication::translate("Ttune", "Bass in fifths: C G D A"), Tnote(6, -1, 0), Tnote(2, -1, 0),
												Tnote(5, -2, 0), Tnote(1, -2, 0), Tnote(0, 0, 0), Tnote(0, 0, 0));
	bassTunes[2] = Ttune(QApplication::translate("Ttune", "5-str. bass: B E A D G"), Tnote(5, -1, 0), Tnote(2, -1, 0),
												Tnote(6, -2, 0), Tnote(3, -2, 0), Tnote(7, -3, 0), Tnote(0, 0, 0));
	bassTunes[3] = Ttune(QApplication::translate("Ttune", "6-str. bass: B E A D G C"), Tnote(1, 0, 0), Tnote(5, -1, 0), Tnote(2, -1, 0),
												Tnote(6, -2, 0), Tnote(3, -2, 0), Tnote(7, -3, 0));
}

//##################################################################################################
Ttune::Ttune(const QString& tuneName, const Tnote& S1, const Tnote& S2, const Tnote& S3,
						 const Tnote& S4, const Tnote& S5, const Tnote& S6) :
	name(tuneName),
	m_strNumber(0),
	m_isGuitar(true)
{
		stringsArray[0] = S1;
		stringsArray[1] = S2;
		stringsArray[2] = S3;
		stringsArray[3] = S4;
		stringsArray[4] = S5;
		stringsArray[5] = S6;
		
		determineStringsNumber();
}


void Ttune::determineStringsNumber() {
		Tnote tmpStrings[6];
		quint8 strCnt = 0;
		for (int i = 0; i < 6; i++) {
			if (stringsArray[i].note != 0) { // put all defined notes to tmpStrings
				tmpStrings[strCnt] = stringsArray[i];
				strCnt++;
			}
		}
		if (strCnt < 6) // if not all were defined fill the rest of tmpStrings with empty notes
			for (int i = strCnt; i < 6; i++)
				tmpStrings[i] = Tnote(0, 0, 0);
		for (int i = 0; i < 6; i++) // copy all to main stringsArray
			stringsArray[i] = tmpStrings[i];
		m_strNumber = strCnt; // define number of strings
		if (m_strNumber < 3)
			m_isGuitar = false;
		else
			m_isGuitar = true;
}


void Ttune::copy(Ttune& t) {
	name = t.name;
	for (int i = 0; i < 6; ++i)
		stringsArray[i] = t.str(i + 1);
	
}

//##################################################################################################
//###################### I/O OPERATORS #############################################################
//##################################################################################################

/** Defined tuning have following identifiers:
 * -1 - user defined
 * 0 - standard EADGBE
 * 1 to 4 - classical/electric 6 strings defined (@p tunes array)
 * 100 to 103 - bass 4-6 strings defined (@p bassTunes array)
 */
void Ttune::toXml(QXmlStreamWriter& xml, bool isExam) {
	int id = -1; // user defined tuning
	if (isExam) { // determine is tuning built-in or user defined
		xml.writeStartElement("tuning");
		if (operator==(stdTune))
			id = 0;
		else {
			for (int i = 0; i < 4; ++i) {
				if (operator==(tunes[i])) {
					id = i + 1;	break;
				}
			}
		}
		if (id == -1) { // defined tuning still not found - try bass
			for (int i = 0; i < 4; ++i) {
				if (operator==(bassTunes[i])) {
					id = i + 100;	break;
				}
			}
		}
		xml.writeAttribute("id", QVariant(id).toString());
	} else
		xml.writeStartElement("staff-details");
	if (id == -1) { // write tuning data
		if (isExam)
				xml.writeTextElement("name", name); // skip tuning name for musicXML (save it for exam only)
		xml.writeTextElement("staff-lines", QVariant(stringNr()).toString());
		for (int i = 0; i < stringNr(); ++i) {
			stringsArray[i].toXml(xml, "staff-tuning", "tuning-", "line", QVariant(i + 1).toString());
		}
	}
	xml.writeEndElement(); // <tuning> or <staff-details>
}


bool Ttune::fromXml(QXmlStreamReader& xml, bool isExam) {
	bool ok = true;
	int id = -1;
	if (isExam) {
		id = xml.attributes().value("id").toInt();
		if (id < -1 || (id > 4 && id < 100) || (id > 103)) {
			qDebug() << "Tuning had wrong 'id'. Standard tuning will be used";
			ok = false;
		}
		switch (id) {
			case -1: break;
			case 1:
			case 2:
			case 3:
			case 4:
				copy(tunes[id]); break;
			case 100:
			case 101:
			case 102:
			case 103:
				copy(bassTunes[id - 100]); break;
			default:
				copy(stdTune); break;
		}
	}
	if (id == -1) {
		int sNrGet = 6, sNrCount = 0;
		while (xml.readNextStartElement()) {
			if (xml.name() == "name") // exam only
				name = xml.readElementText();
			else if (xml.name() == "staff-lines")
				sNrGet = QVariant(xml.readElementText()).toInt();
			else if (xml.name() == "staff-tuning") {
				sNrCount++;
				int line = xml.attributes().value("line").toInt();
				if (line > 0 && line <= sNrGet && line <= 6) {
					stringsArray[line - 1].fromXml(xml, "tuning-");
					if (!stringsArray[line - 1].isValid()) {
						qDebug() << "Wrong note describing a string" << line;
						ok = false;
					}
				} else {
					qDebug() << "Wrong string (line) number";
					ok = false;
				}
			} else
				xml.skipCurrentElement();
		}
		if (sNrGet != sNrCount) {
			qDebug() << "String numbers not matched" << sNrGet << sNrCount;
			ok = false;
		}
		if (!ok)
			copy(stdTune);
    else
      determineStringsNumber();
	} else
			xml.skipCurrentElement(); // we are not reading from <tuning> tag content so it has to be skipped
	return ok;
}


QDataStream &operator<< (QDataStream &out, const Ttune &t) {
    out << t.name;
    for (int i=0; i < 6; i++)
      out << t.stringsArray[i];
    return out;
}


QDataStream &operator>> (QDataStream &in, Ttune &t) {
    in >> t.name;
    for (int i=0; i < 6; i++)
        in >> t.stringsArray[i];
		t.determineStringsNumber();
    return in;
}






