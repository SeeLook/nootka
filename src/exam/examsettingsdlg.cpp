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


#include "examsettingsdlg.h"
#include "tglobals.h"
#include "tlevelheaderwdg.h"
//#include <QDebug>

extern Tglobals *gl;
bool isNotSaved;

/*static*/
QString examSettingsDlg::examSettTxt = tr("exam's settings");
QString examSettingsDlg::levelFilterTxt = tr("Levels (*.nlv)");

examSettingsDlg::examSettingsDlg(QWidget *parent) :
    TsettingsDialogBase(parent)
{
    isNotSaved = false;
    setWindowTitle(examSettTxt);

    navList->addItem(tr("Levels"));
    navList->item(0)->setIcon(QIcon(gl->path+"picts/levelsSettings.png"));
    navList->item(0)->setTextAlignment(Qt::AlignCenter);
    navList->addItem(tr("Questions"));
    navList->item(1)->setIcon(QIcon(gl->path+"picts/questionsSettings.png"));
    navList->item(1)->setTextAlignment(Qt::AlignCenter);
    navList->addItem(tr("Range"));
    navList->item(2)->setIcon(QIcon(gl->path+"picts/rangeSettings.png"));
    navList->item(2)->setTextAlignment(Qt::AlignCenter);

    levelSett = new levelSettings();
    questSett = new questionsSettings();
    rangeSett = new rangeSettings();

    stackLayout->addWidget(levelSett);
    stackLayout->addWidget(questSett);
    stackLayout->addWidget(rangeSett);

    connect(navList, SIGNAL(currentRowChanged(int)), stackLayout, SLOT(setCurrentIndex(int)));

    navList->setCurrentRow(0);

    connect(levelSett->levelSelector, SIGNAL(levelChanged(TexamLevel)), this, SLOT(levelWasSelected(TexamLevel))); // to load level to widgets
    connect(rangeSett, SIGNAL(rangeChanged()), this, SLOT(levelNotSaved()));
    connect(questSett, SIGNAL(questSettChanged()), this, SLOT(levelNotSaved()));
    connect(levelSett->saveBut, SIGNAL(clicked()), this, SLOT(saveToFile()));
    connect(levelSett->loadBut, SIGNAL(clicked()), this, SLOT(loadFromFile()));

}

void examSettingsDlg::levelWasSelected(TexamLevel level) {
    if (isNotSaved)
        saveLevel();
    questSett->loadLevel(level);
    rangeSett->loadLevel(level);
}

void examSettingsDlg::levelNotSaved() {
    navList->item(0)->setIcon(QIcon(gl->path+"picts/notSaved.png"));
    setWindowTitle(examSettTxt + " (level not saved !!)");
}

void examSettingsDlg::saveLevel() {
    if ( QMessageBox::question(this, "", tr("Exam's level was changed\nand not saved !!"), QMessageBox::Save, QMessageBox::Cancel) == QMessageBox::Save ) {
        saveToFile();
    }
    levelSaved();
}

void examSettingsDlg::saveToFile() {
    TlevelHeaderWdg *saveDlg = new TlevelHeaderWdg;
    QStringList nameList = saveDlg->getLevelName();
    TexamLevel newLevel;
    newLevel.name = nameList[0];
    newLevel.desc = nameList[1];
    questSett->saveLevel(newLevel);
    rangeSett->saveLevel(newLevel);
  // Saving to file
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save exam's level"), QDir::toNativeSeparators(QDir::homePath()+"/"+newLevel.name), levelFilterTxt);
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_4_7);
        out << quint32(0x95121701) << newLevel;
    }
    else
        QMessageBox::critical(this, "", tr("Cannot open file for writing\n%1").arg(qPrintable(file.errorString())));

    levelSett->levelSelector->addLevel(newLevel);
    levelSaved();
}

void examSettingsDlg::levelSaved() {
    isNotSaved = false;
    navList->item(0)->setIcon(QIcon(gl->path+"picts/levelsSettings.png"));
    setWindowTitle(examSettTxt);
}

void examSettingsDlg::loadFromFile() {
    if (isNotSaved)
        saveLevel();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load exam's level"),
                                                    QDir::homePath(), levelFilterTxt);
   QFile file(fileName);
   if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_4_7);
        quint32 lv; //level template version
        in >> lv;
        if (lv != 0x95121701) {
            QMessageBox::critical(this, "", tr("File: %1 \n is not Nootka level file !!!").arg(fileName));
            return;
        }
        TexamLevel level;
        in >> level;
        levelSett->levelSelector->addLevel(level);

   } else
       QMessageBox::critical(this, "", tr("Cannot open file for reading\n%1 ").arg(qPrintable(file.errorString())));
}
