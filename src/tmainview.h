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

#ifndef TMAINVIEW_H
#define TMAINVIEW_H

#include <QGraphicsView>

class QGraphicsProxyWidget;

/** 
 * This widget manages of a layout of main Nootka window
 * All widgets are wrapped with QGraphicsProxyWidget 
 * and their layout is managed QGraphicsLayout
 */
class TmainView : public QGraphicsView
{
	
	Q_OBJECT
 
public:
    TmainView(QWidget* toolW, QWidget* statLabW, QWidget* pitchW, QWidget* scoreW, QWidget* guitarW, QWidget* parent = 0);
		
signals:
		void statusTip(const QString&);
		
protected:
    virtual void resizeEvent(QResizeEvent* event);
    virtual bool eventFilter(QObject* ob, QEvent* event);
		
private:
		QGraphicsProxyWidget				*m_tool, *m_status, *m_pitch, *m_score, *m_guitar;
		QGraphicsWidget							*m_form;
};

#endif // TMAINVIEW_H
