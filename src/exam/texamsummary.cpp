/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                             *
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


#include "texamsummary.h"
#include "texamview.h"
#include <exam/texam.h>
#include <exam/tqaunit.h>
#include <exam/textrans.h>
#include <exam/tlevel.h>
#include <level/tlevelpreview.h>
#include <widgets/troundedlabel.h>
#include <tpath.h>
#include <plugins/tpluginsloader.h>
#include <QtWidgets/QtWidgets>
#if defined (Q_OS_ANDROID)
  #include <touch/ttoucharea.h>
  #include <Android/tandroid.h>
#endif
#include <qtr.h>


  /** returns 2 columns row of table */
QString row2(const QString& S1, const QString& S2) {
  return QString("<tr><td>%1: </td><td><b>%2</b></td></tr>").arg(S1).arg(S2);
}


bool showExamSummary(QWidget* mainWindow, Texam* exam, bool cont, bool* startExam) {
	TexamSummary *ES = new TexamSummary(exam, cont, mainWindow);
  TexamSummary::Eactions respond = ES->doExec();
	if (startExam) {
		if (respond == TexamSummary::e_startExam)
			*startExam = true;
		else
			*startExam = false;
	}
  delete ES;
  if (respond == TexamSummary::e_discard)
    return false;
  else
    return true;
}



