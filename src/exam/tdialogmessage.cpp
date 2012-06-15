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



#include "tdialogmessage.h"
#include "tqaunit.h"
#include "ttipchart.h"
#include <tnotename.h>
#include <QLabel>
// #include <QPainter>
#include <QHBoxLayout>

/* static */
QString TdialogMessage::getTextHowAccid(Tnote::Eacidentals accid) {
    QString S = QString("<br><span style=\"color: %1\">").arg(QColor(Qt::magenta).name());
    if (accid) S += tr("Use %1").arg(QString::fromStdString(signsAcid[accid + 2]));
    else S += tr(" Don't use accidentals!");
    S +=  "</span>";
    return S;
}


//#################################### CONSTRUCTOR ###########################################

TdialogMessage::TdialogMessage(TQAunit& question, int questNr, const QRect& parentGeo) :
    QDialog(0, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool)
{
//     setWindowOpacity(0.8);
// 	setStyleSheet("background:transparent;");
//     setAttribute(Qt::WA_TranslucentBackground, true);
    QHBoxLayout *lay = new QHBoxLayout;
    setGeometry(parentGeo.left() + parentGeo.width() / 2, parentGeo.top() + parentGeo.height() / 3, parentGeo.width() / 3, parentGeo.height() / 3);
    QLabel *mainLab = new QLabel(getQuestion(question, questNr), this);
    mainLab->setAlignment(Qt::AlignCenter);
    lay->addStretch(1);
    lay->addWidget(mainLab, 0, Qt::AlignCenter);
    lay->addStretch(1);
    setLayout(lay);
    show();
}

QString TdialogMessage::getQuestion(TQAunit& question, int questNr) {
    QString quest = QString("<b>%1. </b>").arg(questNr);
    switch (question.questionAs) {
      case TQAtype::e_asNote:
        quest += "<table valign=\"middle\" align=\"center\"><tr><td>";
        if (question.answerAs == TQAtype::e_asNote) {
          quest += tr("Show on the score alternative of");
          // TODO ket signature !!
        } else
          if (question.answerAs == TQAtype::e_asName) {
            quest += tr("Give name of note");
          } else
            if (question.answerAs == TQAtype::e_asFretPos) {
              quest += tr("Show on the guitar note");
            } else
              if (question.answerAs == TQAtype::e_asSound) {
                quest += "Play or sing note";
              }
        quest += " " + TtipChart::wrapPixToHtml(question.qa.note, true, question.key) + "</td></tr></table>";
      break;
      
      case TQAtype::e_asName:
        QString noteStr = "<b>  " + TnoteName::noteToRichText(question.qa.note) + "</b>";
        if (question.answerAs == TQAtype::e_asNote) {
          quest += tr("Show on the score") + noteStr;
        } else
          if (question.answerAs == TQAtype::e_asName) {
            quest += tr("Give another name of") + noteStr + "<br>" +
                  getTextHowAccid((Tnote::Eacidentals)question.qa_2.note.acidental);                
          } else
            if (question.answerAs == TQAtype::e_asFretPos) {
              quest += tr("Show on the guitar") + noteStr;
              // TODO string nr
            } else
              if (question.answerAs == TQAtype::e_asSound) {
                quest += tr("Play or sing note") + noteStr;
              }
      break;
      /*
      case TQAtype::e_asFretPos:
        quest += "";
        if (question.answerAs == TQAtype::e_asNote) {
          quest += tr("Show on the score note played on");
        } else
          if (question.answerAs == TQAtype::e_asName) {
            quest += tr("Give name of");
          } else
            if (question.answerAs == TQAtype::e_asFretPos) {
              // NOT IMPLEMENTED
            } else
              if (question.answerAs == TQAtype::e_asSound) {
                quest += tr("Play or sing note");
              }
        quest += "<br>" + TtipChart::wrapPosToHtml(question.qa.pos);
      break;
      
      case TQAtype::e_asSound:
        if (question.answerAs == TQAtype::e_asNote) {
          
        } else
          if (question.answerAs == TQAtype::e_asName) {
          
          } else
            if (question.answerAs == TQAtype::e_asFretPos) {
          
            } else
              if (question.answerAs == TQAtype::e_asSound) {
          
              }
      break;*/
    }
    
    return quest;
}


// void TdialogMessage::paintEvent(QPaintEvent *paintEvent) {
// 	QPainter painter(this);
//     painter.setRenderHint(QPainter::Antialiasing);
//     painter.setPen(Qt::NoPen);
//     painter.setBrush(QColor(255, 0, 0, 127));

//     painter.drawEllipse(0, 0, width(), height());
// }


/*
void MainWindow::on_actionAlways_on_Top_triggered(bool checked)
{
#ifdef Q_OS_WIN
    // #include <windows.h>
    if (checked)
    {
        SetWindowPos(this->winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
    else
    {
        SetWindowPos(this->winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
#else
    Qt::WindowFlags flags = this->windowFlags();
    if (checked)
    {
        this->setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
        this->show();
    }
    else
    {
        this->setWindowFlags(flags ^ (Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
        this->show();
    }
#endif
}

QPixmap qpm(":/alphamask.png"); 
qpm = qpm.scaled(this->width(),this->height(),Qt::IgnoreAspectRatio,Qt::SmoothTransfor‌​mation);
if (!qpm.isNull()) 
  this->setMask(qpm.mask());
  this->setWindowOpacity(0.9);
*/