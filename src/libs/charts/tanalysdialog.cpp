/***************************************************************************
 *   Copyright (C) 2012-2016 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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


#include "tanalysdialog.h"
#include <exam/texam.h>
#include <exam/tlevel.h>
#include <exam/textrans.h>
#include <level/tlevelpreview.h>
#include <graphics/tnotepixmap.h>
#include <texamparams.h>
#include <tscoreparams.h>
#include <tnoofont.h>
#include <tinitcorelib.h>
#include <tcolor.h>
#include <widgets/tsettingsdialogbase.h>
#include "tchart.h"
#include "tmainchart.h"
#include "tlinearchart.h"
#include "tbarchart.h"
#include "tquestionpoint.h"
#include "ttipchart.h"
#include "tyaxis.h"
#include <tpath.h>
#include <qtr.h>
#include <QtWidgets/QtWidgets>




TanalysDialog::TanalysDialog(Texam* exam, QWidget* parent) :
  QDialog(parent),
  m_chart(0),
  m_exam(0),
  m_level(new Tlevel()),
  m_wasExamCreated(false),
  m_isMaximized(false)
{
#if defined(Q_OS_MAC)
    TquestionPoint::setColors(QColor(Tcore::gl()->EanswerColor.name()), QColor(Tcore::gl()->EquestionColor.name()), 
                              QColor(Tcore::gl()->EnotBadColor.name()), Tcolor::shadow, palette().window().color());
#else
    TquestionPoint::setColors(QColor(Tcore::gl()->EanswerColor.name()), QColor(Tcore::gl()->EquestionColor.name()),
											QColor(Tcore::gl()->EnotBadColor.name()), palette().shadow().color(), palette().base().color());
#endif
    
	setWindowIcon(QIcon(Tcore::gl()->path + "picts/charts.png"));
  setWindowTitle(tr("Analyze"));
  setMinimumSize(720, 480);
	if (parent)
			setGeometry(parent->geometry());
  else {
    Tcore::gl()->config->beginGroup("General");
      setGeometry(Tcore::gl()->config->value("geometry", QRect(50, 50, 750, 480)).toRect());
    Tcore::gl()->config->endGroup();
  }
// default chart presets
  m_chartSetts.inclWrongAnsw = false;
  m_chartSetts.separateWrong = true;
  m_chartSetts.order = Tchart::e_byNumber;
  m_chartSetts.type = Tchart::e_linear;
  m_chartSetts.yValue = TmainLine::e_questionTime;
// Tool bar
  m_toolBar = new QToolBar("", this);
// Y value combo (melodies only)
  m_yValLab = new QLabel(tr("Y value:"), this);
  m_YvaluesCombo = new QComboBox(this);
    m_YvaluesCombo->addItem(TexTrans::reactTimeTxt().toLower());
    m_YvaluesCombo->addItem(TexTrans::effectTxt().toLower());
    m_YvaluesCombo->addItem(TYaxis::attemptsNumberTxt().toLower());
    m_YvaluesCombo->addItem(TYaxis::playedNumberTxt().toLower());
    m_YvaluesCombo->addItem(TYaxis::prepareTimeTxt().toLower());
  QLabel *orderLab = new QLabel(tr("ordered by:", "Keep a proper form - whole sentence will be: ordered by: question number, key signature, etc..."), this);
  QLabel *studentLab = new QLabel(tr("student name:"), this);
  QLabel *levelLab = new QLabel(tr("level:"), this);
  
  m_XorderCombo = new QComboBox(this);
    m_XorderCombo->addItem(tr("question number", "see comment in 'ordered by:' entry"));
    m_XorderCombo->addItem(tr("note pitch"));
    m_XorderCombo->addItem(tr("fret number"));
    m_XorderCombo->addItem(tr("key signature"));
    m_XorderCombo->addItem(tr("accidentals"));
    m_XorderCombo->addItem(tr("question type"));
    m_XorderCombo->addItem(tr("mistake"));
  
  m_userLab = new QLabel(" ", this);
  m_levelLab = new QLabel(" ", this);
  m_moreButton = new QPushButton(QIcon(Tcore::gl()->path + "picts/levelCreator.png"), "...", this);
    m_moreButton->setIconSize(QSize(24, 24));
    m_moreButton->setDisabled(true);
    m_moreButton->setToolTip(tr("Level summary:"));
	m_tuningLab = new QLabel(tr("Tuning"), this);
    m_tuningLab->hide();
	m_tuningButton = new QPushButton(this);
    m_tuningButton->hide();
    m_tuningButton->setFont(TnooFont());
	
  m_questNrLab = new QLabel(" ", this);
  m_effectLab = new QLabel(" ", this);
// Chart widget
  m_plotLay = new QVBoxLayout;
  createChart(m_chartSetts);
// Layout
  QVBoxLayout *lay = new QVBoxLayout;
    lay->addWidget(m_toolBar);
    QGridLayout *headLay = new QGridLayout;
      headLay->addWidget(m_yValLab, 0, 0, Qt::AlignCenter);
      headLay->addWidget(m_YvaluesCombo, 1, 0, Qt::AlignCenter);
      headLay->addWidget(orderLab, 0, 1, Qt::AlignCenter);
      headLay->addWidget(m_XorderCombo, 1, 1, Qt::AlignCenter);
      headLay->addWidget(studentLab, 0, 2, Qt::AlignCenter);
      headLay->addWidget(m_userLab, 1, 2, Qt::AlignCenter);
      headLay->addWidget(levelLab, 0, 3, Qt::AlignCenter);
      QHBoxLayout *levLay = new QHBoxLayout;
        levLay->addWidget(m_levelLab);
        levLay->addWidget(m_moreButton);
      headLay->addLayout(levLay, 1, 3, Qt::AlignCenter);
      headLay->addWidget(m_tuningLab, 0, 4, Qt::AlignCenter);
      headLay->addWidget(m_tuningButton, 1, 4, Qt::AlignCenter);
      headLay->addWidget(m_questNrLab, 0, 5, Qt::AlignCenter);
      headLay->addWidget(m_effectLab, 1, 5, Qt::AlignCenter);
    lay->addLayout(headLay);
    lay->addLayout(m_plotLay);
  setLayout(lay);
  createActions();

  if (exam) {
    m_wasExamCreated = false;
    m_openButton->setDisabled(true); // disable "open exam file" action
    if (exam->isExercise())
      setWindowTitle(analyseExerciseWinTitle());
    else 
       setWindowTitle(analyseExamWinTitle());
    setExam(exam);
  } else { // show help in tip
    QString modKey = "";
#if defined(Q_OS_MAC)
    modKey = "CMD";
#else
    modKey = "CTRL";
#endif
    QString helpTipText = "<br>" + tr("Press %1 button to select an exam from a file.").
            arg("<a href=\"charts\"> " + pixToHtml(Tcore::gl()->path + "picts/nootka-exam.png", 38) + " </a>") + "<br>" +
            tr("Use %1 + mouse wheel or %2 buttons to zoom a chart.").
            arg(modKey).arg(pixToHtml(Tpath::img("zoom-in"), 26) + " " + pixToHtml(Tcore::gl()->path + "picts/zoom-out.png", 26)) + "<br>" +
            tr("Click and Drag the cursor to move the chart.") + "<br>";
					TgraphicsTextTip *helpTip = new TgraphicsTextTip(helpTipText, Tcore::gl()->EanswerColor);
						m_chart->scene->addItem(helpTip);
						helpTip->setFlag(QGraphicsItem::ItemIgnoresTransformations);
						helpTip->setPos(120, 80);
      helpTip->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
      connect(helpTip, &TgraphicsTextTip::linkActivated, this, &TanalysDialog::linkOnTipClicked);
  }
  connect(m_XorderCombo, SIGNAL(activated(int)), this, SLOT(xOrderChanged(int)));
  connect(m_YvaluesCombo, SIGNAL(activated(int)), this, SLOT(yValueChanged(int)));
	connect(m_tuningButton, SIGNAL(clicked()), SLOT(showTuningPreview()));

}


TanalysDialog::~TanalysDialog()
{
	TtipChart::defaultClef = Tcore::gl()->S->clef;
  if (m_wasExamCreated) // to avoid memory leaks
    delete m_exam;
}



//##########  PUBLIC METHODS #####################

void TanalysDialog::setExam(Texam* exam) {
  if (exam == 0)
      return;
  m_exam = exam;
  m_userLab->setText("<b>" + m_exam->userName() + "</b>");
  m_levelLab->setText("<b>" + m_exam->level()->name + "</b>");
  m_questNrLab->setText(tr("Question number") + QString(": <b>%1</b>").arg(exam->count()) );
  m_effectLab->setText(TexTrans::effectTxt() + QString(": <b>%1%</b>").arg(m_exam->effectiveness(), 0, 'f', 1, '0') );
  m_moreButton->setDisabled(false);
	if (exam->level()->instrument != e_noInstrument) {
		bool showTun = false;
		if (exam->level()->instrument == e_bassGuitar)
			showTun = true; // always display tuning preview button for bass
		else if (exam->tune() != Ttune::stdTune)
			showTun = true;
		if (showTun) {
			m_tuningButton->setText(instrumentToGlyph(exam->level()->instrument));
			m_tuningLab->show();
			m_tuningButton->show();
		} else {
			m_tuningLab->hide();
			m_tuningButton->hide();
		}
	}
  connect(m_moreButton, &QPushButton::clicked, this, &TanalysDialog::moreLevelInfo, Qt::UniqueConnection);
  // sort by note
  if ((m_exam->level()->canBeScore() || m_exam->level()->canBeName() || m_exam->level()->canBeSound()) && !m_exam->melodies())
      enableComboItem(m_XorderCombo, 1, true);
    else
      enableComboItem(m_XorderCombo, 1, false);
  // sort by fret  
  if (!m_exam->melodies() && (m_exam->level()->canBeGuitar() ||
      m_exam->level()->answersAs[TQAtype::e_onScore].isSound() || // answers as played sound are also important
      m_exam->level()->answersAs[TQAtype::e_asName].isSound() ||
      m_exam->level()->answersAs[TQAtype::e_asSound].isSound()) )
        enableComboItem(m_XorderCombo, 2, true);
    else
        enableComboItem(m_XorderCombo, 2, false);
  // sort by key signature
  if (m_exam->level()->canBeScore() && m_exam->level()->useKeySign && !m_exam->level()->isSingleKey)
      enableComboItem(m_XorderCombo, 3, true);
  else
      enableComboItem(m_XorderCombo, 3, false);
  // sort by accidentals
  if ((m_exam->level()->canBeScore() || m_exam->level()->canBeName() || m_exam->level()->canBeSound()) && !m_exam->melodies())
    enableComboItem(m_XorderCombo, 4, true);
  else
    enableComboItem(m_XorderCombo, 4, false);
	if (m_exam->melodies()) {
		m_yValLab->setVisible(true);
    m_YvaluesCombo->setVisible(true);
    m_barAct->setVisible(false); // no bar charts for melodies
    m_chartSetts.type = Tchart::e_linear;
    enableComboItem(m_XorderCombo, 5, false);
    enableComboItem(m_XorderCombo, 6, false);
		if (m_exam->level()->answerIsSound())
			enableComboItem(m_YvaluesCombo, 4, true);
		else {
			enableComboItem(m_YvaluesCombo, 4, false);
			if (m_chartSetts.yValue == TmainLine::e_prepareTime)
				m_chartSetts.yValue = TmainLine::e_questionTime; // restore it to default
		}
	} else {
    m_chartSetts.yValue = TmainLine::e_questionTime;
    m_yValLab->setVisible(false);
    m_YvaluesCombo->setVisible(false);
    m_barAct->setVisible(true);
    enableComboItem(m_XorderCombo, 5, true);
    enableComboItem(m_XorderCombo, 6, true);
	}
  TtipChart::defaultClef = m_exam->level()->clef;
	if (m_XorderCombo->model()->index((int)m_chartSetts.order, 0).flags() == 0) {
		m_XorderCombo->setCurrentIndex(0); // set default order when combo item is disabled
		xOrderChanged(0);
		return;
	}
  createChart(m_chartSetts);
}


void TanalysDialog::loadExam(QString& examFile) {
  deleteChart();
  if (m_exam)
    delete m_exam;
  m_exam = new Texam(m_level, "");
  m_wasExamCreated = true; // delete exam in destructor
  if (m_exam->loadFromFile(examFile) == Texam::e_file_OK)
    setExam(m_exam); // only when file was OK
  else {
    delete m_exam;
    m_exam = 0;
    m_userLab->setText("");
    m_levelLab->setText("");
    m_questNrLab->setText("");
    m_effectLab->setText("");
    m_moreButton->setDisabled(true);
    createChart(m_chartSetts);
    TgraphicsTextTip *wrongFileTip = new TgraphicsTextTip("<h3>" +
        tr("File: %1 \n is not valid exam file!").arg(examFile).replace("\n", "<br>") + "</h3>", Tcore::gl()->EquestionColor);
    m_chart->scene->addItem(wrongFileTip);
    wrongFileTip->setPos(100, 180);
  }
}

//##########  PRIVATE METHODS #####################

void TanalysDialog::createActions() {
  QMenu *openMenu = new QMenu("open exam file", this);
  QAction *openAct = new QAction(tr("Open an exam to analyze"), this);
    openAct->setIcon(QIcon(Tcore::gl()->path + "picts/charts.png"));
    openAct->setShortcut(QKeySequence::Open);
  openMenu->addAction(openAct);
    connect(openAct, SIGNAL(triggered()), this, SLOT(loadExamSlot()));
  openMenu->addSeparator();
  QString exerciseFile = QDir::toNativeSeparators(QFileInfo(Tcore::gl()->config->fileName()).absolutePath() + "/exercise.noo");
  if (QFileInfo(exerciseFile).exists()) {
    Tlevel lev;
    Texam ex(&lev, QString());
    ex.loadFromFile(exerciseFile);
    QAction *exerciseAct = new QAction(tr("Recent exercise on level") + ": " + lev.name, this);
    exerciseAct->setIcon(QIcon(Tcore::gl()->path + "picts/practice.png"));
    exerciseAct->setShortcut(QKeySequence("CTRL+e"));
    connect(exerciseAct, SIGNAL(triggered()), this, SLOT(openRecentExercise()));
    openMenu->addAction(exerciseAct);
    openMenu->addSeparator();
  }
  openMenu->addAction(tr("recent opened exams:"));
  QStringList recentExams = Tcore::gl()->config->value("recentExams").toStringList();
  for (int i = 0; i < recentExams.size(); i++) {
    QFileInfo fi(recentExams[i]);
    if (fi.exists()) {
      QAction *act = openMenu->addAction(fi.fileName()); // new QAction(recentExams[i], this);
      act->setData(recentExams[i]);
      connect(act, SIGNAL(triggered()), this, SLOT(openRecentExam()));
    }
  }
  m_openButton = new QToolButton(this);
  m_openButton->setToolTip(openAct->text());
  m_openButton->setIcon(QIcon(Tcore::gl()->path + "picts/nootka-exam.png"));
  m_openButton->setMenu(openMenu);
  m_openButton->setPopupMode(QToolButton::InstantPopup);
  QWidgetAction* openToolButtonAction = new QWidgetAction(this);
  openToolButtonAction->setDefaultWidget(m_openButton);

  m_closeAct = new QAction(QIcon(QIcon(Tpath::img("exit"))), tr("Close analyzer window"), this);
  connect(m_closeAct, SIGNAL(triggered()), this, SLOT(close()));

  m_zoomInAct = new QAction(QIcon(Tpath::img("zoom-in")), qTR("QShortcut", "Zoom In"), this);
  connect(m_zoomInAct, SIGNAL(triggered()), this, SLOT(zoomInSlot()));

  m_zoomOutAct = new QAction(QIcon(Tpath::img("zoom-out")), qTR("QShortcut", "Zoom Out"), this);
  connect(m_zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOutSlot()));
// settings menu button
  m_inclWrongAct = new QAction(tr("include time of wrong answers to average"), this);
  m_inclWrongAct->setCheckable(true);
  m_wrongSeparateAct = new QAction(tr("show wrong answers separately"), this);
  m_wrongSeparateAct->setCheckable(true);
  m_wrongSeparateAct->setChecked(m_chartSetts.separateWrong);
  m_inclWrongAct->setChecked(m_chartSetts.inclWrongAnsw);
  m_wrongSeparateAct->setDisabled(true); // that options makes no sense for default chart
  m_inclWrongAct->setDisabled(true);
  QMenu *menu = new QMenu("chart menu", this);
  menu->addAction(m_wrongSeparateAct);
  menu->addAction(m_inclWrongAct);
  connect(m_wrongSeparateAct, SIGNAL(triggered()), this, SLOT(wrongSeparateSlot()), Qt::UniqueConnection);
  connect(m_inclWrongAct, SIGNAL(triggered()), this, SLOT(includeWrongSlot()), Qt::UniqueConnection);

  m_settButt = new QToolButton(this);
  m_settButt->setIcon(QIcon(Tcore::gl()->path + "picts/exam-settings.png"));
  m_settButt->setToolTip(tr("Settings of a chart"));
  m_settButt->setMenu(menu);
  m_settButt->setPopupMode(QToolButton::InstantPopup);

  QWidgetAction* toolButtonAction = new QWidgetAction(this);
  toolButtonAction->setDefaultWidget(m_settButt);
  
  m_maximizeAct = new QAction(QIcon(Tcore::gl()->path + "picts/fullscreen.png"), tr("Maximize"), this);
  connect(m_maximizeAct, SIGNAL(triggered()), this, SLOT(maximizeWindow()));

  QActionGroup *chartTypeGroup = new QActionGroup(this);
  m_linearAct = new QAction(QIcon(Tcore::gl()->path + "picts/linearChart.png"), tr("linear chart"), this);
  m_linearAct->setCheckable(true);
  chartTypeGroup->addAction(m_linearAct);
  m_barAct = new QAction(QIcon(Tcore::gl()->path + "picts/barChart.png"), tr("bar chart"), this);
  m_barAct->setCheckable(true);
  chartTypeGroup->addAction(m_barAct);
  if (m_chartSetts.type == Tchart::e_linear)
    m_linearAct->setChecked(true);
  else
    m_barAct->setChecked(true);
  connect(chartTypeGroup, SIGNAL(triggered(QAction*)), this, SLOT(chartTypeChanged()));

  QAction *helpAct = new QAction(QIcon(Tcore::gl()->path + "picts/help.png"), TsettingsDialogBase::tr("Open online documentation"), this);
    connect(helpAct, &QAction::triggered, this, &TanalysDialog::openOnlineHelp);
  
  m_toolBar->addAction(openToolButtonAction);
  m_toolBar->addAction(toolButtonAction);
  m_toolBar->addSeparator();
  m_toolBar->addAction(m_zoomOutAct);
  m_toolBar->addAction(m_zoomInAct);
  m_toolBar->addSeparator();
  m_toolBar->addAction(m_linearAct);
  m_toolBar->addAction(m_barAct);
  m_toolBar->addSeparator();
  m_toolBar->addAction(helpAct);
  m_toolBar->addSeparator();
  m_toolBar->addAction(m_maximizeAct);
  m_toolBar->addSeparator();
  m_toolBar->addAction(m_closeAct);
}


void TanalysDialog::createChart(Tchart::Tsettings& chartSett) {
  deleteChart();
  if (m_exam) {
    if (chartSett.type == Tchart::e_linear)
      m_chart = new TlinearChart(m_exam, m_chartSetts, this);
    else
      m_chart = new TbarChart(m_exam, m_chartSetts, this);
  }
  else
    m_chart = new Tchart(this); // empty chart by default
  m_plotLay->addWidget(m_chart);
}


void TanalysDialog::deleteChart() {
  if (m_chart) {
    delete m_chart;
    m_chart = 0;
  }
}


void TanalysDialog::enableComboItem(QComboBox* combo, int index, bool enable) {
  QModelIndex ind = combo->model()->index(index, 0);
  QVariant v;
  if (enable)
      v = QVariant(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  else
      v = QVariant(Qt::NoItemFlags); // disable
  combo->model()->setData(ind, v, Qt::UserRole - 1);
}


//##########  SLOTS #####################

void TanalysDialog::linkOnTipClicked() {
    // loadExamSlot() deletes Tchart and cases crash when is invoked directly by tip signal linkActivated().
    // To avoid this we call it outside by QTimer
    QTimer::singleShot(10, this, SLOT(loadExamSlot()));
}


void TanalysDialog::loadExamSlot() {
  QString fileName = QFileDialog::getOpenFileName(this, TexTrans::loadExamFileTxt(), Tcore::gl()->E->examsDir, TexTrans::examFilterTxt());
  if (fileName != "") {
      Tcore::gl()->E->examsDir = QFileInfo(fileName).absoluteDir().absolutePath();
      loadExam(fileName);
			setWindowTitle(analyseExamWinTitle());
  }
}


void TanalysDialog::openRecentExercise() {
	setWindowTitle(analyseExerciseWinTitle());
	QString exerciseFile = QDir::toNativeSeparators(QFileInfo(Tcore::gl()->config->fileName()).absolutePath() + "/exercise.noo");
	loadExam(exerciseFile);
}



void TanalysDialog::openRecentExam() {
  QAction *action = qobject_cast<QAction*>(sender());
  if (action) {
    QString recentFile = action->data().toString();
    loadExam(recentFile);
    setWindowTitle(analyseExamWinTitle());
  }
}


void TanalysDialog::xOrderChanged(int index) {
  if (!m_exam)
    return;

  m_wrongSeparateAct->setDisabled(index == 0);
  m_inclWrongAct->setDisabled(index == 0);

  switch (index) {
    case 0:
      m_chartSetts.order = Tchart::e_byNumber;
      break;
    case 1:
      m_chartSetts.order = Tchart::e_byNote;
      break;
    case 2:
      m_chartSetts.order = Tchart::e_byFret;
      break;
    case 3:
      m_chartSetts.order = Tchart::e_byKey;
      break;
    case 4:
      m_chartSetts.order = Tchart::e_byAccid;
      break;
    case 5:
      m_chartSetts.order = Tchart::e_byQuestAndAnsw;
      break;
    case 6:
      m_chartSetts.order = Tchart::e_byMistake;
      break;
  }
  if (m_chartSetts.order == Tchart::e_byQuestAndAnsw || m_chartSetts.order == Tchart::e_byMistake) {
      disconnect(m_wrongSeparateAct, SIGNAL(triggered()), this, SLOT(wrongSeparateSlot()));
      m_wrongSeparateAct->setChecked(false); // this sorting types require it!!
      m_wrongSeparateAct->setDisabled(true);
      m_chartSetts.separateWrong = false;
      m_inclWrongAct->setDisabled(false);
  } else {
      m_wrongSeparateAct->setDisabled(false);
      connect(m_wrongSeparateAct, SIGNAL(triggered()), this, SLOT(wrongSeparateSlot()), Qt::UniqueConnection);
  }
  createChart(m_chartSetts);
}


void TanalysDialog::yValueChanged(int index) {
  if (!m_exam)
    return;

  m_wrongSeparateAct->setDisabled(index == 0);
  m_inclWrongAct->setDisabled(index == 0);

  switch (index) {
    case 0:
      m_chartSetts.yValue = TmainLine::e_questionTime;
      break;
    case 1:
      m_chartSetts.yValue = TmainLine::e_effectiveness;
      break;
    case 2:
      m_chartSetts.yValue = TmainLine::e_attemptsCount;
      break;
    case 3:
      m_chartSetts.yValue = TmainLine::e_playedCount;
      break;
    case 4:
      m_chartSetts.yValue = TmainLine::e_prepareTime;
      break;
  }
  createChart(m_chartSetts);
}


void TanalysDialog::zoomInSlot() {
    m_chart->zoom(true);
}


void TanalysDialog::zoomOutSlot() {
    m_chart->zoom(false);
}


void TanalysDialog::maximizeWindow() {
  if (m_isMaximized)
    showNormal();
  else
    showMaximized();
  m_isMaximized = !m_isMaximized;
}


void TanalysDialog::wrongSeparateSlot() {
  m_chartSetts.separateWrong = m_wrongSeparateAct->isChecked();
  if (m_wrongSeparateAct->isChecked()) {
    disconnect(m_inclWrongAct, SIGNAL(triggered()), this, SLOT(includeWrongSlot()));
    m_inclWrongAct->setDisabled(true); // wrong separeted - it has no sense - lock it!
    m_chartSetts.inclWrongAnsw = false; // and reset 
    m_inclWrongAct->setChecked(false);
  }
  else {
    disconnect(m_inclWrongAct, SIGNAL(triggered()), this, SLOT(includeWrongSlot()));
    m_inclWrongAct->setDisabled(false); // unlock
    connect(m_inclWrongAct, SIGNAL(triggered()), this, SLOT(includeWrongSlot()), Qt::UniqueConnection);
  }
  createChart(m_chartSetts);
}


void TanalysDialog::includeWrongSlot() {
  m_chartSetts.inclWrongAnsw = m_inclWrongAct->isChecked();
  createChart(m_chartSetts);
}


void TanalysDialog::moreLevelInfo() {
    QDialog *dialog = new QDialog(this, Qt::CustomizeWindowHint | Qt::Dialog);
    TlevelPreview *levelView = new TlevelPreview(dialog);
    if (m_exam) {
        levelView->setLevel(*m_exam->level());
				levelView->adjustToHeight();
		}
    QVBoxLayout *lay = new QVBoxLayout;
    lay->addWidget(levelView);
    QPushButton *okButt = new QPushButton(tr("OK"), dialog);
    lay->addWidget(okButt, 1, Qt::AlignCenter);
    dialog->setLayout(lay);
    connect(okButt, SIGNAL(clicked()), dialog, SLOT(accept()));
    dialog->exec();
    delete dialog;
}


void TanalysDialog::chartTypeChanged() {
  if (m_linearAct->isChecked()) { // linear chart
    if (m_chartSetts.type != Tchart::e_linear) {
      m_chartSetts.type = Tchart::e_linear;
      enableComboItem(m_XorderCombo, 0, true);
      disconnect(m_wrongSeparateAct, SIGNAL(changed()), this, SLOT(wrongSeparateSlot()));
      m_wrongSeparateAct->setDisabled(false);
      connect(m_wrongSeparateAct, SIGNAL(changed()), this, SLOT(wrongSeparateSlot()));
      createChart(m_chartSetts);
    }        
  } else { // bar chart
      if (m_chartSetts.type != Tchart::e_bar) {
        m_chartSetts.type = Tchart::e_bar;
        m_wrongSeparateAct->setDisabled(false); // unlock settings
        m_inclWrongAct->setDisabled(false);
//         m_settButt->setDisabled(false); // unlock settings
        disconnect(m_wrongSeparateAct, SIGNAL(changed()), this, SLOT(wrongSeparateSlot()));
        m_wrongSeparateAct->setDisabled(true);
        m_wrongSeparateAct->setChecked(false);
        m_chartSetts.separateWrong = false;
        m_inclWrongAct->setDisabled(false);
        connect(m_wrongSeparateAct, SIGNAL(changed()), this, SLOT(wrongSeparateSlot()));
        connect(m_inclWrongAct, SIGNAL(changed()), this, SLOT(includeWrongSlot()));
        if (m_chartSetts.order == Tchart::e_byNumber) { // not suported by barChart (no sense)
          if (m_XorderCombo->model()->index(1, 0).flags() == Qt::NoItemFlags) { // notes don't occur'
              m_chartSetts.order = Tchart::e_byFret;
              m_XorderCombo->setCurrentIndex(2);
          } else {
              m_chartSetts.order = Tchart::e_byNote;
              m_XorderCombo->setCurrentIndex(1);
          }
          enableComboItem(m_XorderCombo, 0, false);
        }
        createChart(m_chartSetts);
      }
    }
}


void TanalysDialog::showTuningPreview() {
	if (m_tunTip) {
    m_tunTip->deleteLater();
		return;
	}
	QString prevText = "<b>" + m_exam->tune().name + "</b><table style=\"text-align: center;\"><tr>";
	for (int i = m_exam->tune().stringNr() - 1; i >= 0; i--)
		prevText += "<td>" + m_exam->tune().str(i + 1).toRichText() + "<br>" + TnooFont::span(QString("%1").arg(i + 1), 20) + "</td>";
	prevText += "</tr></table>";
	QPointF offP = m_chart->mapToScene(0, 0);
	m_tunTip = new TgraphicsTextTip(prevText, palette().highlight().color());
	m_chart->scene->addItem(m_tunTip);
	m_tunTip->setZValue(250);
	m_tunTip->setScale(2.0);
	m_tunTip->setPos(offP.x() + (m_chart->width() - m_tunTip->boundingRect().width() * 2.0) / 2,
									offP.y() + (m_chart->height() - m_tunTip->boundingRect().height() * 2.0) / 2);
  // connect tune tip here to delete it also when clicked or after mouse leave
  connect(m_tunTip, SIGNAL(clicked()), SLOT(showTuningPreview()));
  connect(m_tunTip, SIGNAL(leaved()), SLOT(showTuningPreview()));
}


void TanalysDialog::openOnlineHelp() {
  QDesktopServices::openUrl(QUrl(QString("http://nootka.sourceforge.net/index.php?L=%1&C=doc#analyze").
    arg(QString(qgetenv("LANG")).left(2).toLower()), QUrl::TolerantMode));
}


//#################################################################################################
//###################               EVENTS             ############################################
//#################################################################################################

void TanalysDialog::resizeEvent(QResizeEvent* event) {
    m_toolBar->setIconSize(QSize(height()/21, height()/21));
    m_openButton->setIconSize(m_toolBar->iconSize());
    m_settButt->setIconSize(m_toolBar->iconSize());
		if (m_chart && event->oldSize().height() > 0) {
				double coef = ((double)event->size().height() / (double)event->oldSize().height());
				m_chart->scale(coef, coef);
		}
}


