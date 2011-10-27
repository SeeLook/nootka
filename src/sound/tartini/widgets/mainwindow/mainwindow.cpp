/***************************************************************************
                          mainwindow.cpp  -  description
                             -------------------
    begin                : May 2002
    copyright            : (C) 2002-2007 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

//#include <qapplication.h>
#include <qstatusbar.h>
//#include <q3popupmenu.h>
#include <qmenubar.h>
#include <qsplitter.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <q3filedialog.h>
#include <qworkspace.h>
//#include <q3toolbar.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qpixmap.h>
//#include <qlistbox.h>
//#include <qpixmap.h>
//#include <qgrid.h>
//#include <qlayout.h>
#include <qtimer.h>
#include <q3header.h>
#include <qmetaobject.h>
#include <qsizegrip.h>
#include <qfileinfo.h>
#include <qlabel.h>
#include <QToolBar>
#include <qtooltip.h>
#include <qmime.h>
#include <q3textedit.h>
#include <qtextedit.h>
#include <qtextstream.h>
#include <QTextBrowser>

#include <qaction.h>
//Added by qt3to4:
#include <QKeyEvent>
#include <QEvent>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <QCustomEvent>
#include <QPrinter>
#include <QPrintDialog>
#include <QDesktopServices>

#include "gdata.h"
#include "mainwindow.h"

#include "freqpair.h"
//#include "process.h"
#include "soundfile.h"
#include "channel.h"
#include "myscrollbar.h"
#include "mylabel.h"

// Views
#include "openfiles.h"
#include "freqview.h"
#include "summaryview.h"
#include "pitchcompassview.h"
#include "volumemeterview.h"
#include "tunerview.h"
#include "hblockview.h"
#include "hstackview.h"
#include "hbubbleview.h"
#include "hcircleview.h"
#include "waveview.h"
#include "pianoview.h"
#include "htrackview.h"
#include "correlationview.h"
#include "fftview.h"
#include "cepstrumview.h"
#include "debugview.h"
#include "scoreview.h"
#include "vibratoview.h"
#include "musicnotes.h"

#include "savedialog.h"
#include "opendialog.h"
//#include "preferences.h"
#include "tartinisettingsdialog.h"
#include <qlineedit.h>
#include <q3listview.h>
#include <qmessagebox.h>
#include <qwt_slider.h>
#include <QLabel>
#include <QWhatsThis>

#include "wave_stream.h"
#ifdef USE_OGG
#include "ogg_stream.h"
#endif
#ifdef USE_SOX
#include "sox_stream.h"
#endif

#include <math.h>
#include "useful.h"
#include "mystring.h"
//#include <unistd.h>
#include <string.h>

//include the icon .xpm pictures into the program directly
//#include "pics/mic.xpm"
#include "pics/record32x32.xpm"
#include "pics/open.xpm"
#include "pics/close32x32.xpm"
#include "pics/save32x32.xpm"
//#include "pics/pause.xpm"

#include "pics/beginning32x32.xpm"
#include "pics/rewind32x32.xpm"
#include "pics/play32x32.xpm"
#include "pics/playrecord32x32.xpm"
#include "pics/stop32x32.xpm"
#include "pics/fastforward32x32.xpm"
#include "pics/end32x32.xpm"

//#include "pics/background.xpm"
#include "pics/autofollow32x32.xpm"
#include "pics/shadingon32x32.xpm"
#include "pics/shadingoff32x32.xpm"
//#include "pics/chain_open.xpm"
//#include "pics/chain_closed.xpm"
//#include "pics/tartinilogo.xpm"

#include "myglfonts.h"

#ifdef MACX
#include "main.h"
#endif

#include "myassert.h"

MainWindow *mainWindow;
MyGLFont *mygl_font;

ViewData viewData[NUM_VIEWS] = {
                               /*ViewData(title,               menuName,              className,           menu type);*/
                                 ViewData("File List",         "&File List",          "OpenFiles",         0),
                                 ViewData("Pitch Contour",     "&Pitch Contour",      "FreqView",          0),
                                 ViewData("Chromatic Tuner",   "&Chromatic Tuner",    "TunerView",         0),
                                 ViewData("Harmonic Track",    "3D Harmonic &Track",  "HTrackView",        0),
                                 ViewData("Vibrato View",      "V&ibrato View",       "VibratoView",       0),
                                 ViewData("Musical Score",     "&Musical Score",      "ScoreView",         0),
                                 ViewData("Oscilloscope",      "&Oscilloscope",       "WaveView",          1),
                                 ViewData("Correlation View",  "Corre&lation View",   "CorrelationView",   1),
                                 ViewData("FFT View",          "FF&T View",           "FFTView",           1),
                                 ViewData("Cepstrum View",     "C&epstrum View",      "CepstrumView",      1),
                                 ViewData("Debug View",        "&Debug View",         "DebugView",         1),
                                 ViewData("Harmonic Block",    "Harmonic &Block",     "HBlockView",        2),
                                 ViewData("Harmonic Stack",    "&Harmonic Stack",     "HStackView",        2),
                                 ViewData("Harmonic Bubbles",  "H&armonic Bubbles",   "HBubbleView",       2),
                                 ViewData("Harmonic Circle",   "Ha&rmonic Circle",    "HCircleView",       2),
                                 ViewData("Pitch Compass",     "Pitch &Compass",      "PitchCompassView",  2),
                                 ViewData("Piano Keyboard",    "2D Piano &Keyboard",  "PianoView",         3),
                                 ViewData("Summary View",      "&Summary View",       "SummaryView",       3),
                                 ViewData("Volume Meter",      "&Volume Meter",       "VolumeMeterView",   3)
                               };


