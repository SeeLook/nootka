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


#include "tstartexamdlg.h"
#include "texamparams.h"
#include "tlevelselector.h"
#include "levelsettings.h"
#include <troundedlabel.h>
#include <thelpdialogbase.h>
#include <QtGui>
#include <stdlib.h> // for getenv()

QString TstartExamDlg::systemUserName() {
#if defined(Q_OS_WIN32)
        return QString::fromLocal8Bit(getenv("USERNAME"));
#else
        return QString::fromLocal8Bit(getenv("USER"));
#endif
}



TstartExamDlg::TstartExamDlg(QString& nick, QString &path, TexamParams *examParams, QWidget *parent) :
    QDialog(parent),
    m_Acction(e_none),
    m_examParams(examParams)
{
    setWindowTitle(tr("Start exercises or an exam"));
    QVBoxLayout *mainLay = new QVBoxLayout;

    QVBoxLayout *levLay = new QVBoxLayout;
    QHBoxLayout *nameLay = new QHBoxLayout;
    QLabel *nameLab = new QLabel(tr("student name:"), this);
    nameLay->addWidget(nameLab);
    m_nameEdit = new QLineEdit(nick, this);
    if (nick == "")
      m_nameEdit->setText(systemUserName());
    m_nameEdit->setMaxLength(30);
    m_nameEdit->setStatusTip(tr("Enter your name or nick-name."));
    nameLay->addWidget(m_nameEdit);
    levLay->addLayout(nameLay);
    m_levelsView = new TlevelSelector(this);
    levLay->addWidget(m_levelsView);
    QLabel *moreLab = new QLabel(levelSettings::moreLevelLinkTxt(), this);
    moreLab->setOpenExternalLinks(true);
		levLay->addWidget(moreLab, 0, Qt::AlignCenter);
    m_createBut = new QPushButton(tr("Create new level"),this);
    m_createBut->setStatusTip(tr("Dialog window for creating new level<br>will be opened."));
    m_createBut->setIcon(QIcon(path + "picts/levelCreator.png"));
    m_createBut->setIconSize(QSize(48, 48));
    m_newExamBut = new QPushButton(tr("Pass new exam"), this);
    m_newExamBut->setStatusTip(m_newExamBut->text() + "<br><b>" + tr("No level was selected!") + "</b>");
    m_newExamBut->setIcon(QIcon(path + "picts/startExam.png"));
    m_newExamBut->setIconSize(QSize(48, 48));
		m_practiceBut = new QPushButton(tr("Start exercises"), this);
    m_practiceBut->setStatusTip(tr("Start practicing on selected level.") + "<br><b>" + tr("No level was selected!") + "</b>");
    m_practiceBut->setIcon(QIcon(path + "picts/practice.png"));
    m_practiceBut->setIconSize(QSize(48, 48));

		QHBoxLayout *buttNewAndLevelLay = new QHBoxLayout;
			buttNewAndLevelLay->addStretch(1);
			buttNewAndLevelLay->addWidget(m_practiceBut);
			buttNewAndLevelLay->addStretch(1);
			buttNewAndLevelLay->addWidget(m_newExamBut);
			buttNewAndLevelLay->addStretch(1);
			buttNewAndLevelLay->addWidget(m_createBut);
			buttNewAndLevelLay->addStretch(1);
		levLay->addLayout(buttNewAndLevelLay);
    levelGr = new QGroupBox(this);
    levelGr->setStatusTip(tr("Select a level suitable for you<br>or create new one."));
    levelGr->setLayout(levLay);

    mainLay->addWidget(levelGr);
		mainLay->addStretch(2);

    QVBoxLayout *exLay = new QVBoxLayout;
    m_examCombo = new QComboBox(this);
    exLay->addWidget(m_examCombo);
    
    QHBoxLayout *butLay = new QHBoxLayout;
		m_helpButt = new QPushButton(this);
			m_helpButt->setIcon(QIcon(path + "picts/help.png"));
			m_helpButt->setStatusTip(tr("Help"));
			m_helpButt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			m_helpButt->setIconSize(QSize(48, 48));
			butLay->addWidget(m_helpButt);
    m_contExamButt = new QPushButton(tr("continue exam"), this);
			m_contExamButt->setIcon(QIcon(path + "picts/startExam.png"));
			m_contExamButt->setIconSize(QSize(48, 48));
			m_contExamButt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
			butLay->addWidget(m_contExamButt);
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

    m_hintLabel = new TroundedLabel(this);
    m_hintLabel->setFixedHeight(70);
    m_hintLabel->setWordWrap(true);

    mainLay->addWidget(m_hintLabel);
    setLayout(mainLay);
    
    setStatusTip("<b>" + tr("Put in your name and select a level to start exercising or to pass new exam. To continue the previous exam, select it from the list or load from file.") + "</b>" );
    m_hintLabel->setStatusTip(statusTip());

#if defined(Q_OS_WIN32) // I hate mess in Win registry
    QSettings sett(QSettings::IniFormat, QSettings::UserScope, "Nootka", "Nootka");
#else
    QSettings sett;
#endif
    m_recentExams = sett.value("recentExams").toStringList();
    for (int i = m_recentExams.size()-1; i >= 0; i--) {
        QFileInfo fi(m_recentExams[i]);
        if (fi.exists()) {
            m_examCombo->insertItem(0, m_recentExams[i]);
        }
        else
            m_recentExams.removeAt(i);
    }
    m_examCombo->addItem(loadExamFileTxt());
    if (m_recentExams.size()) {
        sett.setValue("recentExams", m_recentExams);
        m_examCombo->setCurrentIndex(0);
    }

    m_contExamButt->setStatusTip(m_contExamButt->text() + "<br><b>" + m_examCombo->currentText() + "</b>");
    
    connect(m_levelsView, SIGNAL(levelToLoad()), this, SLOT(levelToLoad()));
    connect(m_cancelBut, SIGNAL(clicked()), this, SLOT(reject()));
    connect(m_createBut, SIGNAL(clicked()), this, SLOT(createLevel()));
    connect(m_newExamBut, SIGNAL(clicked()), this, SLOT(startAccepted()));
    connect(m_contExamButt, SIGNAL(clicked()), this, SLOT(startAccepted()));
    connect(m_examCombo, SIGNAL(activated(int)), this, SLOT(prevExamSelected(int)));
		connect(m_practiceBut, SIGNAL(clicked()), this, SLOT(practiceSelected()));
    connect(m_levelsView, SIGNAL(levelChanged(TexamLevel)), this, SLOT(levelWasSelected(TexamLevel)));
		connect(m_helpButt,  SIGNAL(clicked()), this, SLOT(helpSelected()));
    
    QApplication::translate("File association entries", "Nootka level file", "for file browsers");
    QApplication::translate("File association entries", "Open with Nootka");
    QApplication::translate("File association entries", "Nootka exam file");
    QApplication::translate("Windows installer entries", "Do you really want to remove Nootka and all its components?");
		
		if (m_examParams->showVeryBeginHelp)
				QTimer::singleShot(10, this, SLOT(helpSelected()));
}

