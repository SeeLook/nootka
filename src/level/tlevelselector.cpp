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
#include <QtGui>

extern Tglobals *gl;

QList<TexamLevel> getExampleLevels() {
    QList<TexamLevel> llist;
    TexamLevel l = TexamLevel();
//----------------------------------------------------------------------------
    l.name = QObject::tr("open strings");
    l.desc = QObject::tr("The simplest. No key signatures, no double accidentals and no sound.<br>Automatically adjusted to current tune.");
    l.questionAs.setAsSound(false);
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
    for (int i = 1; i < 7; i++) { //accids will be used if current tune requires it
        if (gl->Gtune()[i].acidental == 1)
            l.withSharps = true;
        if (gl->Gtune()[i].acidental == -1)
            l.withFlats = true;
        }
    llist << l;
//----------------------------------------------------------------------------
    l.name = QObject::tr("C-major scale");
    l.desc = QObject::tr("In first position. No accidentals, no sound.<br>Octave required.");
    l.questionAs.setAsSound(false);
    l.answersAs[0] = TQAtype(false, true, true, false);
    l.answersAs[1] = TQAtype(true, false, true, false);
    l.withSharps = false;
    l.withFlats = false;
    l.withDblAcc = false;
    l.useKeySign = false;
    l.manualKey = false;
    l.loKey = 0;
    l.hiKey = 0;
    l.forceAccids = false;
    l.requireOctave = true;
    l.requireStyle = false;
    l.showStrNr = false;
    l.loNote = Tnote(1,0,0);
    l.hiNote = Tnote(1,1,0);
    l.hiFret = 3;// loFret is 0 by constuctor
    llist << l;
//----------------------------------------------------------------------------
    l.name = QObject::tr("All to V fret");
    l.desc = QObject::tr("All chromatic notes till V-th fret, no sound.<br>Without double accidentals. Key signatures: C, G, D, A");
    l.questionAs.setAsSound(false);
    l.answersAs[0] = TQAtype(false, true, true, false);
    l.answersAs[1] = TQAtype(true, false, true, false);
    l.withSharps = true;
    l.withFlats = true;
    l.withDblAcc = false;
    l.useKeySign = true;
    l.manualKey = true;
    l.loKey = 0;
    l.hiKey = 3;
    l.forceAccids = true;
    l.requireOctave = true;
    l.requireStyle = false;
    l.showStrNr = true;
    l.loNote = gl->loString();
    l.hiNote = Tnote(gl->hiString().getChromaticNrOfNote() + 5);
    l.hiFret = 5;// loFret is 0 by constuctor
    llist << l;
//----------------------------------------------------------------------------
    l = TexamLevel();
    l.name = QObject::tr("Ear training");
    l.desc = QObject::tr("Played sounds are pointed in score.<br>No guitar, no note names, no key signatures.<br>Scale A - a<sup>1</sup>");
    l.questionAs.setAsFret(false); // no guitar
    l.questionAs.setAsName(false); // no names
    l.questionAs.setAsNote(false); // score only as answer
    l.answersAs[3] = TQAtype(true, false, false, false); // score only
    l.withSharps = true;
    l.withFlats = true;
    l.withDblAcc = false;
    l.useKeySign = false;
    l.manualKey = false;
    l.loKey = 0;
    l.hiKey = 0;
    l.forceAccids = true;
    l.requireOctave = true;
    l.requireStyle = false;
    l.showStrNr = false;
    l.loNote = Tnote(6, -1);
    l.hiNote = Tnote(6, 1);
    l.hiFret = 19;// loFret is 0 by constuctor
    llist << l;
//----------------------------------------------------------------------------
    l = TexamLevel();
    l.name = QObject::tr("Play scores");
    l.desc = QObject::tr("Take Your instrument and just play...<br>No note names, no fretboard. Keys till 4 accids and double accids enabled! Scale of whole guitar without positions.");
    l.questionAs.setAsFret(false); // no guitar
    l.questionAs.setAsName(false); // no names
    l.questionAs.setAsSound(false); // don't play
    l.answersAs[0] = TQAtype(false, false, false, true); // score only
    l.withSharps = true;
    l.withFlats = true;
    l.withDblAcc = true;
    l.useKeySign = true;
    l.manualKey = false;
    l.loKey = -4;
    l.hiKey = 4;
    l.forceAccids = false;
    l.requireOctave = true;
    l.requireStyle = false;
    l.showStrNr = false;
//     l.loNote & l.hiNote from constructor
//     l.hiFret by constuctor
    l.onlyLowPos = true;
    llist << l;
//----------------------------------------------------------------------------
    l = TexamLevel();
    l.name = QObject::tr("Sing scores");
    l.desc = QObject::tr("Just sing a score...<br>No note names, no fretboard, no keys and double accids. Scale doesn't matter because octaves are ignored - You can transpose.");
    l.questionAs.setAsFret(false); // no guitar
    l.questionAs.setAsName(false); // no names
    l.questionAs.setAsSound(false); // don't play
    l.answersAs[0] = TQAtype(false, false, false, true); // score only
    l.withSharps = true;
    l.withFlats = true;
    l.withDblAcc = false;
    l.useKeySign = false;
    l.manualKey = false;
    l.loKey = 0;
    l.hiKey = 0;
    l.forceAccids = false;
    l.requireOctave = false;
    l.requireStyle = false;
    l.showStrNr = false;
    l.loNote = Tnote(5, -1); // G contra
    l.hiNote = Tnote(6, 1); // a one-line
//     l.hiFret by constuctor
    l.onlyLowPos = true;
    llist << l;
    return llist;
}