MainWindow::MainWindow()
  : QMainWindow( NULL, Qt::WDestructiveClose )
{
  createSignalMapper = new QSignalMapper(this);
  connect(createSignalMapper, SIGNAL(mapped(int)), SLOT(openView(int)));

  QFont myFont = QFont();
  myFont.setPointSize(9);
  myFont.setStyleStrategy(QFont::PreferAntialias);
  mygl_font = new MyGLFont(myFont);

#ifndef WINDOWS
  getitimer(ITIMER_PROF, &profiler_value); //for running multi-threaded profiling
#endif
  //resize(1024+10, 790);
  resize(1024-6, 768-6); //6 pixel boarder
  //setUsesBigPixmaps(true);

  //gdata->inputFile = copy_string(inputFile_);
  //if(inputFile_) setCaption(inputFile_);
  //else setCaption("<No File>");
#ifdef MYDEBUG
  setWindowTitle("Tartini (Debug)");
#else
  setWindowTitle("Tartini");
#endif

  //QMimeSourceFactory::defaultFactory()->setPixmap("chain_open.png", QPixmap(chain_open_xpm));
  //QMimeSourceFactory::defaultFactory()->setPixmap("chain_closed.png", QPixmap(chain_closed_xpm));
  
  //Create the main Workspace for the view widgets to go in (for MDI)
  theWorkspace = new QWorkspace(this, "TheWorkspace");
  setCentralWidget( theWorkspace );
  //theWorkspace->setPaletteBackgroundPixmap ( QPixmap(background_xpm ));
  //theWorkspace->setBackgroundRole(QPalette::Window);

  
  
/*  
  QPalette thePalette = qApp->palette(this);
  thePalette.setColor(QPalette::Active, QColorGroup::Background, gdata->getBackgroundColor());
  //thePalette.setColor(QPalette::Active, QColorGroup::Background, QColor(210, 218, 232));
  //thePalette.setBrush(QPalette::Active, QColorGroup::Background, QBrush(QColor(255, 120, 64), Qt::CrossPattern));
  //thePalette.setBrush(QPalette::Active, QColorGroup::Background, QBrush(QColor(188, 200, 213), QPixmap(background_xpm)));
  this->setPalette(thePalette);
*/
  
  
  //Create the file Toolbar
  QToolBar *fileToolBar = new QToolBar("File Actions", this);//  , Qt::DockTop, FALSE);
  addToolBar(Qt::TopToolBarArea, fileToolBar);
  fileToolBar->setIconSize(QSize(32, 32));

  //QAction *openAction = new QAction(QIcon( QPixmap(iconOpen) ), "&Open", this);
  QAction *openAction = new QAction(QIcon(iconOpen), "&Open", this);
  openAction->setShortcut(tr("Ctrl+O"));
  openAction->setWhatsThis("Open a sound from file and process it using the current preferences");
  fileToolBar->addAction(openAction);
  connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));

  QAction *saveAction = new QAction(QIcon(save32x32_xpm), "&Save", this);
  saveAction->setShortcut(tr("Ctrl+S"));
  saveAction->setWhatsThis("Save the active sound to a file");
  fileToolBar->addAction(saveAction);
  connect(saveAction, SIGNAL(triggered()), gdata, SLOT(saveActiveFile()));
  
  QAction *closeAction = new QAction(QIcon(close32x32_xpm), "&Close", this);
  closeAction->setShortcut(tr("Ctrl+W"));
  closeAction->setWhatsThis("Close the active sound. If unsaved will ask to save. Note: Asking can be disabled in the preferences");
  fileToolBar->addAction(closeAction);
  connect(closeAction, SIGNAL(triggered()), gdata, SLOT(closeActiveFile()));
  
  QAction *closeAllAction = new QAction("Close All", this);
  closeAllAction->setWhatsThis("Close all the sounds. If any sounds are unsaved, it will ask to save. Note: Asking can be disabled in the preferences");
  connect(closeAllAction, SIGNAL(triggered()), gdata, SLOT(closeAllFiles()));
  
  QAction *printAction = new QAction("Print", this);
  printAction->setShortcut(tr("Ctrl+P"));
  printAction->setWhatsThis("Print the Pitch Contour, fitting the its current view onto a page");
  connect(printAction, SIGNAL(triggered()), this, SLOT(printPitch()));

  //Create the sound Toolbar
  QToolBar *soundToolBar = new QToolBar("Sound Actions", this); //, Qt::DockBottom, FALSE);
  addToolBar(Qt::BottomToolBarArea, soundToolBar);
  soundToolBar->setIconSize(QSize(32, 32));
  
  QToolButton *beginningButton = new QToolButton(QIcon(beginning32x32_xpm), "Beginning", "Rewind to the beginning", NULL, NULL, soundToolBar, "Beginning");
  beginningButton->setWhatsThis("Jump to the beginning of the sound");
  soundToolBar->addWidget(beginningButton);
  connect(beginningButton, SIGNAL(pressed()), gdata, SLOT(beginning()));
  
  rewindTimer = new QTimer(this);
  connect(rewindTimer, SIGNAL(timeout()), gdata, SLOT(rewind()));
  QToolButton *rewindButton = new QToolButton(QIcon(rewind32x32_xpm), "Rewind", "Rewind", NULL, NULL, soundToolBar, "rewind");
  rewindButton->setWhatsThis("Rewind the sound");
  soundToolBar->addWidget(rewindButton);
  connect(rewindButton, SIGNAL(pressed()), this, SLOT(rewindPressed()));
  connect(rewindButton, SIGNAL(released()), this, SLOT(rewindReleased()));
  
  playIconSet = new QIcon(play32x32_xpm);
  playRecordIconSet = new QIcon(playrecord32x32_xpm);
  stopIconSet = new QIcon(stop32x32_xpm);
  /*QAction **/playStopAction = new QAction(*playIconSet, "&Play", this);
  playStopAction->setShortcut(tr("Space"));
  playStopAction->setWhatsThis("Play/Stop the active sound");
  soundToolBar->addAction(playStopAction);
  connect(playStopAction, SIGNAL(triggered()), this, SLOT(playStopClicked()));

  fastforwardTimer = new QTimer(this);
  connect(fastforwardTimer, SIGNAL(timeout()), gdata, SLOT(fastforward()));
  QToolButton *fastforwardButton = new QToolButton(QIcon(fastforward32x32_xpm), "Fast-forward", "Fast-forward", NULL, NULL, soundToolBar, "fastforward");
  fastforwardButton->setWhatsThis("Fastfoward the sound");
  soundToolBar->addWidget(fastforwardButton);
  connect(fastforwardButton, SIGNAL(pressed()), this, SLOT(fastforwardPressed()));
  connect(fastforwardButton, SIGNAL(released()), this, SLOT(fastforwardReleased()));

  QAction *endAction = new QAction(QIcon(end32x32_xpm), "&End", this);
  endAction->setWhatsThis("Jump to the end of the sound");
  endAction->setShortcut(tr("Ctrl+E"));
  connect(endAction, SIGNAL(triggered()), gdata, SLOT(end()));
  soundToolBar->addAction(endAction);

  //Create the Actions, to be used in Menus and Toolbars
  recordIconSet = new QIcon(record32x32_xpm);
  recordAction = new QAction(*recordIconSet, "&Record", this);
  recordAction->setShortcut(tr("Return"));
  recordAction->setWhatsThis("Record a new sound, using the input device and settings selected in the preferences");
  fileToolBar->addAction(recordAction);
  connect(recordAction, SIGNAL(triggered()), this, SLOT(openRecord()));
  //recordButton = new QToolButton(*recordIconSet, "Record", "Record a new sound", NULL, NULL, fileToolBar, "record");
  //fileToolBar->addWidget(recordButton);
  //connect(recordButton, SIGNAL(clicked()), this, SLOT(openRecord()));

  playRecordAction = new QAction(*playRecordIconSet, "Play and Record", this);
  playRecordAction->setShortcut(tr("Shift+Return"));
  playRecordAction->setWhatsThis("Play the active sound and record a new one at the same time!");
  fileToolBar->addAction(playRecordAction);
  connect(playRecordAction, SIGNAL(triggered()), this, SLOT(openPlayRecord()));

  QAction *quit = new QAction("&Quit", this);
  quit->setShortcut(tr("Ctrl+Q"));
  quit->setWhatsThis("Quit the Tartini application");
  //connect(quit, SIGNAL(triggered()), qApp, SLOT( closeAllWindows() ));
  connect(quit, SIGNAL(triggered()), this, SLOT( close() ));

  //Create the File Menu
  QMenu *fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction(openAction);
  fileMenu->addAction(saveAction);
  fileMenu->addAction(closeAction);
  fileMenu->addAction(closeAllAction);
  fileMenu->addSeparator();
  fileMenu->addAction(playStopAction);
  fileMenu->addAction(recordAction);
  fileMenu->addAction(playRecordAction);
  fileMenu->addSeparator();
  fileMenu->addAction(printAction);
  fileMenu->addSeparator();
  fileMenu->addAction(quit);

  //Channel Menu
  //QAction *exportAction = new QAction(QIcon(iconOpen), "&Export", this);
  QAction *exportAction1 = new QAction("&Export to plain text", this);
  connect(exportAction1, SIGNAL(triggered()), this, SLOT(exportChannelPlainText()));
  QAction *exportAction2 = new QAction("&Export to matlab", this);
  connect(exportAction2, SIGNAL(triggered()), this, SLOT(exportChannelMatlab()));

  QMenu *channelMenu = menuBar()->addMenu("C&hannel");
  channelMenu->addAction(exportAction1);
  channelMenu->addAction(exportAction2);

  // Create actions for the new view menu
  newViewMenu = menuBar()->addMenu("&Create");
  //connect(newViewMenu, SIGNAL(activated(int)), this, SLOT(openView(int)));
  connect(newViewMenu, SIGNAL(aboutToShow()), this, SLOT(newViewAboutToShow()));

  //Create the Window Menu
  windowMenu = menuBar()->addMenu("&Windows");
  windowMenu->setCheckable( true );
  connect( windowMenu, SIGNAL( aboutToShow() ),
	     this, SLOT( windowMenuAboutToShow() ) );

  //Create the Options Menu
  optionsMenu = menuBar()->addMenu("&Options");
  //optionsMenu->insertItem("Apply &Equal loudness curve", this, SLOT( toggleOption(int) ), 0, 0);
  //optionsMenu->setItemChecked(0, gdata->equalLoudness);
  //optionsMenu->insertItem("Apply &Partial Masking", this, SLOT( toggleOption(int) ), 0, 1);
  //optionsMenu->setItemChecked(1, gdata->useMasking);
  //optionsMenu->insertItem("&Write a Ridge File", this, SLOT( toggleOption(int) ), 0, 2);
  //optionsMenu->setItemChecked(2, gdata->useRidgeFile);
  //optionsMenu->insertItem("Process &Stereo", this, SLOT( toggleOption(int) ), 0, 3);
  //optionsMenu->setItemChecked(3, (gdata->process_channels==2));
  //optionsMenu->insertSeparator();
  optionsMenu->addAction("&Preferences", this, SLOT(menuPreferences()));

  QAction *whatsThis = QWhatsThis::createAction(this);
  whatsThis->setToolTip("What's this?");
  whatsThis->setWhatsThis("Click this button, then click something to learn more about it");

  helpMenu = menuBar()->addMenu("&Help");
  helpMenu->addAction(whatsThis);
  helpMenu->addSeparator();
  helpMenu->addAction("Documentation", this, SLOT(showDocumentation()));
  helpMenu->addSeparator();
  helpMenu->addAction("About Tartini", this, SLOT(aboutTartini())); //, 0, 0);
  helpMenu->addAction("About Qt", this, SLOT(aboutQt())); //, 0, 1);
  
  //Create the other toolbar, which contains some option stuff
  QToolBar *analysisToolBar = new QToolBar("Analysis Toobar", this); //, Qt::DockTop, FALSE);
  addToolBar(analysisToolBar);
  analysisToolBar->setIconSize(QSize(32, 32));
