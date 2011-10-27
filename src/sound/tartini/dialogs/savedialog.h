/***************************************************************************
                          savedialog.h  -  description
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

#ifndef SAVE_DIALOG_H
#define SAVE_DIALOG_H

#include <q3filedialog.h>

class QCheckBox;

extern const char *windowSizeBoxStr[];

class SaveDialog : public Q3FileDialog
{
public:
  SaveDialog(/*const QString & dirName, const QString & filter = QString::null, */QWidget * parent = 0);
  ~SaveDialog();
  void accept();

  static QString getSaveWavFileName(QWidget *parent = 0);
  
  QCheckBox *rememberFolderCheckBox;
  QCheckBox *appendWavCheckBox;
};

#endif
