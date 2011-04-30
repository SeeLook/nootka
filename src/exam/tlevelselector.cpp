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
#include "tglobals.h"

extern Tglobals *gl;

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

void TlevelSelector::selectLevel(int id) {
    if (id >= 0 && id < levelsList->count())
        levelsList->setCurrentRow(id);
}

void TlevelSelector::selectLevel() {
    levelsList->setCurrentRow(levelsList->count()-1);
}

//#########################  TlevelSummaryWdg ################################################

/*static*/
QString TlevelSummaryWdg::notesRangeTxt = tr("notes' range:");
QString TlevelSummaryWdg::fretsRangeTxt = tr("frets' range:");

TlevelSummaryWdg::TlevelSummaryWdg(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;
    QLabel *headLab = new QLabel(tr("Level summary:"),this);
    mainLay->addWidget(headLab);
    summLab = new QLabel(tr("\n no level selected"), this);
    mainLay->addWidget(summLab);
    mainLay->addStretch(1);
    setLayout(mainLay);

}

void TlevelSummaryWdg::setLevel(TexamLevel tl) {
    QString S;
    S = "<center><b>" + tl.name + "</b>";
    S += "<table border=\"1\">";
    S += "<tr><td>" + notesRangeTxt + " </td>";
    S += "<td>" + TnoteName::noteToRichText(tl.loNote) + " - "
         + TnoteName::noteToRichText(tl.hiNote) + "</td></tr>";
    if (tl.questionAs.isFret() || tl.answersAs[0].isFret()
        || tl.answersAs[1].isFret() || tl.answersAs[2].isFret()
        || tl.answersAs[3].isFret()) { // level uses guitar
        S += "<tr><td>" + fretsRangeTxt + " </td>";
        S += QString("<td>%1 - %2").arg(int(tl.loFret)).arg(int(tl.hiFret)) + "</td></tr>";
    }
    if (tl.useKeySign) {
        S += "<tr><td>" + tr("key signature:") + " </td><td>";
        S += tl.loKey.getMajorName().remove("-"+gl->SmajKeyNameSufix);
        S += " (" + tl.loKey.accidNumber(true) +")";
        if (!tl.isSingleKey) {
            S += " - " + tl.hiKey.getMajorName().remove("-"+gl->SmajKeyNameSufix);
            S += " (" + tl.hiKey.accidNumber(true) + ")";
        }
        S += "</td></tr>";
    }
    S += "<tr><td>" + tr("accidentals:") + " </td><td>";
    if (!tl.withSharps && !tl.withFlats && !tl.withDblAcc)
        S += tr("none");
    else {
        if (tl.withSharps) S += " <i>#</i>";
        if (tl.withFlats) S += " <i>b</i>";
        if (tl.withDblAcc) S += " <i>x bb</i>";
    }
    S += "</td></tr>";
    S += "</table></center>";
    summLab->setText(S);
}
