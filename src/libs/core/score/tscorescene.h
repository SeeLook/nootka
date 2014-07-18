/***************************************************************************
 *   Copyright (C) 2013-2014 by Tomasz Bojczuk                             *
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

#ifndef TSCORESCENE_H
#define TSCORESCENE_H

#include <nootkacoreglobal.h>
#include <QGraphicsScene>
#include "tscorenote.h"

class TnoteControl;
class QGraphicsItem;

class NOOTKACORE_EXPORT TscoreScene : public QGraphicsScene
{
		
		friend class TscoreNote;
	
  Q_OBJECT
  
public:
    TscoreScene(QObject* parent = 0);
    virtual ~TscoreScene();
        
        
    void setDoubleAccidsEnabled(bool enable);
		
        /** Returns value 2 when double accidentals are enabled and 1 if not. */
    qint8 doubleAccidsFuse() { return m_dblAccFuse; }
    
        
    void setCurrentAccid(char accid); /** Working accidental in  also changed by buttons. */
    char currentAccid() { return m_currentAccid; }
    
        /** Adds blur graphics effect. In the contrary to QGraphicsItem::setGraphicsEffect() 
         * a radius value in global scale.  */
    void addBlur(QGraphicsItem *item, qreal radius);
		
				/** Adjust note cursor and TnoteControl to new staff size. 
				 * For performance reason it has to be called once for all adjustSize() of TscoreNote
				 * because there is only one instance of note cursor and TnoteControl */
		void adjustCursor();
		bool isCursorVisible() { return m_workNote->isVisible(); }
		
				/** Note controllers, appear with cursor. 
				 * There are automatically created with first note instance 
				 * when score scene has a view. */
		TnoteControl* right() { return m_rightBox; }
		TnoteControl* left() { return m_leftBox; }
		void setNameColor(const QColor& nameC) { m_nameColor = nameC; }
		QColor nameColor() { return m_nameColor; }
		
				/** Sets color of pointing (work) note. */
    void setPointedColor(QColor color);
		
		qreal accidYoffset() { return m_accidYoffset; } /** Y offset of accidental item */
		qreal accidScale() { return m_accidScale; } /** Scale of accidental text item */
		
signals:
    void statusTip(QString);
		
protected:
	// note cursor
		TaddLines	upLines, downLines, midLines;
		QColor  workColor;
		int workPosY() { return m_workPosY; }
		void setWorkPosY(int wpY) { m_workPosY = wpY; }
		QGraphicsEllipseItem* workNote() { return m_workNote; }
		void initNoteCursor(TscoreItem* parentIt);
		QGraphicsSimpleTextItem* workAccid() { return m_workAccid; }
		void setAccidYoffset(qreal aYo) { m_accidYoffset = aYo; }
		void setAccidScale(qreal as) { m_accidScale = as; }
		
    
protected slots:
    void statusTipChanged(QString status) { emit statusTip(status); }
    
private:
        /** It is @p 2 if double accidentals are enabled and @p 1 if not*/
    qint8 														m_dblAccFuse;
    char  														m_currentAccid;
	// note cursor
		int                            		m_workPosY;
		QGraphicsEllipseItem          	 *m_workNote;
		QGraphicsSimpleTextItem       	 *m_workAccid;
		QColor														m_nameColor;
		TnoteControl				  				 	 *m_rightBox, *m_leftBox;
		qreal 									 					m_accidYoffset; /** difference between y note position. */
    qreal									 						m_accidScale;
		

};

#endif // TSCORESCENE_H
