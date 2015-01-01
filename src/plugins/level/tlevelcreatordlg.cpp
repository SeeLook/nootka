/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                             *
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


#include "tlevelcreatordlg.h"
#include "tlevelheaderwdg.h"
#include "questionssettings.h"
#include "accidsettings.h"
#include "levelsettings.h"
#include "rangesettings.h"
#include "tmelodysettings.h"
#include <texamparams.h>
#include <music/ttune.h>
#include <widgets/troundedlabel.h>
#include <level/tlevelselector.h>
#include <tinitcorelib.h>
#include <tscoreparams.h>
#include <QtWidgets>
#include <iostream>


bool isNotSaved;

TlevelCreatorDlg::TlevelCreatorDlg(QWidget *parent) :
    TsettingsDialogBase(parent)
{

    isNotSaved = false;
    setWindowTitle(levelCreatorTxt());
		setWindowIcon(QIcon(Tcore::gl()->path + "picts/levelCreator.png"));

    navList->addItem(TlevelSelector::levelFilterTxt());
    navList->item(0)->setIcon(QIcon(Tcore::gl()->path + "picts/levelsSettings.png"));
    navList->item(0)->setTextAlignment(Qt::AlignCenter);
    navList->addItem(tr("Questions"));
    navList->item(1)->setIcon(QIcon(Tcore::gl()->path + "picts/questionsSettings.png"));
    navList->item(1)->setTextAlignment(Qt::AlignCenter);
    navList->addItem(tr("Accidentals"));
    navList->item(2)->setIcon(QIcon(Tcore::gl()->path + "picts/accidSettings.png"));
    navList->item(2)->setTextAlignment(Qt::AlignCenter);
// 		navList->addItem(tr("Melodies"));
//     navList->item(3)->setIcon(QIcon(Tcore::gl()->path + "picts/melodySett.png"));
//     navList->item(3)->setTextAlignment(Qt::AlignCenter);
    navList->addItem(tr("Range"));
    navList->item(3)->setIcon(QIcon(Tcore::gl()->path + "picts/rangeSettings.png"));
    navList->item(3)->setTextAlignment(Qt::AlignCenter);

    m_levelSett = new levelSettings();
    m_questSett = new questionsSettings(this);
    m_accSett = new accidSettings(this);
// 		m_meloSett = new TmelodySettings(this);
    m_rangeSett = new rangeSettings(this);

    stackLayout->addWidget(m_levelSett);
    stackLayout->addWidget(m_questSett);
    stackLayout->addWidget(m_accSett);
// 		stackLayout->addWidget(m_meloSett);
    stackLayout->addWidget(m_rangeSett);
		
		if (Tcore::gl()->instrument == e_noInstrument)
			m_questSett->hideGuitarRelated();
		
    hint->setFixedHeight(fontMetrics().boundingRect("A").height() * 4);

    connect(navList, SIGNAL(currentRowChanged(int)), stackLayout, SLOT(setCurrentIndex(int)));

    navList->setCurrentRow(0);
		okBut = buttonBox->addButton(tr("Check"), QDialogButtonBox::AcceptRole);
			okBut->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
			okBut->setStatusTip(tr("Check, Are your settings for the level possible to perform."));
		cancelBut = buttonBox->addButton(QDialogButtonBox::Close);
			cancelBut->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
    
    connect(okBut, SIGNAL(clicked()), this, SLOT(checkLevelSlot()));
    connect(m_levelSett->levelSelector(), &TlevelSelector::levelChanged, this, &TlevelCreatorDlg::levelWasSelected); // to load level to widgets
    connect(m_rangeSett, SIGNAL(rangeChanged()), this, SLOT(levelNotSaved()));
    connect(m_questSett, SIGNAL(questSettChanged()), this, SLOT(levelNotSaved()));
    connect(m_accSett, SIGNAL(accidsChanged()), this, SLOT(levelNotSaved()));
    connect(m_levelSett->saveButton(), &QPushButton::clicked, this, &TlevelCreatorDlg::saveToFile);
    connect(m_levelSett->levelSelector(), &TlevelSelector::levelToLoad, this, &TlevelCreatorDlg::loadFromFile);
    connect(m_levelSett->startExamButton(), &QPushButton::clicked, this, &TlevelCreatorDlg::startExam);
		connect(m_levelSett->startExerciseButton(), &QPushButton::clicked, this, &TlevelCreatorDlg::startExam);
    
    connect(m_rangeSett, SIGNAL(allStringsChecked(bool)), m_questSett, SLOT(stringsCheckedSlot(bool)));
		m_questSett->setMelodiesEnabled(!Tcore::gl()->S->isSingleNoteMode);
}


