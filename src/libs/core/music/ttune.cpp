/***************************************************************************
 *   Copyright (C) 2006-2020 by Tomasz Bojczuk                             *
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

#include "ttune.h"

#include <QtCore/qvariant.h>
#include <QtCore/qdebug.h>
#include <QtGui/qguiapplication.h>


/*static*/
Ttune Ttune::stdTune = Ttune();

Ttune Ttune::tunes[4];
Ttune Ttune::bassTunes[4];


void Ttune::prepareDefinedTunes() {
  stdTune = Ttune(QGuiApplication::translate("Ttune", "Standard: E A D G B E "), Tnote(3, 1, 0), Tnote(7, 0, 0),
          Tnote(5, 0, 0), Tnote(2, 0, 0), Tnote(6, -1, 0), Tnote(3, -1, 0), Standard_EADGBE);

  tunes[0] = Ttune(QGuiApplication::translate("Ttune", "Dropped D: D A D G B E "), Tnote(3, 1, 0), Tnote(7, 0, 0),
                   Tnote(5, 0, 0), Tnote(2, 0, 0), Tnote(6, -1, 0),Tnote(2, -1, 0), Dropped_D_DADGBE);
  tunes[1] = Ttune(QGuiApplication::translate("Ttune", "Dummy Lute: D A D F# B E "), Tnote(3, 1, 0), Tnote(7, 0, 0),
                   Tnote(4, 0, 1), Tnote(2, 0, 0), Tnote(6, -1, 0),Tnote(2, -1, 0), DummyLute_DADFshBE);
  tunes[2] = Ttune(QGuiApplication::translate("Ttune", "Open: D A D G A D "), Tnote(2, 1, 0), Tnote(6, 0, 0),
                   Tnote(5, 0, 0), Tnote(2, 0, 0), Tnote(6, -1, 0),Tnote(2, -1, 0), Open_DADGAD);
  tunes[3] = Ttune(QGuiApplication::translate("Ttune", "Kouyanbaba: D A D A D F "), Tnote(4, 1, 0), Tnote(2, 1, 0),
                   Tnote(6, 0, 0), Tnote(2, 0, 0), Tnote(6, -1, 0),Tnote(2, -1, 0), Kouyanbaba_DADADF);

  bassTunes[0] = Ttune(QGuiApplication::translate("Ttune", "Standard bass: E A D G"), Tnote(5, 0, 0), Tnote(2, 0, 0),
                        Tnote(6, -1, 0), Tnote(3, -1, 0), Tnote(0, 0, 0), Tnote(0, 0, 0), Bass4_EADG);
  bassTunes[1] = Ttune(QGuiApplication::translate("Ttune", "Bass in fifths: C G D A"), Tnote(6, 0, 0), Tnote(2, 0, 0),
                        Tnote(5, -1, 0), Tnote(1, -1, 0), Tnote(0, 0, 0), Tnote(0, 0, 0), Bass4_5ths_CGDA);
  bassTunes[2] = Ttune(QGuiApplication::translate("Ttune", "5-str. bass: B E A D G"), Tnote(5, 0, 0), Tnote(2, 0, 0),
                        Tnote(6, -1, 0), Tnote(3, -1, 0), Tnote(7, -2, 0), Tnote(0, 0, 0), Bass5_BEADG);
  bassTunes[3] = Ttune(QGuiApplication::translate("Ttune", "6-str. bass: B E A D G C"), Tnote(1, 1, 0), Tnote(5, 0, 0), Tnote(2, 0, 0),
                        Tnote(6, -1, 0), Tnote(3, -1, 0), Tnote(7, -2, 0), Bass6_BEADGC);
}


//##################################################################################################
Ttune::Ttune(const QString& tuneName, const Tnote& S1, const Tnote& S2, const Tnote& S3,
             const Tnote& S4, const Tnote& S5, const Tnote& S6, Etunings tunType) :
  name(tuneName),
  p_tuning(tunType),
  m_strNumber(0)
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
    if (stringsArray[i].note() != 0) { // put all defined notes to tmpStrings
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
}


void Ttune::copy(Ttune& t) {
  name = t.name;
  for (int i = 0; i < 6; ++i)
    stringsArray[i] = t.str(i + 1);
  p_tuning = t.type();
  determineStringsNumber();
}


QString Ttune::definedName(Ttune::Etunings t) {
  switch (t) {
    case Standard_EADGBE: return stdTune.name;
    case Dropped_D_DADGBE: return tunes[0].name;
    case DummyLute_DADFshBE: return tunes[1].name;
    case Open_DADGAD: return tunes[2].name;
    case Kouyanbaba_DADADF: return tunes[3].name;
    case Bass4_EADG: return bassTunes[0].name;
    case Bass4_5ths_CGDA: return bassTunes[1].name;
    case Bass5_BEADG: return bassTunes[2].name;
    case Bass6_BEADGC: return bassTunes[3].name;
    default: return QString();
  }
}


void Ttune::riseOctaveUp() {
  for (int i = 0; i < 6; ++i)
    stringsArray[i].riseOctaveUp();
  p_tuning = findTuning(*this);
}


