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
#include "texamlevel.h"
#include "tglobals.h"
#include "tnotename.h"
#include "mainwindow.h"
#include "texam.h"
#include <QLabel>
#include <QPainter>
#include <QHBoxLayout>
#include <QEvent>
#include <QX11Info>
#include <QDebug>

extern Tglobals *gl;

/* static */
QString TdialogMessage::getTextHowAccid(Tnote::Eacidentals accid) {
    QString S = QString("<br><span style=\"color: %1\">").arg(gl->GfingerColor.name());
    if (accid) S += tr("Use %1").arg(QString::fromStdString(signsAcid[accid + 2]));
    else S += tr(" Don't use accidentals!");
    S +=  "</span>";
    return S;
}


//#################################### CONSTRUCTOR ###########################################

TdialogMessage::TdialogMessage(Texam *exam, MainWindow *parent, Tnote::EnameStyle style) :
    QDialog(0, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool | Qt::X11BypassWindowManagerHint),
    m_scoreFree(true),
    m_nameFree(true),
    m_guitarFree(true),
    m_parent(parent)
{
//     setWindowOpacity(0.8);
// 	setStyleSheet("background:transparent;");
//     setStyleSheet(gl->getBGcolorText(gl->EquestionColor));

#if defined(Q_OS_LINUX)
  // check is Transparent background supported
  // fe. LXDE windows manager doesn't suport composite and displays ugly black frame
  // Windows doesn't support checking but composite works always
    if (QX11Info::isCompositingManagerRunning()) 
        setAttribute(Qt::WA_TranslucentBackground, true);
#else
        setAttribute(Qt::WA_TranslucentBackground, true);
#endif
    QRect parentGeo = parent->geometry();
    QHBoxLayout *lay = new QHBoxLayout;
    m_mainLab = new QLabel(getQuestion(exam->qusetion(exam->count()-1), exam->count(), exam->level(), style), this);
    m_mainLab->setAlignment(Qt::AlignCenter);
    m_mainLab->setWordWrap(true);

    setPosAndSize();
    m_mainLab->setStyleSheet(QString("border-radius: 10px; %1").arg(gl->getBGcolorText(gl->EquestionColor)));
    lay->addStretch(1);
    lay->addWidget(m_mainLab, 0, Qt::AlignCenter);
    lay->addStretch(1);
    setLayout(lay);
    
    connect(parent, SIGNAL(moved(QPoint)), this, SLOT(mainWindowMoved(QPoint)));
    connect(parent, SIGNAL(maximised()), this, SLOT(mainWindowMaximised()));
    connect(parent, SIGNAL(minimised()), this, SLOT(mainWindowMinimised()));
    connect(parent, SIGNAL(sizeChanged()), this, SLOT(mainWindowResized()));
    
    show();
}

void TdialogMessage::setPosAndSize() {
  if (m_guitarFree)
      m_pos = QPoint(m_parent->geometry().left() + m_parent->geometry().width() / 2, m_parent->geometry().top() + (m_parent->geometry().height() /3) * 2);
    else
      if (m_nameFree)
        m_pos = QPoint(m_parent->geometry().left() + (m_parent->geometry().width() / 2),
                       m_parent->geometry().top() + (m_parent->geometry().height() /4));
      else // on the score
        m_pos = QPoint(m_parent->geometry().left() + 10, m_parent->geometry().top() + m_parent->geometry().height() /10);
    setGeometry(m_pos.x(), m_pos.y(), m_parent->geometry().width() / 3,
                m_parent->geometry().height() / 3 - m_parent->geometry().height() / 10);
    m_mainLab->setFixedSize(m_parent->geometry().width() / 3 - 10, m_parent->geometry().height() / 3 - 10);
    QFont f(font());
    f.setPointSize(height() / 10);
    m_mainLab->setFont(f);
}


QString TdialogMessage::getKeyText(TkeySignature& key) {
  QString keyTxt ="";
  if (key.isMinor())
      keyTxt = key.getMinorName();
  else
      keyTxt = key.getMajorName();
  return keyTxt;
}