/*
  QCheckBox *autoFollow = new QCheckBox("AutoFollow Active Channel", analysisToolBar);
  //autoFollow->setChecked(gdata->settings.getBool("View", "autoFollow"));
  autoFollow->setChecked(gdata->view->autoFollow());
  connect(autoFollow, SIGNAL(toggled(bool)), gdata->view, SLOT(setAutoFollow(bool)));
*/
  QIcon *autoFollowIconSet = new QIcon();
  autoFollowIconSet->setPixmap(QPixmap(autofollow32x32_xpm), QIcon::Small, QIcon::Normal);
  QToolButton *autoFollowButton = new QToolButton(*autoFollowIconSet, "Auto Follow", "Moves the view up and down automaticlly with the active channel when playing or recording", NULL, NULL, analysisToolBar, "autoFollow");
  analysisToolBar->addWidget(autoFollowButton);
  autoFollowButton->setToggleButton(true);
  autoFollowButton->setAutoRaise(true);
  autoFollowButton->setOn(gdata->view->autoFollow());
  autoFollowButton->setWhatsThis("Scrolls the Pitch Contour view up and down automaticlly with the active channel when playing or recording. Note: Manual scrolling (vertically) will be disabled during this time.");
  connect(autoFollowButton, SIGNAL(toggled(bool)), gdata->view, SLOT(setAutoFollow(bool)));

  QIcon *backgroundShadingIconSet = new QIcon();
  backgroundShadingIconSet->setPixmap(QPixmap(shadingon32x32_xpm), QIcon::Small, QIcon::Normal, QIcon::On);
  backgroundShadingIconSet->setPixmap(QPixmap(shadingoff32x32_xpm), QIcon::Small, QIcon::Normal, QIcon::Off);
  QToolButton *backgroundShadingButton = new QToolButton(*backgroundShadingIconSet, "Background Shading", "Toggle background shading on/off", NULL, NULL, analysisToolBar, "backgroundShading");
  analysisToolBar->addWidget(backgroundShadingButton);
  backgroundShadingButton->setToggleButton(true);
  backgroundShadingButton->setAutoRaise(true);
  backgroundShadingButton->setOn(gdata->view->backgroundShading());
  backgroundShadingButton->setWhatsThis("Draw solid color underneath the Pitch Contour, to help you find the line");
  connect(backgroundShadingButton, SIGNAL(toggled(bool)), gdata->view, SLOT(setBackgroundShading(bool)));

  analysisToolBar->addAction(whatsThis);

  View *view = gdata->view;
  QToolBar *timeBarDock = new QToolBar("Time-axis Slider", this); //, Qt::DockBottom);
  addToolBar(Qt::BottomToolBarArea, timeBarDock);
  timeBarDock->setIconSize(QSize(32, 32));

/*
  timeScrollBar = new MyScrollBar(gdata->leftTime(), gdata->rightTime(), 0.1, view->viewWidth(), view->currentTime(), 10000, Qt::Horizontal, timeBarDock);
  timeScrollBar->setMinimumSize(300, 16);
  connect(timeScrollBar, SIGNAL(valueChanged(double)), view, SLOT(setCurrentTime(double)));
  connect(timeScrollBar, SIGNAL(sliderMoved(double)), gdata, SLOT(updateActiveChunkTime(double)));
  connect(gdata, SIGNAL(timeRangeChanged(double, double)), timeScrollBar, SLOT(setRange(double, double)));
  connect(view, SIGNAL(currentTimeChanged(double)), timeScrollBar, SLOT(setValue(double)));
  connect(view, SIGNAL(viewWidthChanged(double)), timeScrollBar, SLOT(setPageStep(double)));
*/
  
#if QWT_VERSION == 0x050000
  timeSlider = new QwtSlider(timeBarDock, Qt::Horizontal, QwtSlider::None, QwtSlider::BgBoth);
#else
  timeSlider = new QwtSlider(timeBarDock, Qt::Horizontal, QwtSlider::NoScale, QwtSlider::BgBoth);
