/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
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


#ifndef QUESTIONSSETTINGS_H
#define QUESTIONSSETTINGS_H

#include <QWidget>
#include "tlevel.h"

class QComboBox;
class QCheckBox;
class QButtonGroup;
class QLabel;
class TkeySignComboBox;
class TquestionAsWdg;
class QRadioButton;
class QGroupBox;


class questionsSettings : public QWidget
{
    Q_OBJECT
public:
    explicit questionsSettings(QWidget *parent = 0);

    void loadLevel(Tlevel &level);
    void saveLevel(Tlevel &level);

				/** Hides options related to guitar  */
		void hideGuitarRelated();
		
signals:
    void questSettChanged(); /** If any GUI element changes this signal is emitted. */
    void scoreEnabled(bool); /** emitted when note in score are disabled. */
    void accidEnabled(bool); /** emitted when note in score and names are all disabled or all enabled.  */
    
public slots:
    void stringsCheckedSlot(bool checked);
    
protected:
    void paintEvent(QPaintEvent*); /** Paints lines of a table*/

private:
    QWidget 							*m_tableWdg;
    QLabel 								*m_questLab; // QLabel with 'QUESTION' text
    QLabel				 				*m_answLab; // fake QLabel to keep space
    QLabel 								*m_asSoundLab, *m_asFretLab, *m_soundNooLab, *m_qSoundNooLab;
		QLabel								*m_guitarNooLab, *m_qGuitarNooLab;
    TquestionAsWdg 				*asNoteWdg, *asNameWdg, *asFretPosWdg, *asSoundWdg;
    QCheckBox 						*styleRequiredChB, *octaveRequiredChB, *forceAccChB, *showStrNrChB;
    QCheckBox 						*lowPosOnlyChBox, *currKeySignChBox;
		QComboBox 						*m_intonationCombo;
		bool 									 m_levelIsLoading;

private slots:
    void whenParamsChanged(); /** Every element calls this when clicked. */

};


#endif // QUESTIONSSETTINGS_H
