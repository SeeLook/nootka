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

#ifndef TSCOREPIANOSTAFF_H
#define TSCOREPIANOSTAFF_H

#include <tscorestaff.h>

class TscorePianoStaff : public TscoreStaff
{
    Q_OBJECT

public:
    TscorePianoStaff(TscoreScene *scene, int notesNr);
    ~TscorePianoStaff();
		
		void setEnableKeySign(bool isEnabled);
		void setScoreControler(TscoreControl *scoreControl);
		
		virtual QRectF boundingRect() const;
		
		
protected slots:
				/** This slots ties key signatutes in two staves. */
		void upperStaffChangedKey();
		void lowerStaffChangedKey();
				/** */
		void upperNoteChanged(int noteIndex);
		void lowerNoteChanged(int noteIndex);
				
private:
		TscoreStaff 		*m_lower; // grand (left hand) staff

};

#endif // TSCOREPIANOSTAFF_H
