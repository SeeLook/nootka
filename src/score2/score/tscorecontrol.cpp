/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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

#include "tscorecontrol.h"
#include "tpushbutton.h"
#include "tscorescene.h"
#include <QVBoxLayout>
#include <QGraphicsProxyWidget>

TscoreControl::TscoreControl(TscoreScene* scene):
  QWidget()
{
    m_proxy = scene->addWidget(this);
    m_proxy->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    
    m_dblSharpBut = new TpushButton("x");
    setButtons(m_dblSharpBut);
    m_sharpBut = new TpushButton("#");
    setButtons(m_sharpBut);
    m_flatBut = new TpushButton("b");
    setButtons(m_flatBut);
    m_dblFlatBut = new TpushButton("B");
    setButtons(m_dblFlatBut);
    QVBoxLayout *butLay = new QVBoxLayout;
    butLay->addStretch(1);
    butLay->addWidget(m_dblSharpBut);
    butLay->addWidget(m_sharpBut);
    butLay->addSpacing(5);
    butLay->addWidget(m_flatBut);
    butLay->addWidget(m_dblFlatBut);
    butLay->addStretch(1);
    setLayout(butLay);
    
}


void TscoreControl::setButtons(TpushButton* button) {
    button->setFixedSize(40, 45);
#if defined(Q_OS_MAC)
    button->setFont(QFont("nootka", 25, QFont::Normal));
#else
    button->setFont(QFont("nootka", 20, QFont::Normal));
#endif


}

