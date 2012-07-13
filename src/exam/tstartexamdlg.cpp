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


#include "tstartexamdlg.h"
#include "tlevelselector.h"
#include "levelsettings.h"
#include <QtGui>
#include <stdlib.h> // for getenv()

QString TstartExamDlg::systemUserName() {
#if defined(Q_OS_WIN32)
        return QString::fromLocal8Bit(getenv("USERNAME"));
#else
        return QString::fromLocal8Bit(getenv("USER"));
#endif
}



TstartExamDlg::TstartExamDlg(QString& nick, QString &path, QWidget *parent) :
    QDialog(parent),
    m_Acction(e_none)
{
    setWindowTitle(tr("Start en exam"));
    QVBoxLayout *mainLay = new QVBoxLayout;

    QVBoxLayout *levLay = new QVBoxLayout;
    QHBoxLayout *nameLay = new QHBoxLayout;
    QLabel *nameLab = new QLabel(tr("student's name:"), this);
    nameLay->addWidget(nameLab);
    nameEdit = new QLineEdit(nick, this);
    if (nick == "")
      nameEdit->setText(systemUserName());
    nameEdit->setMaxLength(30);
    nameEdit->setStatusTip(tr("Enter your name or nick."));
    nameLay->addWidget(nameEdit);
    levLay->addLayout(nameLay);
    levelsView = new TlevelSelector(this);
    levLay->addWidget(levelsView);
    QHBoxLayout *lLay = new QHBoxLayout;
    QLabel *moreLab = new QLabel(levelSettings::moreLevelLinkTxt(), this);
    moreLab->setOpenExternalLinks(true);
//    levLay->addWidget(moreLab, 0, Qt::AlignCenter);
    lLay->addWidget(moreLab, 0, Qt::AlignCenter);
    m_createBut = new QPushButton(tr("Create new level"),this);
    m_createBut->setStatusTip(tr("Dialog window for creating new level<br>will be opened."));
    m_createBut->setIcon(QIcon(path + "picts/levelCreator.png"));
    m_createBut->setIconSize(QSize(48, 48));
//    levLay->addWidget(createBut, 1, Qt::AlignCenter);
    lLay->addWidget(m_createBut, 1, Qt::AlignCenter);
    levLay->addLayout(lLay);
    m_newExamBut = new QPushButton(tr("start new exam"), this);
    m_newExamBut->setIcon(QIcon(path + "picts/startExam.png"));
    m_newExamBut->setIconSize(QSize(48, 48));
    levLay->addWidget(m_newExamBut);
    levelGr = new QGroupBox(this);
    levelGr->setStatusTip(tr("Select a level suitable for You<br>or create new one."));
    levelGr->setLayout(levLay);

    mainLay->addWidget(levelGr);

    QVBoxLayout *exLay = new QVBoxLayout;
//     QHBoxLayout *fileLay = new QHBoxLayout;
    m_examCombo = new QComboBox(this);
//     fileLay->addWidget(m_examCombo);
//     m_loadExamBut = new QPushButton(tr("load exam from file"), this);
//     fileLay->addWidget(m_loadExamBut);
//     exLay->addLayout(fileLay);
    exLay->addWidget(m_examCombo);
    
    QHBoxLayout *butLay = new QHBoxLayout;
    m_contExamButt = new QPushButton(tr("continue exam"), this);
    m_contExamButt->setIcon(QIcon(path + "picts/startExam.png"));
    m_contExamButt->setIconSize(QSize(48, 48));
    m_contExamButt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    butLay->addWidget(m_contExamButt);
//     butLay->addStretch(1);
    m_cancelBut = new QPushButton(this);
    m_cancelBut->setIcon(QIcon(style()->standardIcon(QStyle::SP_DialogCloseButton)));
    m_cancelBut->setStatusTip(tr("Discard"));
    m_cancelBut->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_cancelBut->setIconSize(QSize(48, 48));
    butLay->addWidget(m_cancelBut);
    exLay->addLayout(butLay);
    examGr = new QGroupBox(this);
    examGr->setStatusTip(tr("Select previous exam, or get it from a file."));
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
    setLayout(mainLay);
    
    setStatusTip("<b>" + tr("Would You like to start new exam or continue previous one ?<br>To start new one, put your name and select a level.<br>To continue previous, select it from the list or load from file.") + "</b>" );
    hint->setStatusTip(statusTip());

#if defined(Q_OS_WIN32) // I hate mess in Win registry
    QSettings sett(QSettings::IniFormat, QSettings::UserScope, "Nootka", "Nootka");
#else
    QSettings sett;
#endif
    recentExams = sett.value("recentExams").toStringList();
    for (int i = recentExams.size()-1; i >= 0; i--) {
        QFileInfo fi(recentExams[i]);
        if (fi.exists()) {
            m_examCombo->insertItem(0, recentExams[i]);
//            examCombo->insertItem(0, fi.fileName());
        }
        else
            recentExams.removeAt(i);
    }
    m_examCombo->addItem(tr("load exam from file"));
    if (recentExams.size()) {
        sett.setValue("recentExams", recentExams);
        m_examCombo->setCurrentIndex(0);
    }

    connect(levelsView, SIGNAL(levelToLoad()), this, SLOT(levelToLoad()));
    connect(m_cancelBut, SIGNAL(clicked()), this, SLOT(reject()));
//     connect(m_loadExamBut, SIGNAL(clicked()), this, SLOT(loadExam()));
    connect(m_createBut, SIGNAL(clicked()), this, SLOT(createLevel()));
    connect(m_newExamBut, SIGNAL(clicked()), this, SLOT(startAccepted()));
    connect(m_contExamButt, SIGNAL(clicked()), this, SLOT(startAccepted()));
    connect(m_examCombo, SIGNAL(activated(int)), this, SLOT(prevExamSelected(int)));
    
    QApplication::translate("File association entries", "Nootka level file", "for file brrrowsers");
    QApplication::translate("File association entries", "Open with nootka");
    QApplication::translate("File association entries", "Nootka exam file");

}

