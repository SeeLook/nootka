/***************************************************************************
 *   Copyright (C) 2011-2013 by Tomasz Bojczuk                             *
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


#ifndef TLEVELSELECTOR_H
#define TLEVELSELECTOR_H

#include "tlevel.h"
#include <QWidget>
#include <QListWidget>


class TlevelPreview;
class QListWidget;
class QPushButton;
class QFile;



//#######################################################################
        /** Returns list of predefined levels. */
    QList<Tlevel> getExampleLevels();
//#######################################################################

class TlevelSelector : public QWidget
{
    Q_OBJECT
public:
    explicit TlevelSelector(QWidget *parent = 0);
    virtual ~TlevelSelector();
		
        /** It's looking for levels:
        * 1. in Tlevel constructor
        * 2. In default install directory
        * 3. In latest used files */
    void findLevels();
		
    static QString levelFilterTxt() { return tr("Levels"); } /** Levels */
    
        /** Shows message box with error if file cannot be opened.*/
    static void fileIOerrorMsg(QFile &f, QWidget *parent = 0);

    struct SlevelContener {
        Tlevel 						level;
        QString 					file; // file name of a level
        QListWidgetItem  *item; // corresponding entry in QListWidget (m_levelsListWdg)
        bool 							suitable; // true when level is possible to perform by current instrument
    };    
        /** Adds level @param lev to list. 
         * Also corresponding file name.
         * when @param check is true it checks list for duplicates*/
    void addLevel(const Tlevel &lev, QString levelFile = "", bool check = false);
		
        /** Selects @param id level on the list,
        * and shows its summary.*/
    void selectLevel(int id);
		
    void selectLevel(); /** Selects the latest level*/
    Tlevel getSelectedLevel(); /** Returns current selected level*/
		int idOfSelected() { return m_levelsListWdg->currentRow(); }
		
        /** Updates config file with new levels list.
        * Returns true when given level file was added to config. */
    void updateRecentLevels();
		
        /** Checks is given level in range of current tune and frets number.
        * If not, it disables the latest entry in the list - BE SURE to call this
        * only after addLevel() method which puts the last level on the list.*/
    bool isSuitable(Tlevel &l);
		bool isSuitable();  /** Checks current selected level */
		bool isSuitable(int id) { return m_levels[id].suitable; }
		
		void disableNotSuitable(); /** Disables all levels which not allow instrument settings. */

public slots:
    void levelSelected(int id);
		
        /** This is public method to be called externally,
        * because @class TlevelSelector doesn't call it itself
        * after "Load" button pressed, just emits @param levelToLoad() signal. */
    void loadFromFile(QString levelFile = "");

signals:
    void levelChanged(Tlevel level);
    void levelToLoad();

private:
    QListWidget *m_levelsListWdg;
    QList <SlevelContener> m_levels;
    TlevelPreview *m_levelPreview;
    QPushButton *m_loadBut;
    
    
    Tlevel getLevelFromFile(QFile &file);

private slots:
    void loadFromFilePrivate();

};


#endif // TLEVELSELECTOR_H
