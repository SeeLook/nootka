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
#include <QWidget>
#include <QRadioButton>
#include <QMenu>
#include <tclef.h>


class TradioClef : public QWidget
{
	
public:
		explicit TradioClef(Tclef clef, QWidget* parent = 0, bool isMenu = false);
		
		QRadioButton* radio() { return m_radio; }
		Tclef clef() { return m_clef; }
		void setChecked(bool checked);
		
protected:
    bool event(QEvent *event);
    
private:
		QRadioButton *m_radio;
		Tclef					m_clef;
	
};

//####################################################################################################
class TselectClefPrivate : public QWidget
{
	
public:
		TselectClefPrivate(bool isMenu, QWidget *parent);
		
protected:
    TradioClef 	*treble, *treble_8, *bass, *bass_8, *tenor, *alto, *piano;
    
};


//####################################################################################################
/** 
 * This class is to select a clef (or piano staff)
 */
class TselectClef : public TselectClefPrivate
{
    Q_OBJECT

public:
    TselectClef(QWidget *parent);
		
    ~TselectClef();
		void selectClef(Tclef clef);
    
};

//####################################################################################################
class TclefMenu : public QMenu, public TselectClefPrivate
{
		
public:
		TclefMenu(QWidget* parent = 0);
	
};


#endif // TSELECTCLEF_H
