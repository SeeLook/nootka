/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
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
#include "tlevelsdefs.h"
#include <widgets/tquestionaswdg.h>
#include <widgets/tlevelpreview.h>
#include <tglobals.h>
#include <texamparams.h>
#include <music/ttune.h>
// #include <widgets/tfixleveldialog.h> TODO
#include <QtWidgets>

extern Tglobals *gl;

// QList<Tlevel> getExampleLevels() {
//     QList<Tlevel> llist;
//     Tlevel l = Tlevel();
// 		int octaveOffset = 0; // depends on guitar type and for bass drops range octave down
// 		if (gl->instrument == e_bassGuitar)
// 			octaveOffset = -1;
// 		else if (gl->instrument == e_noInstrument)
// 			octaveOffset = 1;
// //----------------------------------------------------------------------------
//     l.name = QObject::tr("open strings");
//     l.desc = QObject::tr("The simplest. No key signatures, no double accidentals and no sound.<br>Automatically adjusted to current tune.");
//     l.questionAs.setAsSound(false);
//     l.answersAs[0] = TQAtype(false, true, true, false); // q: score -- a: name, guitar
//     l.answersAs[1] = TQAtype(true, false, true, false); // q: name -- a: score, guitar
// 		l.answersAs[2] = TQAtype(true, true, false, false); // q: guitar -- a: score, name
//     l.answersAs[3] = TQAtype(false, false, false, false);  
//     l.withSharps = false;
//     l.withFlats = false;
//     l.withDblAcc = false;
//     l.useKeySign = false;
//     l.loKey = 0;
//     l.hiKey = 0;
//     l.forceAccids = false;
//     l.requireOctave = false;
//     l.requireStyle = false;
//     l.showStrNr = false;
// 		l.intonation = 0; // do not check
// 		//clef default, user/tune defined
// 		//instrument default - selected by user
//     l.hiNote = Tnote(gl->hiString().getChromaticNrOfNote()); 
// 		//loNote is lowest by constructor
//     l.hiFret = 0;// loFret is 0 by constuctor
//     for (int i = 1; i < 7; i++) { //accids will be used if current tune requires it
//         if (gl->Gtune()->str(i).acidental == 1)
//             l.withSharps = true;
//         if (gl->Gtune()->str(i).acidental == -1)
//             l.withFlats = true;
// 		}
// 		if (l.instrument == e_noInstrument) // force instrument when not defined
// 			l.instrument = e_classicalGuitar;
//     llist << l;
// //----------------------------------------------------------------------------
//     l.name = QObject::tr("C-major scale");
//     l.desc = QObject::tr("In first position. No accidentals, no sound.<br>Octave required.");
//     l.questionAs.setAsSound(false);
// 		bool isGuitar = true;
// 		if (gl->instrument == e_noInstrument) {
// 			isGuitar = false;
// 			l.desc = QObject::tr("Give note name in C-major scale or show note on the staff knowing its name.");
// 		}
// 		l.instrument = gl->instrument;
// 		l.questionAs.setAsFret(isGuitar);
//     l.answersAs[0] = TQAtype(false, true, isGuitar, false);
//     l.answersAs[1] = TQAtype(true, false, isGuitar, false);
// 		l.answersAs[2] = TQAtype(isGuitar, isGuitar, false, false);
//     l.answersAs[3] = TQAtype(false, false, false, false);
//     l.withSharps = false;
//     l.withFlats = false;
//     l.withDblAcc = false;
//     l.useKeySign = false;
//     l.manualKey = false;
//     l.loKey = 0;
//     l.hiKey = 0;
//     l.forceAccids = false;
//     l.requireOctave = true;
//     l.requireStyle = false;
//     l.showStrNr = false;
//     l.loNote = Tnote(1, 0 + octaveOffset, 0);
//     l.hiNote = Tnote(1, 1 + octaveOffset, 0);
//     l.hiFret = 3; // loFret is 0 by constructor
//     l.intonation = 0; // do not check
//     llist << l;
// //----------------------------------------------------------------------------
//     l.name = QObject::tr("All to V fret");
//     l.desc = QObject::tr("All chromatic notes till V-th fret, no sound.<br>Without double accidentals. Key signatures: C, G, D, A");
//     l.questionAs.setAsSound(false);
//     l.answersAs[0] = TQAtype(false, true, true, false);
//     l.answersAs[1] = TQAtype(true, false, true, false);
//     l.answersAs[3] = TQAtype(false, false, false, false);
//     l.withSharps = true;
//     l.withFlats = true;
//     l.withDblAcc = false;
//     l.useKeySign = true;
//     l.manualKey = true;
//     l.loKey = 0;
//     l.hiKey = 3;
//     l.forceAccids = true;
//     l.requireOctave = true;
//     l.requireStyle = false;
//     l.showStrNr = true;
//     l.loNote = gl->loString();
//     l.hiNote = Tnote(gl->hiString().getChromaticNrOfNote() + 5);
//     l.hiFret = 5;// loFret is 0 by constructor
//     l.intonation = 0; // do not check
//     if (gl->instrument == e_noInstrument) // force instrument when not defined
// 			l.instrument = e_classicalGuitar;
//     llist << l;
// //----------------------------------------------------------------------------
//     l = Tlevel();
//     l.name = QObject::tr("Ear training");
//     l.desc = QObject::tr("Listen to a sound and show it on the staff.<br>Guitar, note names and key signatures are not used.<br>Scale a - a<sup>2</sup>.");
//     l.questionAs.setAsFret(false); // no guitar
//     l.questionAs.setAsName(false); // no names
//     l.questionAs.setAsNote(false); // score only as an question
//     l.answersAs[0] = TQAtype(false, false, false, false);
//     l.answersAs[1] = TQAtype(false, false, false, false);
//     l.answersAs[2] = TQAtype(false, false, false, false);
//     l.answersAs[3] = TQAtype(true, false, false, false); // score only
//     l.withSharps = true;
//     l.withFlats = true;
//     l.withDblAcc = false;
//     l.useKeySign = false;
//     l.manualKey = false;
//     l.loKey = 0;
//     l.hiKey = 0;
//     l.forceAccids = false;
//     l.requireOctave = true;
//     l.requireStyle = false;
//     l.showStrNr = false;
// 		l.clef = Tclef(Tclef::e_treble_G);
// 		l.instrument = e_noInstrument;
// 		l.intonation = 0; // do not check
//     l.loNote = Tnote(6, 0); // a
//     l.hiNote = Tnote(6, 2); // a2
//     l.hiFret = 19;// loFret is 0 by constructor
//     llist << l;
// //----------------------------------------------------------------------------
//     l = Tlevel();
//     l.name = QObject::tr("Play scores");
//     l.desc = QObject::tr("Take your instrument and just play...<br>No note names, no fretboard. Keys till 4 accids and double accids enabled! Scale of whole guitar without positions.");
//     l.questionAs.setAsFret(false); // no guitar
//     l.questionAs.setAsName(false); // no names
//     l.questionAs.setAsSound(false); // don't play
//     l.answersAs[0] = TQAtype(false, false, false, true); // score only
//     l.answersAs[1] = TQAtype(false, false, false,false);
//     l.answersAs[2] = TQAtype(false, false, false,false);
//     l.answersAs[3] = TQAtype(false, false, false,false);
//     l.withSharps = true;
//     l.withFlats = true;
//     l.withDblAcc = true;
//     l.useKeySign = true;
//     l.manualKey = false;
//     l.loKey = -4;
//     l.hiKey = 4;
//     l.forceAccids = false;
//     l.requireOctave = true;
//     l.requireStyle = false;
//     l.showStrNr = false;
// 		//clef default, user/tune defined
// 		//instrument default - selected by user
// //     l.loNote & l.hiNote from constructor
// //     l.hiFret by constructor
// // 		l.intonation = gl->A->intonation; // user preferences (in constructor)
//     l.onlyLowPos = true;
// // 		if (gl->instrument == e_noInstrument) // force instrument when not defined
// // 			l.instrument = gl->instrument;
//     llist << l;
// //----------------------------------------------------------------------------
//     l = Tlevel();
//     l.name = QObject::tr("Sing scores");
//     l.desc = QObject::tr("Just sing a score...<br>No note names, no fretboard, no keys and double accids. Scale doesn't matter because octaves are ignored - you can transpose.");
//     l.questionAs.setAsFret(false); // no guitar
//     l.questionAs.setAsName(false); // no names
//     l.questionAs.setAsSound(false); // don't play
//     l.answersAs[0] = TQAtype(false, false, false, true); // score only
//     l.answersAs[1] = TQAtype(false, false, false,false);
//     l.answersAs[2] = TQAtype(false, false, false,false);
//     l.answersAs[3] = TQAtype(false, false, false,false);
//     l.withSharps = true;
//     l.withFlats = true;
//     l.withDblAcc = false;
//     l.useKeySign = false;
//     l.manualKey = false;
// 		l.clef = Tclef(Tclef::e_treble_G);
// 		l.instrument = gl->instrument;
//     l.loKey = 0;
//     l.hiKey = 0;
//     l.forceAccids = false;
//     l.requireOctave = false;
//     l.requireStyle = false;
//     l.showStrNr = false;
//     l.loNote = Tnote(5, 0); // G contra
//     l.hiNote = Tnote(6, 2); // a one-line
// //     l.hiFret by constructor
// // 		l.intonation = gl->A->intonation; // user preferences (in constructor)
// 		l.instrument = e_noInstrument;
//     l.onlyLowPos = true;
//     llist << l;
//     return llist;
// }