#endif
  timeSlider->setRange(gdata->leftTime(), gdata->rightTime(), 1.0/10000.0, 1000);
  timeSlider->setValue(view->currentTime());
  timeSlider->setTracking(true);
  timeSlider->setThumbWidth(20);
  timeSlider->setThumbLength(60);
  timeSlider->setBorderWidth(4);
  timeSlider->setMargins(2, 2);
  //timeSlider->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed, 10, 0));
  timeSlider->setMinimumWidth(200);
  //timeSlider->setScaleMaxMinor(4);
  //timeSlider->setScaleMaxMajor(8);
  //timeSlider->scaleDraw()->setMargin(0, 0);
  //connect(timeSlider, SIGNAL(valueChanged(double)), view, SLOT(setCurrentTime(double)));
  timeSlider->setWhatsThis("Drag the time slider to move back and forward through the sound file");
  connect(timeSlider, SIGNAL(sliderMoved(double)), gdata, SLOT(updateActiveChunkTime(double)));
  connect(timeSlider, SIGNAL(sliderMoved(double)), view, SLOT(doSlowUpdate()));
  //connect(view, SIGNAL(currentTimeChanged(double)), timeSlider, SLOT(setValue(double)));
  connect(view, SIGNAL(onSlowUpdate(double)), timeSlider, SLOT(setValue(double)));
  connect(gdata, SIGNAL(timeRangeChanged(double, double)), this, SLOT(setTimeRange(double, double)));
  //connect(view, SIGNAL(viewWidthChanged(double)), this, SLOT(setPageStep(double)));
  //connect(view, SIGNAL(onSlowUpdate()), timeSlider, SLOT(repaint()));
  timeBarDock->addWidget(timeSlider);
  
  QToolBar *volumeMeterToolBar = new QToolBar("Volume Meter", this); //, Qt::DockTop, FALSE);
  addToolBar(volumeMeterToolBar);
  volumeMeterToolBar->setIconSize(QSize(32, 32));
  VolumeMeterView *volumeMeterView = new VolumeMeterView(VIEW_VOLUME_METER, volumeMeterToolBar);//->show();
  volumeMeterView->setWhatsThis("Shows the volume (in dB) of the left and right channels of the active sound. Note: If a mono sound the both are the same");
  volumeMeterToolBar->addWidget(volumeMeterView);


  QToolBar *keyToolBar = new QToolBar("Key Toolbar", this);
  keyToolBar->setWhatsThis("Used to set which reference lines are drawn in the Pitch Contour View.");
  addToolBar(keyToolBar);
  QLabel *keyLabel = new QLabel("Key", keyToolBar);
  keyToolBar->addWidget(keyLabel);

  QComboBox *keyComboBox = new QComboBox(keyToolBar);
  keyComboBox->setWindowTitle("Key");
  QStringList s;
  for(int j=0; j<NUM_MUSIC_KEYS; j++) s << gMusicKeyName[j];
  keyComboBox->addItems(s);
  keyComboBox->setCurrentIndex(gdata->musicKey());
  keyToolBar->addWidget(keyComboBox);
  connect(keyComboBox, SIGNAL(activated(int)), gdata, SLOT(setMusicKey(int)));
  connect(gdata, SIGNAL(musicKeyChanged(int)), keyComboBox, SLOT(setCurrentIndex(int)));
  connect(gdata, SIGNAL(musicKeyChanged(int)), gdata->view, SLOT(doUpdate()));

  keyTypeComboBox = new QComboBox(keyToolBar);
  keyTypeComboBox->setWindowTitle("Scale type");
  s.clear();
  for(uint j=0; j<gMusicScales.size(); j++) s << gMusicScales[j].name();
  keyTypeComboBox->addItems(s);
  keyTypeComboBox->setCurrentIndex(gdata->musicKeyType());
  keyToolBar->addWidget(keyTypeComboBox);
  connect(keyTypeComboBox, SIGNAL(activated(int)), gdata, SLOT(setMusicKeyType(int)));
  connect(gdata, SIGNAL(musicKeyTypeChanged(int)), keyTypeComboBox, SLOT(setCurrentIndex(int)));
  connect(gdata, SIGNAL(musicKeyTypeChanged(int)), gdata->view, SLOT(doUpdate()));

  QComboBox *temperedComboBox = new QComboBox(keyToolBar);
  temperedComboBox->setWindowTitle("Tempered type");
  s.clear();
  //s << "Even Tempered" << "Just Intonation";
  for(uint j=0; j<gMusicKeys.size(); j++) s << gMusicKeys[j].name();
  temperedComboBox->addItems(s);
  temperedComboBox->setCurrentIndex(gdata->temperedType());
  keyToolBar->addWidget(temperedComboBox);
  connect(temperedComboBox, SIGNAL(activated(int)), gdata, SLOT(setTemperedType(int)));
  connect(gdata, SIGNAL(temperedTypeChanged(int)), temperedComboBox, SLOT(setCurrentIndex(int)));
  connect(gdata, SIGNAL(temperedTypeChanged(int)), gdata->view, SLOT(doUpdate()));

  QToolBar *freqAToolBar = new QToolBar("Frequency Offset Toolbar", this);
  freqAToolBar->setWhatsThis("The frequency of an even-tempered 'A' used for reference lines in the Pitch Contour View. Default 440 Hz."
    "Note: For other scales the root note is chosen from the even-tempered scale with that 'A'.");
  addToolBar(freqAToolBar);

  QSpinBox *freqASpinBox = new QSpinBox(400, 600, 1, freqAToolBar, "freqASpinBox");
  freqASpinBox->setPrefix("A=");
  freqASpinBox->setSuffix(" Hz");
  freqASpinBox->setFocusPolicy(Qt::NoFocus);
  freqASpinBox->setValue(toInt(gdata->freqA()));
  freqAToolBar->addWidget(freqASpinBox);
  connect(freqASpinBox, SIGNAL(valueChanged(int)), gdata, SLOT(setFreqA(int)));
  connect(freqASpinBox, SIGNAL(valueChanged(int)), gdata->view, SLOT(doUpdate()));
  QFont font("Courier", 12, QFont::Bold);
  
  noteLabel = new MyLabel("Note: 9999", statusBar(), "notelabel");
/*
  noteLabel = new QLabel("", statusBar(), "notelabel");
  noteLabel->setTextFormat(Qt::PlainText);
  noteLabel->setFont(font);
*/
  statusBar()->addPermanentWidget(noteLabel, 0);
  setNoteLabel();
  QToolTip::add(noteLabel, "The current note number in the active file");
  //connect(gdata->view, SIGNAL(currentTimeChanged(double)), this, SLOT(setNoteLabel()));
  connect(gdata->view, SIGNAL(onSlowUpdate(double)), this, SLOT(setNoteLabel()));

  
  chunkLabel = new MyLabel("Chunk: 999999", statusBar(), "chunklabel");
/*
  chunkLabel = new QLabel("", statusBar(), "chunklabel");
  chunkLabel->setTextFormat(Qt::PlainText);
  chunkLabel->setFont(font);
*/
  //statusBar()->addPermanentWidget(chunkLabel, 0, true);
  statusBar()->addPermanentWidget(chunkLabel, 0);
  setChunkLabel();
  QToolTip::add(chunkLabel, "The current chunk number in the active file");
  //connect(gdata->view, SIGNAL(currentTimeChanged(double)), this, SLOT(setChunkLabel()));
  connect(gdata->view, SIGNAL(onSlowUpdate(double)), this, SLOT(setChunkLabel()));

  timeLabel = new MyLabel("Time: -00:00.000", statusBar(), "timelabel");
/*
  timeLabel = new QLabel("", statusBar(), "timelabel");
  //timeLabel->setTextFormat(Qt::RichText);
  timeLabel->setTextFormat(Qt::PlainText);
  //timeLabel->setScaledContents(false);
  timeLabel->setFont(font);
*/
  //statusBar()->addWidget(timeLabel, 0, true);
  statusBar()->addPermanentWidget(timeLabel, 0);
  setTimeLabel(0);
  QToolTip::add(timeLabel, "The current time positon for all files (mins:sec)");
  //connect(gdata->view, SIGNAL(currentTimeChanged(double)), this, SLOT(setTimeLabel(double)));
  connect(gdata->view, SIGNAL(onSlowUpdate(double)), this, SLOT(setTimeLabel(double)));

  //connect(gdata->view, SIGNAL(currentTimeChanged(double)), gdata, SLOT(updateActiveChunkTime(double)));

/*
  //open views in reverse for some reason to get the right order ???
  //openView(VIEW_PITCH_COMPASS);
  //openView(VIEW_VOLUME_METER);
  //openView(VIEW_PIANO);
  //openView(VIEW_CORRELATION);
  //openView(VIEW_WAVE);
  openView(VIEW_OPEN_FILES);
  //openView(VIEW_SCORE);
  openView(VIEW_FREQ);
  //openView(VIEW_HBLOCK);
  //openView(VIEW_TUNER);
  openView(VIEW_VIBRATO);
*/

  statusBar()->message( "Ready", 2000 );

  connect(gdata, SIGNAL(activeChannelChanged(Channel*)), this, SLOT(setTitle(Channel*)));
}

MainWindow::~MainWindow()
{
  delete rewindTimer;
  delete fastforwardTimer;
  fprintf(stderr, "Has pending events = %s\n", (qApp->hasPendingEvents()) ? "Yes" : "No");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  if(gdata->closeAllFiles()) {
    //writeSettings();
    saveViewGeometry();
    event->accept();
  } else {
    event->ignore();
  }
}

//void MainWindow::customEvent( QCustomEvent * e )
bool MainWindow::event( QEvent * e )
{
  if(e->type() == UPDATE_FAST || e->type() == UPDATE_SLOW) {
    SoundFile *soundFile = gdata->audioThread.curSoundFile();
    if(soundFile) {
      soundFile->lock();
      gdata->view->setCurrentTime(soundFile->timeAtCurrentChunk());
      soundFile->unlock();
    }
    gdata->updateViewLeftRightTimes();
/*
    if(e->type() == UPDATE_SLOW) {
      gdata->view->doAutoFollowing();
      gdata->view->doSlowUpdate();
    }
*/
    gdata->need_update = false;
	  return true;
  } else if(e->type() == SOUND_STARTED) {
    //playStopButton->setIconSet(*stopIconSet);
    playStopAction->setIconSet(*stopIconSet);
    playStopAction->setText("Stop");
    //recordButton->setIconSet(*stopIconSet);
    recordAction->setIconSet(*stopIconSet);
    recordAction->setText("Stop");
    playRecordAction->setIconSet(*stopIconSet);
    playRecordAction->setText("Stop");
  } else if(e->type() == SOUND_STOPPED) {
    //playStopButton->setIconSet(*playIconSet);
    playStopAction->setIconSet(*playIconSet);
    playStopAction->setText("Play");
    //recordButton->setIconSet(*recordIconSet);
    recordAction->setIconSet(*recordIconSet);
    recordAction->setText("Record");
    playRecordAction->setIconSet(*playRecordIconSet);
    playRecordAction->setText("Play and Record");
  } else if(e->type() == SETTINGS_CHANGED) {
    gdata->updateQuickRefSettings();
  }
	return QMainWindow::event(e);
}

