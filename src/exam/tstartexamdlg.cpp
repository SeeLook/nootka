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
#include "texam.h"
#include "tlevelselector.h"
#include "levelsettings.h"
#include <widgets/troundedlabel.h>
#include <thelpdialogbase.h>
#include <tpixmaker.h>
#include <tmainhelp.h>
#include <texamhelp.h>
#include <QtGui>
#include <stdlib.h> // for getenv()

QString TstartExamDlg::systemUserName() {
#if defined(Q_OS_WIN32)
        return QString::fromLocal8Bit(getenv("USERNAME"));
#else
        return QString::fromLocal8Bit(getenv("USER"));
#endif
}


QString statusTipText;
TstartExamDlg::TstartExamDlg(QString& nick, QString &path, TexamParams *examParams, QWidget *parent) :
    QDialog(parent),
    m_Acction(e_none),
    m_examParams(examParams),
    m_path(path)
{
    setWindowTitle(tr("Start exercises or an exam"));

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
		levelGr = new QGroupBox(this);
			levelGr->setStatusTip(tr("Select a level suitable for you<br>or create new one."));
			levelGr->setLayout(levLay);
			
		m_createLevelButt = new QPushButton(this);
			m_createLevelButt->setStatusTip(tr("Dialog window for creating new level<br>will be opened."));
			m_createLevelButt->setIcon(QIcon(path + "picts/levelCreator.png"));
			setIconSize(m_createLevelButt);
		QLabel *newLevelLab = new QLabel(tr("Create new level").replace(" ", "<br>"), this);
			newLevelLab->setAlignment(Qt::AlignCenter);
			newLevelLab->setStatusTip(m_createLevelButt->statusTip());

		m_newExerciseButt = new QPushButton(tr("start new exercise"));
			m_newExerciseButt->setIcon(QIcon(path + "picts/practice.png"));
			setIconSize(m_newExerciseButt);
		m_contExerciseButt = new QPushButton(tr("continue exercise"));
			m_contExerciseButt->setIcon(QIcon(path + "picts/practice.png"));
			setIconSize(m_contExerciseButt);
		
		m_newExamButt = new QPushButton(tr("pass new exam"), this);
			m_newExamButt->setIcon(QIcon(path + "picts/nootka-exam.png"));
			setIconSize(m_newExamButt);
		m_contExamButt = new QPushButton(tr("continue exam"), this);
			m_contExamButt->setIcon(QIcon(path + "picts/nootka-exam.png"));
			setIconSize(m_contExamButt);
    m_examCombo = new QComboBox(this);
    
		m_helpButt = new QPushButton(this);
			m_helpButt->setIcon(QIcon(path + "picts/help.png"));
			m_helpButt->setStatusTip(tr("Help"));
			m_helpButt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			setIconSize(m_helpButt);
    m_cancelBut = new QPushButton(this);
			m_cancelBut->setIcon(QIcon(style()->standardIcon(QStyle::SP_DialogCloseButton)));
			m_cancelBut->setStatusTip(tr("Discard"));
			m_cancelBut->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			setIconSize(m_cancelBut);

    m_hintLabel = new TroundedLabel(this);
    m_hintLabel->setFixedHeight(70);
    m_hintLabel->setWordWrap(true);
		
		
		QHBoxLayout *upperLay = new QHBoxLayout;
			upperLay->addWidget(levelGr);
			QVBoxLayout *helpLevelLay = new QVBoxLayout;
				helpLevelLay->addWidget(m_helpButt);
				helpLevelLay->addStretch(1);
				helpLevelLay->addWidget(m_createLevelButt);
				helpLevelLay->addWidget(newLevelLab);
				helpLevelLay->addStretch(1);
			upperLay->addLayout(helpLevelLay);
		QVBoxLayout *mainLay = new QVBoxLayout;
			mainLay->addLayout(upperLay);
			QHBoxLayout *exerciseButtLay = new QHBoxLayout;
				exerciseButtLay->addWidget(m_newExerciseButt);
				exerciseButtLay->addWidget(m_contExerciseButt);
			mainLay->addLayout(exerciseButtLay);
		QHBoxLayout *examLay = new QHBoxLayout;
			examLay->addWidget(m_newExamButt);
			QVBoxLayout *contExamLay = new QVBoxLayout;
				contExamLay->addWidget(m_contExamButt);
				contExamLay->addWidget(m_examCombo);
			examLay->addLayout(contExamLay);
		mainLay->addLayout(examLay);
			QHBoxLayout *bottomLay = new QHBoxLayout;
				bottomLay->addWidget(m_hintLabel);
				bottomLay->addWidget(m_cancelBut);
		mainLay->addLayout(bottomLay);
			
    setLayout(mainLay);
    
		statusTipText = tr("To start exercising or to pass new exam put in your name and select a level. To continue the previous exam, select it from the list or load from file." );
    m_hintLabel->setStatusTip("<b>" + statusTipText + "</b>");

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
		
		updateButtonStatusText("");
		QString exerciseFile = QDir::toNativeSeparators(QFileInfo(sett.fileName()).absolutePath() + "/exercise.noo");
		bool continuePosiible = false;
		if (QFileInfo(exerciseFile).exists()) {
				Tlevel l;
				Texam exam(&l, "");
        Texam::EerrorType err = exam.loadFromFile(exerciseFile);
        if (err == Texam::e_file_OK || err == Texam::e_file_corrupted) {
						continuePosiible = true; // TODO prevExerciseLevel makes no sense - level name is taken from real file
						m_examParams->prevExerciseLevel = exam.level()->name;
				} else {
					qDebug() << "exercise file was corrupted... and deleted...";
					QFile::remove(exerciseFile);
				}
		}
		if (continuePosiible)
				m_contExerciseButt->setStatusTip(m_contExerciseButt->text() + "<br>" +
															tr("on level:") + " <b>" + m_examParams->prevExerciseLevel + "</b>");
		else
				m_contExerciseButt->setDisabled(true);
		m_contExamButt->setStatusTip(m_contExamButt->text() + "<br>" + " <b>" + m_examCombo->currentText() + "</b>");
		
    connect(m_levelsView, SIGNAL(levelToLoad()), this, SLOT(levelToLoad()));
    connect(m_cancelBut, SIGNAL(clicked()), this, SLOT(reject()));
    connect(m_createLevelButt, SIGNAL(clicked()), this, SLOT(createLevel()));
    connect(m_newExamButt, SIGNAL(clicked()), this, SLOT(startAccepted()));
    connect(m_contExamButt, SIGNAL(clicked()), this, SLOT(startAccepted()));
		connect(m_newExerciseButt, SIGNAL(clicked()), this, SLOT(startAccepted()));
		connect(m_contExerciseButt, SIGNAL(clicked()), this, SLOT(startAccepted()));
    connect(m_examCombo, SIGNAL(activated(int)), this, SLOT(prevExamSelected(int)));
    connect(m_levelsView, SIGNAL(levelChanged(Tlevel)), this, SLOT(levelWasSelected(Tlevel)));
		connect(m_helpButt,  SIGNAL(clicked()), this, SLOT(helpSelected()));
		
    
    QApplication::translate("File association entries", "Nootka level file", "for file browsers");
    QApplication::translate("File association entries", "Open with Nootka");
    QApplication::translate("File association entries", "Nootka exam file");
    QApplication::translate("Windows installer entries", "Do you really want to remove Nootka and all its components?");
		
		if (m_examParams->showVeryBeginHelp)
				QTimer::singleShot(10, this, SLOT(helpSelected()));
}


