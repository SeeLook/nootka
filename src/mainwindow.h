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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "tnote.h"
#include "texamlevel.h"
#include <texamexecutor.h>
#include <QMainWindow>

class QVBoxLayout;
class TnootkaLabel;
class TroundedLabel;
class TmainScore;
class QLabel;
class TfingerBoard;
class TnoteName;
class TexamView;
class TprogressWidget;
class TpitchView;
class Tsound;
class QCheckBox;


class MainWindow : public QMainWindow
{
    friend class TexamExecutor;
    friend class Tcanvas;

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
    QWidget *innerWidget;
        /** Returns point of bottomLeft examResults.*/
    QPoint relatedPoint();

public slots:
    void openFile(QString runArg); // opens *.nel or *.noo file
    void createSettingsDialog();
    void openLevelCreator(QString levelFile = "");
    void startExamSlot();
    void aboutSlot();
    void analyseSlot();

    void noteWasClicked(int index, Tnote note);
    void noteNameWasChanged(Tnote note);
    void guitarWasClicked(Tnote note);
    void soundWasPlayed(Tnote note);
    
signals:
    void sizeChanged(QSize size);


protected:
    TmainScore *score;
    TnoteName *noteName;
    TfingerBoard *guitar;
    Tsound *sound;
    TexamView *examResults;
    TexamExecutor *ex;
		TnootkaLabel *nootLabel; // displays Nootka logo
    QCheckBox *autoRepeatChB, *expertAnswChB;
    TpitchView *pitchView;
    TprogressWidget *progress;

    QToolBar *nootBar;
    QAction *settingsAct, *levelCreatorAct, *startExamAct, *aboutAct, *analyseAct;
    void clearAfterExam(TexamExecutor::Estate examState = TexamExecutor::e_finished);

    void resizeEvent(QResizeEvent *event);
    bool event(QEvent *event);
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject* obj, QEvent* event);
    void paintEvent(QPaintEvent *);
        
protected slots:
    void restoreMessage();
    void hintsStateChanged(bool enable);
    void showSupportDialog();

private:

    TroundedLabel 				*m_statLab;
    QString 							m_statusText, m_prevMsg;
        /** Keeps true when statusMesage is locked by temporary message
        * and stops any status messages in this time.*/
    bool 									m_lockStat;
    QColor 								m_prevBg, m_curBG;
    QPixmap 							m_bgPixmap, m_rosettePixmap;
    int 									m_statFontSize;
    QCheckBox 						*m_hintsChB;
         /** Keeps true when Dialog windows is opened, to avoid opening another file. */
    bool 									m_levelCreatorExist;
    TexamLevel 						m_level;
		bool 									m_isPlayerFree;
		QWidget 							*m_pitchContainer;
		QVBoxLayout 					*m_rightLay;


    void createActions();
        /** it sets icon and status text in startExamAct. */
    void setStartExamActParams();
        /** Updates position and sizes of the widgets. */
    void updsateSize();

};

#endif // MAINWINDOW_H