void MainWindow::keyPressEvent ( QKeyEvent * e )
{
  View *view = gdata->view;
  
  double speed = 1;
  if(e->state() & Qt::ShiftModifier) speed *= 4;
  if(e->state() & Qt::ControlModifier) speed *= 10;
  double newTime;
  
  switch(e->key()) {
#ifdef MYDEBUG
  case Qt::Key_Escape:
    gdata->audioThread.stopAndWait();
    qApp->closeAllWindows();
    break;
#endif
/*  case Qt::Key_Space:
      gdata->pauseSound();
      break;*/
  case Qt::Key_Left:
    if(gdata->running == STREAM_FORWARD) {
      gdata->rewind();
    } else {
      if(gdata->getActiveChannel())
        newTime = view->currentTime() - gdata->getActiveChannel()->timePerChunk()*speed;
      else
        newTime = view->currentTime() - 0.10*speed;  //move 1/5th of a second back
      gdata->updateActiveChunkTime(newTime);
      //view->setCurrentTime(newTime);
    }
    break;
  case Qt::Key_Right:
    if(gdata->running == STREAM_FORWARD) {
      gdata->fastforward();
    } else {
      if(gdata->getActiveChannel())
        newTime = view->currentTime() + gdata->getActiveChannel()->timePerChunk()*speed;
      else
        newTime = view->currentTime() + 0.10*speed; //move 1/5th of a second forward
      gdata->updateActiveChunkTime(newTime);
    }
    break;
  case Qt::Key_Up:
      view->setViewBottom(view->viewBottom() + 0.2*speed); //move 1/5 of a semi-tone
      gdata->view->doSlowUpdate();
    break;
  case Qt::Key_Down:
      view->setViewBottom(view->viewBottom() - 0.2*speed); //move 1/5 of a semi-tone
      gdata->view->doSlowUpdate();
    break;
  case Qt::Key_PageUp:
      view->setViewBottom(view->viewBottom() + 1.0*speed); //move 1/5 of a semi-tone
      gdata->view->doSlowUpdate();
    break;
  case Qt::Key_PageDown:
      view->setViewBottom(view->viewBottom() - 1.0*speed); //move 1/5 of a semi-tone
      gdata->view->doSlowUpdate();
    break;
  case Qt::Key_Home:
      gdata->updateActiveChunkTime(gdata->leftTime());
    break;
  case Qt::Key_End:
      gdata->updateActiveChunkTime(gdata->rightTime());
    break;
  case Qt::Key_Comma:
      view->setViewOffset(view->viewOffset() - view->viewWidth()/20.0);
      view->doSlowUpdate();
      break;
  case Qt::Key_Period:
      view->setViewOffset(view->viewOffset() + view->viewWidth()/20.0);
      view->doSlowUpdate();
      break;
  case Qt::Key_Plus:
  case Qt::Key_Equal:
      emit zoomInPressed();
      //view->viewZoomInX();
      //view->viewZoomInY();
      break;
  case Qt::Key_Minus:
      emit zoomOutPressed();
      //view->viewZoomOutX();
      //view->viewZoomOutY();
      break;
  default:
    e->ignore();
  }
}

void MainWindow::openFile()
{
/*
  OpenDialog d(this);
  if(d.exec() != QDialog::Accepted) return;
  QString s = QString(d.selectedFile());
*/
  //QString s = OpenDialog::getOpenWavFileName(this);
  QString lastFolder = QDir::convertSeparators(gdata->qsettings->value("Dialogs/openFilesFolder", QDir::currentDirPath()).toString());
  QString s = QFileDialog::getOpenFileName(this, "Open File", lastFolder, "Sounds (*.wav)");
  if(s.isEmpty()) return;
  s = QDir::convertSeparators(s);
  gdata->qsettings->setValue("Dialogs/openFilesFolder", s);
  openFile(s.latin1());
}

void MainWindow::openFile(const char *filename)
{
  SoundFile *newSoundFile = new SoundFile();
  if(!newSoundFile->openRead(filename)) {
    fprintf(stderr, "Error opening %s\n", filename);
    delete newSoundFile;
    return;
  }
  
  newSoundFile->preProcess();
  gdata->updateViewLeftRightTimes();
  
  gdata->addFileToList(newSoundFile);
  gdata->setActiveChannel(newSoundFile->channels(0));
  //gdata->view->setCurrentTime(0.0);
  QApplication::postEvent(mainWindow, new QCustomEvent(UPDATE_SLOW));
  //gdata->view->doFastUpdate();
  gdata->view->doUpdate();
  //playSound();
}

void MainWindow::openRecord()
{
  openRecord(false);
}

void MainWindow::openPlayRecord()
{
  openRecord(true);
}

