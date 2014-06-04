/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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


#include "tscoreactions.h"
#include "tmainscore.h"
#include <widgets/tpushbutton.h>
#include <QAction>
#include <QStyle>


TscoreActions::TscoreActions(TmainScore* sc, const QString& path) :
	QObject(sc)
{
	m_outZoom = new QAction(QIcon(path + "/picts/zoom-out.png"), "", this);
		m_outZoom->setStatusTip(tr("Zoom score out"));
// 		m_outZoom->setShortcut(QKeySequence(QKeySequence::ZoomOut));
// 		m_outZoom->setShortcutContext(Qt::ApplicationShortcut);
		connect(m_outZoom, SIGNAL(triggered()), sc, SLOT(zoomScoreSlot()));
	m_inZoom = new QAction(QIcon(path + "/picts/zoom-in.png"), "", this);
		m_inZoom->setStatusTip(tr("Zoom score in"));
		connect(m_inZoom, SIGNAL(triggered()), sc, SLOT(zoomScoreSlot()));
	m_firstNote = new QAction(QIcon(sc->style()->standardIcon(QStyle::SP_ArrowBack)), "", this);
		m_firstNote->setStatusTip(tr("Go to the first note (Home)"));
		connect(m_firstNote, SIGNAL(triggered()), sc, SLOT(moveSelectedNote()));
	m_staffUp = new QAction(QIcon(sc->style()->standardIcon(QStyle::SP_ArrowUp)), "", this);
		m_staffUp->setStatusTip(tr("Go to the staff above"));
		connect(m_staffUp, SIGNAL(triggered()), sc, SLOT(moveSelectedNote()));
	m_staffDown = new QAction(QIcon(sc->style()->standardIcon(QStyle::SP_ArrowDown)), "", this);
		m_staffDown->setStatusTip(tr("Go to the staff below"));
		connect(m_staffDown, SIGNAL(triggered()), sc, SLOT(moveSelectedNote()));
	m_lastNote = new QAction(QIcon(sc->style()->standardIcon(QStyle::SP_ArrowForward)), "", this);
		m_lastNote->setStatusTip(tr("Go to the last note (End)"));
		connect(m_lastNote, SIGNAL(triggered()), sc, SLOT(moveSelectedNote()));
	
	m_clear = new QAction(QIcon(path + "picts/clear-score.png"), "", this);
	m_clear->setStatusTip(tr("Delete all notes on the score"));
	connect(m_clear, SIGNAL(triggered()), sc, SLOT(deleteNotes()));
	
	m_accidsButt = new TpushButton("$", sc); // (#)
		m_accidsButt->setStatusTip(tr("Shows accidentals from the key signature also next to a note. <b>WARRING! It never occurs in real scores - use it only for theoretical purposes.</b>"));
		m_accidsButt->setThisColors(Qt::red, sc->palette().highlightedText().color());
	m_namesButt = new TpushButton("c", sc);
		m_namesButt->setStatusTip(tr("Shows names of all notes on the score"));
		m_namesButt->setThisColors(Qt::darkCyan, sc->palette().highlightedText().color());
	QList<TpushButton*> buttons;
	buttons << m_accidsButt << m_namesButt;
	for (int i = 0; i < buttons.size(); i++) {
		QFont nf("nootka");
		nf.setPointSizeF(24.0);
		QFontMetrics fm(nf);
		nf.setPointSizeF(qRound(nf.pointSizeF() * (nf.pointSizeF() / fm.boundingRect("x").height())));
		buttons[i]->setFont(nf);
		fm = QFontMetrics(nf);
		buttons[i]->setMaximumWidth(fm.boundingRect("xx").width());
	}
	connect(m_accidsButt, SIGNAL(clicked()), sc, SLOT(extraAccidsSlot()));
	connect(m_namesButt, SIGNAL(clicked()), sc, SLOT(showNamesSlot()));

		
}
