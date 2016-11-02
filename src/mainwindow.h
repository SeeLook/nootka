/***************************************************************************
 *   Copyright (C) 2011-2016 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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
#include <QtWidgets/qmainwindow.h>
#include <QtCore/qpointer.h>


class Tchunk;
class TtoolBar;
class TmainView;
class QPushButton;
class TstatusLabel;
class TmainScore;
class QLabel;
class TfingerBoard;
class TnoteName;
class TmelMan;
class TpitchView;
class Tsound;
class TpluginsLoader;
class TexamView;
class TprogressWidget;
#if !defined (Q_OS_ANDROID)
  class TnootkaLabel;
#endif


/**
 * Implementation of main Nootka window
 */
class MainWindow : public QMainWindow
{

  Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

public slots:
  void noteWasClicked(int index, Tnote note);
  void guitarWasClicked(const Tnote& note);
  void soundWasStarted(const Tnote& note);
  void soundWasFinished(Tchunk* chunk);


	void openFile(QString runArg); // opens *.nel or *.noo file
	void createSettingsDialog();
	void openLevelCreator(QString levelFile = QString());
	void startExamSlot();
	void aboutSlot();
	void analyseSlot();

protected:
  QPointer<TpluginsLoader> m_examPlugin;

	void updateSize(QSize newS); /**< Updates position and sizes of the widgets. */

	void setSingleNoteMode(bool isSingle); /**< Performs changes when insert mode differs then the set one. */
	void closeEvent(QCloseEvent *event);
// #if defined (Q_OS_MAC)
  bool eventFilter(QObject *obj, QEvent *event); /**< Handles opening exam/level files from Finder */
// #endif

protected slots:
	void showSupportDialog();
  void updaterMessagesSlot(int m);
  void examMessageSlot(int demand);

      /** This slot is invoked when clef is changed by clicking score.
        * It adjust ambitus to score possibilities if clef is differ than default
        * or to instrument scale if clef backs to default */
  void adjustAmbitus();
  void pcmStatusMessage(const QString& msg);

private:
  void startExamPlugin(const QString& pluginArgs);

private:
  Tsound               *m_sound;
  TmainView            *m_innerWidget;
  TtoolBar             *m_bar; /**< Main Nootka tool bar. */
  TmainScore           *m_score;
  TnoteName            *m_noteName;
  TpitchView           *m_pitchView;
  TfingerBoard         *m_guitar;
  TexamView            *m_examResults;
  TprogressWidget      *m_progress;
	int 									m_statFontSize;
	bool 									m_levelCreatorExist; /**< Keeps true when Dialog windows is opened, to avoid opening another file. */
	bool 									m_isPlayerFree;
	int										m_startedSoundId; /**< Index of note on the score that has been just started.  */
  TmelMan							 *m_melButt;
  bool                  m_executorAllowsClose;

#if !defined (Q_OS_ANDROID)
  TstatusLabel 			   *m_statusLabel;
#endif
  QPointer<TpluginsLoader> m_updaterPlugin;
  bool                  m_updaterStoppedSound;
};

#endif // MAINWINDOW_H
