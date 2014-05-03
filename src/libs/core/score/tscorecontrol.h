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

#ifndef TSCORECONTROL_H
#define TSCORECONTROL_H

#include <nootkacoreglobal.h>
#include <QWidget>

class QVBoxLayout;
class TscoreScene;
class TpushButton;

/** 
 * This class is to control a score. 
 * It contains buttons with accidentals symbols
 * Also it can show button with (#) - to manage extra accidentals
 *  */
class NOOTKACORE_EXPORT TscoreControl : public QWidget
{
    Q_OBJECT

public:
  
    TscoreControl(QWidget *parent = 0);
		
		void enableDoubleAccidentals(bool isEnabled);

public slots:
				/** Checks or unchecks appropriate button. */
    void setAccidental(int accNr);
		
				/** Adds buttons: 
				 * - one to control extra accidentals. 
				 * - one to control names of score notes
				 * It only manages its layout but doesn't preform any changes in a score.
				 * @p extraAccidsChanged and @p showNamesChanged() signals are emitting after click 
				 * - this is way you can handle it. */
		void addExtraButtons();
		
				/** Extra accid button is deleted. */
		void removeExtraButtons();
		
				/** Returns true only when extra accidentals button exist and it is checked */
		bool extraAccidsEnabled();
		
		bool showNamesEnabled();
		
		
				/** Sets size of buttons font */
		void setFontSize(qreal fs);
		qreal fontSize() { return m_fontSize; }
		
signals:
		void accidButtonPressed(int accid);
		void extraAccidsChanged();
		void showNamesChanged();
		
		
protected slots:
		void onAcidButtonPressed();
		void onExtraAccidsButtonPressed();
		void onNameButtonPressed();
    

private:
    TpushButton 								*m_sharpBut, *m_flatBut, *m_dblSharpBut, *m_dblFlatBut; 
		TpushButton 								*m_accidsButt, *m_namesButt;
		qreal												m_fontSize;
		QList<TpushButton*>					m_buttons; /** Keeps pointers for all buttons */
		QVBoxLayout 								*m_butLay;

};

#endif // TSCORECONTROL_H
