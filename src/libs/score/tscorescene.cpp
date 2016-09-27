/***************************************************************************
 *   Copyright (C) 2013-2015 by Tomasz Bojczuk                             *
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

#include "tscorescene.h"
#include "tnotecontrol.h"
#include "trhythmpane.h"
#include "tscoremeter.h"
#include "tscoremeasure.h"
#include <graphics/tdropshadoweffect.h>
#include <tnoofont.h>
#include <music/trhythm.h>
#include <music/tmeter.h>
#include <QtWidgets/qgraphicsview.h>
#include <QtWidgets/qgraphicseffect.h>
#include <QtWidgets/qapplication.h>
#include <QtCore/qtimer.h>
#include <QtCore/qdebug.h>


#define WORK_HIDE_DELAY (5000)
#define REST_Y (19.0)
#define WORK_X (1.2) // x coordinate of the cursor note head


TscoreScene::TscoreScene(QObject* parent) :
  QGraphicsScene(parent),
  m_workPosY(0),
  m_workNote(0),
  m_workAccid(0),
  m_nameColor(Qt::darkCyan),
  m_rightBox(0), m_leftBox(0),
  m_rhythmBox(0),
  m_accidYoffset(0.0),
  m_accidScale(-1.0),
  m_scoreNote(0),
  m_controlledNotes(true),
  m_mouseOverKey(false), m_rectIsChanging(false),
  m_scoreMeter(nullptr)
{
  m_showTimer = new QTimer(this);
  m_hideTimer = new QTimer(this);
  m_workRhythm = new Trhythm(Trhythm::e_none);
  setDoubleAccidsEnabled(true);
  m_currentAccid = 0;

  connect(m_showTimer, SIGNAL(timeout()), this, SLOT(showTimeOut()));
  connect(m_hideTimer, SIGNAL(timeout()), this, SLOT(hideTimeOut()));
}


TscoreScene::~TscoreScene()
{
  if (m_rightBox) { // all items are into scene so they will be deleted
    delete m_rightBox; // but the last TscoreNote has to skip deleting depending items itself
    m_rightBox = 0;
  }
  delete m_workRhythm;
  delete m_scoreMeter;
}


void TscoreScene::setCurrentAccid(char accid) {
  char prevAcc = m_currentAccid;
  m_currentAccid = (char)qBound((int)-m_dblAccFuse, (int)accid, (int)m_dblAccFuse);
  if (m_workAccid && prevAcc != m_currentAccid) {
    m_workAccid->setText(TscoreNote::getAccid(m_currentAccid));
    if (m_currentAccid == 0)
      m_workAccid->hide();
    else
      m_workAccid->show();
    if (m_leftBox)
      m_leftBox->setAccidental(m_currentAccid);
    if (m_hideTimer->isActive())
      m_hideTimer->start(1000);
  }
}


void TscoreScene::setDoubleAccidsEnabled(bool enable) {
  if (enable)
    m_dblAccFuse = 2;
  else
    m_dblAccFuse = 1;
}


void TscoreScene::addBlur(QGraphicsItem* item, qreal radius) {
  QGraphicsBlurEffect *blur = new QGraphicsBlurEffect();
  blur->setBlurRadius(radius / views()[0]->transform().m11());
  item->setGraphicsEffect(blur);
}


void TscoreScene::adjustCursor(TscoreNote* sn) {
  if (m_rightBox && !views().isEmpty()) {
    m_rightBox->adjustSize();
    m_leftBox->adjustSize();
    workLines()->adjustLines(sn);
    setPointedColor(workColor);
  }
}


void TscoreScene::setPointedColor(const QColor& color) {
  workColor = color;
  m_workNote->setColor(color);
//   m_workNote->setPen(QPen(workColor, 0.2));
//   m_workNote->setBrush(QBrush(workColor, Qt::SolidPattern));
  m_workAccid->setBrush(QBrush(workColor));
  m_workLines->setColor(color);
}


void TscoreScene::setScoreMeter(TscoreMeter* m) {
  if (m == nullptr && m_scoreMeter) { // disable rhythms
      m_workRhythm->setRhythm(Trhythm::e_none);
      m_rightBox->setRhythm(m_workRhythm); // hide rhythm symbol on the right pane
      delete m_rhythmBox;
      m_rhythmBox = nullptr;
      m_workNote->setRhythm(*m_workRhythm); // bare note head
  } else if (m && m_scoreMeter == nullptr) { // enable rhythms
      m_workRhythm->setRhythm(Trhythm(m->meter()->lower() == 4 ? Trhythm::e_quarter : Trhythm::e_eighth));
      m_rightBox->setRhythm(m_workRhythm);
      m_rhythmBox = new TrhythmPane(0, this);
      m_workNote->setRhythm(*m_workRhythm);
      connect(m_rightBox, &TnoteControl::rhythmItemClicked, this, &TscoreScene::showRhythmPane, Qt::UniqueConnection);
      connect(m_rhythmBox, &TrhythmPane::rhythmChanged, [=]{
                m_workRhythm->setRhythm(*m_rhythmBox->rhythm());
                m_workNote->setRhythm(*m_rhythmBox->rhythm());
                m_rightBox->setRhythm(m_rhythmBox->rhythm());
      });
  }
  m_scoreMeter = m;
}


//##########################################################################################
//#######################        Note CURSOR     ###########################################
//##########################################################################################

void TscoreScene::noteEntered(TscoreNote* sn) {
  m_hideTimer->stop();
  if (!m_rectIsChanging && sn != m_scoreNote && sn != 0) {
    m_scoreNote = sn;
    if (controlledNotes()) {
      if (right()->isEnabled()) {
        right()->setPos(sn->x() + sn->width(),
                        (sn->parentItem()->boundingRect().height() - right()->boundingRect().height() + 6.0) / 2.0);
        right()->setScoreNote(sn);
      }
      if (left()->isEnabled()) {
        left()->setPos(sn->pos().x() /*- ((7.0 - sn->width()) / 2.0)*/ - left()->boundingRect().width(),
                       (sn->parentItem()->boundingRect().height() - left()->boundingRect().height() + 6.0) / 2.0);
        left()->setScoreNote(sn);
      }
    }
    if (workNote()->parentItem() != sn)
        setCursorParent(sn);
  }
}


