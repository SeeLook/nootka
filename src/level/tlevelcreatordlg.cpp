/***************************************************************************
 *   Copyright (C) 2011-2012 by Tomasz Bojczuk                             *
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
#include "tglobals.h"
#include "tlevelheaderwdg.h"
#include "questionssettings.h"
#include "accidsettings.h"
#include "levelsettings.h"
#include "rangesettings.h"
#include <QtGui>

extern Tglobals *gl;
bool isNotSaved;

TlevelCreatorDlg::TlevelCreatorDlg(QWidget *parent) :
    TsettingsDialogBase(parent)
{

    isNotSaved = false;
    setWindowTitle(levelCreatorTxt());

    navList->addItem(TlevelSelector::levelFilterTxt());
    navList->item(0)->setIcon(QIcon(gl->path+"picts/levelsSettings.png"));
    navList->item(0)->setTextAlignment(Qt::AlignCenter);
    navList->addItem(tr("Questions"));
    navList->item(1)->setIcon(QIcon(gl->path+"picts/questionsSettings.png"));
    navList->item(1)->setTextAlignment(Qt::AlignCenter);
    navList->addItem(tr("Accidentals"));
    navList->item(2)->setIcon(QIcon(gl->path+"picts/accidSettings.png"));
    navList->item(2)->setTextAlignment(Qt::AlignCenter);
    navList->addItem(tr("Range"));
    navList->item(3)->setIcon(QIcon(gl->path+"picts/rangeSettings.png"));
    navList->item(3)->setTextAlignment(Qt::AlignCenter);

    levelSett = new levelSettings(gl->path);
    questSett = new questionsSettings();
    accSett = new accidSettings();
    rangeSett = new rangeSettings();

    stackLayout->addWidget(levelSett);
    stackLayout->addWidget(questSett);
    stackLayout->addWidget(accSett);
    stackLayout->addWidget(rangeSett);
    
    hint->setFixedHeight(fontMetrics().boundingRect("A").height() * 4);

    connect(navList, SIGNAL(currentRowChanged(int)), stackLayout, SLOT(setCurrentIndex(int)));

    navList->setCurrentRow(0);
    cancelBut->setText(tr("Close"));
    okBut->setText(tr("Check"));
    okBut->setStatusTip(tr("Check, Are Your settings of the level possible to perform."));
    disconnect(okBut, SIGNAL(clicked()), this, SLOT(accept()));
    connect(okBut, SIGNAL(clicked()), this, SLOT(checkLevelSlot()));

    connect(levelSett->levelSelector, SIGNAL(levelChanged(TexamLevel)),
            this, SLOT(levelWasSelected(TexamLevel))); // to load level to widgets
    connect(rangeSett, SIGNAL(rangeChanged()), this, SLOT(levelNotSaved()));
    connect(questSett, SIGNAL(questSettChanged()), this, SLOT(levelNotSaved()));
    connect(accSett, SIGNAL(accidsChanged()), this, SLOT(levelNotSaved()));
    connect(levelSett->saveBut, SIGNAL(clicked()), this, SLOT(saveToFile()));
    connect(levelSett->levelSelector, SIGNAL(levelToLoad()), this, SLOT(loadFromFile()));
    connect(levelSett->startExamBut, SIGNAL(clicked()), this, SLOT(startExam()));
    
    connect(questSett, SIGNAL(scoreEnabled(bool)), accSett, SLOT(enableKeys(bool)));
    connect(questSett, SIGNAL(accidEnabled(bool)), accSett, SLOT(enableAccids(bool)));
    connect(rangeSett, SIGNAL(allStringsChecked(bool)), questSett, SLOT(stringsCheckedSlot(bool)));
}

void TlevelCreatorDlg::levelWasSelected(TexamLevel level) {
    if (isNotSaved)
        saveLevel();
    questSett->loadLevel(level);
    accSett->loadLevel(level);
    rangeSett->loadLevel(level);
    levelSett->startExamBut->setDisabled(false);
}

void TlevelCreatorDlg::levelNotSaved() {
    navList->item(0)->setIcon(QIcon(gl->path+"picts/notSaved.png"));
    setWindowTitle(levelCreatorTxt() + "  (" + tr("level not saved !!") + ")");
}

void TlevelCreatorDlg::saveLevel() {
    if ( QMessageBox::question(this, "", tr("Exam's level was changed\nand not saved !!"), QMessageBox::Save, QMessageBox::Cancel) == QMessageBox::Save ) {
        saveToFile();
    }
    levelSaved();
}

void TlevelCreatorDlg::saveToFile() {
    TexamLevel newLevel;
    questSett->saveLevel(newLevel);
    accSett->saveLevel(newLevel);
    rangeSett->saveLevel(newLevel);
    if (!newLevel.canBeGuitar()) {  // adjust frets' range - validator will skip it for non guitar level
      newLevel.loFret = 0; // Set range to frets number and rest will do function prepearing questions list
      newLevel.hiFret = gl->GfretsNumber;
    // set all strings as available
      for (int str = 0; str < 6; str++)
        newLevel.usedStrings[str] = true;
    }
    QString isLevelValid = validateLevel(newLevel);
    if (isLevelValid != "") {
        showValidationMessage(isLevelValid);
        return;
    }
    TlevelHeaderWdg *saveDlg = new TlevelHeaderWdg(this);
    QStringList nameList = saveDlg->getLevelName();
    newLevel.name = nameList[0];
    newLevel.desc = nameList[1];
  // Saving to file
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save exam's level"),
                       QDir::toNativeSeparators(QDir::homePath()+"/"+newLevel.name),
                                              TlevelSelector::levelFilterTxt() + "(*.nel)");
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_4_7);
        out << TlevelSelector::levelVersion << newLevel;
        levelSett->levelSelector->updateRecentLevels(fileName);
    }
    else
        QMessageBox::critical(this, "", tr("Cannot open file for writing\n%1")
		.arg(QString::fromLocal8Bit(qPrintable(file.errorString()))));
    isNotSaved = false;

    levelSett->levelSelector->addLevel(newLevel);
    levelSett->levelSelector->selectLevel(); // select the last
    levelSaved();
}

void TlevelCreatorDlg::levelSaved() {
    isNotSaved = false;
    navList->item(0)->setIcon(QIcon(gl->path+"picts/levelsSettings.png"));
    setWindowTitle(levelCreatorTxt());
}

void TlevelCreatorDlg::loadFromFile() {
    if (isNotSaved)
        saveLevel();
    levelSett->levelSelector->loadFromFile();
}

void TlevelCreatorDlg::acceptLevel() {
//    mainLevel = levelSett->levelSelector->getSelectedLevel();
}

QString TlevelCreatorDlg::validateLevel(TexamLevel &l) {
    QString res = "";
  // Check has a level sence - are there an questions and answers
    if (!l.canBeScore() && ! l.canBeName() && !l.canBeGuitar() && !l.canBeSound()) {
        res = "There are not any questions nor answers selected.<br>Level has no sence";
        return res;
    }      
  // checking range
  // determine the highest note of frets' range on available strings
    if (l.canBeGuitar()) { // only when guitar is enabled otherwise frets range was adjusted automatically
      int hiAvailStr, loAvailStr, cnt=-1;
      do {
          cnt ++;
      } while (!l.usedStrings[gl->strOrder(cnt)] && cnt < 6);
      hiAvailStr = gl->strOrder(cnt);
      cnt = 6;
      do {
          cnt--;
      } while (!l.usedStrings[gl->strOrder(cnt)] && cnt >= 0);
      loAvailStr = gl->strOrder(cnt);
      if (l.loNote.getChromaticNrOfNote() > gl->Gtune()[hiAvailStr+1].getChromaticNrOfNote()+l.hiFret ||
          l.hiNote.getChromaticNrOfNote() < gl->Gtune()[loAvailStr+1].getChromaticNrOfNote()+l.loFret)
          res += tr("<li>Range of frets is beyond scale of this level</li>");
    }
  // checking are accids needed because of hi and low notes in range
    char acc = 0;
    if (l.loNote.acidental) acc = l.loNote.acidental;
    if (l.hiNote.acidental) acc = l.hiNote.acidental;
    if (acc)   {
        if ( (acc == 1 && !l.withSharps) || (acc == -1 && !l.withFlats))
            res += tr("<li>In range of notes some accidental is used<br>but not available in this level</li>");
    }
  // Check is posible of useing nameing style
    if (l.requireStyle && !l.canBeName())
        res += tr("<li>Nameing styles was checked but neither question nor answers as note name are checked.<br>Check some or uncheck nemeing styles.</li>");
  // Check are questions and answers as note in the score have sence (are different)
    if (l.questionAs.isNote() && l.answersAs[TQAtype::e_asNote].isNote())
      if (!l.manualKey && !l.forceAccids)
        res += tr("<li>Questions and answers as note in the score will be the same. Manual selecting keys or forceing accidentals has to be selected to avoid that.</li>");
  // Check is posible of manualKey
    if (l.useKeySign && l.manualKey)
      if (!l.answersAs[TQAtype::e_asNote].isNote() || !l.answersAs[TQAtype::e_asName].isNote() ||
        !l.answersAs[TQAtype::e_asFretPos].isNote() || !l.answersAs[TQAtype::e_asSound].isNote() )
          res += tr("<li>Manual selecting of a key signature was checked but any answer as note in the score was not checked.</li>");
  // Resume warrings
    if (res != "") {
        res.prepend("<ul>");
        res += "</ul></center>";
    }
    return res;

}

void TlevelCreatorDlg::loadLevelFile(QString levelFile) {
    levelSett->levelSelector->loadFromFile(levelFile);
}


void TlevelCreatorDlg::startExam() {
    accept();
}


TexamLevel TlevelCreatorDlg::selectedLevel() {
    return levelSett->levelSelector->getSelectedLevel();
}

void TlevelCreatorDlg::checkLevelSlot() {
    TexamLevel tmpLevel;
    questSett->saveLevel(tmpLevel);
    accSett->saveLevel(tmpLevel);
    rangeSett->saveLevel(tmpLevel);
    QString validMessage =  validateLevel(tmpLevel);
    if (validMessage != "")
      showValidationMessage(validMessage);
    else
      QMessageBox::information(this, "", tr("Level seems to be correct"));
}

void TlevelCreatorDlg::showValidationMessage(QString message) {
      if (message != "") {
        if (message.contains("</li>")) { // when <li> exist - warring
          message.prepend(tr("<center><b>It seems the level has got some mistakes:</b>"));
          QMessageBox::warning(this, "", message); 
        }
        else { // no questions nor answers
          QMessageBox::critical(this, "", message); 
        }
    }
}
