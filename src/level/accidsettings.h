/***************************************************************************
 *   Copyright (C) 2012-2013 by Tomasz Bojczuk                             *
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


#ifndef ACCIDSETTINGS_H
#define ACCIDSETTINGS_H

#include <QWidget>

class Tlevel;
class TkeySignComboBox;
class QCheckBox;
class QButtonGroup;
class QRadioButton;
class QGroupBox;

class accidSettings : public QWidget
{
  Q_OBJECT
  
public:
    explicit accidSettings(QWidget* parent = 0);
    
    static QString accidsText() { return tr("accidentals"); } // returns accidentals text 
    
    void loadLevel(Tlevel &level);
    void saveLevel(Tlevel &level);
    
signals:
    void accidsChanged();
    
public slots:
      /** Slots invoked when score becomes disable or enable in level*/
    void enableAccids(bool enable);
    void enableKeys(bool enable);
    
private:
    
    QGroupBox *m_accidGr, *m_keySignGr;
    QRadioButton *m_singleKeyRadio, *m_rangeKeysRadio;
    QButtonGroup *m_rangeButGr;
    QCheckBox *m_sharpsChB, *m_flatsChB, *m_doubleAccChB;
    TkeySignComboBox *m_fromKeyCombo, *m_toKeyCombo;
    QCheckBox *m_keyInAnswerChB;
    
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
