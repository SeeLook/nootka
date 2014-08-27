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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <music/tnote.h>
#include <exam/tlevel.h>
// #include <texamexecutor.h>
#include <QMainWindow>

class TmelMan;
class TexamExecutor;
class TmainView;
class QPushButton;
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


class MainWindow : public QMainWindow
{
    friend class TexamExecutor;
    friend class Tcanvas;

    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setStatusMessage(const QString& msg, int time);
    void setMessageBg (QColor bg);
    
        /**  Returns font size used in status message label in pixels.
        * Its size depends on whole window size and is used for fonts' sizes
        * of others widgets. */
//     int getFontSize() {return m_statFontSize; }
    QString statusMessage() { return m_statusText; }
    TmainView *innerWidget;
    
        /** Returns point of bottomLeft examResults.*/
//     QPoint relatedPoint();

public slots:
		void setStatusMessage(const QString& msg);
    void openFile(QString runArg); // opens *.nel or *.noo file
    void createSettingsDialog();
    void openLevelCreator(QString levelFile = "");
    void startExamSlot();
//     void aboutSlot();
//     void analyseSlot();
// 
    void noteWasClicked(int index, Tnote note);
//     void noteNameWasChanged(Tnote note);
    void guitarWasClicked(Tnote note);
    void soundWasPlayed(Tnote note);
    
// signals:
//     void sizeChanged(QSize size);


protected:
    TmainScore *score;
    TnoteName *noteName;
    TfingerBoard *guitar;
    Tsound *sound;
    TexamView *examResults;
    TexamExecutor *ex;
// 		TnootkaLabel *nootLabel; // displays Nootka logo
//     TanimedChBox *autoRepeatChB, *expertAnswChB, *correctChB;
    TpitchView *pitchView;
    TprogressWidget *progress;

    QToolBar *nootBar;
    QAction *settingsAct, *levelCreatorAct, *startExamAct, *aboutAct, *analyseAct;
    void clearAfterExam(int examState);
		
    void updateSize(QSize newS); /** Updates position and sizes of the widgets. */
		
		void setSingleNoteMode(bool isSingle); /** Performs changes when insert mode differs then the set one. */
// 				/* Invokes TnoteName::resize(). Also does the same for TexamView (examResults) and TprogressWidget.
// 				 * Font size is calculated from m_statFontSize + m_extraFontOffset */
// 		void setWidgetsFont();
// 
    void resizeEvent(QResizeEvent *event);
//     void closeEvent(QCloseEvent *event);
//     bool eventFilter(QObject* obj, QEvent* event);
    void paintEvent(QPaintEvent *);
        
protected slots:
    void restoreMessage();
		void messageSlot(const QString& msg);
		
//     void showSupportDialog();
		
				/** Checks is score not too width and places pitchView under when it is.
				 * Or opposite - moves pitchView.
				 * Also avoid collision score and note name. */
// 		void fixPitchViewPos();
		
				/** In first attempt it tries to increase window size if there is screen space in spare.
				 * If not, Invokes TnoteName::resize() with smallest font size to decrease it.
				 * Also does the same for TexamView (examResults) and TprogressWidget */
// 		void fixNoteNameSize();
		
				/** This slot is invoked when clef is changed by clicking score.
				 * It adjust ambitus to score possibilities if clef is differ than default
				 * or to instrument scale if clef backs to default */
// 		void adjustAmbitus();

private:

    TroundedLabel 			  *m_statLab;
    QString 							m_statusText, m_prevMsg;
		TmelMan								*m_melButt;
		
        /** Keeps true when statusMesage is locked by temporary message
        * and stops any status messages in this time.*/
    bool 									m_lockStat;
    QColor 								m_prevBg, m_curBG;
    QPixmap 							m_bgPixmap, m_rosettePixmap;
    int 									m_statFontSize;
// 		int 									m_extraFontOffset;
    bool 									m_levelCreatorExist; /** Keeps true when Dialog windows is opened, to avoid opening another file. */
    Tlevel 						    m_level;
		bool 									m_isPlayerFree;
// 		QWidget 							*m_pitchContainer;
// 
// 
    void createActions();
    void setStartExamActParams(); /** it sets icon and status text in startExamAct. */

};

#endif // MAINWINDOW_H
