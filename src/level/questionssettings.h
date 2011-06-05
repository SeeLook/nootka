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
    void saveLevel(TexamLevel &level);

signals:
    void asNoteChanged();


private:
    TquestionAsWdg *asNoteGr;
    QGroupBox *accidGr, *keySignGr;
    QRadioButton *singleKeyRadio, *rangeKeysRadio;
    QButtonGroup *rangeButGr;
    QCheckBox *sharpsChB, *flatsChB, *doubleAccChB;
    TkeySignComboBox *fromKeyCombo, *toKeyCombo;
    QCheckBox *keyInAnswerChB;

private slots:
        /** is called when radio button are checked
        * to enable/disable second TkeySignComboBox. */
    void keyRangeChanged();
        /** Is called when user changes any parameter.
            It emits asNoteChanged() signal. */
    void whenParamsChanged();
        /** It is called when user changes TkeySignComboBox-es
        * and goal is to check sharps or flats, otherwiese exam level
        * has no sense.*/
    void keySignChanged();
};



class TasNameWdg : public QWidget
{
    Q_OBJECT
public:
    explicit TasNameWdg(QWidget *parent = 0);

    void loadLevel(TexamLevel level);
    void saveLevel(TexamLevel &level);

signals:
    void asNameChanged();

private:
    TquestionAsWdg *asNameGr;
    QCheckBox *octaveRequiredChB, *styleRequiredChB;

private slots:
        /** Is called to block styleRequiredChB when note name is
            either question and answer.*/
    void disableStyleChBox();
        /** Is called when user changes any parameter.
            It emits asNameChanged signal. */
    void whenParamsChanged();

};



class TasFretPosWdg : public QWidget
{
    Q_OBJECT
public:
    explicit TasFretPosWdg(QWidget *parent = 0);

    void loadLevel(TexamLevel level);
    void saveLevel(TexamLevel &level);

signals:
    void asFretPosChanged();

private:
    TquestionAsWdg *asPosGr;
    QCheckBox *forceAccChB;

private slots:
        /** Is called when user changes any parameter.
            It emits asNameChanged signal. */
    void whenParamsChanged();

};

class questionsSettings : public QWidget
{
    Q_OBJECT
public:
    explicit questionsSettings(QWidget *parent = 0);

    void loadLevel(TexamLevel level);
    void saveLevel(TexamLevel &level);

signals:
    void questSettChanged();

private:
    QToolBox *questAsToolBox;
    TasNoteWdg *asNoteWdg;
    TasNameWdg *asNameWdg;
    TasFretPosWdg *asFretPosWdg;

private slots:
    void whenParamsChanged();

};

#endif // QUESTIONSSETTINGS_H
