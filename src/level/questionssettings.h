/***************************************************************************
 *   Copyright (C) 2011-2012 by Tomasz Bojczuk                             *
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
#include "texamlevel.h"

class QComboBox;
class QCheckBox;
class QButtonGroup;
class QLabel;
class TkeySignComboBox;
class TquestionAsWdg;
class QRadioButton;
class QGroupBox;

/**
 * This class paints 90deg. roteted text as a Label 
 * It supports clean text only, not HTML
*/
class TverticalLabel : public QWidget
{ 
public:
    explicit TverticalLabel(QString text, QWidget* parent = 0);
    QString text() { return m_text; }
    
protected:
    void paintEvent(QPaintEvent* );
    
private:
  QRect m_rect;
  QString m_text;
};
        

/****************************************************************************
 **                         - questionsSettings -                          **
 ***************************************************************************/

class questionsSettings : public QWidget
{
    Q_OBJECT
public:
    explicit questionsSettings(QWidget *parent = 0);

    void loadLevel(TexamLevel &level);
    void saveLevel(TexamLevel &level);

				/** Hides options related to guitar  */
		void hideGuitarRelated();
		
signals:
      /** If any GUI element changes this signal is emited. */
    void questSettChanged();
      /** emited when note in score are diablled. */
    void scoreEnabled(bool);
      /** emited when note in score and names are all disabled or all enabled.  */
    void accidEnabled(bool);
    
public slots:
    void stringsCheckedSlot(bool checked);
    
protected:
      /** Paints lines of a table*/
    void paintEvent(QPaintEvent*);

private:
    QWidget 							*m_tableWdg;
    QLabel 								*m_questLab; // QLabel with 'QUESTION' text
    TverticalLabel 				*m_answLab; // TverticalLabel with 'ANSWER' text
    QLabel 								*m_asSoundLab, *m_asFretLab, *m_soundNooLab, *m_qSoundNooLab;
		QLabel								*m_guitarNooLab, *m_qGuitarNooLab;
    TquestionAsWdg 				*asNoteWdg, *asNameWdg, *asFretPosWdg, *asSoundWdg;
    QCheckBox 						*styleRequiredChB, *octaveRequiredChB, *forceAccChB, *showStrNrChB;
    QCheckBox 						*lowPosOnlyChBox, *currKeySignChBox;
		QComboBox 						*m_intonationCombo;

private slots:
      /** Every element calls this when clicked. */
    void whenParamsChanged();

};


#endif // QUESTIONSSETTINGS_H
