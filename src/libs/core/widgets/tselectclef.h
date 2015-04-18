/***************************************************************************
 *   Copyright (C) 2013-2015 by Tomasz Bojczuk                             *
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

#include <nootkacoreglobal.h>
#include <QObject>
#include <QWidget>
#include <QRadioButton>
#include <QMenu>
#include <music/tclef.h>

/** 
 * Classes below are used to switching a clef.
 * @class TradioClef is like menu entry
 * @class TselectClefPrivate is base class for selecting a clef
 * @class TclefMenu is context menu with clefs list
 * @class TselectClef is a QWidget with clefs to select 
 */



//######################################################################################
/** 
 * This is a QRadioButton, clef pixmap and name.
 * It is used as a QMenu entry or in some settings widgets
 */
class NOOTKACORE_EXPORT TradioClef : public QWidget
{
		Q_OBJECT
public:
		explicit TradioClef(Tclef clef, QWidget* parent = 0, bool isMenu = false);
		
		QRadioButton* radio() { return m_radio; }
		Tclef clef() { return m_clef; }
		void setChecked(bool checked);
		
signals:
		void selectedClef(Tclef);
		void statusTipWanted(QString);
		
protected:
				/** Event handler for capturing mouse - it manages hihgtlight */
		virtual bool event(QEvent* event);
		virtual void paintEvent(QPaintEvent* event);
		
protected slots:
		void clefClickedSlot();
    
private:
		QRadioButton *m_radio;
		Tclef					m_clef;
		bool 					m_hasMouseOver;
	
};

//####################################################################################################
/** 
 * It keeps all TradioClef-s objects and selects one of them by @p selectClef(Tclef)
 * Virtual method clefWasSelected has to be imlpemented in derived classes. 
 */
class NOOTKACORE_EXPORT TselectClefPrivate : public QWidget
{
	Q_OBJECT
public:
		TselectClefPrivate(bool isMenu, QWidget *parent);
		
		void selectClef(Tclef clef);
		
signals:
		void statusTipRequired(QString);

		
protected:
    TradioClef 	*treble, *treble_8, *bass, *bass_8, *tenor, *alto, *piano;
		
protected slots:
        virtual void clefWasSelected(Tclef clef) {}
		void onStatusTip(QString tip) { emit statusTipRequired(tip); }
    
		public slots:
		
};


//####################################################################################################
/** 
 * This class is a QWidget to select a clef (or piano staff)
 */
class NOOTKACORE_EXPORT TselectClef : public TselectClefPrivate
{
    Q_OBJECT
public:
    TselectClef(QWidget *parent);
		Tclef selectedClef();
		
signals:
		void clefSelected(Tclef);
		
protected slots:
		virtual void clefWasSelected(Tclef clef);
    
};

//####################################################################################################
/**
 * Menu (context) to select a clef.
 */
class NOOTKACORE_EXPORT TclefMenu : public TselectClefPrivate
{
		Q_OBJECT
public:
  TclefMenu(QMenu* parent = 0);

  void setMenu(QMenu* menuParent); /** Changes QMenu parent to given menu */

  Tclef exec(QPoint pos);

protected:
  virtual bool eventFilter(QObject* ob, QEvent* e);

protected slots:
  virtual void clefWasSelected(Tclef clef);

private:
  Tclef m_curentClef;
  QMenu *m_menu;

	
};


#endif // TSELECTCLEF_H
