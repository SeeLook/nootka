/***************************************************************************
 *   Copyright (C) 2011-2013 by Tomasz Bojczuk                             *
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


#include "tlevelselector.h"
#include "tquestionaswdg.h"
#include "tlevelpreview.h"
#include "tnotename.h"
#include "tglobals.h"
#include <texamparams.h>
#include <ttune.h>
#include <QtGui>

extern Tglobals *gl;

QList<TexamLevel> getExampleLevels() {
    QList<TexamLevel> llist;
    TexamLevel l = TexamLevel();
		int octaveOffset = 0; // depends on guitar type and for bass drops range octave down
		if (gl->instrument == e_bassGuitar)
			octaveOffset = -1;
//----------------------------------------------------------------------------
    l.name = QObject::tr("open strings");
    l.desc = QObject::tr("The simplest. No key signatures, no double accidentals and no sound.<br>Automatically adjusted to current tune.");
    l.questionAs.setAsSound(false);
    l.answersAs[0] = TQAtype(false, true, true, false);
    l.answersAs[1] = TQAtype(true, false, true, false);
    l.answersAs[3] = TQAtype(false, false, false, false);
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
		//clef default, user/tune defined
		//instrument default - selected by user
    l.hiNote = Tnote(gl->hiString().getChromaticNrOfNote()); 
		//loNote is lowest by constructor
//     l.isNoteHi = false;
    l.hiFret = 0;// loFret is 0 by constuctor
//     l.isFretHi = false;
    for (int i = 1; i < 7; i++) { //accids will be used if current tune requires it
        if (gl->Gtune()->str(i).acidental == 1)
            l.withSharps = true;
        if (gl->Gtune()->str(i).acidental == -1)
            l.withFlats = true;
        }
    llist << l;
//----------------------------------------------------------------------------
    l.name = QObject::tr("C-major scale");
    l.desc = QObject::tr("In first position. No accidentals, no sound.<br>Octave required.");
    l.questionAs.setAsSound(false);
    l.answersAs[0] = TQAtype(false, true, true, false);
    l.answersAs[1] = TQAtype(true, false, true, false);
    l.answersAs[3] = TQAtype(false, false, false,false);
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
    l.loNote = Tnote(1, 0 + octaveOffset, 0);
    l.hiNote = Tnote(1, 1 + octaveOffset, 0);
    l.hiFret = 3; // loFret is 0 by constuctor
    llist << l;
//----------------------------------------------------------------------------
    l.name = QObject::tr("All to V fret");
    l.desc = QObject::tr("All chromatic notes till V-th fret, no sound.<br>Without double accidentals. Key signatures: C, G, D, A");
    l.questionAs.setAsSound(false);
    l.answersAs[0] = TQAtype(false, true, true, false);
    l.answersAs[1] = TQAtype(true, false, true, false);
    l.answersAs[3] = TQAtype(false, false, false, false);
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
    l.desc = QObject::tr("Listen a sound and show it in a score.<br>Guitar, note names and key signatures are not used.<br>Scale a - a<sup>2</sup>");
    l.questionAs.setAsFret(false); // no guitar
    l.questionAs.setAsName(false); // no names
    l.questionAs.setAsNote(false); // score only as an question
    l.answersAs[0] = TQAtype(false, false, false, false);
    l.answersAs[1] = TQAtype(false, false, false, false);
    l.answersAs[2] = TQAtype(false, false, false, false);
    l.answersAs[3] = TQAtype(true, false, false, false); // score only
    l.withSharps = true;
    l.withFlats = true;
    l.withDblAcc = false;
    l.useKeySign = false;
    l.manualKey = false;
    l.loKey = 0;
    l.hiKey = 0;
    l.forceAccids = false;
    l.requireOctave = true;
    l.requireStyle = false;
    l.showStrNr = false;
		l.clef = Tclef(Tclef::e_treble_G);
		l.instrument = e_noInstrument;
    l.loNote = Tnote(6, 0);
    l.hiNote = Tnote(6, 2);
    l.hiFret = 19;// loFret is 0 by constuctor
    llist << l;
//----------------------------------------------------------------------------
    l = TexamLevel();
    l.name = QObject::tr("Play scores");
    l.desc = QObject::tr("Take your instrument and just play...<br>No note names, no fretboard. Keys till 4 accids and double accids enabled! Scale of whole guitar without positions.");
    l.questionAs.setAsFret(false); // no guitar
    l.questionAs.setAsName(false); // no names
    l.questionAs.setAsSound(false); // don't play
    l.answersAs[0] = TQAtype(false, false, false, true); // score only
    l.answersAs[1] = TQAtype(false, false, false,false);
    l.answersAs[2] = TQAtype(false, false, false,false);
    l.answersAs[3] = TQAtype(false, false, false,false);
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
		//clef default, user/tune defined
		//instrument default - selected by user
//     l.loNote & l.hiNote from constructor
//     l.hiFret by constuctor
    l.onlyLowPos = true;
    llist << l;
//----------------------------------------------------------------------------
    l = TexamLevel();
    l.name = QObject::tr("Sing scores");
    l.desc = QObject::tr("Just sing a score...<br>No note names, no fretboard, no keys and double accids. Scale doesn't matter because octaves are ignored - you can transpose.");
    l.questionAs.setAsFret(false); // no guitar
    l.questionAs.setAsName(false); // no names
    l.questionAs.setAsSound(false); // don't play
    l.answersAs[0] = TQAtype(false, false, false, true); // score only
    l.answersAs[1] = TQAtype(false, false, false,false);
    l.answersAs[2] = TQAtype(false, false, false,false);
    l.answersAs[3] = TQAtype(false, false, false,false);
    l.withSharps = true;
    l.withFlats = true;
    l.withDblAcc = false;
    l.useKeySign = false;
    l.manualKey = false;
		l.clef = Tclef(Tclef::e_treble_G);
		l.instrument = e_noInstrument;
    l.loKey = 0;
    l.hiKey = 0;
    l.forceAccids = false;
    l.requireOctave = false;
    l.requireStyle = false;
    l.showStrNr = false;
    l.loNote = Tnote(5, 0); // G contra
    l.hiNote = Tnote(6, 2); // a one-line
//     l.hiFret by constuctor
    l.onlyLowPos = true;
    llist << l;
    return llist;
}

/*static*/
const qint32 TlevelSelector::levelVersion = 0x95121701;


