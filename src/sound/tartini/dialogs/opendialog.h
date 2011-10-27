/***************************************************************************
                          opendialog.h  -  description
                             -------------------
    begin                : Feb 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef OPENDIALOG_H
#define OPENDIALOG_H

//#include <q3filedialog.h>
#include <QFileDialog>

class QCheckBox;

class OpenDialog : public QFileDialog
{
public:
  OpenDialog(/*const QString & dirName, const QString & filter, */QWidget * parent);
  ~OpenDialog();
  void accept();
  
  static QString getOpenWavFileName(QWidget *parent = 0);
  
  QCheckBox *rememberFolderCheckBox;
};

#endif