void TlevelCreatorDlg::levelWasChanged() {
	if (sender() != m_questSett)
		m_questSett->changed();
	if (sender() != m_accSett)
		m_accSett->changed();
// 	if (sender() != m_meloSett)
// 		m_meloSett->changed();
	if (sender() != m_rangeSett)
		m_rangeSett->changed();
	levelNotSaved();
}


void TlevelCreatorDlg::levelWasSelected(Tlevel level) {
	if (isNotSaved)
			saveLevel();
	if (!level.name.isEmpty()) {
		m_questSett->loadLevel(&level);
		m_accSett->loadLevel(&level);
	// 		m_meloSett->loadLevel(&level);
		m_rangeSett->loadLevel(&level);
	}
	if (m_levelSett->levelSelector()->isSuitable()) {
			m_levelSett->startExamButton()->setDisabled(false);
			m_levelSett->startExerciseButton()->setDisabled(false);
	} else {
			m_levelSett->startExamButton()->setDisabled(true);
			m_levelSett->startExerciseButton()->setDisabled(true);
	}
}


void TlevelCreatorDlg::levelNotSaved() {
	if (!isNotSaved) {
    navList->item(0)->setIcon(QIcon(Tcore::gl()->path+"picts/notSaved.png"));
    setWindowTitle(levelCreatorTxt() + "  (" + tr("level not saved!") + ")");
		isNotSaved = true;
	}
}


void TlevelCreatorDlg::saveLevel() {
    if ( QMessageBox::question(this, tr("level not saved!"), tr("Level was changed and not saved!"),
			QMessageBox::Save, QMessageBox::Cancel) == QMessageBox::Save ) {
        saveToFile();
    } else 
				levelSaved();
}


void TlevelCreatorDlg::saveToFile() {
    Tlevel newLevel;
    m_questSett->saveLevel(&newLevel);
    m_accSett->saveLevel(&newLevel);
// 		m_meloSett->saveLevel(&newLevel);
    m_rangeSett->saveLevel(&newLevel);
    if (!newLevel.canBeGuitar() && !newLevel.answerIsSound() ) { // no guitar and no played sound  
      // adjust fret range - validation will skip it for non guitar levels
      newLevel.loFret = 0; // Set range to fret number and rest will be done by function preparing question list
      newLevel.hiFret = Tcore::gl()->GfretsNumber;
      newLevel.onlyLowPos = true; // otherwise the above invokes doubled/tripled questions in the list
    // set all strings as available
      for (int str = 0; str < 6; str++)
        newLevel.usedStrings[str] = true;
    }
    QString isLevelValid = validateLevel(newLevel);
    if (isLevelValid != "") {
        showValidationMessage(isLevelValid);
        return;
    }
    // set instrument to none when it is not important for the level
		newLevel.instrument = newLevel.detectInstrument(Tcore::gl()->instrument);
    TlevelHeaderWdg *saveDlg = new TlevelHeaderWdg(this);
    QStringList nameList = saveDlg->getLevelName();
    newLevel.name = nameList[0];
    newLevel.desc = nameList[1];
  // Saving to file
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save exam level"),
                                                    QDir::toNativeSeparators(Tcore::gl()->E->levelsDir + "/" + newLevel.name + ".nel"),
                                              TlevelSelector::levelFilterTxt() + " (*.nel)", 0 , QFileDialog::DontUseNativeDialog);
    if (fileName == "")
        return;
    if (fileName.right(4) != ".nel")
      fileName += ".nel";
    Tcore::gl()->E->levelsDir = QFileInfo(fileName).absoluteDir().absolutePath();
		if (!Tlevel::saveToFile(newLevel, fileName)) {
				QMessageBox::critical(this, " ", tr("Cannot open file for writing"));
				return;
		}
    isNotSaved = false;
    m_levelSett->levelSelector()->addLevel(newLevel, fileName, true);
    m_levelSett->levelSelector()->selectLevel(); // select the last
    levelSaved();
}


