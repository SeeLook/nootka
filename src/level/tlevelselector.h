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


#ifndef TLEVELSELECTOR_H
#define TLEVELSELECTOR_H

#include "texamlevel.h"
#include <QWidget>

class QLabel;
class QListWidget;
class QPushButton;
class QFile;


class TlevelSummaryWdg : public QWidget
{
    Q_OBJECT
public:
    explicit TlevelSummaryWdg(QWidget *parent = 0);

    static QString notesRangeTxt() { return tr("notes' range:"); }
    static QString fretsRangeTxt() { return tr("frets' range:"); }

    void setLevel(TexamLevel &tl);

public slots:

private:
    QLabel *summLab;

};

//#######################################################################
        /** Returns list of predefined levels. */
    QList<TexamLevel> getExampleLevels();
//#######################################################################

class TlevelSelector : public QWidget
{
    Q_OBJECT
public:
    explicit TlevelSelector(QWidget *parent = 0);
        /** It's looking for levels:
        * 1. in TexamLevel constructor
        * 2. In default install dir
        * 3. In latest used files */
    void findLevels();
        /** Magic number in level file to identify it.*/
    static const qint32 levelVersion;
    static QString levelFilterTxt() { return tr("Levels"); }
        /** Shows message box with error if file cannot be opened.*/
    static void fileIOerrorMsg(QFile &f, QWidget *parent = 0);

        /** Adds level @param lev to list.*/
    void addLevel(const TexamLevel &lev );
        /** Selects @param id level on the list,
        * and shows its summary.*/
    void selectLevel(int id);
        /** Selects the latest level*/
    void selectLevel();
        /** Returns current level*/
    TexamLevel getSelectedLevel();
    void updateRecentLevels(QString levelFile);
        /** Checks is given level is in range of current tune and frets number.
        * If not, it disables the latest entry in the list - BE SURE to call this
        * only after addLevel() method whtch puts the last level on the list.*/
    bool isSuitable(TexamLevel &l);


public slots:
    void levelSelected(int id);
        /** This is public method to be called externally,
        * because @class TlevelSelector doesn't call it itself
        * after "Load" button pressed, just emits @param levelToLoad() signal. */
    void loadFromFile(QString levelFile = "");

signals:
    void levelChanged(TexamLevel level);
    void levelToLoad();

private:
    QListWidget *levelsList;
    QList <TexamLevel> levList;
    TlevelSummaryWdg *summWdg;
    QPushButton *loadBut;
    TexamLevel getLevelFromFile(QFile &file);

private slots:
    void m_loadFromFile();

};


#endif // TLEVELSELECTOR_H
