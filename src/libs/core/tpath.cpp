/***************************************************************************
 *   Copyright (C) 2014-2017 by Tomasz Bojczuk                             *
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


#include "tpath.h"
#include <QtCore/qdebug.h>


QString Tpath::main = QString();
Tpath* Tpath::m_instance = nullptr;


Tpath::Tpath(QObject* parent) :
  QObject(parent)
{
  if (m_instance) {
    qDebug() << "Tpath instance already exists";
    return;
  }
  m_instance = this;
}


Tpath::~Tpath() { m_instance = nullptr; }


QString Tpath::img(const char* imageFileName, const char* ext) {
  return QString("%1picts/%2%3").arg(main, imageFileName, ext);
}


QString Tpath::pix(const QString& imageFileName) {
  return QString("file:%1picts/%2.png").arg(main, imageFileName);
}
