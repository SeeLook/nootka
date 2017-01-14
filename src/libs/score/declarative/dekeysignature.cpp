/***************************************************************************
 *   Copyright (C) 2017 by Tomasz Bojczuk                                  *
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


#include "dekeysignature.h"
#include "descore.h"

#include <QtCore/qdebug.h>


DeKeySignature::DeKeySignature(QQuickItem* parent) :
  QQuickItem(parent),
  m_key(0),
  m_score(nullptr)
{
}


DeKeySignature::~DeKeySignature()
{
  qDebug() << "Bye Bye!";
}


void DeKeySignature::setKeySignature(qint8 k) {
  if (k > -8 && k < 8) {
    m_key = k;
    if (m_score) {
      m_score->setKeySignature(k);
      emit keySignatureChanged();
    }
  }
}
