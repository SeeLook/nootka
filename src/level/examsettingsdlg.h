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


#ifndef EXAMSETTINGSDLG_H
#define EXAMSETTINGSDLG_H

#include "tsettingsdialogbase.h"
#include "texamlevel.h"

class accidSettings;
class levelSettings;
class rangeSettings;
class questionsSettings;


    /** The level creator */
class TlevelCreatorDlg : public TsettingsDialogBase
{
    Q_OBJECT
public:
    explicit TlevelCreatorDlg(QWidget *parent = 0);

    static QString levelCreatorTxt() { return tr("Levels creator"); }
        /** It can be called externally to load level file. */
    void loadLevelFile(QString levelFile);
        /** Returns user selected level or level with empty name if not selected. */
    TexamLevel selectedLevel();

private:
    levelSettings *levelSett;
    questionsSettings *questSett;
    accidSettings *accSett;
    rangeSettings *rangeSett;

    void saveLevel();
        /** Returns formated string with problms in given &l level
        * or empty QString if all is right.*/
    QString validateLevel(TexamLevel &l);

private slots:
        /** This slot is called when user is chooseing exam's level.
            It loads level's parameters to questSett and rangeSett.    */
    void levelWasSelected(TexamLevel level);
        /** Is called when user changes level or want to close dialog window
            without saveing changed level*/
    void levelNotSaved();
    void saveToFile();
        /** This method restored levelSettings tab icon and
            @param isNotSaved value to @p false and window title.*/
    void levelSaved();
    void loadFromFile();
    void acceptLevel();
        /** Performed when user cliks StartExam Button. */
    void startExam();

};

#endif // EXAMSETTINGSDLG_H
