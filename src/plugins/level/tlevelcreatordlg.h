/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk  				                   *
 *   seelook@gmail.com   						                                       *
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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#ifndef EXAMSETTINGSDLG_H
#define EXAMSETTINGSDLG_H

#include <widgets/tsettingsdialogbase.h>
#include <exam/tlevel.h>

// class TmelodySettings;
class accidSettings;
class levelSettings;
class rangeSettings;
class questionsSettings;


/**
 * The level creator dialog window
 */
class TlevelCreatorDlg : public TsettingsDialogBase
{
    Q_OBJECT
public:
    explicit TlevelCreatorDlg(QWidget *parent = 0);

    static QString levelCreatorTxt() { return tr("Levels creator"); }
    
        /** It can be called externally to load level file. */
    void loadLevelFile(QString levelFile);
		
        /** Returns user selected level or level with empty name if not selected. */
    Tlevel& selectedLevel();
    
    QString& communicate() { return m_communicate; } /** Text returned by level creator */
		
public slots:
				/** This slot is invoked by every settings widget(page) when its settings were changed  */
		void levelWasChanged();
    
#if defined (Q_OS_ANDROID)
protected:
    void tapMenu() override;
#endif

private:
    levelSettings 							*m_levelSett;
    questionsSettings 					*m_questSett;
    accidSettings 							*m_accSett;
// 		TmelodySettings 						*m_meloSett;
    rangeSettings 							*m_rangeSett;
    
    QString                      m_communicate;

    void saveLevel();
		
        /** Returns formatted string with problems in given &l level
        * or empty QString if all is right.*/
    QString validateLevel(Tlevel &l);
		
        /** Displays QMessageBox with given text wrapped with:
         * 'It seems the level has got some mistakes:' */
    void showValidationMessage(QString message);

private slots:
        /** This slot is called when user is choosing exam's level.
            It loads level's parameters to questSett and rangeSett.    */
    void levelWasSelected(Tlevel level);
		
        /** Is called when user changes level or want to close dialog window
            without saving changed level*/
    void levelNotSaved();
    void saveToFile();
		
        /** This method restored levelSettings tab icon and
            @param isNotSaved value to @p false and window title.*/
    void levelSaved();
    void loadFromFile();
    void startExam(); /** Performed when user clicks StartExam Button. */
    void checkLevelSlot();
    void helpSlot();

};

#endif // EXAMSETTINGSDLG_H