void TlevelSelector::fileIOerrorMsg(QFile &f, QWidget *parent) {
	if (f.fileName() != "") {
	  QMessageBox::critical(parent, "", tr("Cannot open file\n %1 \n for reading").arg(f.fileName()));
	} else
	  QMessageBox::critical(parent, "", tr("No file name specified"));
}
/*end static*/


TlevelSelector::TlevelSelector(QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout *mainLay = new QHBoxLayout;

    QVBoxLayout *levLay = new QVBoxLayout;
    QLabel *levLab = new QLabel(levelFilterTxt() + ":",this);
    levLay->addWidget(levLab);
    m_levelsListWdg = new QListWidget(this);
    m_levelsListWdg->setMouseTracking(true);
    m_levelsListWdg->setFixedWidth(200);
    levLay->addWidget(m_levelsListWdg);
    m_loadBut = new QPushButton(tr("Load"), this);
    m_loadBut->setStatusTip(tr("Load exam's level from file"));
    levLay->addStretch(1);
    levLay->addWidget(m_loadBut);

    mainLay->addLayout(levLay);

    m_levelPreview = new TlevelPreview(this);
    mainLay->addWidget(m_levelPreview);

    setLayout(mainLay);

    findLevels();

    connect(m_levelsListWdg, SIGNAL(currentRowChanged(int)), this, SLOT(levelSelected(int)));
    connect(m_loadBut, SIGNAL(clicked()), this, SLOT(loadFromFilePrivate()));
}

TlevelSelector::~TlevelSelector() {
    updateRecentLevels();
}


void TlevelSelector::levelSelected(int id) {
    m_levelPreview->setLevel(m_levels[id].level);
    emit levelChanged(m_levels[id].level);
}


