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
		
		virtual void setNote(int index, Tnote &note);
		
		virtual void setEnableKeySign(bool isEnabled);
		
		virtual void setScoreControler(TscoreControl *scoreControl);
				
		virtual void setDisabled(bool disabled);
		
		virtual void setScordature(Ttune &tune);
		virtual bool hasScordature() { return m_lower->hasScordature(); } /** @p TRUE when staff has got scordature. */
		
		virtual QRectF boundingRect() const;
		
		
protected slots:
				/** Those slots ties key signatutes in two staves. */
		void upperStaffChangedKey();
		void lowerStaffChangedKey();
				/** It has doubled noteSegments (for each staff).
         * Those slots manage this. */
		void upperNoteChanged(int noteIndex);
		void lowerNoteChanged(int noteIndex);
        /** Takes care about signal clefChanged from lower staff. */
    void lowerStaffClefChanged(Tclef clef);
				
private:
		TscoreStaff 		*m_lower; // grand (left hand) staff

};

#endif // TSCOREPIANOSTAFF_H