// /*static*/

QString TlevelSelector::checkLevel(Tlevel& l) {
	QString warringText = "";
	if (gl->instrument == e_noInstrument && l.instrument != e_noInstrument)
					warringText = tr("Level is not suitable for current instrument type");
	else if (l.canBeGuitar() || (l.instrument != e_noInstrument && l.canBeSound())) {
    if (l.hiFret > gl->GfretsNumber || gl->Gtune()->stringNr() < 3 ||
        l.loNote.getChromaticNrOfNote() < gl->loString().getChromaticNrOfNote() ||
			  l.hiNote.getChromaticNrOfNote() > gl->hiNote().getChromaticNrOfNote())
					warringText = tr("Level is not suitable for current tuning and/or fret number");
  }
	return warringText;
}

/*end static*/


//##########################################################################################################
//########################################## CONSTRUCTOR ###################################################
//##########################################################################################################
TlevelSelector::TlevelSelector(QWidget *parent) :
    QWidget(parent)
{
    QLabel *levLab = new QLabel(levelFilterTxt() + ":",this);
    m_levelsListWdg = new QListWidget(this);
			m_levelsListWdg->setMouseTracking(true);
			m_levelsListWdg->setFixedWidth(200);

    m_loadBut = new QPushButton(tr("Load"), this);
			m_loadBut->setStatusTip(tr("Load level from file"));
			m_loadBut->setIcon(QIcon(gl->path + "picts/levelCreator.png"));
			m_loadBut->setIconSize(QSize(22, 22));
		m_removeButt = new QPushButton(tr("Remove"), this);
			m_removeButt->setStatusTip(TremoveLevel::removeTxt());
			m_removeButt->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
			m_removeButt->setIconSize(QSize(22, 22));
			m_removeButt->setDisabled(true);
    
    m_levelPreview = new TlevelPreview(this);
		m_levelPreview->setFixInstrEnabled(true);

		QHBoxLayout *mainLay = new QHBoxLayout;
    QVBoxLayout *levLay = new QVBoxLayout;
			levLay->addWidget(levLab);
			levLay->addWidget(m_levelsListWdg);
			levLay->addStretch();
			QHBoxLayout *buttLay = new QHBoxLayout;
				buttLay->addWidget(m_loadBut);

				buttLay->addWidget(m_removeButt);
			levLay->addLayout(buttLay);
    mainLay->addLayout(levLay);
			mainLay->addWidget(m_levelPreview);
    setLayout(mainLay);

    findLevels();

    connect(m_levelsListWdg, SIGNAL(currentRowChanged(int)), this, SLOT(levelSelected(int)));
    connect(m_loadBut, SIGNAL(clicked()), this, SLOT(loadFromFilePrivate()));
		connect(m_levelPreview, SIGNAL(instrumentLevelToFix()), this, SLOT(fixInstrumentSlot()));
		connect(m_removeButt, SIGNAL(clicked()), this, SLOT(removeLevelSlot()));
}

