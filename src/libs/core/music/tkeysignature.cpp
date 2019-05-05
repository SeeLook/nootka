/***************************************************************************
 *   Copyright (C) 2011-2017 by Tomasz Bojczuk                             *
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


#include "tkeysignature.h"
#include "tglobals.h"
#include "tscoreparams.h"
#include <QtCore/qxmlstream.h>
#include <QtCore/qvariant.h>


/*static*/
const char TkeySignature::scalesDefArr[15][7] = {
    {-1,-1,-1,-1,-1,-1,-1},       // Cb/ab
    {-1,-1,-1, 0,-1,-1,-1},       // Gb/eb
    { 0,-1,-1, 0,-1,-1,-1},       // Db/bb
    { 0,-1,-1, 0, 0,-1,-1},       // Ab/f
    { 0, 0,-1, 0, 0,-1,-1},       // Eb/c
    { 0, 0,-1, 0, 0, 0,-1},       // B/g
    { 0, 0, 0, 0, 0, 0,-1},       // F/d
    { 0, 0, 0, 0, 0, 0, 0},       // C/a
    { 0, 0, 0, 1, 0, 0, 0},       // G/e
    { 1, 0, 0, 1, 0, 0, 0},       // D/h
    { 1, 0, 0, 1, 1, 0, 0},       // A/f#
    { 1, 1, 0, 1, 1, 0, 0},       // E/c#
    { 1, 1, 0, 1, 1, 1, 0},       // B/g#
    { 1, 1, 1, 1, 1, 1, 0},       // F#/d#
    { 1, 1, 1, 1, 1, 1, 1},       // C#/a#
};

const char TkeySignature::majorKeys[15] = { 0, 4, 1, 5, 2, 6, 3, 0, 4, 1, 5, 2, 6, 3, 0 };
const char TkeySignature::minorKeys[15] = { 5, 2, 6, 3, 0, 4, 1, 5, 2, 6, 3, 0, 4, 1, 5 };

QString TkeySignature::majorNames[15] = { QString(), QString(), QString(), QString(), QString(),
                                          QString(), QString(), QString(), QString(), QString(),
                                          QString(), QString(), QString(), QString(), QString()};
QString TkeySignature::minorNames[15] = { QString(), QString(), QString(), QString(), QString(),
                                          QString(), QString(), QString(), QString(), QString(),
                                          QString(), QString(), QString(), QString(), QString()};

void TkeySignature::setNameStyle(Tnote::EnameStyle style, const QString& majSuf, const QString& minSuf) {
  Tnote n;
  QString majS, minS;
  auto minus = QStringLiteral("-");
  if (majSuf.isEmpty()) {
      majS = minus + majorSufixTxt();
      GLOB->S->majKeyNameSufix = majorSufixTxt();
  } else
      if (majSuf != QLatin1String(" "))
        majS = minus + majSuf;

  if (minSuf.isEmpty()) {
      minS = minus + minorSufixTxt();
      GLOB->S->minKeyNameSufix = minorSufixTxt();
  } else
    if (minSuf != QLatin1String(" "))
      minS = minus + minSuf;

    for (int i = 0; i < 15; i++) {
        n = Tnote(majorKeys[i] + 1, 0, scalesDefArr[i][int(majorKeys[i])]);
        majorNames[i] = n.toText(style, false);
        majorNames[i] += majS;
        n = Tnote(minorKeys[i] + 1, 0, scalesDefArr[i][int(minorKeys[i])]);
        minorNames[i] = n.toText(style, false ).toLower();
        minorNames[i] += minS;
    }
}

Tnote TkeySignature::inKey(TkeySignature k, const Tnote& n) {
    return inKeyPrivate(k.value(), n);
}


TkeySignature::TkeySignature()
{
  m_key = 0;
  m_isMinor = false;
}


TkeySignature::TkeySignature(char keyS, bool isMinor)
{
  if (keyS > -8 && keyS < 8)
      m_key = keyS;
  else
      m_key = 0;
  m_isMinor = isMinor;
}


QString TkeySignature::accidNumber(bool inHtml) const {
    QString a;
    if (m_key < 0)
        a = QStringLiteral("b");
    if (m_key > 0)
        a = QStringLiteral("#");
    QString S = QString("%1").arg(int(qAbs(m_key)));
    if (inHtml)
        S += QLatin1String("<span style=\"font-family: nootka;\">");
    S += a;
    if (inHtml)
        S += QLatin1String("</span>");
    return S;
}


Tnote TkeySignature::inKey(const Tnote& n) const {
  return inKeyPrivate(value(), n);
}


Tnote TkeySignature::tonicNote(int octave) const {
  char tonicNoteNr = isMinor() ? minorKeys[value() + 7] : majorKeys[value() + 7];
  return Tnote(tonicNoteNr + 1, octave, scalesDefArr[value() + 7][static_cast<int>(tonicNoteNr)]);
}


void TkeySignature::toXml(QXmlStreamWriter& xml) {
  xml.writeStartElement("key");
    xml.writeTextElement("fifths", QVariant(static_cast<int>(value())).toString());
    QString mode = isMinor() ? QLatin1String("minor") : QLatin1String("major");
    xml.writeTextElement("mode", mode);
  xml.writeEndElement(); // key
}


void TkeySignature::fromXml(QXmlStreamReader& xml) {
  if (xml.name() == QLatin1String("key")) {
    while (xml.readNextStartElement()) {
      if (xml.name() == QLatin1String("fifths")) {
        m_key = (char)qBound(-7, xml.readElementText().toInt(), 7);
      } else if (xml.name() == QLatin1String("mode")) {
          if (xml.readElementText() == QLatin1String("minor"))
              m_isMinor = true;
          else
              m_isMinor = false;
      } else
        xml.skipCurrentElement();
    }
  }
}


int TkeySignature::difference(const TkeySignature& otherKey) const {
  int diff = TkeySignature(otherKey.value()).tonicNote().chromatic() - TkeySignature(m_key).tonicNote().chromatic();
  if (qAbs(diff) > 6) {
    diff = diff - 12;
    if (diff < -6)
      diff += 12;
  }
  return diff;
}


QDataStream &operator << (QDataStream &out, TkeySignature &key) {
    quint8 kk = key.value();
    if (key.isMinor())
      kk += 15;
//     out << qint8(key.value());
    out << kk;
    return out;
}


bool getKeyFromStream(QDataStream &in, TkeySignature &k) {
    bool ok = true;
    qint8 kk;
    in >> kk;
    if (kk < -7 || kk > 22) {
        kk = 0; ok = false;
    }
    if (ok && kk > 7 ) // is minor key
  k = TkeySignature(char(kk - 15), true);
    else
  k = TkeySignature(char(kk));
    return ok;
}


//#################################################################################################
//###################              PRIVATE             ############################################
//#################################################################################################
Tnote TkeySignature::inKeyPrivate(char val, const Tnote& n) {
  int v = val + 7;
  if (scalesDefArr[v][n.note() - 1] == n.alter())
      return n;
  Tnote tmpN = n.showWithFlat();
  if (scalesDefArr[v][tmpN.note() - 1] == tmpN.alter())
      return tmpN;
  tmpN = n.showWithSharp();
  if (scalesDefArr[v][tmpN.note() - 1] == tmpN.alter())
      return tmpN;
  tmpN = n.showAsNatural();
  if (scalesDefArr[v][tmpN.note() - 1] == tmpN.alter())
      return tmpN;

  return Tnote(0, 0, 0);
}




