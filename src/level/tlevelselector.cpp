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

QList<TexamLevel> getExampleLevels() {
    QList<TexamLevel> llist;
    TexamLevel l = TexamLevel();

    l.name = QObject::tr("open strings");
    l.desc = QObject::tr("The simplest. No key signatures, no double accidentals.<br>Automatically adjusted to current tune.");
    l.answersAs[0] = TQAtype(false, true, true, false);
    l.answersAs[1] = TQAtype(true, false, true, false);
    l.withSharps = false;
    l.withFlats = false;
    l.withDblAcc = false;
    l.useKeySign = false;
    l.loKey = 0;
    l.hiKey = 0;
    l.forceAccids = false;
    l.requireOctave = false;
    l.requireStyle = false;
    l.showStrNr = false;
    l.hiNote = Tnote(gl->hiString().getChromaticNrOfNote()); //loNote is lowest by constructor
    l.isNoteHi = false;
    l.hiFret = 0;// loFret is 0 by constuctor
    l.isFretHi = false;
    llist << l;

    l.name = QObject::tr("C-major scale");
    l.desc = QObject::tr("In first position. No accidentals.<br>Octave required.");
    l.answersAs[0] = TQAtype(false, true, true, false);
    l.answersAs[1] = TQAtype(true, false, true, false);
    l.withSharps = false;
    l.withFlats = false;
    l.withDblAcc = false;
    l.useKeySign = false;
    l.loKey = 0;
    l.hiKey = 0;
    l.forceAccids = false;
    l.requireOctave = true;
    l.requireStyle = false;
    l.showStrNr = false;
    l.loNote = Tnote(1,0,0);
    l.isNoteLo = false;
    l.hiNote = Tnote(1,1,0);
    l.isNoteHi = false;
    l.hiFret = 3;// loFret is 0 by constuctor
    l.isFretHi = false;
    llist << l;

    return llist;
}

/*static*/
const qint32 TlevelSelector::levelVersion = 0x95121701;
QString TlevelSelector::levelFilterTxt = QObject::tr("Levels (*.nlv)");

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
    loadBut = new QPushButton(tr("Load"), this);
    loadBut->setStatusTip(tr("Load exam's level from file"));
    levLay->addStretch(1);
    levLay->addWidget(loadBut);

    mainLay->addLayout(levLay);

    summWdg = new TlevelSummaryWdg(this);
    mainLay->addWidget(summWdg);

    setLayout(mainLay);

    findLevels();

    connect(levelsList, SIGNAL(currentRowChanged(int)), this, SLOT(levelSelected(int)));
    connect(loadBut, SIGNAL(clicked()), this, SLOT(m_loadFromFile()));
}

void TlevelSelector::levelSelected(int id) {
    summWdg->setLevel(levList[id]);
    emit levelChanged(levList[id]);
}

void TlevelSelector::findLevels() {
    TexamLevel lev = TexamLevel();
  // from constructor
    addLevel(lev);
  // from predefined list
    QList<TexamLevel> llist = getExampleLevels();
    for (int i = 0; i < llist.size(); i++)
        addLevel(llist[i]);
  // from setting file - recent load/saved levels
    QSettings sett;
    QStringList recentLevels = sett.value("recentLevels").toStringList();
    for (int i = recentLevels.size()-1; i >= 0; i--) {
        QFile file(recentLevels[i]);
        if (file.exists()) {
            TexamLevel level = getLevelFromFile(file);
            if (level.name != "")
                addLevel(level);
            else
                recentLevels.removeAt(i);
        }
        else
            recentLevels.removeAt(i);
    }

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

void TlevelSelector::m_loadFromFile() {
    emit levelToLoad();
}

void TlevelSelector::loadFromFile() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load exam's level"),
                                                     QDir::homePath(), levelFilterTxt);
    QFile file(fileName);
    TexamLevel level = getLevelFromFile(file);
    if (level.name != "") {
        addLevel(level);
        selectLevel(); // select the last
        updateRecentLevels(fileName);
    }
}

TexamLevel TlevelSelector::getLevelFromFile(QFile &file) {
    TexamLevel level;
    level.name = "";
    if (file.open(QIODevice::ReadOnly)) {
         QDataStream in(&file);
         in.setVersion(QDataStream::Qt_4_7);
         quint32 lv; //level template version
         in >> lv;
         if (lv != levelVersion) {
             QMessageBox::critical(this, "", tr("File: %1 \n is not Nootka level file !!!").arg(file.fileName()));
             return level;
         }
         if (!getLevelFromStream(in, level))
             QMessageBox::warning(0, "", tr("Level file\n %1 \n was corrupted and repaired !!\nCheck please, are its parameters such as expected.").arg(file.fileName()));
    } else
        QMessageBox::critical(this, "", tr("Cannot open file\n %1 \n for reading\n%2 ").arg(file.fileName()).arg(qPrintable(file.errorString())));
    return level;
}

TexamLevel TlevelSelector::getSelectedLevel() {
    if (levelsList->currentRow() == -1 ) {
        TexamLevel l = TexamLevel();
        l.name = ""; l.desc = "";
        return l;
    } else
        return levList[levelsList->currentRow()];
}

void TlevelSelector::updateRecentLevels(QString levelFile) {
    QSettings sett;
    QStringList recentLevels = sett.value("recentLevels").toStringList();
    recentLevels.removeAll(levelFile);
    recentLevels.prepend(levelFile);
    sett.setValue("recentLevels", recentLevels);
}

//#########################  TlevelSummaryWdg ################################################

/*static*/
QString TlevelSummaryWdg::notesRangeTxt = QObject:: tr("notes' range:");
QString TlevelSummaryWdg::fretsRangeTxt = QObject::tr("frets' range:");

TlevelSummaryWdg::TlevelSummaryWdg(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;
    QLabel *headLab = new QLabel(tr("Level summary:"),this);
    mainLay->addWidget(headLab);
    summLab = new QLabel(tr("\n no level selected"), this);
    summLab->setFixedWidth(300);
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