TexamSummary::TexamSummary(Texam* exam, bool cont, QWidget* parent) :
  QDialog(parent),
  m_exam(exam),
  m_state(e_discard),
  m_closeButt(0), m_examButton(0),
  m_mainWIndow(parent)
{
#if defined (Q_OS_ANDROID)
  showMaximized();
#else
  setWindowTitle(tr("Exam results"));
  setWindowIcon(QIcon(Tpath::img("startExam")));
#endif
  QHBoxLayout *lay = new QHBoxLayout();
//-------  left layout -----------------------
  m_leftLay = new QVBoxLayout();
  QString font20 = "<b><big>";
  QLabel *userNameLab = new QLabel(tr("student:") + QString("  %2<u>%1</u></b>").arg(exam->userName()).arg(font20), this);
  m_leftLay->addWidget(userNameLab, 0, Qt::AlignCenter);
  TroundedLabel *questNrLab = new TroundedLabel("<center>" + tr("Number of questions:") + QString("%2  %1</big></b>").arg(exam->count()).arg(font20) +
                    QString("<br>%1: %2%3</big></b>").arg(TexTrans::corrAnswersNrTxt()).arg(font20).
                        arg(exam->count() - exam->mistakes() - exam->halfMistaken()) +
                    QString("<br>%1: %2%3</big></b>").arg(TexTrans::mistakesNrTxt()).arg(font20).arg(exam->mistakes()) +
                    QString("<br>%1: %2%3</big></b>").arg(TexTrans::halfMistakenTxt()).arg(font20).arg(exam->halfMistaken())
      ,this);
  m_leftLay->addWidget(questNrLab);
  QVBoxLayout *timeLay = new QVBoxLayout();
  QGroupBox *timeGr = new QGroupBox(tr("times:"), this);
  TroundedLabel *timeLab = new TroundedLabel("<table>" +
  row2(TexTrans::totalTimetxt(), TexamView::formatedTotalTime(exam->totalTime() * 1000)) +
  row2(tr("Time taken to answer"), TexamView::formatedTotalTime(exam->workTime() * 1000)) +
  row2(TexTrans::averAnsverTimeTxt(), QString("%1 s").
      arg((qreal)exam->averageReactonTime()/10.0, 0, 'f', 1, '0')) +
  "</table>", this);
  timeLab->setContentsMargins(5, 5, 5, 5);
  timeLay->addWidget(timeLab);

  timeGr->setLayout(timeLay);
  m_leftLay->addWidget(timeGr);

  QPushButton *analyseButt = new QPushButton(tr("Analyze"), this);
    analyseButt->setIcon(QIcon(Tpath::img("charts")));
    analyseButt->setIconSize(QSize(48, 48));
  if (exam->count() == 0)
    analyseButt->setDisabled(true);
#if defined (Q_OS_ANDROID) // TODO: delete if mobile version will support analysis
    analyseButt->hide();
  m_sendButt = new QPushButton(qTR("QShortcut", "Send"), this);
    m_sendButt->setIcon(QIcon(Tpath::img("nootka-exam")));
    m_sendButt->setIconSize(QSize(48, 48));
#endif

  m_okButt = new QPushButton(qTR("QPlatformTheme", "Close"), this);
  if (cont) {
      m_okButt->setText(qTR("QWizard", "Continue"));
      m_okButt->setIcon(QIcon(Tpath::img("exam")));
      m_closeButt = new QPushButton(qTR("QPlatformTheme", "Discard"), this);
      m_closeButt->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
      m_closeButt->setIconSize(QSize(48, 48));
      connect(m_closeButt, &QPushButton::clicked, this, &TexamSummary::closeSlot);
  } else
      m_okButt->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
  m_okButt->setIconSize(QSize(48, 48));

  auto buttLay = new QHBoxLayout;
    buttLay->addWidget(m_okButt);
    buttLay->addWidget(analyseButt);

  m_leftLay->addStretch(1);
  m_leftLay->addLayout(buttLay);
#if defined (Q_OS_ANDROID)
  m_leftLay->addWidget(m_sendButt);
#endif
  if (cont)
    m_leftLay->addWidget(m_closeButt);

	lay->addLayout(m_leftLay);

//-------  right layout -----------------------	
	QVBoxLayout *rightLay = new QVBoxLayout();
	TlevelPreview *levelWdg = new TlevelPreview(this);
	rightLay->addWidget(levelWdg);
	levelWdg->setLevel(*(exam->level()));
	levelWdg->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	QVBoxLayout *resLay = new QVBoxLayout();
	QGroupBox *resGr = new QGroupBox(tr("Results:"), this);
  QString effStr;
  if (exam->mistakes() || exam->halfMistaken()) {
//     effStr = row2(TexamView::mistakesNrTxt(), QString::number(exam->mistakes()));
//     effStr += row2(TexamView::corrAnswersNrTxt(), QString::number(exam->count()-exam->mistakes()));
    float wAccid = 0.0, wKey = 0.0, wNote = 0.0, wOctave = 0.0, wStyle = 0.0, wPos = 0.0, wString = 0.0, wTotal;
    float wInto = 0.0, wLittle = 0.0, wPoor = 0.0;
    for(int i=0; i<exam->count(); i++) {
      if (!exam->question(i)->isCorrect()) {
          if(exam->question(i)->wrongAccid())       wAccid++;
          if(exam->question(i)->wrongKey())         wKey++;
          if(exam->question(i)->wrongNote())        wNote++;
          if(exam->question(i)->wrongOctave())      wOctave++;
          if(exam->question(i)->wrongStyle())       wStyle++;
          if(exam->question(i)->wrongPos())         wPos++;
          if(exam->question(i)->wrongString())      wString++;
					if(exam->question(i)->wrongIntonation())  wInto++;
          if(exam->question(i)->littleNotes())      wLittle++;
          if(exam->question(i)->poorEffect())       wPoor++;
      }
    }
    effStr += "<tr><td colspan=\"2\">-------- " + tr("Kinds of mistakes") + QStringLiteral(": --------</td></tr>");
    wTotal = wAccid + wKey + wNote + wOctave + wStyle + wPos + wString + wInto + wLittle + wPoor;
    QString cp = QStringLiteral("%)"); // closing percent '%)'
    if (wNote)
      effStr += row2(tr("Wrong notes"), QString("%1 (").arg(wNote) + QString::number(qRound(wNote * 100.0 / wTotal)) + cp);
    if (wAccid)
      effStr += row2(tr("Wrong accidentals"), QString("%1 (").arg(wAccid) + QString::number(qRound(wAccid * 100.0 / wTotal)) + cp);
    if (wKey)
      effStr += row2(tr("Wrong key signatures"), QString("%1 (").arg(wKey) + QString::number(qRound(wKey * 100.0 / wTotal)) + cp);
    if (wOctave)
      effStr += row2(tr("Wrong octaves"), QString("%1 (").arg(wOctave) + QString::number(qRound(wOctave * 100.0 / wTotal)) + cp);
    if (wStyle)
      effStr += row2(tr("Wrong note names"), QString("%1 (").arg(wStyle)) + QString::number(qRound(wStyle * 100.0 / wTotal)) + cp;
    if (wPos)
      effStr += row2(tr("Wrong positions on guitar"), QString("%1 (").arg(wPos) + QString::number(qRound(wPos * 100.0 / wTotal)) + cp);
    if (wString)
      effStr += row2(tr("Wrong strings"), QString("%1 (").arg(wString) + QString::number(qRound(wString * 100.0 / wTotal)) + cp);
		if (wInto)
      effStr += row2(tr("Out of tune"), QString("%1 (").arg(wInto) + QString::number(qRound(wInto * 100.0 / wTotal)) + cp);
    if (wLittle)
      effStr += row2(QApplication::translate("AnswerText", "little valid notes", "the amount of correct notes in an answer is little"),
                     QString("%1 (").arg(wLittle) + QString::number(qRound(wLittle * 100.0 / wTotal)) + cp);
    if (wPoor)
      effStr += row2(QApplication::translate("AnswerText", "poor effectiveness"),
                     QString("%1 (").arg(wPoor) + QString::number(qRound(wPoor * 100.0 / wTotal)) + cp);
  }
	TroundedLabel *resLab = new TroundedLabel(QStringLiteral("<table>") +
    row2(TexTrans::effectTxt(), QString::number(qRound(exam->effectiveness())) + QStringLiteral("%")) + effStr + QStringLiteral("</table>"), this);
  resLab->setContentsMargins(5, 5, 5, 5);
	resLay->addWidget(resLab);

	resGr->setLayout(resLay);
	rightLay->addWidget(resGr);

	lay->addLayout(rightLay);

#if defined (Q_OS_ANDROID)
  auto ta = new TtouchArea(this);
  ta->setLayout(lay);
  auto touchLay = new QVBoxLayout;
  touchLay->addWidget(ta);
  setLayout(touchLay);
#else
  setLayout(lay);
#endif

  connect(analyseButt, &QPushButton::clicked, this, &TexamSummary::analyseSlot);
  connect(m_okButt, &QPushButton::clicked, this, &TexamSummary::continueSlot);
#if defined (Q_OS_ANDROID)
  connect(m_sendButt, &QPushButton::clicked, this, &TexamSummary::sendExamSlot);
#endif

  if (m_exam->isExercise())
    setForExercise();
}


