/***************************************************************************
 *   Copyright (C) 2012-2013 by Tomasz Bojczuk                             *
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


#include <QtGui>
#include "tanalysdialog.h"
#include "texam.h"
#include "texamlevel.h"
#include "tlevelpreview.h"
#include "tstartexamdlg.h"
#include "texamview.h"
#include "tchart.h"
#include "tmainchart.h"
#include "tgraphicstexttip.h"
#include "tlinearchart.h"
#include "tbarchart.h"
#include "tquestionpoint.h"
#include "tpixmaker.h"
#include "tglobals.h"


extern Tglobals *gl;


TanalysDialog::TanalysDialog(Texam* exam, QWidget* parent) :
    QDialog(parent),
    m_exam(0),
    m_level(new TexamLevel()),
    m_chart(0),
    m_wasExamCreated(false),
    m_isMaximized(false)
{
 
  setWindowTitle(tr("Analyse of an exam results"));
  setWindowFlags(Qt::Tool | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
  setGeometry(parent->geometry());
  QVBoxLayout *lay = new QVBoxLayout;
// default chart presets
  m_chartSetts.inclWrongAnsw = false;
  m_chartSetts.separateWrong = true;
  m_chartSetts.order = Tchart::e_byNumber;
  m_chartSetts.type = Tchart::e_linear; // TODO: remember previous type selected by user

  lay = new QVBoxLayout;
  
  m_toolBar = new QToolBar("", this);
  lay->addWidget(m_toolBar);

  QGridLayout *headLay = new QGridLayout;
  headLay->addWidget(new QLabel(tr("ordered by:", "Keep a proper form - whole sentence will be: ordered by: question number, key signature, etc..."), this), 0, 0, Qt::AlignCenter);
  headLay->addWidget(new QLabel(tr("student name:"), this), 0, 1, Qt::AlignCenter);
  headLay->addWidget(new QLabel(tr("level:"), this), 0, 2, Qt::AlignCenter);
  m_chartListCombo = new QComboBox(this);
  m_chartListCombo->addItem(tr("question number", "see coment in 'ordered by:' entry"));
  m_chartListCombo->addItem(tr("note pitch"));
  m_chartListCombo->addItem(tr("fret number"));
  m_chartListCombo->addItem(tr("key signature"));
  m_chartListCombo->addItem(tr("accidentals"));
  m_chartListCombo->addItem(tr("question type"));
  headLay->addWidget(m_chartListCombo, 1, 0, Qt::AlignCenter);
  m_userLab = new QLabel(" ", this);
  headLay->addWidget(m_userLab, 1, 1, Qt::AlignCenter);
  m_levelLab = new QLabel(" ", this);
  m_moreButton = new QPushButton("...", this);
  m_moreButton->setDisabled(true);
  m_moreButton->setToolTip(tr("Level summary:"));
  QHBoxLayout *levLay = new QHBoxLayout;
  levLay->addWidget(m_levelLab);
  levLay->addWidget(m_moreButton);
//  headLay->addWidget(m_levelLab, 1, 2, Qt::AlignCenter);
  headLay->addLayout(levLay, 1, 2, Qt::AlignCenter);
  m_questNrLab = new QLabel(" ", this);
  headLay->addWidget(m_questNrLab, 0, 3, Qt::AlignCenter);
  m_effectLab = new QLabel(" ", this);
  headLay->addWidget(m_effectLab, 1, 3, Qt::AlignCenter);

  lay->addLayout(headLay);

  m_plotLay = new QVBoxLayout;
  lay->addLayout(m_plotLay);
  createChart(m_chartSetts);
  
  setLayout(lay);
  
  createActions();
  
 QTimer::singleShot(100, this, SLOT(testSlot()));
  if (exam) {
    m_wasExamCreated = false;
    m_openButton->setDisabled(true); // disable "open exam file" acction
    setExam(exam);
  } else { // show help in tip
    QString modKey = "";
#if defined(Q_OS_MAC)
    modKey = "CMD";
#else
    modKey = "CTRL";
#endif
    m_chart->setInteractive(true);
    TgraphicsTextTip *helpTip = new TgraphicsTextTip("<br>" + tr("Press %1 button<br> to select an exam from a file<br>Use %2 + mouse wheel to zoom a chart.<br>Drag a cursor to move the chart.").arg("<a href=\"charts\"> " + pixToHtml(gl->path + "picts/nootka-exam.png", 38) + " </a>").arg(modKey)  + "<br>", TquestionPoint::bgColor());
      m_chart->scene->addItem(helpTip);
      helpTip->setFlag(QGraphicsItem::ItemIgnoresTransformations);
//       helpTip->setPos((m_chart->width() - helpTip->boundingRect().width()) / 2, 
//                       (m_chart->height() - helpTip->boundingRect().height()) / 2 );
      helpTip->setPos(200, 80);
      helpTip->setTextInteractionFlags(Qt::TextBrowserInteraction);
      connect(helpTip, SIGNAL(linkActivated(QString)), this, SLOT(loadExamSlot()));
  }
  
  connect(m_chartListCombo, SIGNAL(activated(int)), this, SLOT(analyseChanged(int)));

}

TanalysDialog::~TanalysDialog()
{
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
  m_questNrLab->setText(tr("Questions number:") + QString(" <b>%1</b>").arg(exam->count()) );
  m_effectLab->setText(TexamView::effectTxt() + QString(": <b>%1%</b>")
                       .arg(m_exam->effectiveness(), 0, 'f', 1, '0') );
  m_moreButton->setDisabled(false);
  connect(m_moreButton, SIGNAL(clicked()), this, SLOT(moreLevelInfo()));
  // sort by note
  if (m_exam->level()->canBeScore() || m_exam->level()->canBeName() || m_exam->level()->canBeSound())
        enableComboItem(1, true);
    else
        enableComboItem(1, false);
  // sort by fret  
  if (m_exam->level()->canBeGuitar() ||
      m_exam->level()->answersAs[TQAtype::e_asNote].isSound() || // answers as played sound are also important
      m_exam->level()->answersAs[TQAtype::e_asName].isSound() ||
      m_exam->level()->answersAs[TQAtype::e_asSound].isSound() )
        enableComboItem(2, true);
    else
        enableComboItem(2, false);
  // sort by key signature
  if (m_exam->level()->canBeScore() && m_exam->level()->useKeySign)
      enableComboItem(3, true);
  else
      enableComboItem(3, false);
  // sort by accidentals
  if (m_exam->level()->canBeScore() || m_exam->level()->canBeName() || m_exam->level()->canBeSound())
    enableComboItem(4, true);
  else
    enableComboItem(4, false);
  
  createChart(m_chartSetts);
}


void TanalysDialog::loadExam(QString& examFile) {
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
    }

}

//##########  PRIVATE METHODS #####################

void TanalysDialog::createActions() {
    QMenu *openMenu = new QMenu("open exam file", this);
    QAction *openAct = new QAction(tr("Open an exam to analyse"), this);
    openAct->setIcon(QIcon(gl->path + "picts/charts.png"));
    openMenu->addAction(openAct);
    connect(openAct, SIGNAL(triggered()), this, SLOT(loadExamSlot()));
    openMenu->addSeparator();
    openMenu->addAction(tr("recent opened exams:"));
    QStringList recentExams = gl->config->value("recentExams").toStringList();
    for (int i = 0; i < recentExams.size(); i++) {
        QFileInfo fi(recentExams[i]);
        if (fi.exists()) {
            QAction *act = new QAction(recentExams[i], this);
            openMenu->addAction(act);
            connect(act, SIGNAL(triggered()), this, SLOT(openRecentExam()));
        }
    }
    m_openButton = new QToolButton(this);
    m_openButton->setToolTip(openAct->text());
    m_openButton->setIcon(QIcon(gl->path + "picts/nootka-exam.png"));
    m_openButton->setMenu(openMenu);
    m_openButton->setPopupMode(QToolButton::InstantPopup);
    QWidgetAction* openToolButtonAction = new QWidgetAction(this);
    openToolButtonAction->setDefaultWidget(m_openButton);

    m_closeAct = new QAction(QIcon(style()->standardIcon(QStyle::SP_DialogCloseButton)), tr("Close analyser"), this);
    connect(m_closeAct, SIGNAL(triggered()), this, SLOT(close()));

    m_zoomInAct = new QAction(QIcon(gl->path+"picts/zoom-in.png"), tr("zoom in"), this);
    connect(m_zoomInAct, SIGNAL(triggered()), this, SLOT(zoomInSlot()));

    m_zoomOutAct = new QAction(QIcon(gl->path+"picts/zoom-out.png"), tr("zoom out"), this);
    connect(m_zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOutSlot()));
// settings menu button
    m_inclWrongAct = new QAction(tr("include time of wrong answers to average"), this);
    m_inclWrongAct->setCheckable(true);
    m_wrongSeparateAct = new QAction(tr("show wrong answers separately"), this);
    m_wrongSeparateAct->setCheckable(true);
    QMenu *menu = new QMenu("chart menu", this);
    menu->addAction(m_wrongSeparateAct);
    menu->addAction(m_inclWrongAct);
    connect(m_wrongSeparateAct, SIGNAL(changed()), this, SLOT(wrongSeparateSlot()));
    connect(m_inclWrongAct, SIGNAL(changed()), this, SLOT(includeWrongSlot()));
    m_wrongSeparateAct->setChecked(m_chartSetts.separateWrong);
    m_inclWrongAct->setChecked(m_chartSetts.inclWrongAnsw);
    
    m_settButt = new QToolButton(this);
    m_settButt->setIcon(QIcon(gl->path+"picts/systemsettings.png"));
    m_settButt->setToolTip(tr("Settings of a chart"));
    m_settButt->setMenu(menu);
    m_settButt->setPopupMode(QToolButton::InstantPopup);
    m_settButt->setDisabled(true); // that options have no sense for default chart
    
    QWidgetAction* toolButtonAction = new QWidgetAction(this);
    toolButtonAction->setDefaultWidget(m_settButt);
    
    m_maximizeAct = new QAction(QIcon(gl->path + "picts/fullscreen.png"), tr("Maximize"), this);
    connect(m_maximizeAct, SIGNAL(triggered()), this, SLOT(maximizeWindow()));

    QActionGroup *chartTypeGroup = new QActionGroup(this);
    m_linearAct = new QAction(QIcon(gl->path + "picts/linearChart.png"), tr("linear chart"), this);
    m_linearAct->setCheckable(true);
    chartTypeGroup->addAction(m_linearAct);
    m_barAct = new QAction(QIcon(gl->path + "picts/barChart.png"), tr("bar chart"), this);
    m_barAct->setCheckable(true);
    chartTypeGroup->addAction(m_barAct);
    if (m_chartSetts.type == Tchart::e_linear)
      m_linearAct->setChecked(true);
    else
      m_barAct->setChecked(true);
    connect(chartTypeGroup, SIGNAL(triggered(QAction*)), this, SLOT(chartTypeChanged()));
    
    m_toolBar->addAction(openToolButtonAction);
    m_toolBar->addAction(toolButtonAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_zoomOutAct);
    m_toolBar->addAction(m_zoomInAct);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_linearAct);
    m_toolBar->addAction(m_barAct);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_maximizeAct);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_closeAct);
}

void TanalysDialog::createChart(Tchart::Tsettings& chartSett) {
    if (m_chart) {
      delete m_chart;
      m_chart = 0;
    }
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

void TanalysDialog::enableComboItem(int index, bool enable) {
    QModelIndex ind = m_chartListCombo->model()->index(index, 0);
    QVariant v;
    if (enable)
        v = QVariant(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    else
        v = QVariant(Qt::NoItemFlags); // disable
    m_chartListCombo->model()->setData(ind, v, Qt::UserRole - 1);
}


//##########  SLOTS #####################

void TanalysDialog::loadExamSlot() {
  
  QString fileName = QFileDialog::getOpenFileName(this, TstartExamDlg::loadExamFileTxt(), QDir::homePath(),
												  TstartExamDlg::examFilterTxt(), 0, QFileDialog::DontUseNativeDialog);
  if (fileName != "") {
      if (m_chart) {
        m_chart->deleteLater();
        m_chart = 0;
      }
      loadExam(fileName);
  }
}

void TanalysDialog::openRecentExam() {
    QAction *action = qobject_cast<QAction *>(sender());
        if (action) {
            QString file = action->text();
            loadExam(file);
        }
}

void TanalysDialog::analyseChanged(int index) {
  if (!m_exam)
    return;
    
  switch (index) {
    case 0:
      m_chartSetts.order = Tchart::e_byNumber;
      m_settButt->setDisabled(true);
      break;
    case 1:
      m_chartSetts.order = Tchart::e_byNote;
      m_settButt->setDisabled(false);
      break;
    case 2:
      m_chartSetts.order = Tchart::e_byFret;
      m_settButt->setDisabled(false);
      break;
    case 3:
      m_chartSetts.order = Tchart::e_byKey;
      m_settButt->setDisabled(false);
      break;
    case 4:
      m_chartSetts.order = Tchart::e_byAccid;
      m_settButt->setDisabled(false);
      break;
    case 5:
      m_chartSetts.order = Tchart::e_byQuestAndAnsw;
      m_settButt->setDisabled(false);
      break;
  }
  if (m_chartSetts.order == Tchart::e_byQuestAndAnsw) {
      disconnect(m_wrongSeparateAct, SIGNAL(changed()), this, SLOT(wrongSeparateSlot()));
      m_wrongSeparateAct->setChecked(false); // this sorting requires it!!
      m_wrongSeparateAct->setDisabled(true);
      m_chartSetts.separateWrong = false;
      m_inclWrongAct->setDisabled(false);
  } else {
      m_wrongSeparateAct->setDisabled(false);
      connect(m_wrongSeparateAct, SIGNAL(changed()), this, SLOT(wrongSeparateSlot()));
  }  
  createChart(m_chartSetts);
}


void TanalysDialog::testSlot() {
  QString testFile = gl->path + "/../../test.noo";
  loadExam(testFile);
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
    disconnect(m_inclWrongAct, SIGNAL(changed()), this, SLOT(includeWrongSlot()));
    m_inclWrongAct->setDisabled(true); // wrong separeted - it has no sense - lock it!
    m_chartSetts.inclWrongAnsw = false; // and reset 
    m_inclWrongAct->setChecked(false);
  }
  else {
    disconnect(m_inclWrongAct, SIGNAL(changed()), this, SLOT(includeWrongSlot()));
    m_inclWrongAct->setDisabled(false); // unlock
    connect(m_inclWrongAct, SIGNAL(changed()), this, SLOT(includeWrongSlot()));
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
    if (m_exam)
        levelView->setLevel(*m_exam->level());
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
        enableComboItem(0, true);
        createChart(m_chartSetts);
      }        
    } else { // bar chart
        if (m_chartSetts.type != Tchart::e_bar) {
          m_chartSetts.type = Tchart::e_bar;
          if (m_chartSetts.order == Tchart::e_byNumber) { // not suported by barChart (no sense)
            m_chartSetts.order = Tchart::e_byNote;
            m_chartListCombo->setCurrentIndex(1);
            enableComboItem(0, false);
          }
          createChart(m_chartSetts);
        }
      }
}



//##########  EVENTS #####################
void TanalysDialog::resizeEvent(QResizeEvent* )
{
    m_toolBar->setIconSize(QSize(height()/21, height()/21));
    m_openButton->setIconSize(m_toolBar->iconSize());
    m_settButt->setIconSize(m_toolBar->iconSize());     
}


