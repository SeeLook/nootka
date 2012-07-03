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
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/



#include "tsupportnootka.h"
#include <QVBoxLayout>
#include <QLabel>

TsupportNootka::TsupportNootka(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *lay = new QVBoxLayout;
    QLabel *lab = new QLabel("<b>You also can help with makeing Notka better</b><br><ul><li><b>Translate Nootka</b><br>It does not require any programming skills.<br></li><li><b>Test Nootka intensively</b></li><li><b>Maintain Nootka package</b><br>for Your system or Distibution</li><li><b>Donate Nootka</b></li><li><b>Vote on Nootka</b><br>There are a lot of services. For example:<br> - <a href=\"http://kde-apps.org/content/show.php/Nootka?content=143598\">Kde-Apps</a><br> - <a href=\"http://qt-apps.org/content/show.php/Nootka?content=148705\">Qt-Apps</a><br> - <a href=\"http://www.softpedia.com/get/Others/Home-Education/Nootka.shtml\">Softpedia</a></li></ul>", this);
//    lab->setAlignment(Qt:: AlignCenter);
    lab->setOpenExternalLinks(true);
    lay->addWidget(lab);

    setLayout(lay);

}
