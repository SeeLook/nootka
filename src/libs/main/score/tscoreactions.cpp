/***************************************************************************
 *   Copyright (C) 2014-2018 by Tomasz Bojczuk                             *
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


#include "tscoreactions.h"
#include "tmainscore.h"
#include <widgets/tpushbutton.h>
#include <graphics/tnotepixmap.h>
#include <tpath.h>
#include <gui/tmenu.h>
#include <QtWidgets>

TscoreActions::TscoreActions(TmainScore* score) :
	QObject(score),
	m_score(score)
{
  m_menu = new Tmenu();
  m_button = new QToolButton(score);
  m_button->setIcon(QIcon(Tpath::img("score")));
  m_button->setText(tr("Score", "it could be 'notation', 'staff' or whatever is associated with that 'place to display musical notes' and this the name is quite short and looks well."));
  m_button->setStatusTip(tr("Manage and navigate the score"));
  m_button->setPopupMode(QToolButton::InstantPopup);
  m_button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  
  m_mainAction = new QWidgetAction(score);
  m_mainAction->setDefaultWidget(m_button);
  
  m_accidsAct = createAction(tr("Additional accidentals"), SLOT(extraAccidsSlot()));
  m_accidsAct->setStatusTip(tr("Shows accidentals from the key signature also next to a note. <b>WARING! It never occurs in real scores - use it only for theoretical purposes.</b>"));
  m_accidsAct->setCheckable(true);
  m_namesAct = createAction(tr("Show note names"), SLOT(showNamesSlot()));
  m_namesAct->setStatusTip(tr("Shows names of all notes on the staff."));
  m_namesAct->setCheckable(true);
  m_menu->addSeparator();
  
  m_outZoom = createAction(tr("Zoom score out"), SLOT(zoomScoreSlot()), QKeySequence::ZoomOut, QIcon(Tpath::img("zoom-out")));
  m_inZoom = createAction(tr("Zoom score in"), SLOT(zoomScoreSlot()), QKeySequence(), QIcon(Tpath::img("zoom-in")));
#if !defined (Q_OS_ANDROID)
  m_inZoom->setShortcuts(QKeySequence::keyBindings(QKeySequence::ZoomIn));
#endif
  m_menu->addSeparator();
  m_prevNote = createAction(tr("Previous note"), SLOT(moveSelectedNote()), QKeySequence(Qt::Key_Left), 
                             QIcon(score->style()->standardIcon(QStyle::SP_ArrowBack)));
  m_firstNote = createAction(tr("First note"), SLOT(moveSelectedNote()), QKeySequence(Qt::Key_Home), 
                             QIcon(score->style()->standardIcon(QStyle::SP_MediaSkipBackward)));
  m_staffUp = createAction(tr("Staff above"), SLOT(moveSelectedNote()), QKeySequence(Qt::Key_PageUp), 
                             QIcon(score->style()->standardIcon(QStyle::SP_ArrowUp)));
  m_staffDown = createAction(tr("Staff below"), SLOT(moveSelectedNote()), QKeySequence(Qt::Key_PageDown), 
                             QIcon(score->style()->standardIcon(QStyle::SP_ArrowDown)));
  m_lastNote = createAction(tr("Last note"), SLOT(moveSelectedNote()), QKeySequence(Qt::Key_End), 
                             QIcon(score->style()->standardIcon(QStyle::SP_MediaSkipForward)));
  m_nextNote = createAction(tr("Next note"), SLOT(moveSelectedNote()), QKeySequence(Qt::Key_Right), 
                             QIcon(score->style()->standardIcon(QStyle::SP_ArrowForward)));
  QFont nf("nootka", m_menu->style()->pixelMetric(QStyle::PM_SmallIconSize));
  m_delCurrNote = createAction(tr("Delete note"), SLOT(removeCurrentNote()), QKeySequence(Qt::Key_Delete), 
                             QIcon(pixFromString("o", nf, Qt::red)));
  m_menu->addSeparator();
	
  m_clear = createAction(tr("Delete all notes"), SLOT(deleteNotes()), QKeySequence("Shift+DEL"), QIcon(Tpath::img("clear-score")));
  
  m_button->setMenu(m_menu);
}


TscoreActions::~TscoreActions()
{
  delete m_menu;
}


void TscoreActions::disableActions(bool dis) {
	if (dis != m_firstNote->signalsBlocked()) { // disability of actions is different than required, so do it
		foreach(QAction* a, m_actions)
			a->blockSignals(dis);
	}
}


void TscoreActions::setForExam(bool isExam) {
  m_accidsAct->setVisible(!isExam);
  m_namesAct->setVisible(!isExam);
}


//####################################################################################################
//########################################## PRIVATE #################################################
//####################################################################################################

QAction* TscoreActions::createAction(const QString& t, const char* slot, const QKeySequence& k, const QIcon& i) {
  QAction *a = new QAction(this);
  if (!i.isNull())
    a->setIcon(i);
#if !defined (Q_OS_ANDROID)
  if (!k.isEmpty())
    a->setShortcut(k);
#endif
  a->setText(t);
  connect(a, SIGNAL(triggered()), m_score, slot);
  m_menu->addAction(a);
  return a;
}









