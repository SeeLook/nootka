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


QString TdialogMessage::getKeyText(TkeySignature& key) {
  QString keyTxt ="";
  if (key.isMinor())
      keyTxt = key.getMinorName();
  else
      keyTxt = key.getMajorName();
  return keyTxt;
}


QString TdialogMessage::onStringTxt(quint8 strNr) {
    return tr("on <span style=\"font-family: nootka;\">%1</span> string.").arg(strNr);
}


//#################################### CONSTRUCTOR ###########################################

TdialogMessage::TdialogMessage(Texam *exam, MainWindow *parent, Tnote::EnameStyle style) :
    QWidget(parent->innerWidget),
    m_scoreFree(true),
    m_nameFree(true),
    m_guitarFree(true),
    m_parent(parent)
{


    setAttribute(Qt::WA_TransparentForMouseEvents);
      /** During exams right mouse click is captured by filter on application level,
       * There is some bug that right click over this widget causes seg. fault. */
    QHBoxLayout *lay = new QHBoxLayout;
    m_mainLab = new QLabel(getQuestion(exam->qusetion(exam->count()-1), exam->count(), exam->level(), style), this);
    m_mainLab->setAlignment(Qt::AlignCenter);
    m_mainLab->setWordWrap(true);
    QSize pSize(parent->width(), parent->height());
#if defined(Q_OS_LINUX)
    m_size = QSize(pSize.width() * 0.45, pSize.height() * 0.35);
#else
    m_size = QSize(pSize.width() * 0.4, pSize.height() * 0.3);
#endif
    setPosAndSize(pSize);
    
    QFont f(font());
#if defined(Q_OS_MAC)
    f.setPointSize(m_size.height() * 0.1);
#else
    f.setPointSize((double)m_size.height() * 0.075);
#endif
    m_mainLab->setFont(f);
    m_mainLab->setFixedSize(m_size.width() - 10, m_size.height() - 10);
    
    lay->addStretch(1);
    lay->addWidget(m_mainLab, 0, Qt::AlignCenter);
    lay->addStretch(1);
    setLayout(lay);
    
    connect(parent, SIGNAL(sizeChanged(QSize)), this, SLOT(mainWindowResized(QSize)));
    show();
}

void TdialogMessage::setPosAndSize(QSize &size) {
  if (m_guitarFree)
      m_pos = QPoint(size.width() * 0.4, size.height() * 0.65);
    else
      if (m_nameFree)
        m_pos = QPoint((size.width() / 2), (size.height() /4));
      else // on the score
        m_pos = QPoint(2, size.height() / 10);

#if defined(Q_OS_LINUX)
    m_size = QSize(size.width() * 0.45, size.height() * 0.35);
#else
    m_size = QSize(size.width() * 0.4, size.height() * 0.3);
#endif
    setGeometry(m_pos.x(), m_pos.y(), m_size.width(), m_size.height());
    QFont f(font());
#if defined(Q_OS_MAC)
    f.setPointSize(m_size.height() * 0.1);
#else
    f.setPointSize((double)m_size.height() * 0.075);
#endif
    
    m_mainLab->setFont(f);
    m_mainLab->setFixedSize(m_size.width() - 10, m_size.height() - 10);
}


QString TdialogMessage::getNiceNoteName(Tnote note) {
    return QString("<b><span style=\"%1\">&nbsp;").arg(gl->getBGcolorText(gl->EquestionColor)) +
            TnoteName::noteToRichText(note) + " </span></b>";
}


void TdialogMessage::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    QRect rect = m_mainLab->geometry();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    // shade
    painter.setPen(Qt::NoPen);
    QColor c = palette().text().color();
    c.setAlpha(100);
    painter.setBrush(QBrush(c));
    painter.drawRoundedRect(rect.x() + 7, rect.y() +7, rect.width(), rect.height(), 12, 12);
    // solid background
    painter.setBrush(QBrush(palette().background().color()));
    painter.drawRoundedRect(rect, 12, 12);
    // gradient backround in question color
    c = gl->EquestionColor;
    c.setAlpha(75);
    QLinearGradient grad(rect.topLeft(), rect.bottomRight());
    grad.setColorAt(0.1, c);
    grad.setColorAt(0.9, gl->EquestionColor);
    painter.setBrush(QBrush(grad));
    painter.drawRoundedRect(rect, 12, 12);
    // Question mark
    c = palette().background().color();
    c.setAlpha(100);
    painter.setPen(QPen(c));
    painter.setFont(QFont("nootka", (double)height() / 1.3));
    painter.drawText(rect, Qt::AlignCenter ,"?");
}