QString TdialogMessage::getQuestion(TQAunit &question, int questNr, TexamLevel* level, Tnote::EnameStyle style) {
    QString quest = QString("<b>%1. </b><br>").arg(questNr);
    QString apendix = "";
    QString noteStr;
    switch (question.questionAs) {
      case TQAtype::e_asNote:
        m_scoreFree = false;
        if (question.answerAs == TQAtype::e_asNote) {
          quest += tr("Change enharmonicaly and show in the score");
          if (level->useKeySign && level->manualKey) {
            apendix = tr("<br><b>in %1 key.</b>", "in key signature").arg(getKeyText(question.key));
          }
          if (level->forceAccids)
            quest += "<br>" + getTextHowAccid((Tnote::Eacidentals)question.qa_2.note.acidental);
        } else
          if (question.answerAs == TQAtype::e_asName) {
            m_nameFree = false;
            quest += tr("Give name of");
          } else
            if (question.answerAs == TQAtype::e_asFretPos) {
              m_guitarFree = false;
              quest += tr("Show on the guitar");
              if (level->showStrNr)
                apendix = "<br><b>" + tr(" on <span style=\"font-family: nootka;\">%1</span> string.").
                      arg((int)question.qa.pos.str()) + "</b>";
            } else
              if (question.answerAs == TQAtype::e_asSound) {
                quest += "Play or sing";
              }
        quest += "<br>" + TtipChart::wrapPixToHtml(question.qa.note, true, question.key);
        if (apendix != "")
          quest += apendix;
      break;
      
      case TQAtype::e_asName:
        m_nameFree = false;
        noteStr = "<br><b>" + TnoteName::noteToRichText(question.qa.note) + "</b>";
        if (question.answerAs == TQAtype::e_asNote) {
          m_nameFree = false;
          quest += tr("Show in the score") + noteStr;
          if (level->useKeySign && level->manualKey) {
            quest += tr("<br><b>in %1 key.</b>", "in key signature").arg(getKeyText(question.key));
          }
        } else
          if (question.answerAs == TQAtype::e_asName) {
            m_nameFree = false;
            Tnote::EnameStyle tmpStyle = gl->NnameStyleInNoteName;
            gl->NnameStyleInNoteName = style;
            noteStr = "<br><b>" + TnoteName::noteToRichText(question.qa.note) + "</b>";
            if (question.qa.note.acidental != question.qa_2.note.acidental)
                quest += tr("Change enharmonicaly and give name of");
            else
                quest += tr("Use another style to give name of");
            quest += noteStr + "<br>" + getTextHowAccid((Tnote::Eacidentals)question.qa_2.note.acidental);
            gl->NnameStyleInNoteName = tmpStyle;
            // It is not so elegant to get note name in different style this way
            // but there is no other way
          } else
            if (question.answerAs == TQAtype::e_asFretPos) {
              m_guitarFree = false;
              quest += tr("Show on the guitar") + noteStr;
              if (level->showStrNr)
                quest += "<br><b>" + tr(" on <span style=\"font-family: nootka;\">%1</span> string.").
                      arg((int)question.qa.pos.str()) + "</b>";
            } else
              if (question.answerAs == TQAtype::e_asSound) {
                quest += "<br>" + tr("Play or sing") + noteStr;
              }
      break;
      
      case TQAtype::e_asFretPos:
        quest += "";
        m_guitarFree = false;
        if (question.answerAs == TQAtype::e_asNote) {
          m_scoreFree = false;
          quest += tr("Show on the score note played on");
          if (level->useKeySign && level->manualKey) {
            apendix = tr("<br><b>in %1 key.</b>", "in key signature").arg(getKeyText(question.key));
          }
        } else
          if (question.answerAs == TQAtype::e_asName) {
            m_nameFree = false;
            quest += tr("Give name of");
          } else
            if (question.answerAs == TQAtype::e_asFretPos) {
              quest += "not implemented";
            } else
              if (question.answerAs == TQAtype::e_asSound) {
                quest += tr("Play or sing");
              }
        quest += "<br><span style=\"font-size: 25px;\">" + TtipChart::wrapPosToHtml(question.qa.pos) + "</span>";
        if (apendix != "")
          quest += "<br>" + apendix;
        if (question.answerAs == TQAtype::e_asNote || question.answerAs == TQAtype::e_asName)
          if (level->forceAccids)
            quest += "<br>" + getTextHowAccid((Tnote::Eacidentals)question.qa.note.acidental);
        
      break;
      
      case TQAtype::e_asSound:
        if (question.answerAs == TQAtype::e_asNote) {
          m_scoreFree = false;
          quest += tr("Listened sound show in the score");
          if (level->useKeySign && level->manualKey) {
            QString keyTxt;
            if (question.key.isMinor())
              keyTxt = question.key.getMinorName();
            else
              keyTxt = question.key.getMajorName();
            quest += tr("<br><b>in %1 key.</b>", "in key signature").arg(keyTxt);
          }
          if (level->forceAccids)
            quest += "<br>" + getTextHowAccid((Tnote::Eacidentals)question.qa.note.acidental);
        } else
          if (question.answerAs == TQAtype::e_asName) {
            m_nameFree = false;
            quest += tr("Give name of listened sound");
            if (level->forceAccids)
                quest += "<br>" + getTextHowAccid((Tnote::Eacidentals)question.qa.note.acidental);
          } else
            if (question.answerAs == TQAtype::e_asFretPos) {
              m_guitarFree = false;
              quest += tr("Listened sound show on the guitar");
              if (level->showStrNr)
              quest += "<br><b>" + tr(" on <span style=\"font-family: nootka;\">%1</span> string.").
                      arg((int)question.qa.pos.str()) + "</b>";
            } else
              if (question.answerAs == TQAtype::e_asSound) {
                quest += tr("Play or sing listened sound");          
              }
      break;
    }
    
    return quest;
}


void TdialogMessage::paintEvent(QPaintEvent *) {
	QPainter painter(this);
    QRect rect = m_mainLab->geometry();
//     QRect rect = geometry();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    QColor c = palette().text().color();
    c.setAlpha(100);
    painter.setBrush(QBrush(c));
    painter.drawRoundedRect(rect.x() + 5, rect.y() +5, rect.width(), rect.height(), 10, 10);
    painter.setBrush(QBrush(palette().background().color()));
    painter.drawRoundedRect(rect, 10, 10);
}

//#################################### SLOTS ###########################################

void TdialogMessage::mainWindowMoved(QPoint vector) {
    m_pos = m_pos + vector;
    move(m_pos);
}

void TdialogMessage::mainWindowMinimised() {
  hide();
}

void TdialogMessage::mainWindowMaximised() {
  show();
}

void TdialogMessage::mainWindowResized() {
  setPosAndSize();
}



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