TlevelSelector::~TlevelSelector() {
    updateRecentLevels();
}


//##########################################################################################################
//########################################## PUBLIC #######################################################
//##########################################################################################################

void TlevelSelector::levelSelected(int id) {
	if (id >= 0 && id < m_levelsListWdg->count()) {
    m_levelPreview->setLevel(m_levels[id].level);
		if (m_levels[id].file == "")
				m_removeButt->setDisabled(true);
		else
				m_removeButt->setDisabled(false);
    emit levelChanged(m_levels[id].level);
	} else // clear level preview if none
			m_levelPreview->setLevel();
}


void TlevelSelector::findLevels() {
    Tlevel lev = Tlevel();
  // from predefined list
    QList<Tlevel> llist;
		getExampleLevels(llist);
    for (int i = 0; i < llist.size(); i++) {
        addLevel(llist[i]);
        m_levels.last().suitable = isSuitable(llist[i]);
    }
  // from constructor (Master of Masters)
    addLevel(lev);
		m_levels.last().suitable = true;
  // from setting file - recent load/saved levels
    QStringList recentLevels = gl->config->value("recentLevels").toStringList();
    for (int i = recentLevels.size()-1; i >= 0; i--) {
        QFile file(recentLevels[i]);
        if (file.exists()) {
            Tlevel level = getLevelFromFile(file);
            if (level.name != "") {
                addLevel(level, file.fileName());
                m_levels.last().suitable = isSuitable(level);
            } else
                recentLevels.removeAt(i);
        } else
            recentLevels.removeAt(i);
    }
    gl->config->setValue("recentLevels", recentLevels);
}