Ttune::Etunings Ttune::findTuning(const Ttune& t) {
  if (t.stringNr() == 0)
    return Ttune::NoTuning;
  else if (t.stringNr() < 3)
    return Ttune::Scale;
  else {
    if (t == Ttune::stdTune)
        return Ttune::Standard_EADGBE;
    else {
        for (int i = 0; i < 4; ++i) {
          if (t == Ttune::tunes[i])
            return Ttune::tunes[i].type();
          else if (t == Ttune::bassTunes[i])
            return Ttune::bassTunes[i].type();
        }
    }
  }
  return Ttune::Custom;
}

//##################################################################################################
//###################### I/O OPERATORS #############################################################
//##################################################################################################

/**
 * Defined tuning have following identifiers:
 * -1 - user defined
 * 0 - standard EADGBE
 * 1 to 4 - classical/electric 6 strings defined (@p tunes array)
 * 100 to 103 - bass 4-6 strings defined (@p bassTunes array)
 *
 * FIXME: We skip Ttune::SCALE type and write it as such as Ttune::Custom
 * It doesn't harm, just jet...
 */
void Ttune::toXml(QXmlStreamWriter& xml, bool isExam) {
  int id = -1; // user defined tuning
  if (isExam) { // determine is tuning built-in or user defined
      xml.writeStartElement(QStringLiteral("tuning"));
      id = static_cast<int>(findTuning(*this));
      if (id < -1)
        id = -1;
      xml.writeAttribute(QStringLiteral("id"), QVariant(id).toString());
  } else
      xml.writeStartElement(QStringLiteral("staff-details"));

  if (id == -1) { // write tuning data
    if (isExam)
      xml.writeTextElement(QStringLiteral("name"), name); // skip tuning name for musicXML (save it for exam only)
    xml.writeTextElement(QStringLiteral("staff-lines"), QVariant(stringNr()).toString());
    for (int i = 0; i < stringNr(); ++i) {
      stringsArray[i].toXml(xml, QStringLiteral("staff-tuning"), QStringLiteral("tuning-"), QStringLiteral("line"), QVariant(i + 1).toString());
    }
  }
  xml.writeEndElement(); // <tuning> or <staff-details>
}


bool Ttune::fromXml(QXmlStreamReader& xml, bool isExam) {
  bool ok = true;
  int id = -1;
  if (isExam) {
    id = xml.attributes().value(QStringLiteral("id")).toInt();
    if (id < -1 || (id > 4 && id < 100) || (id > 103)) {
      qDebug() << "[Ttune] Tuning had wrong 'id'. Standard tuning will be used";
      ok = false;
    }
    switch (static_cast<Etunings>(id)) {
      case Custom: break;
      case Dropped_D_DADGBE:
      case DummyLute_DADFshBE:
      case Open_DADGAD:
      case Kouyanbaba_DADADF:
        copy(tunes[id]); break;
      case Bass4_EADG:
      case Bass4_5ths_CGDA:
      case Bass5_BEADG:
      case Bass6_BEADGC:
        copy(bassTunes[id - 100]); break;
      default:
        copy(stdTune); break;
    }
  }
  if (id == -1) {
      int sNrGet = 6, sNrCount = 0;
      while (xml.readNextStartElement()) {
        if (xml.name() == QLatin1String("name")) // exam only
            name = xml.readElementText();
        else if (xml.name() == QLatin1String("staff-lines"))
            sNrGet = QVariant(xml.readElementText()).toInt();
        else if (xml.name() == QLatin1String("staff-tuning")) {
            sNrCount++;
            int line = xml.attributes().value(QStringLiteral("line")).toInt();
            if (line > 0 && line <= sNrGet && line <= 6) {
                stringsArray[line - 1].fromXml(xml, QStringLiteral("tuning-"));
                if (!stringsArray[line - 1].isValid()) {
                  qDebug() << "[Ttune] Wrong note describing a string" << line;
                  ok = false;
                }
            } else {
                qDebug() << "[Ttune] Wrong string (line) number";
                ok = false;
            }
        } else
            xml.skipCurrentElement();
      }
      if (sNrGet != sNrCount) {
        qDebug() << "[Ttune] String numbers not match" << sNrGet << sNrCount;
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
  for (int i = 0; i < 6; i++)
    out << t.stringsArray[i];
  return out;
}


QDataStream &operator>> (QDataStream &in, Ttune &t) {
  in >> t.name;
  for (int i=0; i < 6; i++)
      in >> t.stringsArray[i];
  t.determineStringsNumber();
  // determine tuning type
  if (t.stringNr() == 0)
      t.p_tuning = Ttune::NoTuning;
  else if (t.stringNr() < 3)
      t.p_tuning = Ttune::Scale;
  else {
      if (t == Ttune::stdTune)
          t.p_tuning = Ttune::Standard_EADGBE;
      else {
          for (int i = 0; i < 4; ++i) {
            if (t == Ttune::tunes[i]) {
                t.p_tuning = Ttune::tunes[i].type();
                break;
            } else if (t == Ttune::bassTunes[i]) {
                t.p_tuning = Ttune::bassTunes[i].type();
                break;
            }
          }
          if (t.type() == Ttune::NoTuning)
            t.p_tuning = Ttune::Custom;
      }
  }

  return in;
}