QString TdialogMessage::getQuestion(TQAunit &question, int questNr, TexamLevel* level, Tnote::EnameStyle style) {
    QString quest = QString("<b><u>&nbsp;%1.&nbsp;</u></b><br>").arg(questNr);
    QString apendix = "";
    QString noteStr;
    switch (question.questionAs) {
      case TQAtype::e_asNote:
        m_scoreFree = false;
        if (question.answerAs == TQAtype::e_asNote) {
            if (question.qa.note.acidental != question.qa_2.note.acidental)
                quest += tr("Change enharmonicaly and show in the score");
            else
                quest += tr("Given note show in the score");
          if (level->useKeySign && level->manualKey) {
            apendix = tr("<br><b>in %1 key.</b>", "in key signature").arg(getKeyText(question.key));
          }
          if (level->forceAccids)
            quest += getTextHowAccid((Tnote::Eacidentals)question.qa_2.note.acidental);
        } else
          if (question.answerAs == TQAtype::e_asName) {
            m_nameFree = false;
            quest += tr("Give name of");
          } else
            if (question.answerAs == TQAtype::e_asFretPos) {
              m_guitarFree = false;
              quest += tr("Show on the guitar");
              if (level->showStrNr)
                apendix = "<br><b> " + onStringTxt(question.qa.pos.str()) + "</b>";
            } else
              if (question.answerAs == TQAtype::e_asSound) {
                quest += tr("Play or sing");
              }
        if (level->useKeySign && level->manualKey) // hide key signature
            quest += "<br>" + TtipChart::wrapPixToHtml(question.qa.note, true, TkeySignature(0));
        else
            quest += "<br>" + TtipChart::wrapPixToHtml(question.qa.note, true, question.key);
        if (apendix != "")
          quest += apendix;
      break;
      
      case TQAtype::e_asName:
        m_nameFree = false;
        noteStr = "<br>" + getNiceNoteName(question.qa.note);
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
            noteStr = "<br>" + getNiceNoteName(question.qa.note);
            if (question.qa.note.acidental != question.qa_2.note.acidental)
                quest += tr("Change enharmonicaly and give name of");
            else
                quest += tr("Use another style to give name of");
            quest += noteStr + getTextHowAccid((Tnote::Eacidentals)question.qa_2.note.acidental);
            gl->NnameStyleInNoteName = tmpStyle;
            // It is not so elegant to get note name in different style this way
            // but there is no other way
          } else
            if (question.answerAs == TQAtype::e_asFretPos) {
              m_guitarFree = false;
              quest += tr("Show on the guitar") + noteStr;
              if (level->showStrNr)
                quest += "<br><b> " + onStringTxt(question.qa.pos.str()) + "</b>";
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
            apendix = tr("<b>in %1 key.</b>", "in key signature").arg(getKeyText(question.key));
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
        quest += QString("<br><span style=\"font-size: 30px; %1\">&nbsp;").arg(gl->getBGcolorText(gl->EquestionColor)) +
                    TtipChart::wrapPosToHtml(question.qa.pos) + " </span>";
        if (apendix != "")
          quest += "<br>" + apendix;
        if (question.answerAs == TQAtype::e_asNote || question.answerAs == TQAtype::e_asName)
          if (level->forceAccids)
            quest += "<br" + getTextHowAccid((Tnote::Eacidentals)question.qa.note.acidental);
        
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
            quest += getTextHowAccid((Tnote::Eacidentals)question.qa.note.acidental);
        } else
          if (question.answerAs == TQAtype::e_asName) {
            m_nameFree = false;
            quest += tr("Give name of listened sound");
            if (level->forceAccids)
                quest += getTextHowAccid((Tnote::Eacidentals)question.qa.note.acidental);
          } else
            if (question.answerAs == TQAtype::e_asFretPos) {
              m_guitarFree = false;
              quest += tr("Listened sound show on the guitar");
              if (level->showStrNr)
              quest += "<br><b> " + onStringTxt(question.qa.pos.str()) + "</b>";
            } else
              if (question.answerAs == TQAtype::e_asSound) {
                quest += tr("Play or sing listened sound");          
              }
      break;
    }
    
    return quest;
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

void TdialogMessage::mainWindowResized(QSize parentSize) {
  setPosAndSize(parentSize);
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
//Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool | Qt::X11BypassWindowManagerHint
*/
