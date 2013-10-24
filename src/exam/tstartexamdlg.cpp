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
#include <widgets/troundedlabel.h>
#include <thelpdialogbase.h>
#include <tpixmaker.h>
#include <tfirstrunwizzard.h>
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
    QVBoxLayout *rightLay = new QVBoxLayout;

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
    m_createLevelButt = new QPushButton(tr("Create new level"),this);
			m_createLevelButt->setStatusTip(tr("Dialog window for creating new level<br>will be opened."));
			m_createLevelButt->setIcon(QIcon(path + "picts/levelCreator.png"));
			setIconSize(m_createLevelButt);
    m_newButt = new QPushButton(this);
			m_newButt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
			setIconSize(m_newButt);

		QHBoxLayout *buttNewAndLevelLay = new QHBoxLayout;
			buttNewAndLevelLay->addWidget(m_newButt);
			buttNewAndLevelLay->addWidget(m_createLevelButt);
		levLay->addLayout(buttNewAndLevelLay);
    levelGr = new QGroupBox(this);
    levelGr->setStatusTip(tr("Select a level suitable for you<br>or create new one."));
    levelGr->setLayout(levLay);

    rightLay->addWidget(levelGr);
		rightLay->addStretch(2);

    QVBoxLayout *exLay = new QVBoxLayout;
    m_examCombo = new QComboBox(this);
    exLay->addWidget(m_examCombo);
    
    QHBoxLayout *butLay = new QHBoxLayout;
		m_helpButt = new QPushButton(this);
			m_helpButt->setIcon(QIcon(path + "picts/help.png"));
			m_helpButt->setStatusTip(tr("Help"));
			m_helpButt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			setIconSize(m_helpButt);
    m_continueButt = new QPushButton(this);
			setIconSize(m_continueButt);
			m_continueButt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
			butLay->addWidget(m_continueButt);
    m_cancelBut = new QPushButton(this);
			m_cancelBut->setIcon(QIcon(style()->standardIcon(QStyle::SP_DialogCloseButton)));
			m_cancelBut->setStatusTip(tr("Discard"));
			m_cancelBut->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			setIconSize(m_cancelBut);
			butLay->addWidget(m_cancelBut);
    exLay->addLayout(butLay);
    examGr = new QGroupBox(this);
    examGr->setStatusTip(tr("Select previous exam, or get it from a file."));
    examGr->setLayout(exLay);

    rightLay->addWidget(examGr);

    m_hintLabel = new TroundedLabel(this);
    m_hintLabel->setFixedHeight(70);
    m_hintLabel->setWordWrap(true);

    rightLay->addWidget(m_hintLabel);
		
		m_exerciseButt = prepareButtons("practice");
			m_exerciseButt->setStatusTip(tr("Select this button if you want to exercising."));
		m_examButt = prepareButtons("nootka-exam");
			m_examButt->setStatusTip(tr("Select this button if you want to passing an exam."));
		QLabel *exerciseLab = new QLabel(tr("Exercises"), this);
		QLabel *examLab = new QLabel(tr("Exams"), this);
		
		QVBoxLayout* leftLay = new QVBoxLayout;
			leftLay->addStretch(2);
			leftLay->addWidget(m_exerciseButt, 0, Qt::AlignCenter);
			leftLay->addWidget(exerciseLab, 0, Qt::AlignCenter);
			leftLay->addStretch(1);
			leftLay->addWidget(m_examButt, 0, Qt::AlignCenter);
			leftLay->addWidget(examLab, 0, Qt::AlignCenter);
			leftLay->addStretch(2);
			leftLay->addWidget(m_helpButt, 0, Qt::AlignCenter);
			
		QHBoxLayout *mainLay = new QHBoxLayout;
			mainLay->addLayout(leftLay);
			mainLay->addLayout(rightLay);		
		
    setLayout(mainLay);
    
		setStatusTip(tr("Select button on the left-hand side to determine do you want an exercise or an exam?"));
		statusTipText = tr("To start exercising or to pass new exam put in your name and select a level. To continue the previous exam, select it from the list or load from file. To continue exercise just click 'continue' button." );
//     setStatusTip("<b>" + statusTipText + "</b>" );
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
		
		m_settings = QFileInfo(sett.fileName()).absolutePath() + "/exercise.noo";
		
		m_exerciseButt->setChecked(true); // recent mode
		exerciseOrExamSlot();
    
    connect(m_levelsView, SIGNAL(levelToLoad()), this, SLOT(levelToLoad()));
    connect(m_cancelBut, SIGNAL(clicked()), this, SLOT(reject()));
    connect(m_createLevelButt, SIGNAL(clicked()), this, SLOT(createLevel()));
    connect(m_newButt, SIGNAL(clicked()), this, SLOT(startAccepted()));
    connect(m_continueButt, SIGNAL(clicked()), this, SLOT(startAccepted()));
    connect(m_examCombo, SIGNAL(activated(int)), this, SLOT(prevExamSelected(int)));
    connect(m_levelsView, SIGNAL(levelChanged(TexamLevel)), this, SLOT(levelWasSelected(TexamLevel)));
		connect(m_helpButt,  SIGNAL(clicked()), this, SLOT(helpSelected()));
		connect(m_exerciseButt, SIGNAL(toggled(bool)), this, SLOT(exerciseOrExamSlot()));
		connect(m_examButt, SIGNAL(toggled(bool)), this, SLOT(exerciseOrExamSlot()));
    
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
	TexamLevel l = m_levelsView->getSelectedLevel();
	if (l.name == "") { // nothing selected
			QMessageBox::warning(this, "", tr("No level was selected!"));
			return false;
	}
	return true;
}


