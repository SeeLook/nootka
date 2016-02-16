/***************************************************************************
 *   Copyright (C) 2012-2014 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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


#ifndef ACCIDSETTINGS_H
#define ACCIDSETTINGS_H


#include "tabstractlevelpage.h"


class TlevelCreatorDlg;
class Tlevel;
class TkeySignComboBox;
class QCheckBox;
class QButtonGroup;
class QRadioButton;
class QGroupBox;

class accidSettings : public TabstractLevelPage
{
  Q_OBJECT
  
public:
    explicit accidSettings(TlevelCreatorDlg* creator);
    
    static QString accidsText() { return tr("accidentals"); } // returns accidentals text 
    
    virtual void loadLevel(Tlevel *level);
    virtual void saveLevel(Tlevel *level);
		
    
signals:
    void accidsChanged();
    
public slots:
	
		virtual void changed();
		
      /** Slots invoked when score becomes disable or enable in level*/
    void enableAccids(bool enable);
    void enableKeys(bool enable);
    
private:
    
    QGroupBox *m_accidGr, *m_keySignGr;
    QRadioButton *m_singleKeyRadio, *m_rangeKeysRadio;
    QButtonGroup *m_rangeButGr;
    QCheckBox *m_sharpsChB, *m_flatsChB, *m_doubleAccChB, *m_forceAccChB;
    TkeySignComboBox *m_fromKeyCombo, *m_toKeyCombo;
    QCheckBox *m_keyInAnswerChB, *m_currKeySignChBox;
    
private slots:
        /** is called when radio button are checked
        * to enable/disable second TkeySignComboBox. */
    void keyRangeChanged();
		
        /** Is called when user changes any parameter.
            It emits asNoteChanged() signal. */
    void whenParamsChanged();
		
        /** It is called when user changes TkeySignComboBox-es
        * and goal is to check sharps or flats, otherwise exam level
        * has no sense. */
    void keySignChanged();
		
		void updateStatusTip();
};

#endif // ACCIDSETTINGS_H
