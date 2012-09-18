/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
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
#include "tstartexamdlg.h"
#include "texamview.h"
#include "tchart.h"
#include "tmainchart.h"
#include "tgraphicstexttip.h"
#include "tglobals.h"


extern Tglobals *gl;


TanalysDialog::TanalysDialog(Texam* exam, QWidget* parent) :
    QDialog(parent),
    m_exam(0),
    m_level(new TexamLevel()),
    m_chart(0),
    m_wasExamCreated(false)
{
 
  setWindowTitle(tr("Analyse of an exam results"));
  setGeometry(parent->geometry());
  QVBoxLayout *lay = new QVBoxLayout;
// default chart presets
  m_chartSetts.inclWrongAnsw = false;
  m_chartSetts.separateWrong = true;
  m_chartSetts.order = Tchart::e_byNumber;

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
  headLay->addWidget(m_chartListCombo, 1, 0, Qt::AlignCenter);
  m_userLab = new QLabel(" ", this);
  headLay->addWidget(m_userLab, 1, 1, Qt::AlignCenter);
  m_levelLab = new QLabel(" ", this);
  headLay->addWidget(m_levelLab, 1, 2, Qt::AlignCenter);
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
  
//   QTimer::singleShot(100, this, SLOT(testSlot()));
  if (exam) {
    m_wasExamCreated = false;
    m_openExamAct->setVisible(false); // hide "open exam file" acction
    setExam(exam);
  } else { // show help in tip
    QString modKey = "";
#if defined(Q_OS_MAC)
    modKey = "CMD";
#else
    modKey = "CTRL";
#endif
      TgraphicsTextTip *helpTip = new TgraphicsTextTip("<br>" + tr("Select an exam from a file<br>Use %1 + mouse wheel to zoom a chart.<br>Drag a cursor to move the chart.").arg(modKey)  + "<br>", TquestionPoint::bgColor());
      m_chart->scene->addItem(helpTip);
      helpTip->setFlag(QGraphicsItem::ItemIgnoresTransformations);
//       helpTip->setPos((m_chart->width() - helpTip->boundingRect().width()) / 2, 
//                       (m_chart->height() - helpTip->boundingRect().height()) / 2 );
      helpTip->setPos(200, 80);
  }
  
  connect(m_chartListCombo, SIGNAL(activated(int)), this, SLOT(analyseChanged(int)) );

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
  m_userLab->setText(m_exam->userName());
  m_levelLab->setText(m_exam->level()->name);
  m_questNrLab->setText(tr("Questions number:") + QString(" %1").arg(exam->count()) );
  m_effectLab->setText(TexamView::effectTxt() + QString(": %1%")
                       .arg(m_exam->effectiveness()) );
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
    }
      
}

//##########  PRIVATE METHODS #####################

void TanalysDialog::createActions() {
    m_openExamAct = new QAction(QIcon(gl->path + "picts/nootka-exam.png"), tr("Open an exam"), this);
    connect(m_openExamAct, SIGNAL(triggered()), this, SLOT(loadExamSlot()));

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
    m_settButt->setMenu(menu);
    m_settButt->setPopupMode(QToolButton::InstantPopup);
    m_settButt->setDisabled(true); // that options have no sense for default chart
    
    QWidgetAction* toolButtonAction = new QWidgetAction(this);
    toolButtonAction->setDefaultWidget(m_settButt);

    m_toolBar->addAction(m_openExamAct);
    m_toolBar->addAction(toolButtonAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_zoomOutAct);
    m_toolBar->addAction(m_zoomInAct);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_closeAct);
}

void TanalysDialog::createChart(Tchart::Tsettings& chartSett) {
    if (m_chart) {
      delete m_chart;
      m_chart = 0;
    }
    if (m_exam)
      m_chart = new TmainChart(m_exam, m_chartSetts, this);
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
  
  QString fileName = QFileDialog::getOpenFileName(this, tr("Load an exam file"),
                               QDir::homePath(), TstartExamDlg::examFilterTxt());
  loadExam(fileName);
}


void TanalysDialog::analyseChanged(int index) {
  if (!m_exam)
    return;
    
  switch (index) {
    case 0:
      m_chartSetts.order = TmainChart::e_byNumber;
      m_settButt->setDisabled(true);
      break;
    case 1:
      m_chartSetts.order = TmainChart::e_byNote;
      m_settButt->setDisabled(false);
      break;
    case 2:
      m_chartSetts.order = TmainChart::e_byFret;
      m_settButt->setDisabled(false);
      break;
    case 3:
      m_chartSetts.order = TmainChart::e_byKey;
      m_settButt->setDisabled(false);
      break;
  }
  createChart(m_chartSetts);
}


void TanalysDialog::testSlot() {
  QString testFile = "../nootka/test.noo";
  loadExam(testFile);
}

void TanalysDialog::zoomInSlot() {
    m_chart->zoom(true);
}

void TanalysDialog::zoomOutSlot() {
    m_chart->zoom(false);
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

//##########  EVENTS #####################
void TanalysDialog::resizeEvent(QResizeEvent* )
{
    m_toolBar->setIconSize(QSize(height()/21, height()/21));
}
