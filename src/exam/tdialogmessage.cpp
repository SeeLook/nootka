/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/



#include "tdialogmessage.h"
#include <QLabel>

TdialogMessage::TdialogMessage(const QRect &parentGeo, TQAunit &question, QWidget *parent) :
    QWidget(0, Qt::FramelessWindowHint)
{
    setWindowOpacity(100);
    setGeometry(parentGeo.left() + parentGeo.width() / 2, parentGeo.height() + parentGeo.height() / 2, parentGeo.width() / 4, parentGeo.height() / 4);
    QLabel *mainLab = new QLabel("<span style=\"font-size: 30px;\">lets try</span>", this);
    show();
}