void TlevelCreatorDlg::levelSaved() {
    isNotSaved = false;
    navList->item(0)->setIcon(QIcon(Tcore::gl()->path+"picts/levelsSettings.png"));
    setWindowTitle(levelCreatorTxt());
}


void TlevelCreatorDlg::loadFromFile() {
    if (isNotSaved)
        saveLevel();
    m_levelSett->levelSelector()->loadFromFile();
}


QString TlevelCreatorDlg::validateLevel(Tlevel &l) {
    QString res = "";
  // Check has a level sense - are there an questions and answers
    if (!l.canBeScore() && ! l.canBeName() && !l.canBeGuitar() && !l.canBeSound()) {
        res = tr("There aren't any questions or answers selected.<br>Level makes no sense.");
        return res;
    }      
  // checking range
  // determine the highest note of fret range on available strings
    if (l.canBeGuitar() || (l.instrument != e_noInstrument && l.answerIsSound())) {
			// only when guitar is enabled otherwise frets range was adjusted automatically
      int hiAvailStr, loAvailStr, cnt = -1;
      do {
          cnt ++;
      } while (!l.usedStrings[Tcore::gl()->strOrder(cnt)] && cnt < Tcore::gl()->Gtune()->stringNr());
      hiAvailStr = Tcore::gl()->strOrder(cnt);
      cnt = Tcore::gl()->Gtune()->stringNr();
      do {
          cnt--;
      } while (!l.usedStrings[Tcore::gl()->strOrder(cnt)] && cnt >= 0);
      loAvailStr = Tcore::gl()->strOrder(cnt);
      if (l.loNote.chromatic() > Tcore::gl()->Gtune()->str(hiAvailStr + 1).chromatic() + l.hiFret ||
          l.hiNote.chromatic() < Tcore::gl()->Gtune()->str(loAvailStr + 1).chromatic() + l.loFret)
// 			if (l.loNote.chromatic() > Tcore::gl()->Gtune()->str(loAvailStr + 1).chromatic() + l.loFret ||
//           l.hiNote.chromatic() < Tcore::gl()->Gtune()->str(hiAvailStr + 1).chromatic() + l.hiFret)
          res += tr("<li>Range of frets is beyond the scale of this level</li>");
    }
	// Check is level range fit to instrument scale
		if (l.canBeGuitar() || l.answerIsSound()) {
			if (!l.inScaleOf(Tcore::gl()->loString().chromatic(), Tcore::gl()->hiString().chromatic() + Tcore::gl()->GfretsNumber))
				res += "<li>" + TlevelSelector::rangeBeyondScaleTxt() + "</li>";
		}
  // checking are accidentals needed because of hi and low notes in range
    char acc = 0;
    if (l.loNote.alter) acc = l.loNote.alter;
    if (l.hiNote.alter) acc = l.hiNote.alter;
    if (acc) {
        if ( (acc == 1 && !l.withSharps) || (acc == -1 && !l.withFlats))
            res += tr("<li>In range of notes some accidentals are used<br>but not available in this level</li>");
    }
  // Force accidentals enabled but any accidental was selected
    if (l.forceAccids && (!l.withFlats && !l.withSharps && !l.withDblAcc))
			res += tr("<li>Force appropriate accidental is enabled but any accidental was selected.</li>");
	// When no accidentals and no style and question and answers are note name
    if ((l.questionAs.isName() && l.answersAs[TQAtype::e_asName].isName())) {
			if ((l.withFlats && l.withSharps) || l.withDblAcc || l.requireStyle) {
				// do nothing - level is valid
			} else 
				res += tr("<li>Questions and answers as note names will be the same. To avoid that level has to use flats and sharps and/or double accidentals and/or to use different name styles.</li>");
		}
  // Check is possible of using naming style
    if (l.requireStyle && !l.canBeName())
        res += tr("<li>'Use different naming styles' was checked but neither questions nor answers as note name are checked.<br>Check this type of answer/question or uncheck 'Use different naming styles'.</li>");
  // Check are questions and answers as note on the staff have sense (are different)
    if (l.questionAs.isNote() && l.answersAs[TQAtype::e_asNote].isNote())
      if (!l.manualKey && !l.forceAccids)
        res += tr("<li>Questions and answers as notes on the staff will be the same. Manually selecting keys or forcing accidentals has to be selected to avoid that.</li>");
  // Check is possible of manualKey
    if (l.useKeySign && l.manualKey) {
      if (!l.answersAs[TQAtype::e_asNote].isNote() && !l.answersAs[TQAtype::e_asName].isNote() &&
        !l.answersAs[TQAtype::e_asFretPos].isNote() && !l.answersAs[TQAtype::e_asSound].isNote() )
          res += tr("<li>Manual selecting of a key signature was checked but answer as note on the staff was not checked.</li>");
		}
	// 'Fret to fret' has to have suitable fret range to be possible
		if (l.questionAs.isFret() && l.answersAs[TQAtype::e_asFretPos].isFret()) {
			int minRange = 0; // first determine a minimal range for current tune
			int startStr = Tcore::gl()->Gtune()->str(Tcore::gl()->strOrder(0) + 1).chromatic();
			for (int i = 1; i < Tcore::gl()->Gtune()->stringNr(); i++) {
				minRange = qMax(minRange, startStr - Tcore::gl()->Gtune()->str(Tcore::gl()->strOrder(i) + 1).chromatic());
				startStr = Tcore::gl()->Gtune()->str(Tcore::gl()->strOrder(i) + 1).chromatic();
			}
			if (l.hiFret - l.loFret < minRange)
				res += tr("<li>Fret range is not enough to find any note in different positions. At least <b>%1</b> frets range is required.</li>").arg(minRange);
		}
	// Melodies finished on tonic have to contain the tonic note in range
		if (l.canBeMelody() && l.endsOnTonic) {
			bool inKeyNotes[7]; // array with notes used by key signatures from range
			for (int i = 0; i < 7; ++i) inKeyNotes[i] = false;
			for (int i = l.loKey.value(); i <= l.hiKey.value(); ++i) {
				inKeyNotes[TkeySignature::minorKeys[i + 7]] = true;
				inKeyNotes[TkeySignature::majorKeys[i + 7]] = true;
			}
			int startNote = l.loNote.note + (l.loNote.octave + 5) * 7 - 1;
			int endNote = l.hiNote.note + (l.hiNote.octave + 5) * 7 - 1;
			for (int n = 0; n < 7; ++n) {
				if (inKeyNotes[n]) {
					bool found = false;
					for (int i = startNote; i <= endNote; ++i) {
						if (n == i % 7) {
							found = true;
							break;
						}
					}
					if (!found) {
						res += tr("<li>Possible missing a tonic note in a note range for some key signatures.</li>");
						break;
					}
				}
			}
		}
  // Resume warnings
    if (res != "") {
        res.prepend("<ul>");
        res += "</ul></center>";
    }
    return res;

}

