/***************************************************************************
                          baseview.cpp  -  description
                             -------------------
    begin                : May 18 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

/** To create a new tartini widget
    1. Copy the base folder (and files)
    2. Rename the base* files to myclass* files, e.g. mv baseview.cpp myclassview.cpp
    3. Change the class names to MyClassView & MyClassWidget (where MyClass is the name of your new class)
    4. In MainWindow.h increment NUM_VIEWS and add to the enum VIEW_MYCLASS
    5. Add a ViewData item to viewData in MainWindow.cpp
    6. Add #include "myclassview.h" near top of MainWindow.cpp
    7. In MainWindow::openView(int viewID) add a case to create your class
    8. Add your files names and folder names into the pitch.pro (HEADERS, SOURCES and MYPATHS respectivly)
    9. Done. Just rebuild the project (regenerating the Makefile)
*/

#include "baseview.h"
#include "basewidget.h"
#include "gdata.h"

BaseView::BaseView( int viewID_, QWidget *parent )
 : ViewWidget( viewID_, parent)
{
  //create a drawing object
  baseWidget = new BaseWidget(this);
  baseWidget->show();

  //make any connections
  connect(gdata->view, SIGNAL(onFastUpdate(double)), baseWidget, SLOT(update()));
}

BaseView::~BaseView()
{
  delete baseWidget;
}

void BaseView::resizeEvent(QResizeEvent *)
{
  baseWidget->resize(size());
}