TstartExamDlg::Eactions TstartExamDlg::showDialog(QString &txt, Tlevel &lev) {
    exec();
    if (result() == QDialog::Accepted) {
        if (m_Acction == e_newExam || m_Acction == e_newExercise) {
            txt = m_nameEdit->text();
            lev = m_levelsView->getSelectedLevel();
            return m_Acction;
        }
        else if (m_Acction == e_contExam) {
            if (m_examCombo->currentIndex() != -1) {
                txt = m_examCombo->currentText();
                return e_contExam;
            } else
								return e_none;
        } else {
						return e_contExercise;
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
        m_hintLabel->setText("<center>" + se->tip() + "</center>");
    }
    return QDialog::event(event);
}


bool TstartExamDlg::isAnyLevelSelected() {
	Tlevel l = m_levelsView->getSelectedLevel();
	if (l.name == "") { // nothing selected
			QMessageBox::warning(this, "Nootka", tr("No level was selected!"));
			return false;
	}
	return true;
}


void TstartExamDlg::startAccepted() {
    if (sender() == m_newExamButt || sender() == m_newExerciseButt) { // new exam or exercise on selected level
        if (!isAnyLevelSelected())
            return;
        else {
            if (m_nameEdit->text() == "") {
                QMessageBox::warning(this, "", tr("Give a user name!"));
                return;
            }
            if (sender() == m_newExerciseButt)
							m_Acction = e_newExercise;
						else
							m_Acction = e_newExam;
            accept();
        }
    } else { // exam or exercise to continue
				if (sender() == m_contExerciseButt) {
							m_Acction = e_contExercise;
							accept();
				} else
					if (m_examCombo->currentIndex() < m_examCombo->count() - 1)    {
            m_Acction = e_contExam;
            accept();
        }
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
        m_Acction = e_contExam;
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
        m_Acction = e_contExam;
        accept();
    }
}


void TstartExamDlg::updateButtonStatusText(const QString& levelName) {
	QString statusText;
	if (levelName == "")
			statusText += "<br><b>" + tr("No level was selected!") + "</b>";
	else 
			statusText += "<br><b>" + levelName + "</b>";
	m_newExamButt->setStatusTip(tr("Pass new exam on level:") + statusText);
	m_newExerciseButt->setStatusTip(tr("New exercise on level:") + statusText);
}


void TstartExamDlg::levelWasSelected(Tlevel level) {
	updateButtonStatusText(level.name);		
}


void TstartExamDlg::helpSelected() {
		ThelpDialogBase *help = new ThelpDialogBase(this);
		help->setFixedSize(width(), height() * 0.8);
		QString ht = "<center><h2>" + help->pix("practice", 64) + " " + tr("To exercise or to pass an exam?") + " " + 
		help->pix("nootka-exam", 64) + "</h2>" +
		TmainHelp::youWillLearnText() + "<br><br>" +
		"</center><table><tr><td style=\"padding: 10px;\" align=\"center\">" + 
		TmainHelp::duringExercisingText(m_path) + "<br>" + TexamHelp::exerciseFeaturesText() +
		"</td></tr><tr><td style=\"padding: 10px;\" align=\"center\">" + 
		TmainHelp::duringExamsText(m_path) + "<br>" + TexamHelp::examFeaturesText() + "</td></tr></table>";
		
		help->helpText()->setHtml(ht);
		help->showCheckBox(&m_examParams->showVeryBeginHelp);
// 		qDebug() << help->helpText()->toHtml();
		help->exec();
		delete help;
}


void TstartExamDlg::setIconSize(QPushButton* button) {
	button->setIconSize(QSize(48, 48));
}








