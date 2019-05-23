/***************************************************************************
 *   Copyright (C) 2013-2019 by Tomasz Bojczuk                             *
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

#include "tinstrument.h"
#include <QtGui/qguiapplication.h>

#include <QtCore/qdebug.h>


//#################################################################################################
//###################     static const definitions     ############################################
//#################################################################################################

static const char* const nameArray[INSTR_COUNT] = {
  QT_TRANSLATE_NOOP("Tinstrument", "other instrument"), QT_TRANSLATE_NOOP("Tinstrument", "Classical Guitar"),
  QT_TRANSLATE_NOOP("Tinstrument", "Electric Guitar"), QT_TRANSLATE_NOOP("Tinstrument", "Bass Guitar"),
  QT_TRANSLATE_NOOP("Tinstrument", "Piano"), QT_TRANSLATE_NOOP("Tinstrument", "Bandoneon"),
  QT_TRANSLATE_NOOP("Tinstrument", "Alt Saxophone"), QT_TRANSLATE_NOOP("Tinstrument", "Tenor Saxophone")
};


static const char* const glyphArray[INSTR_COUNT] = {
  "v", "h", "i", "j", "f", "e", "P", "Q"
};


static const quint8 clefArray[INSTR_COUNT] = {
  1, 8, 8, 2, 128, 128, 1, 1
};


static const char* const qmlFileArray[INSTR_COUNT] = {
  "", "Guitar", "Guitar", "Guitar", "Piano", "Bandoneon", "Sax", "Sax"
};


static const qreal heightArray[INSTR_COUNT] = {
  0.0, 4.0, 4.0, 4.0, 5.0, 3.0, 1.0, 1.0
};


static const qint8 transArray[INSTR_COUNT] = {
  0, 0, 0, -12, 0, 0, -9, -14
};

//#################################################################################################
//###################              Tinstrument         ############################################
//#################################################################################################
Tinstrument::Tinstrument(Tinstrument::Etype type) :
  m_type(type)
{
}


QString Tinstrument::name() const {
  return staticName(m_type);
}


QString Tinstrument::staticName(Tinstrument::Etype t) {
  int ti = static_cast<int>(t);
  return ti < 0 || ti > INSTR_COUNT - 1 ?  QString() : QGuiApplication::translate("Tinstrument", nameArray[ti]);
}


QString Tinstrument::glyph() const {
  return QString(glyphArray[static_cast<int>(m_type)]);
}


int Tinstrument::clef() const {
  return clefArray[static_cast<int>(m_type)];
}


QString Tinstrument::qmlFile() const {
  return QString(qmlFileArray[static_cast<int>(m_type)]);
}



qreal Tinstrument::heightPart() const {
  return heightArray[static_cast<int>(m_type)];
}


int Tinstrument::transposition() const {
  return transArray[static_cast<int>(m_type)];
}


qreal Tinstrument::skipStillerVal() const {
  switch (m_type) {
    case ClassicalGuitar:
    case ElectricGuitar:
    case BassGuitar: return 80.0;
    default: return 0.0;
  }
}


qreal Tinstrument::minSplitVol() const {
  switch (m_type) {
    case ClassicalGuitar: return 7.0;
    case ElectricGuitar:
    case BassGuitar: return 10.0;
    default: return 0;
  }
}


int Tinstrument::fretNumber() const {
  switch (m_type) {
    case ClassicalGuitar: return 19;
    case ElectricGuitar: return 23;
    case BassGuitar: return 20;
    default: return 0;
  }
}
