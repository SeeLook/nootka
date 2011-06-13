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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "tscorewidget.h"
#include "tnotename.h"
#include "tfingerboard.h"
#include "tnote.h"
#include "texamlevel.h"
#include "texamview.h"
#include "texamexecutor.h"


class MainWindow : public QMainWindow
{
    friend class TexamExecutor;

    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setStatusMessage(QString msg, int time);
    void setStatusMessage(QString msg);
    void setMessageBg (QColor bg);
        /**  Returns font size used in status message label in pixels.
        * Its size depends on whole window size and is used for fonts' sizes
        * of others widgets. */
    int getFontSize() {return m_statFontSize; }
    QString statusMessage() { return m_statusText; }

public slots:
    void createSettingsDialog();
    void createExamSettingsDlg();
    void startExamSlot();
    void aboutSlot();

    void noteWasClicked(int index, Tnote note);
    void noteNameWasChanged(Tnote note);
    void guitarWasClicked(Tnote note);


protected:
    TscoreWidget *score;
    TnoteName *noteName;
    TfingerBoard *guitar;
    TexamView *examResults;

    QToolBar *nootBar;
    QAction *settingsAct, *levelCreatorAct, *startExamAct, *aboutAct;

    void resizeEvent(QResizeEvent *);
    bool event(QEvent *event);

protected slots:
    void restoreMessage();

private:

    QLabel *m_statLab;
    QString m_statusText, m_prevMsg;
        /** Keeps true when statusMesage is locked by temporary message
        * and stops any status messages in this time.*/
    bool m_lockStat;
    QColor m_prevBg;
    int m_statFontSize;
    QVBoxLayout *nameLay;
    QWidget *widget;

    void createActions();

};

#endif // MAINWINDOW_H