void MainWindow::openRecord(bool andPlay)
{
  if(gdata->running) {
    gdata->stop();
    return;
  }
	
  SoundFile *playSoundFile = (andPlay) ? gdata->getActiveSoundFile() : NULL;

  //int rate = gdata->settings.getInt("Sound", "sampleRate");
  int rate = gdata->qsettings->value("Sound/sampleRate", 44100).toInt();
  //QString numberOfChannels = gdata->settings.getString("Sound", "numberOfChannels");
  QString numberOfChannels = gdata->qsettings->value("Sound/numberOfChannels", "mono").toString();
  int channels;
  if(numberOfChannels.lower() == "mono") channels = 1;
  else channels = 2;
  //int bits = gdata->settings.getInt("Sound", "bitsPerSample");
  int bits = gdata->qsettings->value("Sound/bitsPerSample", 16).toInt();
  int windowSize = gdata->getAnalysisBufferSize(rate);
  int stepSize = gdata->getAnalysisStepSize(rate);

  //If playing and recording then overide the record settings to the same as the playing file
  if(playSoundFile) {
    rate = playSoundFile->rate();
    channels = playSoundFile->numChannels();
    bits = playSoundFile->bits();
    windowSize = playSoundFile->bufferSize();
    stepSize = playSoundFile->bufferSize()/2;
  }

  //QString tempFileFolder = gdata->settings.getString("General", "tempFilesFolder");
  QString tempFileFolder = gdata->qsettings->value("General/tempFilesFolder", QDir::convertSeparators(QDir::currentDirPath())).toString();
  QDir dir = QDir(tempFileFolder);
  QFileInfo fileInfo; //(dir, fileName);
  QString fileName;
  bool fileExists;
  do {
    fileExists = false;
    fileName = gdata->getFilenameString();
    fileInfo.setFile(dir, fileName);
    if(fileInfo.exists()) {
      fileExists = true;
      //int fileGeneratingNumber = gdata->settings.getInt("General", "fileGeneratingNumber");
      int fileGeneratingNumber = gdata->qsettings->value("General/fileGeneratingNumber", 1).toInt();
      //gdata->settings.setInt("General", "fileGeneratingNumber", fileGeneratingNumber+1);
      gdata->qsettings->setValue("General/fileGeneratingNumber", fileGeneratingNumber+1);
    }
      /*
      if(QMessageBox::question(this, tr("Overwrite File?"),
                                //tr("A file called %1 already exists.\n Do you want to overwrite it?").arg( filename ),
                                tr("A file called %1 already exists.\n Do you want to overwrite it?").arg( fileInfo.fileName() ),
                                tr("&Yes"), tr("&No"), QString::null, 0, 1 )) {
          return;
      }
      */
  } while(fileExists);
  
  SoundFile *newSoundFile = new SoundFile();
  QString newFilename(fileInfo.absFilePath().latin1());
//#ifdef WINDOWS
//  newFilename.replace(QChar('/'), QChar('\\'));
//#endif
  newFilename = QDir::convertSeparators(newFilename);
  if(!newSoundFile->openWrite(newFilename, rate, channels, bits, windowSize, stepSize)) {
    //fprintf(stderr, "Error opening %s for writing\n", fileInfo.absFilePath().latin1());
    //QString s = QString("Error opening ") + newFilename + QString(" for writing.\nPossible cause, folder is read-only. Please select a writable Temp Folder");
    //QMessageBox::warning(mainWindow, "Error", s, QMessageBox::Ok, QMessageBox::NoButton);
    //menuPreferences();
    delete newSoundFile; newSoundFile = NULL;
    return;
  }
  
  if(!gdata->openPlayRecord(newSoundFile, playSoundFile)) {
    gdata->closeFile(newSoundFile, GData::NEVER_SAVE/*, false*/);
    QMessageBox::warning(mainWindow, "Error", QString("Error opening sound device for recording"), QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }
  gdata->addFileToList(newSoundFile);
  gdata->view->setCurrentTime(0.0);
  gdata->setActiveChannel(newSoundFile->channels(0));

  //int fileGeneratingNumber = gdata->settings.getInt("General", "fileGeneratingNumber");
  int fileGeneratingNumber = gdata->qsettings->value("General/fileGeneratingNumber", 1).toInt();
  //gdata->settings.setInt("General", "fileGeneratingNumber", fileGeneratingNumber+1);
  gdata->qsettings->setValue("General/fileGeneratingNumber", fileGeneratingNumber+1);
}

void MainWindow::windowMenuAboutToShow()
{
  windowMenu->clear();

  QWidgetList windows = theWorkspace->windowList();
  for ( int i = 0; i < int(windows.count()); ++i ) {
  	int id = windowMenu->insertItem(windows.at(i)->caption(),
				 this, SLOT( windowMenuActivated( int ) ) );
  	windowMenu->setItemParameter( id, i );
  	windowMenu->setItemChecked( id, theWorkspace->activeWindow() == windows.at(i) );
  }
  
  windowMenu->insertSeparator();
  int cascade = windowMenu->insertItem( "&Cascade", theWorkspace, SLOT( cascade() ), 0 );
  //int tile = windowMenu->insertItem( "&Tile", theWorkspace, SLOT( tile() ), 0 );
  int close = windowMenu->insertItem( "Close &All", this, SLOT( closeAllWidgets() ), 0 );

  if (windows.isEmpty()) {
    windowMenu->setItemEnabled( cascade, false );
  	//windowMenu->setItemEnabled( tile, false );
  	windowMenu->setItemEnabled( close, false );
  }
}

void MainWindow::windowMenuActivated( int id )
{
    QWidget* w = theWorkspace->windowList().at( id );
    if ( w ) {
    	w->showNormal();
      w->setFocus();
    }
}

void MainWindow::message(QString s, int msec)
{
  QStatusBar *sb = statusBar();
  if(sb) sb->message(s, msec);
}

/*
void MainWindow::toggleOption(int id)
{
  if(optionsMenu->isItemChecked(id)) {
    optionsMenu->setItemChecked(id, false);
  } else {
	  optionsMenu->setItemChecked(id, true);
  }
    if(id == 0) gdata->equalLoudness = !(gdata->equalLoudness);
    else if(id == 1) gdata->useMasking = !(gdata->useMasking);
    //else if(id == 2) gdata->useRidgeFile = !(gdata->useRidgeFile);
    else if(id == 3) {
	if(optionsMenu->isItemChecked(id))
	  if(gdata->in_channels < 2) {
	    optionsMenu->setItemChecked(id, false);
		  gdata->process_channels = 1;
	  } else
		  gdata->process_channels = 2;
	else
	  gdata->process_channels = 1;
  }
}
*/

void MainWindow::closeAllWidgets()
{
  QWidgetList opened = theWorkspace->windowList();
  for(QWidgetList::iterator it = opened.begin(); it < opened.end(); it++)
    (*it)->close();
}

void MainWindow::menuPreferences()
{
	//Ui_SettingsDialog *settings = new Ui_SettingsDialog(this, "Settings", false, Qt::WDestructiveClose);
  TartiniSettingsDialog *settings = new TartiniSettingsDialog(this);
  //SettingsDialog *settings = new SettingsDialog(this);
	settings->show();
}


/**
 * Opens a view based on a viewId (which should be #defined).
 * All widgets should be created anonymously - ie no saving pointers!
 *
 * @param viewID the view to open's ID.
 */
QWidget *MainWindow::openView(int viewID)
{
  QWidget *w = NULL;
  int useTopLevelWidgets = gdata->qsettings->value("Display/useTopLevelWidgets", false).toBool();
  QWidget *parent = (useTopLevelWidgets) ? NULL : theWorkspace;

  switch(viewID) {
    case VIEW_OPEN_FILES:
      w = new OpenFiles(viewID, parent);
    break;
    case VIEW_FREQ:
	  {
        FreqView *freqView = new FreqView(viewID, parent);
        //freqView->show();
        connect(this, SIGNAL(zoomInPressed()), freqView, SLOT(zoomIn()));
        connect(this, SIGNAL(zoomOutPressed()), freqView, SLOT(zoomOut()));
        w = freqView;
	  }
	  break;
    case VIEW_SUMMARY:
      w = new SummaryView( viewID, parent);
      break;
   case VIEW_PITCH_COMPASS:
      w = new PitchCompassView(viewID, parent);
      break;
   case VIEW_VOLUME_METER:
      w = new VolumeMeterView(viewID, parent);
      break;
   case VIEW_TUNER:
      w = new TunerView(viewID, parent);
      break;
   case VIEW_HBLOCK:
      w = new HBlockView(viewID, parent);
      break;
   case VIEW_HSTACK:
      w = new HStackView(viewID, parent);
      break;
   case VIEW_HBUBBLE:
      w = new HBubbleView(viewID, parent);
      break;
   case VIEW_HCIRCLE:
      w = new HCircleView(viewID, parent);
      break;
   case VIEW_WAVE:
      w = new WaveView(viewID, parent);
      break;
   case VIEW_PIANO:
      w = new PianoView(viewID, parent);
      break;
   case VIEW_HTRACK:
      w = new HTrackView(viewID, parent);
      break;
   case VIEW_CORRELATION:
      w = new CorrelationView(viewID, parent);
      break;
   case VIEW_FFT:
      w = new FFTView(viewID, parent);
      break;
   case VIEW_CEPSTRUM:
      w = new CepstrumView(viewID, parent);
      break;
   case VIEW_DEBUGVIEW:
      w = new DebugView(viewID, parent);
      break;
   case VIEW_SCORE:
      w = new ScoreView(viewID, parent);
      break;
   case VIEW_VIBRATO:
      w = new VibratoView(viewID, parent);
      break;

   //default:
    	//return;
  }
  if(w) {
    if(parent) theWorkspace->addWindow(w);
    w->show();
  }
  return w;
}



void MainWindow::newViewAboutToShow()
{
  newViewMenu->clear();

  QMenu *technicalMenu = new QMenu("Technical");
  QMenu *experimentalMenu = new QMenu("Experimental");
  QMenu *otherMenu = new QMenu("Other");

  QWidgetList opened = theWorkspace->windowList();

  for(int j=0; j < NUM_VIEWS; j++) {
    QAction *action;
	if(viewData[j].menuType == 0) action = newViewMenu->addAction(viewData[j].menuName);
	else if(viewData[j].menuType == 1) action = technicalMenu->addAction(viewData[j].menuName);
	else if(viewData[j].menuType == 2) action = experimentalMenu->addAction(viewData[j].menuName);
  else if(viewData[j].menuType == 3) action = otherMenu->addAction(viewData[j].menuName);
	else continue;

	connect(action, SIGNAL(triggered()), createSignalMapper, SLOT(map()));
	createSignalMapper->setMapping(action, j);
	for(QWidgetList::iterator it=opened.begin(); it<opened.end(); it++) {
	  if(QString((*it)->metaObject()->className()) == viewData[j].className) {
		action->setEnabled(false);
		break;
	  }
	}
  }
  newViewMenu->addSeparator();
  newViewMenu->addMenu(technicalMenu);
  newViewMenu->addMenu(experimentalMenu);
  newViewMenu->addMenu(otherMenu);
}

void MainWindow::setTimeLabel(double t)
{
  char temp[128];
  if(timeLabel) {
    int sign = (t < 0) ? -1 : 1;
    t = fabs(t);
    int mins = int(floor(t)) / 60;
    //double secs = t - double(mins * 60);
    int secs = int(floor(t - double(mins * 60)));
    int milliSecs = int(floor((t - double(mins * 60) - double(secs))*1000.0));
    //sprintf(temp, "<qt><nobr><b><big>Time: %s%02d:%02d</big>.%03d</b></nobr></qt>", (sign == -1) ? "-" : " ", mins, secs, milliSecs);
    sprintf(temp, "Time: %s%02d:%02d.%03d", (sign == -1) ? "-" : " ", mins, secs, milliSecs);
    timeLabel->setText(QString(temp));
  }
}

void MainWindow::setChunkLabel()
{
  char temp[128];
  Channel *active = gdata->getActiveChannel();
  if(active) {
    sprintf(temp, "Chunk: %d", active->currentChunk());
  } else {
    sprintf(temp, "Chunk: 0");
  }
  chunkLabel->setText(QString(temp));
}

void MainWindow::setNoteLabel()
{
  char temp[128];
  Channel *active = gdata->getActiveChannel();
  if(active) {
    active->lock();
    AnalysisData *data = active->dataAtCurrentChunk();
    if(data && active->isVisibleNote(data->noteIndex) && active->isLabelNote(data->noteIndex)) sprintf(temp, "Note: %d", data->noteIndex);
    else sprintf(temp, "Note:    ");
    active->unlock();
  } else {
    sprintf(temp, "Note:    ");
  }
  noteLabel->setText(QString(temp));
}

void MainWindow::setTimeRange(double min_, double max_)
{
  //if(timeSlider) timeSlider->setRange(min_, max_, timeSlider->step(), timeSlider->pageSize());
  if(timeSlider) timeSlider->setRange(min_, max_, timeSlider->step(), 1000);
}

/*
void MainWindow::setPageStep(double step_)
{
  if(timeSlider) {
    //timeSlider->setRange(timeSlider->minValue(), timeSlider->maxValue(), timeSlider->step(), toInt(step_ / timeSlider->step() / 4.0));
    //timeSlider->setRange(timeSlider->minValue(), timeSlider->maxValue(), timeSlider->step(), 100);
  }
}
*/

void MainWindow::rewindPressed()
{
  gdata->rewind();
  rewindTimer->start(gdata->fastUpdateSpeed()); //every 0.2 seconds
}

void MainWindow::rewindReleased()
{
  rewindTimer->stop(); //every 0.2 seconds
}

void MainWindow::playStopClicked()
{
  if(gdata->running) {
    gdata->stop();
  } else {
    gdata->play();
  }  
}

/*
void MainWindow::playNote()
{
  if(gdata->running) {
    gdata->stop();
    //playStopButton->setIconSet(*playIconSet);
  } else {
    if(gdata->play()) {
      playStopButton->setIconSet(*stopIconSet);
    }
  }  
}
*/

void MainWindow::fastforwardPressed()
{
  gdata->fastforward();
  fastforwardTimer->start(gdata->fastUpdateSpeed()); //every 0.2 seconds
}

void MainWindow::fastforwardReleased()
{
  fastforwardTimer->stop(); //every 0.2 seconds
}

void MainWindow::setTitle(Channel *ch)
{
#ifdef MYDEBUG
  if(ch) setCaption(QString("Tartini (debug) - ") + ch->getUniqueFilename());
  else setCaption(QString("Tartini (debug)"));
#else
  if(ch) setCaption(QString("Tartini - ") + ch->getUniqueFilename());
  else setCaption(QString("Tartini"));
#endif
}

void MainWindow::aboutTartini()
{
  TartiniDialog *tartiniDialog = new TartiniDialog(this);
  tartiniDialog->exec();
  delete tartiniDialog;
}

void MainWindow::aboutGPL()
{
  GPLDialog *gplDialog = new GPLDialog(this);
  gplDialog->exec();
  delete gplDialog;
}

void MainWindow::aboutQt()
{
  QMessageBox::aboutQt(this, "About Qt");
}

void MainWindow::showDocumentation()
{
  QDesktopServices::openUrl(QUrl("http://www.tartini.net/doc"));
}

TartiniDialog::TartiniDialog(QWidget *parent) : QDialog(parent, NULL, true)
{
  setCaption("About Tartini - Version " TARTINI_VERSION_STR);
  setBackgroundColor(Qt::black);
  Q3BoxLayout *layout = new Q3VBoxLayout(this, 6, 6);

  Q3BoxLayout *topLayout = new Q3HBoxLayout(layout);
  QLabel *tartiniPicture = new QLabel(this, "TartiniPicture");
  //QPixmap *tartiniPixmap = new QPixmap();
  QPixmap tartiniPixmap(":/res/tartinilogo.png", "PNG");
  tartiniPicture->setPixmap(tartiniPixmap);
  tartiniPicture->setBackgroundColor(Qt::black);
  topLayout->addStretch(10);
  topLayout->addWidget(tartiniPicture);
  topLayout->addStretch(10);

  //QTextEdit *tartiniTextEdit = new QTextEdit(this, "TartiniTextEdit");
  QTextBrowser *tartiniTextEdit = new QTextBrowser(this, "TartiniTextEdit");
  //tartiniTextEdit->setReadOnly(true);
  tartiniTextEdit->setOpenExternalLinks(true);
  tartiniTextEdit->setTextFormat(Qt::RichText);

  QColor linkColor(Qt::cyan);
  QString sheet = QString::fromLatin1("a { text-decoration: underline; color: %1 }").arg(linkColor.name());
  tartiniTextEdit->document()->setDefaultStyleSheet(sheet);

  tartiniTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

  //tartiniTextEdit->setText(
  tartiniTextEdit->setHtml(
    /*"<qt text=\"white\">*/"Tartini is a graphical tool for analysing the music of solo instruments.<br>"
    "This program was created by Philip McLeod as part of PhD work at the University of Otago, New Zealand<br>"
    "You can find the latest info about Tartini at <a href=\"http://www.tartini.net\">http://www.tartini.net</a><br><br>"
    "Copyright 2002-2007 Philip McLeod (pmcleod@cs.otago.ac.nz).<br><br>"

    "New features in version 1.2 include:<br>"
    "- The ability to set an offset frequency. i.e. not fixed to A=440 Hz<br>"
    "- Vertical reference lines<br>"
    "- Fixed a bug in note detection which caused a number of pitch detection problems<br><br>"

    "New features in version 1.1 include:<br>"
    "- A vibrato analysis tool<br>"
    "- Full duplex sound, allowing play and record at the same time<br>"
    "- Choice of musical scales<br>"
    "- Some basic printing abilities (pitch view only)<br>"
    "- Some basic exporting abilities (plain text or matlab code)<br>"
    "- A refined pitch algorithm<br>"
    "- A musical score generator (still in very early stages of development)<br><br>"

    "<i>Thanks to:-</i><br>"
    "Prof. Geoff Wyvill - PhD Supervisor (Computer Science)<br>"
    "Dr. Donald Warrington - PhD Supervisor (Physics)<br>"
    "Rob Ebbers - Programming<br>"
    "Maarten van Sambeek - Programming<br>"
    "Stuart Miller - Programming<br>"
    "Mr Kevin Lefohn - Support (Violin)<br>"
    "Miss Judy Bellingham - Support (Voice)<br>"
    "Jean-Philippe Grenier - Logo design<br>"
    "People at my lab, especially Alexis Angelidis, Sui-Ling Ming-Wong, Brendan McCane and Damon Simpson<br><br>"
    "Tartini version " TARTINI_VERSION_STR " is released under the GPL license <a href=\"http://www.gnu.org/licenses/gpl.html\">http://www.gnu.org/licenses/gpl.html</a>.<br><br>"

    "<i>Libraries used by tartini include:</i><br>"
    "Qt - <a href=\"http://www.trolltech.com/qt\">http://www.trolltech.com/qt</a><br>"
    "FFTW - <a href=\"http://www.fftw.org\">http://www.fftw.org</a><br>"
    "Qwt - <a href=\"http://qwt.sourceforge.net\">http://qwt.sourceforge.net</a><br>"
    "RtAudio - <a href=\"http://www.music.mcgill.ca/~gary/rtaudio/index.html\">http://www.music.mcgill.ca/~gary/rtaudio/index.html</a><br>"
    "<br>"
    "This program is free software; you can redistribute it and/or modify "
    "it under the terms of the GNU General Public License as published by "
    "the Free Software Foundation; either version 2 of the License, or "
    "(at your option) any later version.<br><br>"
    "This program is distributed in the hope that it will be useful, "
    "but WITHOUT ANY WARRANTY; without even the implied warranty of "
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
    "GNU General Public License for more details.<br><br>"
    "You should have received a copy of the GNU General Public License "
    "along with this program; if not, write to the Free Software "
    "Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.<br><br>"
    "Please click 'Read License' or see LICENSE.txt for details.<br>");//</qt>");

  QPalette pal;
  pal = palette();
  pal.setColor(QPalette::Base, Qt::black);
  pal.setColor(QPalette::Text, Qt::white);
  tartiniTextEdit->setPalette(pal);
  
  layout->addWidget(tartiniTextEdit);
  
  Q3BoxLayout *bottomLayout = new Q3HBoxLayout(layout);
  QPushButton *okButton = new QPushButton("&Ok", this);
  /*okButton->setBackgroundColor(Qt::black);
  pal = palette();
  okButton->setPalette(pal);*/
  QPushButton *GPLButton = new QPushButton("&Read License", this);
  //GPLButton->setBackgroundColor(Qt::black);
  //GPLButton->setPalette(pal);
  bottomLayout->addStretch(10);
  bottomLayout->addWidget(okButton);
  bottomLayout->addStretch(10);
  bottomLayout->addWidget(GPLButton);
  bottomLayout->addStretch(10);
  setFocusProxy(okButton);
  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
  connect(GPLButton, SIGNAL(clicked()), mainWindow, SLOT(aboutGPL()));
}

GPLDialog::GPLDialog(QWidget *parent) : QDialog(parent, NULL, true)
{
  setCaption("GPL Licence");
  Q3BoxLayout *layout = new Q3VBoxLayout(this, 6, 6);

  Q3TextEdit *GPLTextEdit = new Q3TextEdit(this, "GPLTextEdit");
  GPLTextEdit->setReadOnly(true);
  GPLTextEdit->setTextFormat(Qt::PlainText);
  QString theText;
  {
    QFile theFile(":res/LICENSE.txt");
/*#ifdef MACX
    QFile theFile(macxPathString + QString("Contents/Resources/LICENSE.txt"));
#else
    QFile theFile("LICENSE.txt");
#endif*/
    theFile.open(QIODevice::ReadOnly);
    QTextStream textStream(&theFile);
    theText = textStream.read();
    theFile.close();
  }
  GPLTextEdit->setText(theText);
  
  layout->addWidget(GPLTextEdit);
  
  Q3BoxLayout *bottomLayout = new Q3HBoxLayout(layout);
  QPushButton *okButton = new QPushButton("&Ok", this);
  bottomLayout->addStretch(10);
  bottomLayout->addWidget(okButton);
  bottomLayout->addStretch(10);
  setFocusProxy(okButton);
  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
}

#include "freqdrawwidget.h"
#include "drawwidget.h"

void MainWindow::printPitch()
{
  QPrinter printer(QPrinter::HighResolution);
  printer.setOrientation(QPrinter::Landscape);
  QPrintDialog printDialog(&printer, this);
  if(printDialog.exec() == QDialog::Accepted) {
    View *view = gdata->view;
    //printf("starting printing\n");
    QPainter p;
    p.begin(&printer);
    int w = printer.width();
    int h = printer.height();
    //printf("printer width = %d\n", w);
    //printf("printer height = %d\n", h);
    double leftTime = view->viewLeft();
    double rightTime = view->viewRight();//gdata->totalTime();
    double viewBottom = view->viewBottom();
    double viewTop = view->viewTop();//gdata->topPitch();
    double zoomX = (rightTime-leftTime) / double(w);
    double zoomY = (viewTop-viewBottom) / double(h);
    FreqDrawWidget::drawReferenceLines(printer, p, leftTime, 0.0, zoomX, viewBottom, zoomY, DRAW_VIEW_PRINT);

    double dotsPerLineStepY = 1.0 / zoomY;
    double dotsPerMM = double(printer.height()) / double(printer.heightMM()); /*printer.resolution()*/
    //printf("heightMM = %d\n",printer. heightMM());
    //printf("dotsPerMM = %f\n", dotsPerMM);
    DrawWidget::setLineWidth(toInt(std::min(dotsPerMM*1.0, dotsPerLineStepY*0.2))); //1mm thick line
    //draw all the visible channels
    for (uint i = 0; i < gdata->channels.size(); i++) {
      Channel *ch = gdata->channels.at(i);
      if(!ch->isVisible()) continue;
      DrawWidget::drawChannel(printer, ch, p, leftTime, 0.0, zoomX, viewBottom, zoomY, DRAW_VIEW_PRINT);
      if(ch == gdata->getActiveChannel()) {
        p.setPen(Qt::black);
        QString s = ch->getUniqueFilename();
        QFont f = p.font();
        f.setPointSize(18);
        p.setFont(f);
        p.drawText(QRect(0, 0, printer.width(), printer.height()), Qt::AlignHCenter | Qt::AlignTop, s);
      }
    }

    p.end();
    //printf("finished printing\n");
  }
}

void MainWindow::exportChannel(int type, QString typeString)
{
  Channel *ch = gdata->getActiveChannel();
  if(ch) {
    ch->exportChannel(type, typeString);
  } else {
    fprintf(stderr, "No channel to export.\n");
  }
}

void MainWindow::exportChannelPlainText()
{
  exportChannel(0, "Text (*.txt)");
}

void MainWindow::exportChannelMatlab()
{
  exportChannel(1, "Matlab code (*.m)");
}

bool MainWindow::loadViewGeometry()
{
  QSettings *s = gdata->qsettings;
  QPoint pos;
  QSize size;
  int counter = 0;

  for(int j=0; j<NUM_VIEWS; j++) {
    QString base = QString("geometry/") + viewData[j].className;
    if(s->value(base+"/visible", false).toBool() == true) {
      pos = s->value(base+"/pos", QPoint(0, 0)).toPoint();
      size = s->value(base+"/size", QSize(100, 100)).toSize();
      QWidget *w = openView(j);
      QWidget *p = (QWidget*)(w->parent()); //get the subwindow frame
      if(p) {
        p->resize(size);
        p->move(pos);
      }
      counter++;
    }
  }
  if(counter == 0) {
    openView(VIEW_OPEN_FILES);
    openView(VIEW_FREQ);
    openView(VIEW_TUNER);
    openView(VIEW_VIBRATO);
    return false;
  }
  return true;
}

void MainWindow::saveViewGeometry()
{
  QSettings *s = gdata->qsettings;

  QWidgetList opened = theWorkspace->windowList();

  for(int j=0; j<NUM_VIEWS; j++) {
    QString base = QString("geometry/") + viewData[j].className;
    bool found = false;
    for(QWidgetList::iterator it=opened.begin(); it<opened.end(); it++) {
      if(QString((*it)->metaObject()->className()) == viewData[j].className) {
        QWidget *p = (QWidget*)((*it)->parent()); //get the subwindow frame
        if(!p) break;
        s->setValue(base+"/visible", true);
        s->setValue(base+"/pos", p->pos());
        s->setValue(base+"/size", p->size());
        //printf("pos.x=%d, pos.y=%d\n", p->pos().x(), p->pos().y());
        found = true;
        break;
      }
    }
    if(!found) {
      s->setValue(base+"/visible", false);
      //s->setValue(base+"/pos", QPoint(0, 0));
      //s->setValue(base+"/size", QSize(100, 100));
    }
  }
}