void TlevelCreatorDlg::loadLevelFile(QString levelFile) {
  m_levelSett->levelSelector()->loadFromFile(levelFile);
}


void TlevelCreatorDlg::startExam() {
	if (m_levelSett->levelSelector()->idOfSelected() > -1) {
		m_communicate = "exam:";
		if (sender() == m_levelSett->startExerciseButton())
			m_communicate = "exercise:";
		m_communicate += QString::number(m_levelSett->levelSelector()->idOfSelected());
	}
	accept();
}


Tlevel& TlevelCreatorDlg::selectedLevel() {
    return m_levelSett->levelSelector()->getSelectedLevel();
}


void TlevelCreatorDlg::checkLevelSlot() {
    Tlevel tmpLevel;
    m_questSett->saveLevel(&tmpLevel);
    m_accSett->saveLevel(&tmpLevel);
    m_rangeSett->saveLevel(&tmpLevel);
    QString validMessage =  validateLevel(tmpLevel);
    if (validMessage != "")
      showValidationMessage(validMessage);
    else
      QMessageBox::information(this, tr("Level validation"), tr("Level seems to be correct"));
}


void TlevelCreatorDlg::showValidationMessage(QString message) {
      if (message != "") {
				QString title = tr("Level validation");
        if (message.contains("</li>")) { // when <li> exist - warring
          message.prepend(tr("<center><b>It seems the level has some mistakes:</b>"));
          QMessageBox::warning(this, title, message); 
        }
        else { // no questions nor answers
          QMessageBox::critical(this, title, message); 
        }
    }
}
