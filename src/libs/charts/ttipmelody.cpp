/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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


#include "ttipmelody.h"
#include "tquestionpoint.h"
#include "tmelodyview.h"
#include <exam/tqaunit.h>
#include <exam/textrans.h>
#include <exam/texam.h>
#include <exam/tlevel.h>
#include <exam/tattempt.h>
#include <exam/tresulttext.h>
#include <music/tmelody.h>
#include <widgets/tquestionaswdg.h>
#include <QtWidgets>



TtipMelody::TtipMelody(TquestionPoint *point) :
  TtipChart(point)
{
  setBgColor(point->color());
  setPlainText(" ");
  
  m_w = new QWidget();
    m_w->setObjectName("m_melodyView");
    m_w->setStyleSheet("QWidget#m_melodyView { background: transparent }");
  QString txt;
  if (point->nr())
    txt = QString(TquestionAsWdg::questionTxt() + " <big><b>%1.</b></big>").arg(point->nr());
  if (point->question()->questionOnScore() && point->question()->answerAsSound())
      txt += (" <b>" + TexTrans::playMelodyTxt() + "</b>");
  else if (point->question()->questionAsSound() && point->question()->answerOnScore())
      txt += (" <b>" + TexTrans::writeMelodyTxt() + "</b>");
  QLabel *headLab = new QLabel(txt, m_w);
  headLab->setAlignment(Qt::AlignCenter);
  m_score = new TmelodyView(qa()->question()->melody(), m_w);
  m_score->setFixedHeight(qApp->desktop()->availableGeometry().height() / 12);
  if (point->question()->exam()) {
    if (point->question()->exam()->level()->showStrNr)
      m_score->showStringNumbers(true);
  }
  QSpinBox *spinAtt = new QSpinBox(m_w);
    spinAtt->setRange(0, qa()->question()->attemptsCount());
    spinAtt->setPrefix(TexTrans::attemptTxt() + " ");
    spinAtt->setSuffix(" " + tr("of", "It will give text: 'Attempt x of y'") + QString(" %1").arg(qa()->question()->attemptsCount()));
  m_attemptLabel = new QLabel(m_w);
  m_resultLabel = new QLabel(wasAnswerOKtext(point->question(), point->color()).replace("<br>", " "), m_w);
  m_resultLabel->setAlignment(Qt::AlignCenter);
//   txt = wasAnswerOKtext(point->question(), point->color()).replace("<br>", " ") + "<br>";
  txt = tr("Melody was played <b>%n</b> times", "", qa()->question()->totalPlayBacks()) + "<br>";
  txt += TexTrans::effectTxt() + QString(": <big><b>%1%</b></big>, ").arg(point->question()->effectiveness(), 0, 'f', 1, '0');
  txt += TexTrans::reactTimeTxt() + QString("<big><b>  %1</b></big>").arg(Texam::formatReactTime(point->question()->time, true));
  QLabel *sumLab = new QLabel(txt, m_w);
  sumLab->setAlignment(Qt::AlignCenter);
  
  QVBoxLayout *lay = new QVBoxLayout;
    lay->addWidget(headLab);
    lay->addWidget(m_score, 0, Qt::AlignCenter);
    QHBoxLayout *attLay = new QHBoxLayout;
      attLay->addStretch();
      attLay->addWidget(spinAtt);
      attLay->addStretch();
    lay->addLayout(attLay);
    lay->addWidget(m_attemptLabel);
    lay->addWidget(m_resultLabel);
    lay->addWidget(sumLab);
    
    m_w->setLayout(lay);
  m_widget = point->scene()->addWidget(m_w);
  m_widget->setParentItem(this);
  
  connect(spinAtt, SIGNAL(valueChanged(int)), this, SLOT(attemptChanged(int)));
}


TtipMelody::~TtipMelody() {
	m_widget->clearFocus(); // This avoids crashes when some of tip widgets (spin or score) got focus before
	delete m_widget;
}


QRectF TtipMelody::boundingRect() const {
  return m_widget->boundingRect();
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TtipMelody::attemptChanged(int attNr) {
  if (attNr) {
    m_score->markMistakes(qa()->question()->attempt(attNr -1)->mistakes);
    if (qa()->question()->attemptsCount() > 1)
      m_attemptLabel->setText(QString("<b>%1: </b>").arg((attNr)) +
              tr("played", "a melody was played (and number follows)") + QString(" <b>%1</b>").arg(qa()->question()->attempt(attNr - 1)->playedCount()) + ", " + TexTrans::effectTxt().toLower() +
              QString(": <b>%1%</b>").arg(qa()->question()->attempt(attNr - 1)->effectiveness(), 0, 'f', 1, '0') + ", " + tr("time") + ": " +
              QString("<b>  %1</b>").arg(Texam::formatReactTime(qa()->question()->attempt(attNr - 1)->totalTime(), true)));
  } else {
    m_score->clearMistakes();
    m_attemptLabel->setText("");
  }
  if (qa()->question()->attemptsCount() > 1) {
    if (attNr && attNr < qa()->question()->attemptsCount()) {
      QColor attemptColor = TquestionPoint::goodColor();
      if (qa()->question()->attempt(attNr - 1)->summary()) {
        if (qa()->question()->attempt(attNr - 1)->summary() & TQAunit::e_wrongNote)
          attemptColor = TquestionPoint::wrongColor();
        else
          attemptColor = TquestionPoint::notBadColor();
      }
      m_resultLabel->setText(wasAnswerOKtext(qa()->question(), attemptColor, -1, attNr).replace("<br>", " "));
    } else
      m_resultLabel->setText(wasAnswerOKtext(qa()->question(), qa()->color()).replace("<br>", " "));
  }
}







