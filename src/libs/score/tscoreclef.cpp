/***************************************************************************
 *   Copyright (C) 2013-2016 by Tomasz Bojczuk                             *
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

#include "tscoreclef.h"
#include "tscorescene.h"
#include "tscorestaff.h"
#include <music/tclef.h>
#include <tnoofont.h>
#if defined (Q_OS_ANDROID)
  #include <touch/ttouchproxy.h>
  #include <touch/ttouchparams.h>
  #include <ttouchmessage.h>
#endif
#include <QtWidgets/qgraphicssceneevent.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qdesktopwidget.h>
#include <QtGui/qpalette.h>
#include <QtWidgets/qgraphicsview.h>

#include <QDebug>

/*static*/
QChar TscoreClef::clefToChar(Tclef clef) {
  QChar ch;
  switch(clef.type()) {
    case Tclef::e_none:
      ch = QChar(0); break;
    case Tclef::e_treble_G:
      ch = QChar(0xe172); break;
    case Tclef::e_bass_F:
      ch = QChar(0xe170); break;
    case Tclef::e_alto_C:
      ch = QChar(0xe16e); break;
    case Tclef::e_treble_G_8down:
      ch = QChar(0xe173); break;
    case Tclef::e_bass_F_8down:
      ch = QChar(0xe171); break;
    case Tclef::e_tenor_C:
      ch = QChar(0xe16e); break;
    default :
      ch = QChar(0x20); break; // space
  }
  return ch;
}

QList<Tclef::Etype> TscoreClef::m_typesList = QList<Tclef::Etype>();



TscoreClef::TscoreClef(TscoreScene* scene, TscoreStaff* staff, Tclef clef) :
  TscoreItem(scene),
  m_clef(Tclef(Tclef::e_none)),
  m_textClef(0),
  m_readOnly(false)
{
  setStaff(staff);
  setParentItem(staff);
  setFlag(QGraphicsItem::ItemHasNoContents);
  if (m_typesList.size() == 0) // initialize types list
    m_typesList << Tclef::e_treble_G << Tclef::e_bass_F << Tclef::e_bass_F_8down <<
    Tclef::e_alto_C << Tclef::e_tenor_C << Tclef::e_treble_G_8down;

  m_textClef = new QGraphicsSimpleTextItem();
  registryItem(m_textClef);
  m_textClef->setBrush(qApp->palette().text().color());

  m_textClef->setFont(TnooFont(18));
  setClef(clef);
}


TscoreClef::~TscoreClef() {
//   if (m_clefMenu)
//     delete m_clefMenu;
}


void TscoreClef::setClef(Tclef clef) {
  if (clef.type() == Tclef::e_pianoStaff) {
    if (!m_lowerClef) {
      m_lowerClef = new TscoreClef(scoreScene(), staff(), Tclef(Tclef::e_bass_F));
      m_lowerClef->setPos(0.5, getYclefPos(m_lowerClef->clef()) - (16.0 - staff()->lowerLinePos()) + 0.1);
      connect(m_lowerClef, SIGNAL(clefChanged(Tclef)), this, SLOT(lowerClefChanged(Tclef)));
    } else // clefs already set to piano mode
        return;
    clef.setClef(Tclef::e_treble_G);
  } else {
    if (m_lowerClef)
      delete m_lowerClef;
  }
      m_clef = clef;
      m_currClefInList = getClefPosInList(m_clef);
      m_textClef->setText(QString(clefToChar(m_clef.type())));
      qreal fineOff = 0.1;
      if (clef.type() == Tclef::e_bass_F || clef.type() == Tclef::e_bass_F_8down)
        fineOff = 0.0;
      setPos(0.5, getYclefPos(m_clef) - (16.0 - staff()->upperLinePos()) + fineOff);
      getStatusTip();
}


QRectF TscoreClef::boundingRect() const {
      return QRectF(0, 0, 6, 18); // optimal height for all clef types
}

void TscoreClef::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  Q_UNUSED(painter)
  Q_UNUSED(option)
  Q_UNUSED(widget)
//   paintBackground(painter, Qt::magenta);
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TscoreClef::touched(const QPointF& scenePos) {
  Q_UNUSED(scenePos)
  m_tapTimer.start();
#if defined (Q_OS_ANDROID)
  if (!TtouchParams::i()->clefWasTouched && !tMessage->isVisible() && tMessage->mainWindowOnTop()) {
    tMessage->setMessage(TtouchProxy::touchClefHelp(), 0);
    TtouchParams::i()->clefWasTouched = true;
  }
#endif
}


void TscoreClef::untouched(const QPointF& scenePos) {
  if (!scenePos.isNull() && m_tapTimer.hasExpired(300)) {
    QGraphicsSceneMouseEvent me(QEvent::MouseButtonPress);
    me.setPos(mapFromScene(scenePos));
    me.setButton(Qt::LeftButton);
    mousePressEvent(&me);
  }
}


void TscoreClef::mousePressEvent(QGraphicsSceneMouseEvent* event) {
/*
  if (m_readOnly) {
    TscoreItem::mousePressEvent(event);
  } else {
    if (!m_menu) {
      m_menu = new QMenu();
      if (!m_clefMenu) {
          m_clefMenu = new TclefMenu(m_menu);
          connect(m_clefMenu, SIGNAL(statusTipRequired(QString)), this, SLOT(clefMenuStatusTip(QString)));
      } else
          m_clefMenu->setMenu(m_menu);
      Tclef curClef = m_clef;
      if (staff()->isPianoStaff())
        curClef = Tclef(Tclef::e_pianoStaff);
      m_clefMenu->selectClef(curClef);
      #if defined (Q_OS_ANDROID)
        Tclef cl = m_clefMenu->exec(QPoint(qApp->desktop()->availableGeometry().width() / 8, 10));
      #else
        Tclef cl = m_clefMenu->exec(QCursor::pos());
      #endif
      m_clefMenu->setMenu(0);
      delete m_menu;
      if (cl.type() == Tclef::e_none)
        return;
      if (curClef.type() != cl.type()) {
        emit clefChanged(cl);
      }
    }
  }
  */
}


void TscoreClef::lowerClefChanged(Tclef clef) {
  if (clef.type() != Tclef::e_pianoStaff) // lower staff means piano staff already
    emit clefChanged(clef);
}

//##########################################################################################################
//########################################## PRIVATE     ###################################################
//##########################################################################################################

void TscoreClef::getStatusTip() {
  QString tip = "<b>" + m_clef.name() + "</b>  (" + m_clef.desc() + ")";
  if (!readOnly())
    tip += "<br>" + tr("Click to select another clef.");
  setStatusTip(tip);
}



int TscoreClef::getYclefPos(Tclef clef) {
  int pos = 0;
  if (clef.type() == Tclef::e_treble_G || clef.type() == Tclef::e_treble_G_8down)
    pos = 11;
  else if (clef.type() == Tclef::e_bass_F|| clef.type() == Tclef::e_bass_F_8down)
    pos = 11;
  else if (clef.type() == Tclef::e_alto_C)
    pos = 11;
  else if (clef.type() == Tclef::e_tenor_C)
    pos = 9;
  return pos;
}


int TscoreClef::getClefPosInList(Tclef clef) {
  for (int i = 0; i < m_typesList.size(); i++)
    if (m_typesList[i] == clef.type()) {
      return i;
    }
  qDebug() << "getClefPosInList(): no clef was found";
  return 0;
}




