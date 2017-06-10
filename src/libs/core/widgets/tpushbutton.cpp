/***************************************************************************
 *   Copyright (C) 2011-2016 by Tomasz Bojczuk                             *
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

#include "tpushbutton.h"
#include <QtWidgets/qapplication.h>
#include <QtGui/qpalette.h>


/*static*/
QColor TpushButton::m_backColor = Qt::black;
QColor TpushButton::m_textColor = Qt::white;


void TpushButton::setCheckColor(QColor background, QColor text) {
  m_backColor = background;
  m_textColor = text;
}


void setNativeColors(TpushButton* b) {
  b->setStyleSheet(QString("background-color: %1; color: palette(text);").arg(qApp->palette().window().color().lighter(105).name()));
}


TpushButton::TpushButton(const QString& text, QWidget* parent):
   QPushButton(text, parent),
   m_Ichecked(false)
{
  m_textThis = m_textColor;
  m_backThis = m_backColor;
  setNativeColors(this);
}

void TpushButton::setThisColors(QColor background, QColor text) {
  m_backThis = background;
  m_textThis = text;
  setChecked(isChecked()); // it will be refreshed when is checked
}


void TpushButton::setChecked(bool isChecked) {
  if (isChecked)
    setStyleSheet(QString("background-color: %1; color: %2;").arg(m_backThis.name()).arg(m_textThis.name()));
  else
    setNativeColors(this);
  m_Ichecked = isChecked;
}








