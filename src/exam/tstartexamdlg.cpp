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


#include "tstartexamdlg.h"
#include "tlevelselector.h"

/*static*/
const QString TstartExamDlg::examFilterTxt = QObject::tr("Exams' results (*.noo)");


TstartExamDlg::TstartExamDlg(QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;

    levelRadio = new QRadioButton(tr("start new exam"), this);
    mainLay->addWidget(levelRadio);

    QVBoxLayout *levLay = new QVBoxLayout;
    QHBoxLayout *nameLay = new QHBoxLayout;
    QLabel *nameLab = new QLabel(tr("student's name:"), this);
    nameLay->addWidget(nameLab);
    nameEdit = new QLineEdit(this);
    nameEdit->setMaxLength(20);
    nameEdit->setStatusTip(tr("Enter your name or nick."));
    nameLay->addWidget(nameEdit);
    levLay->addLayout(nameLay);
    levelsView = new TlevelSelector(this);
    levLay->addWidget(levelsView);
//    createBut = new QPushButton(tr("create new level"),this);
//    createBut->setStatusTip(tr("Dialog window for creating new level<br>will be opened."));
//    levLay->addWidget(createBut, 1, Qt::AlignCenter);
    levelGr = new QGroupBox(this);
    levelGr->setStatusTip(tr("Select a level suitable for You<br>or create new one."));
    levelGr->setLayout(levLay);

    mainLay->addWidget(levelGr);

    contRadio = new QRadioButton(tr("continue exam"), this);
    mainLay->addWidget(contRadio);

    QVBoxLayout *exLay = new QVBoxLayout;
    examCombo = new QComboBox(this);
    exLay->addWidget(examCombo);
    loadExamBut = new QPushButton(tr("load exam from file"), this);
    exLay->addWidget(loadExamBut, 1, Qt::AlignCenter);
    examGr = new QGroupBox(this);
    examGr->setStatusTip(tr("Select previous exam, or get it from file."));
    examGr->setLayout(exLay);

    mainLay->addWidget(examGr);

    QGroupBox *hGr = new QGroupBox(this);
    QVBoxLayout *hLay = new QVBoxLayout;
    hint = new QLabel(this);
    hLay->addWidget(hint);
    hGr->setLayout(hLay);
    hint->setFixedHeight(70);
    hint->setWordWrap(true);

    mainLay->addWidget(hGr);

    QHBoxLayout *butLay = new QHBoxLayout;
    startBut = new QPushButton(tr("Start"), this);
    startBut->setStatusTip(tr("Start or continue exam."));
    butLay->addWidget(startBut);
    cancelBut = new QPushButton(tr("Discard"), this);
    butLay->addWidget(cancelBut);

    mainLay->addLayout(butLay);
    setLayout(mainLay);

    QButtonGroup *radioGr = new QButtonGroup(this);
    radioGr->addButton(levelRadio);
    radioGr->addButton(contRadio);
    levelRadio->setChecked(true);
    levelOrExamChanged();

#if defined(Q_OS_WIN32) // I hate mess in Win registry
    QSettings sett(QSettings::IniFormat, QSettings::UserScope, "Nootka", "Nootka");
#else
    QSettings sett;
#endif
    QStringList recentExams = sett.value("recentExams").toStringList();
    for (int i = 0; i < recentExams.size(); i++) {
        examCombo->addItem(recentExams[i]);

    }

    connect(radioGr, SIGNAL(buttonClicked(int)), this, SLOT(levelOrExamChanged()));
    connect(levelsView, SIGNAL(levelToLoad()), this, SLOT(levelToLoad()));
    connect(startBut, SIGNAL(clicked()), this, SLOT(startAccepted()));
    connect(cancelBut, SIGNAL(clicked()), this, SLOT(reject()));

}

TstartExamDlg::Eactions TstartExamDlg::showDialog(QString &txt, TexamLevel &lev) {
    exec();
    if (result() == QDialog::Accepted) {
        if (levelRadio->isChecked()) {
            txt = nameEdit->text();
            lev = levelsView->getSelectedLevel();
            return e_newLevel;
        }
        else
            return e_continue;
    } else
        return e_none;
}

void TstartExamDlg::levelOrExamChanged() {
    if (levelRadio->isChecked()) {
        levelGr->setDisabled(false);
        examGr->setDisabled(true);
    } else {
        levelGr->setDisabled(true);
        examGr->setDisabled(false);
    }

}

void TstartExamDlg::levelToLoad() {
    levelsView->loadFromFile();
}

bool TstartExamDlg::event(QEvent *event) {
    if (event->type() == QEvent::StatusTip) {
        QStatusTipEvent *se = static_cast<QStatusTipEvent *>(event);
        hint->setText("<center>"+se->tip()+"</center>");
    }
    return QDialog::event(event);
}

void TstartExamDlg::startAccepted() {
    if (levelRadio->isChecked()) {// new exam on selsected level
        TexamLevel l = levelsView->getSelectedLevel();
        if (l.name == "") { // nothing selected
            QMessageBox::warning(this, "", tr("Any level was not selected !!"));
            return;
        } else {
            if (nameEdit->text() == "") {
                QMessageBox::warning(this, "", tr("Give some user name !!"));
                return;
            }
            accept();
        }
    }
}