void TlevelSelector::addLevel(const Tlevel& lev, QString levelFile, bool check) {
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
		l.suitable = true;
    m_levels << l;
}


bool TlevelSelector::isSuitable(Tlevel &l) {
	QString warringText = checkLevel(l);
	if (warringText != "") {
			m_levels.last().item->setStatusTip("<span style=\"color: red;\">" + warringText + "</span>");
			m_levels.last().item->setForeground(QBrush(Qt::red));
			return false;
	}
	return true;
}


void TlevelSelector::disableNotSuitable() {
	for (int i = 0; i < m_levels.size(); i++)
		if (!m_levels[i].suitable) {
			m_levels[i].item->setFlags(Qt::NoItemFlags);
			m_levels[i].item->setForeground(QBrush(palette().color(QPalette::Disabled, QPalette::Text)));
		}
}


bool TlevelSelector::isSuitable() {
	if (idOfSelected() > -1 )
			return m_levels[idOfSelected()].suitable;
	else
			return false;
}


void TlevelSelector::selectLevel(int id) {
    if (id >= 0 && id < m_levelsListWdg->count())
        m_levelsListWdg->setCurrentRow(id);
}


void TlevelSelector::selectLevel() {
    m_levelsListWdg->setCurrentRow(m_levelsListWdg->count()-1);
}


void TlevelSelector::loadFromFile(QString levelFile) {
    if (levelFile == "")
        levelFile = QFileDialog::getOpenFileName(this, tr("Load exam's level"),
                                        gl->E->levelsDir, levelFilterTxt() + " (*.nel)", 0 , QFileDialog::DontUseNativeDialog);
    QFile file(levelFile);
    Tlevel level = getLevelFromFile(file);
    if (level.name != "") {
        gl->E->levelsDir = QFileInfo(levelFile).absoluteDir().absolutePath();
        addLevel(level, levelFile, true);
        if (isSuitable(level))
            selectLevel(); // select the last
    }
}


