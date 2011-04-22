/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#ifndef QUESTIONSSETTINGS_H
#define QUESTIONSSETTINGS_H

#include <QtGui>
#include "tquestionaswdg.h"
#include "tkeysigncombobox.h"
#include "texamlevel.h"


class TasNoteWdg : public QWidget
{
    Q_OBJECT
public:
    explicit TasNoteWdg(QWidget *parent = 0);

    void loadLevel(TexamLevel level);

private:
    TquestionAsWdg *asNoteGr;
    QGroupBox *accidGr, *keySignGr;
    QRadioButton *singleKeyRadio, *rangeKeysRadio;
    QButtonGroup *rangeButGr;
    QCheckBox *sharpsChB, *flatsChB, *doubleAccChB;
    TkeySignComboBox *fromKeyCombo, *toKeyCombo;
    QCheckBox *keyInAnswerChB, *forceAccChB;

private slots:
        /** is called when radio button are checked
        * to enable/disable second TkeySignComboBox. */
    void keyRangeChanged();
};



class TasNameWdg : public QWidget
{
    Q_OBJECT
public:
    explicit TasNameWdg(QWidget *parent = 0);

    void loadLevel(TexamLevel level);

private:
    TquestionAsWdg *asNameGr;
    QCheckBox *octaveRequiredChB, *styleRequiredChB;

private slots:
        /** Is called to block styleRequiredChB when note name is
        * either question and answer.*/
    void disableStyleChBox();
};



class TasFretPosWdg : public QWidget
{
    Q_OBJECT
public:
    explicit TasFretPosWdg(QWidget *parent = 0);

    void loadLevel(TexamLevel level);

private:
    TquestionAsWdg *asPosGr;

};

class questionsSettings : public QWidget
{
    Q_OBJECT
public:
    explicit questionsSettings(QWidget *parent = 0);

    void loadLevel(TexamLevel level);

private:
    QToolBox *questAsToolBox;
    TasNoteWdg *asNoteWdg;
    TasNameWdg *asNameWdg;
    TasFretPosWdg *asFretPosWdg;


};



#endif // QUESTIONSSETTINGS_H