TstartExamDlg::Eactions TstartExamDlg::showDialog(QString &txt, TexamLevel &lev) {
    exec();
    if (result() == QDialog::Accepted) {
        if (m_Acction == e_newLevel || m_Acction == e_practice) {
            txt = m_nameEdit->text();
            lev = m_levelsView->getSelectedLevel();
            return m_Acction;
        }
        else {
            if (m_examCombo->currentIndex() != -1) {
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
    m_levelsView->loadFromFile();
}


bool TstartExamDlg::event(QEvent *event) {
    if (event->type() == QEvent::StatusTip) {
        QStatusTipEvent *se = static_cast<QStatusTipEvent *>(event);
        m_hintLabel->setText("<center>"+se->tip()+"</center>");
    }
    return QDialog::event(event);
}


bool TstartExamDlg::isAnyLevelSelected() {
	TexamLevel l = m_levelsView->getSelectedLevel();
	if (l.name == "") { // nothing selected
			QMessageBox::warning(this, "", tr("No level was selected!"));
			return false;
	}
	return true;
}


void TstartExamDlg::startAccepted() {
    if (sender() == m_newExamBut) { // new exam on selected level
        if (!isAnyLevelSelected())
            return;
        else {
            if (m_nameEdit->text() == "") {
                QMessageBox::warning(this, "", tr("Give a user name!"));
                return;
            }
            m_Acction = e_newLevel;
            accept();
        }
    } else { // exam to continue
        if (m_examCombo->currentIndex() < m_examCombo->count() - 1)    {
            m_Acction = e_continue;
            accept();
        } //else
//             noLevelMessage();
    }
}


void TstartExamDlg::loadExam() {
    QString fileName = QFileDialog::getOpenFileName(this, loadExamFileTxt(),
                               m_examParams->examsDir, examFilterTxt(), 0, QFileDialog::DontUseNativeDialog);
    if (fileName != "") {
        m_examParams->examsDir = QFileInfo(fileName).absoluteDir().absolutePath();
        m_examCombo->insertItem(0, fileName);
        m_recentExams.prepend(fileName);
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
    m_contExamButt->setStatusTip(m_contExamButt->text() + "<br><b>" + m_examCombo->currentText() + "</b>");
    if (index == m_examCombo->count() - 1) {
        loadExam();
    } else {
        m_Acction = e_continue;
        accept();
    }
}

void TstartExamDlg::levelWasSelected(TexamLevel level) {
    m_newExamBut->setStatusTip(tr("Pass new exam on level:") + "<br><b>"+ level.name + "</b>");
		m_practiceBut->setStatusTip(tr("Practice on level:") + "<br><b>"+ level.name + "</b>");
}


void TstartExamDlg::practiceSelected() {
		if (isAnyLevelSelected()) {
				m_Acction = e_practice;
				accept();
		}
}


void TstartExamDlg::helpSelected() {
		ThelpDialogBase *help = new ThelpDialogBase(this);
		help->setFixedSize(width(), height() * 0.8);
		QString ht = "<center><h2>" + help->pix("practice", 64) + " " + tr("To exercise or to pass an exam?") + " " + help->pix("startExam", 64) + "</h2>" +
		tr("Learning with Nootka works through questioning and answering but there are two different ways how it happens:<br><b>exercising</b> and <b>passing exams</b>") +
		"</center><div style=\"padding: 10px; text-align: justify;\"><p>" + help->pix("practice") + "&nbsp;&nbsp;&nbsp;" + 
		tr("During exercising the program will be your understanding and friendly teacher. When answering a question you make mistake, Nootka patiently show you a correct answer. The application will be watch your progress silently and suggest you to pass an exam when you will go well. You will can see the details of exercising progress in every time during practice.") + "</p><p>" + help->pix("startExam") + "&nbsp;&nbsp;&nbsp;" +
		tr("During exams Nootka will be your strict master. You will see fast elapsing time and when you make mistake you get additional, penalties questions but any clues. You can stop exam in any time and its results can be stored in file to further continue. If you be lucky to pass an exam you got a certificate.") +
		"</p></div>";
		help->helpText()->setHtml(ht);
		help->showCheckBox(&m_examParams->showVeryBeginHelp);
		help->exec();
		delete help;
}





