/***************************************************************************
                          opendialog.cpp  -  description
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

#include "opendialog.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qcheckbox.h>
//Added by qt3to4:
//#include <Q3VBoxLayout>
#include <QVBoxLayout>
#include "gdata.h"

OpenDialog::OpenDialog(QWidget * parent)
// : Q3FileDialog(QDir::convertSeparators(gdata->settings.getString("Dialogs", "openFilesFolder")),
// : QFileDialog(QDir::convertSeparators(gdata->qsettings->value("Dialogs/openFilesFolder", QDir::currentDirPath()).toString()),
 : QFileDialog(parent, "Open File", QDir::convertSeparators(gdata->qsettings->value("Dialogs/openFilesFolder", QDir::currentDirPath()).toString()),
 "Wave files (*.wav)")
{
  setCaption("Choose a file to open");
  setMode(QFileDialog::ExistingFile);

/*
  QWidget *baseWidget = new QWidget(this);
  addWidgets(NULL, baseWidget, NULL);
  Q3BoxLayout *baseLayout = new QVBoxLayout(baseWidget);

  rememberFolderCheckBox = new QCheckBox("Remember current folder", baseWidget);
  rememberFolderCheckBox->setChecked(gdata->qsettings->value("Dialogs/rememberOpenFolder", true).toBool());
  baseLayout->addSpacing(10);
  baseLayout->addWidget(rememberFolderCheckBox);
*/
}

OpenDialog::~OpenDialog()
{
}

void OpenDialog::accept()
{
/*
  bool remember = rememberFolderCheckBox->isChecked();
  //gdata->settings.setBool("Dialogs", "rememberOpenFolder", remember);
  gdata->qsettings->setValue("Dialogs/rememberOpenFolder", remember);
  if(remember == true) {
    const QDir *curDir = dir();
    //gdata->settings.setString("Dialogs", "openFilesFolder", curDir->absPath());
    gdata->qsettings->setValue("Dialogs/openFilesFolder", curDir->absPath());
    delete curDir;
  }
*/
  QFileDialog::accept();
}

QString OpenDialog::getOpenWavFileName(QWidget *parent)
{
  OpenDialog d(parent);
  if(d.exec() != QDialog::Accepted) return QString();
  return d.selectedFile();
}
