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
 

#ifndef TSIMPLESCORE_H
#define TSIMPLESCORE_H

#include <QWidget>
#include "tclef.h"
#include "tnote.h"
#include "tkeysignature.h"

class QGraphicsView;
class TscoreControl;
class TscoreStaff;
class TscoreScene;


/** This class implements score.  */
class TsimpleScore : public QWidget
{
  Q_OBJECT
  
public:
    TsimpleScore(int notesNumber, QWidget *parent = 0, bool controler = true);
    ~TsimpleScore();

		void setNote(int index, Tnote note);
        /** It returns @p note with hope that index points existed Tnote element. */
    Tnote getNote(int index);
        /** It hides pionted note and sets to Tnote(0,0,0)*/
    void clearNote(int index);		
		

				/** This method returns the key signature if score has got TscoreKeySign item, 
				 * key can be:
         * @li 0 - C-maj (a-min)
         * @li 1 - G-maj and so on up to 7 (for Cis-maj)
         * @li -1 - F-maj (d-min) and so on down to -7 (for Ges-maj)    
				 * When TscoreKeySign doesn't exist 0: - a-min/C-maj is returned. */
    TkeySignature keySignature();
    void setKeySignature(TkeySignature keySign);
		void setEnableKeySign(bool isEnabled);		
		
				/** Sets TsimpleScore to piano staff view. Clefs are readOnly. */
		void setPianoStaff(bool isPiano);
		bool isPianoStaff() { return m_isPianoStaff; }
		
				/** Sets background color. Alpha value will be overriden. */
    void setBGcolor(QColor bgColor);
		
		
signals:
				/** As long as QGraphicsScene items haven't got status tips TscoreItems has its own mechanism of tips.
				 * This signal is emited when any TscoreScene element gots hoverEnterEvent 
				 * with status tip to display. */
    void statusTip(QString);
		void noteHasChanged(int index, Tnote note); // TODO change this name - clamsy english
		
public slots:
		void noteWasClicked(int index);
		
protected:
		QGraphicsView* score() { return m_score; }
		TscoreScene* scene() { return m_scene; }
		TscoreStaff* staff() { return m_staff; }
    
    
protected slots:
    void statusTipChanged(QString status) { emit statusTip(status); }
    void resizeEvent(QResizeEvent* event);
				/** This is response for user demand to chenge to or from piano staff. */
		void switchToPianoStaff(Tclef clef);
  
private:
    TscoreScene     *m_scene;
    TscoreStaff     *m_staff, *m_lowerStaff;
    TscoreControl   *m_scoreControl;
    QGraphicsView   *m_score;
		bool 						m_isPianoStaff;
		int 						m_notesNr;
		qreal 					m_pianoFactor; // factor of a score size depends on is it piano staff(smaller) or not
  
};

#endif // TSIMPLESCORE_H
