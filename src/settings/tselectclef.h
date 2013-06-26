/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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

#ifndef TSELECTCLEF_H
#define TSELECTCLEF_H

#include <QObject>
#include <tclef.h>

class QToolBar;
class QAction;
class QMenu;


/** 
 * This class is to select a clef (or piano staff)
 * It can be like context menu or like QToolBar depends on constructor parameter.
 */
class TselectClef : public QObject
{
    Q_OBJECT

public:
    TselectClef(QMenu *parent);
    TselectClef(QToolBar *parent);
		
    ~TselectClef();
		
		
		void selectClef(Tclef clef);

private:
    void createActions();
		QAction* getAction(Tclef clef);
    
private:
		QObject 	*m_parent; // QMenu or QToolBar
    QAction 	*m_trebleAct, *m_treble_8Act, *m_bassAct, *m_bass_8Act, *m_tenorAct, *m_altoAct, *m_pianoAct;
		QMenu 		*m_menu;
		QToolBar 	*m_toolBar;

};

#endif // TSELECTCLEF_H