/*static*/
const qint32 TlevelSelector::levelVersion = 0x95121701;

void TlevelSelector::fileIOerrorMsg(QFile &f, QWidget *parent) {
    QMessageBox::critical(parent, "",
                          tr("Cannot open file\n %1 \n for reading\n%2 ").arg(
                                  f.fileName()).arg(QString::fromLocal8Bit(qPrintable(f.errorString()))));
}

/*end static*/

TlevelSelector::TlevelSelector(QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout *mainLay = new QHBoxLayout;

    QVBoxLayout *levLay = new QVBoxLayout;
    QLabel *levLab = new QLabel(levelFilterTxt() + ":",this);
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
  // from predefined list
    QList<TexamLevel> llist = getExampleLevels();
    for (int i = 0; i < llist.size(); i++)
        addLevel(llist[i]);
  // from constructor
    addLevel(lev);
  // from setting file - recent load/saved levels
#if defined(Q_OS_WIN32) // I hate mess in Win registry
    QSettings sett(QSettings::IniFormat, QSettings::UserScope, "Nootka", "Nootka");
#else
    QSettings sett;
#endif
    QStringList recentLevels = sett.value("recentLevels").toStringList();
    for (int i = recentLevels.size()-1; i >= 0; i--) {
        QFile file(recentLevels[i]);
        if (file.exists()) {
            TexamLevel level = getLevelFromFile(file);
            if (level.name != "") {
                addLevel(level);
                isSuitable(level);
            } else
                recentLevels.removeAt(i);
        } else
            recentLevels.removeAt(i);
    }
    sett.setValue("recentLevels", recentLevels);
}

void TlevelSelector::addLevel(const TexamLevel &lev) {
    levelsList->addItem(lev.name);
    levList << lev;
    levelsList->item(levList.size()-1)->setStatusTip(lev.desc);
}

bool TlevelSelector::isSuitable(TexamLevel &l) {
    if (l.hiFret > gl->GfretsNumber ||
        l.loNote.getChromaticNrOfNote() < gl->loString().getChromaticNrOfNote() ) {
        levelsList->item(levList.size()-1)->setStatusTip("<span style=\"color: red;\">" +
                tr("Level is not suitable for current tune and/or frets number") + "</span>");
//         levelsList->item(levList.size()-1)->setFlags(!Qt::ItemIsEnabled);
        levelsList->item(levList.size()-1)->setFlags(Qt::NoItemFlags); //FIXME
        return false;
    } else
        return true;
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

void TlevelSelector::loadFromFile(QString levelFile) {
    if (levelFile == "")
        levelFile = QFileDialog::getOpenFileName(this, tr("Load exam's level"),
                                        QDir::homePath(), levelFilterTxt() + "(*.nel)");
    QFile file(levelFile);
    TexamLevel level = getLevelFromFile(file);
    if (level.name != "") {
        addLevel(level);
        if (isSuitable(level))
            selectLevel(); // select the last
        updateRecentLevels(levelFile);
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
             QMessageBox::critical(this, "",
                         tr("File: %1 \n is not Nootka level file !!!").arg(file.fileName()));
             return level;
         }
         if (!getLevelFromStream(in, level))
             QMessageBox::warning(0, "", tr("Level file\n %1 \n was corrupted and repaired !!\nCheck please, are its parameters as expected.").arg(file.fileName()));
         file.close();
    } else
        fileIOerrorMsg(file, this);
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
#if defined(Q_OS_WIN32) // I hate mess in Win registry
    QSettings sett(QSettings::IniFormat, QSettings::UserScope, "Nootka", "Nootka");
#else
    QSettings sett;
#endif
    QStringList recentLevels = sett.value("recentLevels").toStringList();
    recentLevels.removeAll(levelFile);
    recentLevels.prepend(levelFile);
    sett.setValue("recentLevels", recentLevels);
}

//#########################  TlevelSummaryWdg ################################################

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
    S += "<tr><td>" + notesRangeTxt() + " </td>";
    S += "<td>" + TnoteName::noteToRichText(tl.loNote) + " - "
         + TnoteName::noteToRichText(tl.hiNote) + "</td></tr>";
    if (tl.questionAs.isFret() || tl.answersAs[0].isFret()
        || tl.answersAs[1].isFret() || tl.answersAs[2].isFret()
        || tl.answersAs[3].isFret()) { // level uses guitar
        S += "<tr><td>" + fretsRangeTxt() + " </td>";
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

