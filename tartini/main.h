/***************************************************************************
                          main.h  -  description
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

#include <stdio.h>
#include <stdlib.h>


#include "mainwindow.h"


#include <qapplication.h>


#ifdef MACX
#include <qstring.h>
extern QString macxPathString;
#endif

//extern QColor stdcol[19];
//extern MainWindow *mainWindow;

/*
class PhilApp : public QApplication
{
  Q_OBJECT

public:
  PhilApp();
  PhilApp(int argc, char **argv) : QApplication(argc, argv) {}

protected:
  //void customEvent( QCustomEvent * e ) {
    bool event( QEvent * e ) {
    //if(e->type() == QEvent::User+1) {
    //    m->doStuff();
    //    return true;
    //} else
    return QApplication::event(e);
    }
};
*/