TexamSummary::Eactions TexamSummary::doExec() {
  QDialog::exec();
  return m_state;
}


void TexamSummary::setForExercise() {
	setWindowTitle(tr("Progress of exercises"));
	m_examButton = new QPushButton(tr("Pass an exam"), this);
		m_examButton->setToolTip(tr("Finish exercise and pass an exam on this level."));
		m_examButton->setIcon(QIcon(Tpath::img("exam")));
		m_examButton->setIconSize(QSize(48, 48));
  connect(m_examButton, &QPushButton::clicked, this, &TexamSummary::startExamSlot);
	if (m_closeButt) {
		m_okButt->setIcon(QIcon(Tpath::img("practice")));
		m_closeButt->setText(tr("Finish this exercise"));
		m_leftLay->insertWidget(m_leftLay->count() - 1, m_examButton);
	} else
		m_leftLay->addWidget(m_examButton);
#if defined (Q_OS_ANDROID)
  m_sendButt->hide(); // don't send exercise file
#endif
}


//#################################################################
//               SLOTS
//#################################################################

void TexamSummary::analyseSlot() {
  TpluginsLoader loader;
  if (loader.load(TpluginsLoader::e_analyzer)) {
    loader.init(QString(), m_mainWIndow, m_exam);
  }
}


#if defined (Q_OS_ANDROID)
void TexamSummary::sendExamSlot() {
  QString space = QStringLiteral(" ");
  QString br = QStringLiteral("\n");
  QString message = qTR("TexamSummary", "student:") + space + m_exam->userName() + br;
  message += qTR("TanalysDialog", "level:") + space + m_exam->level()->name + br;
  message += qTR("TexamSummary", "Number of questions:") + space + QString::number(m_exam->count()) + br;
  Tandroid::sendExam(tr("Send exam file"), message, m_exam->fileName());
}
#endif

void TexamSummary::closeSlot() {
  m_state = e_discard;
  close();
}

void TexamSummary::continueSlot() {
  m_state = e_continue;
  close();
}


void TexamSummary::startExamSlot() {
	m_state = e_startExam;
	close();
}