void TscoreScene::noteMoved(TscoreNote* sn, int yPos) {
  if (!m_rectIsChanging) {
    setWorkPosY(yPos);
    workNote()->setX(WORK_X);
    if (m_workRhythm->isRest()) {
        workNote()->setY(REST_Y);
    } else {
        m_workRhythm->setStemDown(yPos <= 18);
        workNote()->setRhythm(*m_workRhythm);
        workNote()->setY(workPosY());
        workLines()->checkLines(yPos);
    }
    if (!workNote()->isVisible())
      showTimeOut();
    if (sn != m_scoreNote) {
        noteEntered(sn);
        if (TscoreItem::touchEnabled())
          m_workNote->show(); // show note immediately when touched
        else
          m_showTimer->start(300); // show with delay when mouse over to avoid flickering
    } else {
        m_hideTimer->start(WORK_HIDE_DELAY);
    }
  }
}


void TscoreScene::noteLeaved(TscoreNote* sn) {
  Q_UNUSED(sn)
  if (!m_rectIsChanging) {
    m_showTimer->stop();
    m_hideTimer->start(TscoreItem::touchEnabled() ? 2000 : 300);
  }
}


void TscoreScene::noteDeleted(TscoreNote* sn) {
  if (right() && (workNote()->parentItem() == sn || right()->parentItem() == sn->parentItem())) {
    right()->setScoreNote(0);
    left()->setScoreNote(0);
    setCursorParent(0);
    hideTimeOut();
    statusTipChanged(QString()); // hide status tip of deleting note
  }
  m_scoreNote = 0;
}


void TscoreScene::controlMoved() {
  m_hideTimer->start(WORK_HIDE_DELAY);
}


void TscoreScene::prepareToChangeRect() {
  m_rectIsChanging = true;
  hideTimeOut();
}


void TscoreScene::restoreAfterRectChange() {
  m_rectIsChanging = false;
}


//##########################################################################################
//#######################        PROTECTED       ###########################################
//##########################################################################################

void TscoreScene::initNoteCursor(TscoreNote* scoreNote) {
  if (!m_workNote) {
    m_workLines = new TscoreLines(scoreNote);
    m_workLines->setPos(0.4, 0.0);
    workColor = qApp->palette().highlight().color();
    workColor.setAlpha(200);
    m_workNote = new TnoteItem(this, *m_workRhythm);
    m_workNote->setItAsCursor();
    m_workNote->setParentItem(scoreNote);
    QGraphicsDropShadowEffect *workEffect = new QGraphicsDropShadowEffect();
    workEffect->setOffset(3.0, 3.0);
    workEffect->setBlurRadius(15);
    workEffect->setColor(qApp->palette().text().color());
    m_workNote->setGraphicsEffect(workEffect);
    m_workNote->setZValue(35);
    m_workAccid = new QGraphicsSimpleTextItem();
    m_workAccid->setBrush(QBrush(workColor));
    m_workAccid->setParentItem(m_workNote);
    m_workAccid->setFont(TnooFont(5));
    m_workAccid->setScale(accidScale());
    m_workAccid->setPos(-2.3, - accidYoffset());
    m_workAccid->hide();
    setPointedColor(workColor);

    m_rightBox = new TnoteControl(false, scoreNote->staff(), this);
    m_leftBox = new TnoteControl(true, scoreNote->staff(), this);
    m_leftBox->addAccidentals();
  }
}


void TscoreScene::setCursorParent(TscoreNote* sn) {
  workNote()->setParentItem(sn);
  m_workLines->setParent(sn);
}


//##########################################################################################
//#######################     PROTECTED SLOTS    ###########################################
//##########################################################################################

void TscoreScene::showPanes() {
  if (left()->isEnabled())
    left()->show();
  if (right()->isEnabled()) {
    right()->show();
  }
}


void TscoreScene::hidePanes() {
  if (left()->isEnabled())
    left()->hide();
  if (right()->isEnabled()) {
    right()->hide();
  }
}


void TscoreScene::showRhythmPane() {
  right()->hide();
  if (m_rhythmBox) {
    m_rhythmBox->setParentItem(m_scoreNote->parentItem());
    m_rhythmBox->setCurrentNote(m_scoreNote);
    m_rhythmBox->setPos(right()->x(), (m_scoreNote->boundingRect().height() - m_rhythmBox->boundingRect().height()) / 2.0);
    m_rhythmBox->show();
  }
}



void TscoreScene::showTimeOut() {
  m_showTimer->stop();
  m_workNote->show();
  showPanes();
}


void TscoreScene::hideTimeOut() {
  m_hideTimer->stop();
  if (m_scoreNote)
    m_scoreNote->hideWorkNote();
  hidePanes();
  auto *sn = m_scoreNote;
  m_scoreNote = 0;
  if (TscoreItem::touchEnabled() && sn)
    sn->update();
}