void TstartExamDlg::startAccepted() {
    if (sender() == m_newButt) { // new exam or exercise on selected level
        if (!isAnyLevelSelected())
            return;
        else {
            if (m_nameEdit->text() == "") {
                QMessageBox::warning(this, "", tr("Give a user name!"));
                return;
            }
            if (m_exerciseButt->isChecked())
							m_Acction = e_newExercise;
						else
							m_Acction = e_newExam;
            accept();
        }
    } else { // exam or exercise to continue
				if (m_exerciseButt->isChecked()) {
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
    m_continueButt->setStatusTip(m_continueButt->text() + "<br><b>" + m_examCombo->currentText() + "</b>");
    if (index == m_examCombo->count() - 1) {
        loadExam();
    } else {
        m_Acction = e_contExam;
        accept();
    }
}


void TstartExamDlg::levelWasSelected(TexamLevel level) {
	QString statText;
	if (m_exerciseButt->isChecked())
		statText = ("New exercise on level:");
	else
    statText = tr("Pass new exam on level:");
	if (level.name == "")
			statText += "<br><b>" + tr("No level was selected!") + "</b>";
	else 
			statText += "<br><b>"+ level.name + "</b>";
	m_newButt->setStatusTip(statText);
		
}


void TstartExamDlg::exerciseOrExamSlot() {
	levelWasSelected(m_levelsView->getSelectedLevel());
	if (m_exerciseButt->isChecked()) {
		m_newButt->setIcon(QIcon(m_path + "picts/practice.png"));
		m_newButt->setText(tr("new exercise"));
		m_continueButt->setText(tr("continue exercise"));
		m_continueButt->setIcon(QIcon(m_path + "picts/practice.png"));
		m_examCombo->hide();
		if (QFileInfo(m_settings).exists()) {
			m_continueButt->setDisabled(false);
			m_continueButt->setStatusTip(m_continueButt->text() + "<br>" + 
						tr("on level:") + " <b>" + m_examParams->prevExerciseLevel + "</b>");
		} else {
			m_continueButt->setDisabled(true);
			m_continueButt->setStatusTip("");
		}
	} else {
		m_newButt->setIcon(QIcon(m_path + "picts/nootka-exam.png"));
		m_newButt->setText(tr("pass new exam"));
		m_continueButt->setText(tr("continue exam"));
		m_continueButt->setIcon(QIcon(m_path + "picts/nootka-exam.png"));
		m_continueButt->setStatusTip(m_continueButt->text() + "<br><b>" + m_examCombo->currentText() + "</b>");
		m_examCombo->show();
		m_continueButt->setDisabled(false);
	}
}


void TstartExamDlg::helpSelected() {
		ThelpDialogBase *help = new ThelpDialogBase(this);
		help->setFixedSize(width(), height() * 0.8);
		QString ht = "<center><h2>" + help->pix("practice", 64) + " " + tr("To exercise or to pass an exam?") + " " + 
		help->pix("nootka-exam", 64) + "</h2>" +
		TmainHelp::exerciseAndExamText() +
// 		tr("Learning with Nootka works through questioning and answering but there are two different ways how it happens:<br><b>exercising</b> and <b>passing exams</b>") +
// 		"</center><table><tr><td style=\"padding: 10px; text-align: justify;\">" + help->pix("practice") + "&nbsp;&nbsp;&nbsp;" + 
// 		tr("During exercising the program will be your understanding and friendly teacher. When answering a question you make mistake, Nootka patiently show you a correct answer. The application will be watch your progress silently and suggest you to pass an exam when you will go well. You will can see the details of exercising progress in every time during practice.") + 
// 		"</td></tr><tr><td style=\"padding: 10px; text-align: justify;\">" + help->pix("nootka-exam") + "&nbsp;&nbsp;&nbsp;" +
// 		tr("During exams Nootka will be your strict master. You will see fast elapsing time and when you make mistake you get additional, penalties questions but any clues. You can stop exam in any time and its results can be stored in file to further continue. If you be lucky to pass an exam you got a certificate.") +
		"</p></div>";
		help->helpText()->setHtml(ht);
		help->showCheckBox(&m_examParams->showVeryBeginHelp);
		help->exec();
		delete help;
}


void TstartExamDlg::setIconSize(QPushButton* button) {
	button->setIconSize(QSize(48, 48));
}


QPushButton* TstartExamDlg::prepareButtons(const QString& icon) {
	QPushButton *button = new QPushButton(this);
	button->setIcon(QIcon(m_path + "picts/" + icon + ".png"));
	button->setIconSize(QSize(64, 64));
	button->setCheckable(true);
	button->setAutoExclusive(true);
	return button;
}