Tlevel TlevelSelector::getSelectedLevel() {
    if (m_levelsListWdg->currentRow() == -1 ) {
        Tlevel l = Tlevel();
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


//##########################################################################################################
//########################################## PRIVATE #######################################################
//##########################################################################################################


void TlevelSelector::loadFromFilePrivate() {
    emit levelToLoad();
}


Tlevel TlevelSelector::getLevelFromFile(QFile &file) {
    Tlevel level;
    level.name = "";
    if (file.open(QIODevice::ReadOnly)) {
         QDataStream in(&file);
         in.setVersion(QDataStream::Qt_5_2);
         quint32 lv; // level version identifier
         in >> lv;
				 bool wasLevelValid = true;
				 bool wasLevelFile = true;
				 if (Tlevel::levelVersionNr(lv) == 1 || Tlevel::levelVersionNr(lv) == 2) // *.nel with binary data
						wasLevelValid = getLevelFromStream(in, level, lv); // *.nel in XML
				 else if (Tlevel::levelVersionNr(lv) == 3) {
					 QXmlStreamReader xml(in.device());
					 Tlevel::EerrorType er = level.loadFromXml(xml);
					 switch (er) {
						 case Tlevel::e_levelFixed:
								wasLevelValid = false; break;
						 case Tlevel::e_noLevelInXml:
						 case Tlevel::e_otherError:
							 wasLevelFile = false; break;
						 default:
							 break;
					 }
				 } else
							wasLevelFile = false;
				 file.close();
				 if (!wasLevelFile) {
						QMessageBox::critical(this, "", tr("File: %1 \n is not Nootka level file!").arg(file.fileName()));
						level.name = "";
						return level;
				 } else if (!wasLevelValid)
             QMessageBox::warning(0, "", tr("Level file\n %1 \n was corrupted and repaired!\n Check please, if its parameters are as expected.").arg(file.fileName()));
    } else {
				if (file.fileName() != "") // skip empty file names (ignored by user)
					Tlevel::fileIOerrorMsg(file, this);
    }
    return level;
}


void TlevelSelector::fixInstrumentSlot() {
// 	if (fixLevelInstrument(m_levels[m_levelsListWdg->currentRow()].level, m_levels[m_levelsListWdg->currentRow()].file, 
// 						gl->instrumentToFix, this)) {
// 			if (!Tlevel::saveToFile(m_levels[m_levelsListWdg->currentRow()].level, m_levels[m_levelsListWdg->currentRow()].file)) {
// 						qDebug() << "Failed when writing fixed level to:" << m_levels[m_levelsListWdg->currentRow()].file;
// 			} else {
// 				m_levelPreview->setLevel(m_levels[m_levelsListWdg->currentRow()].level);
// 			}
// 	}
}


void TlevelSelector::removeLevelSlot() { 
		QPointer<TremoveLevel> removeDialog = new TremoveLevel(m_levels[idOfSelected()].level.name,
			m_levels[idOfSelected()].file, this);
		if (removeDialog->exec() == QDialog::Accepted) {
			int selId = idOfSelected();
			m_levelsListWdg->setCurrentRow(-1);
			m_levels.removeAt(selId);
			QListWidgetItem *toTrash = m_levelsListWdg->takeItem(selId);
			delete toTrash;
			updateRecentLevels();
		}
}



//%%%%%%%%%%%%%%%%%%%%%%%%%% TremoveLevel %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

TremoveLevel::TremoveLevel(const QString& levelName, const QString& fileName, QWidget* parent) : 
	QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint),
	m_levelFile(fileName)
{
		setWindowTitle(removeTxt().replace("<b>", "").replace("</b>", ""));
		QLabel *removeLab = new QLabel(removeTxt(levelName), this);
		m_deleteChB = new QCheckBox(tr("Also delete level file:"), this);
		QLabel *fNameLab = new QLabel("<b>" + fileName + "</b>", this);
		
		QDialogButtonBox *stdButtons = new QDialogButtonBox(this);
		QPushButton *removeButton = stdButtons->addButton(tr("Remove"), QDialogButtonBox::AcceptRole);
			removeButton->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
		QPushButton *cancelButton =	stdButtons->addButton(QDialogButtonBox::Cancel);
			cancelButton->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
			
		QVBoxLayout *lay = new QVBoxLayout;
			lay->addWidget(removeLab, 0, Qt::AlignCenter);
			lay->addSpacing(20);
			lay->addWidget(m_deleteChB, 0, Qt::AlignCenter);
			lay->addWidget(fNameLab, 0, Qt::AlignCenter);
			lay->addSpacing(20);
			lay->addWidget(stdButtons, 0, Qt::AlignCenter);
		setLayout(lay);
			
		connect(this, SIGNAL(accepted()), this, SLOT(acceptedSlot()));
		connect(stdButtons, SIGNAL(accepted()), this, SLOT(accept()));
		connect(stdButtons, SIGNAL(rejected()), this, SLOT(reject()));
}


void TremoveLevel::acceptedSlot() {
		if (m_deleteChB->isChecked()) {
			QFile levF(m_levelFile);
			if (!levF.remove())
				qDebug() << "Can't remove level file" << m_levelFile ;
		}
}