TstartExamDlg::Eactions TstartExamDlg::showDialog(QString &txt, TexamLevel &lev) {
    exec();
    if (result() == QDialog::Accepted) {
//         if (levelRadio->isChecked()) {
        if (m_Acction == e_newLevel) {
            txt = nameEdit->text();
            lev = levelsView->getSelectedLevel();
            return e_newLevel;
        }
        else {
            if (m_examCombo->currentIndex() != -1) {
//                txt = recentExams[examCombo->currentIndex()];
                txt = m_examCombo->currentText();
                return e_continue;
            } else
              return e_none;
        }
    } else {
		if (m_Acction == e_levelCreator)
			return e_levelCreator;
		else
			return e_none;
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
    if (sender() == m_newExamBut) { // new exam on selsected level
        TexamLevel l = levelsView->getSelectedLevel();
        if (l.name == "") { // nothing selected
            QMessageBox::warning(this, "", tr("Any level was not selected !!"));
            return;
        } else {
            if (nameEdit->text() == "") {
                QMessageBox::warning(this, "", tr("Give any user name !!"));
                return;
            }
            m_Acction = e_newLevel;
            accept();
        }
    } else { // exam to continue
//         if (m_examCombo->currentText() != "")    {
        if (m_examCombo->currentIndex() < m_examCombo->count() - 1)    {
            m_Acction = e_continue;
            accept();
        } else
            QMessageBox::warning(this, "", tr("Any exam wasn't selected !!"));
    }
}

void TstartExamDlg::loadExam() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load an exam's' file"),
                               QDir::homePath(), examFilterTxt());
    if (fileName != "") {
        m_examCombo->insertItem(0, fileName);
        recentExams.prepend(fileName);
        m_examCombo->setCurrentIndex(0);
        m_Acction = e_continue;
        accept();
    }
}


TstartExamDlg::Eactions TstartExamDlg::createLevel() {
    m_Acction = e_levelCreator;
    close();
}

void TstartExamDlg::prevExamSelected(int index) {
    if (index == m_examCombo->count() - 1) {
        loadExam();
    } else {
        m_Acction = e_continue;
        accept();
    }
}
