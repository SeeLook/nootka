/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
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


#include "tlevelselector.h"
#include "tnotename.h"

TlevelSelector::TlevelSelector(QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout *mainLay = new QHBoxLayout;

    QVBoxLayout *levLay = new QVBoxLayout;
    QLabel *levLab = new QLabel(tr("Levels:"),this);
    levLay->addWidget(levLab);
    levelsList = new QListWidget(this);
    levelsList->setMouseTracking(true);
    levelsList->setFixedWidth(200);
    levLay->addWidget(levelsList);

    mainLay->addLayout(levLay);

    summWdg = new TlevelSummaryWdg(this);
    mainLay->addWidget(summWdg);

    setLayout(mainLay);

    findLevels();

    connect(levelsList, SIGNAL(currentRowChanged(int)), this, SLOT(levelSelected(int)));
}

void TlevelSelector::levelSelected(int id) {
    summWdg->setLevel(levList[id]);
    emit levelChanged(levList[id]);
}

void TlevelSelector::findLevels() {
    TexamLevel lev = TexamLevel();
    // from constructor
    addLevel(lev);
}

void TlevelSelector::addLevel(const TexamLevel &lev) {
    levelsList->addItem(lev.name);
    levList << lev;
    levelsList->item(levList.size()-1)->setStatusTip(lev.desc);
}


//#########################  TlevelSummaryWdg ################################################

TlevelSummaryWdg::TlevelSummaryWdg(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;
    QLabel *headLab = new QLabel(tr("Level summary:"),this);
    mainLay->addWidget(headLab);
    summLab = new QLabel(this);
    mainLay->addWidget(summLab);
    mainLay->addStretch(1);
    setLayout(mainLay);

    summLab->setText("Here will be<br>exam's level summary.");
}

void TlevelSummaryWdg::setLevel(const TexamLevel &lev) {
    QString S;
    S = "<center><b>" + lev.name + "</b>";
    S += "<table>";
    S += "<tr><td>" + tr("note's range: ") + "</td>";
    S += "<td>" + TnoteName::noteToRichText(lev.loNote) + " - "
         + TnoteName::noteToRichText(lev.hiNote) + "</td>";
    S += "</table></center>";
    summLab->setText(S);
}
