/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : May 2002
    copyright            : (C) 2002-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include "main.h"

#ifdef MACX
#include <CoreFoundation/CoreFoundation.h>
QString macxPathString;
#endif

//#include <qtimer.h>
//#include <stdio.h>
#include "gdata.h"
#include "myassert.h"
#include "myglfonts.h"

//there is some compiler bug which initilises the colors wrong
//So we fix them with a cheap hack
#ifdef MACX
void fixColours()
{
/*
	Qt::black =       QColor(   0,   0,   0 );   // index 0     black
  Qt::white =       QColor( 255, 255, 255 );   // index 255   white
  Qt::darkGray =    QColor( 128, 128, 128 );   // index 248   dark gray
  Qt::gray =        QColor( 160, 160, 164 );   // index 247   gray
  Qt::lightGray =   QColor( 192, 192, 192 );   // index 7     light gray
  Qt::red =         QColor( 255,   0,   0 );   // index 249   red
  Qt::green =       QColor(   0, 255,   0 );   // index 250   green
  Qt::blue =        QColor(   0,   0, 255 );   // index 252   blue
  Qt::cyan =        QColor(   0, 255, 255 );   // index 254   cyan
  Qt::magenta =     QColor( 255,   0, 255 );   // index 253   magenta
  Qt::yellow =      QColor( 255, 255,   0 );   // index 251   yellow
  Qt::darkRed =     QColor( 128,   0,   0 );   // index 1     dark red
  Qt::darkGreen =   QColor(   0, 128,   0 );   // index 2     dark green
  Qt::darkBlue =    QColor(   0,   0, 128 );   // index 4     dark blue
  Qt::darkCyan =    QColor(   0, 128, 128 );   // index 6     dark cyan
  Qt::darkMagenta = QColor( 128,   0, 128 );   // index 5     dark magenta
  Qt::darkYellow =  QColor( 128, 128,   0 );   // index 3     dark yellow	
*/
}
#endif

int main( int argc, char **argv )
{

#ifdef WINDOWS
	freopen("stdout.txt", "w", stdout);
	freopen("stderr.txt", "w", stderr);
#endif
	
#ifdef MACX
	fixColours();
  
  CFURLRef pluginRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
  CFStringRef macPath = CFURLCopyFileSystemPath(pluginRef, kCFURLPOSIXPathStyle);
  const char *pathPtr = CFStringGetCStringPtr(macPath, CFStringGetSystemEncoding());
  macxPathString = QString(pathPtr);
  if(!macxPathString.endsWith("/")) macxPathString.append('/');
  CFRelease(pluginRef);
  CFRelease(macPath);
#endif
	
  QApplication a( argc, argv );
  Q_INIT_RESOURCE(pitch);

  fprintf(stderr, "QT_VERSION_STR=%s\n", QT_VERSION_STR);
  fprintf(stderr, "QT_VERSION=%x\n", QT_VERSION);

  //Create one instance only of the global data
  gdata = new GData();
  /* The view needs to be created here, not in GData's constructor because of the View's
  autofollow method and the signals in the View class. It can try to reference the GData 
  object before the constructor finishes, which causes an access violation in 
  Visual Studio 6. */
  gdata->view = new View();

  //mainWindow = new MainWindow();
  //mainWindow->show();
  mainWindow = new MainWindow();
  //a.setStyle("Windows");

  //call init after we know the windows size
  gdata->view->init();

	mainWindow->showMaximized();
    
  a.setMainWidget(mainWindow);
  mainWindow->show();

  a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );

  if(!mainWindow->loadViewGeometry()) {
    mainWindow->aboutTartini(); //open the tartini dialog on first time open
  }

  //open any files on the command line
  if(argc >= 2) {
    for(int j=1; j<argc; j++) mainWindow->openFile(argv[j]);
  }
  
  int ret = a.exec();
    
  delete gdata->view;
  delete gdata;
  
  return ret;
}