void TlevelSelector::findLevels() {
    TexamLevel lev = TexamLevel();
  // from predefined list
    QList<TexamLevel> llist = getExampleLevels();
    for (int i = 0; i < llist.size(); i++) {
        addLevel(llist[i]);
        isSuitable(llist[i]);
    }
  // from constructor (Master of Masters)
    addLevel(lev);
  // from setting file - recent load/saved levels
    QStringList recentLevels = gl->config->value("recentLevels").toStringList();
    for (int i = recentLevels.size()-1; i >= 0; i--) {
        QFile file(recentLevels[i]);
        if (file.exists()) {
            TexamLevel level = getLevelFromFile(file);
            if (level.name != "") {
                addLevel(level, file.fileName());
                isSuitable(level);
            } else
                recentLevels.removeAt(i);
        } else
            recentLevels.removeAt(i);
    }
    gl->config->setValue("recentLevels", recentLevels);
}


void TlevelSelector::addLevel(const TexamLevel& lev, QString levelFile, bool check) {
    if (check && levelFile != "") {
      int pos = -1;
      for (int i = 0; i < m_levels.size(); i++)
        if (m_levels[i].file == levelFile) // file and level exist
            pos = i;

      QListWidgetItem *it = m_levelsListWdg->takeItem(pos);
      delete it;
      m_levels.removeAt(pos);
    }
    SlevelContener l;
    m_levelsListWdg->addItem(lev.name);
    l.level = lev;
    l.file = levelFile;
    l.item = m_levelsListWdg->item(m_levelsListWdg->count() - 1);
    l.item->setStatusTip(lev.desc);
    m_levels << l;
}

bool TlevelSelector::isSuitable(TexamLevel &l) {
 if (l.questionAs.isFret() || // check only for levels using guitar
  (l.questionAs.isNote() && l.answersAs[TQAtype::e_asNote].isFret()) ||
  (l.questionAs.isName() && l.answersAs[TQAtype::e_asName].isFret()) ||
  (l.questionAs.isSound() && l.answersAs[TQAtype::e_asSound].isFret())
 )
    if (l.hiFret > gl->GfretsNumber ||
        l.loNote.getChromaticNrOfNote() < gl->loString().getChromaticNrOfNote() ||
			  l.hiNote.getChromaticNrOfNote() > gl->hiString().getChromaticNrOfNote() + gl->GfretsNumber ) {
        m_levels.last().item->setStatusTip("<span style=\"color: red;\">" +
                tr("Level is not suitable for current tune and/or frets number") + "</span>");
        m_levels.last().item->setFlags(Qt::NoItemFlags);
        return false;
    } else
        return true;
}


void TlevelSelector::selectLevel(int id) {
    if (id >= 0 && id < m_levelsListWdg->count())
        m_levelsListWdg->setCurrentRow(id);
}


void TlevelSelector::selectLevel() {
    m_levelsListWdg->setCurrentRow(m_levelsListWdg->count()-1);
}


void TlevelSelector::loadFromFilePrivate() {
    emit levelToLoad();
}


void TlevelSelector::loadFromFile(QString levelFile) {
    if (levelFile == "")
        levelFile = QFileDialog::getOpenFileName(this, tr("Load exam's level"),
                                        gl->E->levelsDir, levelFilterTxt() + " (*.nel)", 0 , QFileDialog::DontUseNativeDialog);
    QFile file(levelFile);
    TexamLevel level = getLevelFromFile(file);
    if (level.name != "") {
        gl->E->levelsDir = QFileInfo(levelFile).absoluteDir().absolutePath();
        addLevel(level, levelFile, true);
        if (isSuitable(level))
            selectLevel(); // select the last
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
    } else {
      if (file.fileName() != "") // skip empty file names (ignored by user)
        fileIOerrorMsg(file, this);
    }
    return level;
}


TexamLevel TlevelSelector::getSelectedLevel() {
    if (m_levelsListWdg->currentRow() == -1 ) {
        TexamLevel l = TexamLevel();
        l.name = ""; l.desc = "";
        return l;
    } else
        return m_levels[m_levelsListWdg->currentRow()].level;
}


void TlevelSelector::updateRecentLevels() {
    QStringList recentLevels;
    for (int i = m_levels.size() - 1; i > 1; i--) {
      if (m_levels[i].file != "")
        recentLevels << m_levels[i].file;
    }
    gl->config->setValue("recentLevels", recentLevels);    
}

