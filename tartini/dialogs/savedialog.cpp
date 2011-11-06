/***************************************************************************
                          savedialog.cpp  -  description
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

#include "savedialog.h"
#include <qcheckbox.h>
#include <qlayout.h>
#include <qlabel.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include "gdata.h"

/*
const char *sampleRateStr[] = { "8000", "11025", "22050", "44100", "48000", "96000", NULL };
const char *channelsBoxStr[] = { "Mono", "Stereo", NULL };
const char *bitsBoxStr[] = { "8", "16", NULL };
const char *windowSizeBoxStr[] = { "64", "128", "256", "512", "1024", "2048", "4096", "8192", NULL };
*/

SaveDialog::SaveDialog(/*const QString & dirName, const QString & filter, */QWidget * parent)
// : Q3FileDialog(QDir::convertSeparators(gdata->settings.getString("Dialogs", "saveFilesFolder")),
 : Q3FileDialog(QDir::convertSeparators(gdata->qsettings->value("Dialogs/saveFilesFolder", QDir::currentDirPath()).toString()),
               "Wave files (*.wav)", parent, NULL, true)
{
  setCaption("Choose a filename to save under");
  setMode(Q3FileDialog::AnyFile);
  
  //QLabel* label = new QLabel( "Added widgets", this );
  //QLineEdit* lineedit = new QLineEdit( this );
  //addWidgets(label, lineedit, NULL);

        
/*  layout()->setDirection(QBoxLayout::TopToBottom);
  QBoxLayout *hBox1 = new QBoxLayout(this->layout(), QBoxLayout::LeftToRight);
  //addLayout(hBox1);
  QLabel *labelMono = new QLabel("Channels:", this);
  hBox1->addWidget(labelMono);
  channelsBox = new QComboBox(false, this, "Channels");
  channelsBox->insertStrList(channelsBoxStr);
  channelsBox->setCurrentItem(0);
  hBox1->addWidget(channelsBox);
  */
  /*
  //QHBox *hBox1 = new QHBox(this);
  //addWidgets(0, hBox1, 0);
  QLabel *labelMono = new QLabel("Channels:", this);
  //new QSpacerItem(0, 0);
  //QCheckBox *theCheckBox = new QCheckBox("Mono", hBox1);
  channelsBox = new QComboBox(false, this, "Channels");
  channelsBox->insertStrList(channelsBoxStr);
  channelsBox->setCurrentItem(0);
  addWidgets(labelMono, channelsBox, NULL);
    
  
  //QHBox *hBox2 = new QHBox(this);
  //addWidgets(0, hBox2, 0);
  QLabel *labelRate = new QLabel("Sample rate:", this);
  sampleRateBox = new QComboBox(true, this, "Sample rate");
  sampleRateBox->insertStrList(sampleRateStr);
  sampleRateBox->setCurrentItem(3);
  addWidgets(labelRate, sampleRateBox, NULL);

  //QHBox *hBox3 = new QHBox(this);
  //addWidgets(0, hBox3, 0);
  QLabel *labelBits = new QLabel("Bits:", this);
  bitsBox = new QComboBox(false, this, "bits");
  bitsBox->insertStrList(bitsBoxStr);
  bitsBox->setCurrentItem(1);
  addWidgets(labelBits, bitsBox, NULL);

  //QHBox *hBox4 = new QHBox(this);
  //addWidgets(0, hBox4, 0);
  QLabel *labelWindowSize = new QLabel("Window size:", this);
  windowSizeBox = new QComboBox(true, this, "Window size");
  windowSizeBox->insertStrList(windowSizeBoxStr);
  windowSizeBox->setCurrentItem(5);
  addWidgets(labelWindowSize, windowSizeBox, NULL);
*/
  QWidget *baseWidget = new QWidget(this);
  addWidgets(NULL, baseWidget, NULL);
  //QGridLayout *baseLayout = new QGridLayout(baseWidget, 1, 5, 0, 5);
  //QGridLayout *baseLayout = new QGridLayout(baseWidget, 2, 1);
  Q3BoxLayout *baseLayout = new Q3VBoxLayout(baseWidget);
/*
  baseLayout->addWidget(new QLabel("Channels:", baseWidget), 0, 0);
  channelsBox = new QComboBox(false, baseWidget, "Channels");
  channelsBox->insertStrList(channelsBoxStr);
  //channelsBox->setCurrentItem(0);
  channelsBox->setCurrentText(gdata->settings.getString("Sound", "numberOfChannels"));
  baseLayout->addWidget(channelsBox, 0, 1);
  
  baseLayout->addWidget(new QLabel("Sample rate:", baseWidget), 0, 3);
  sampleRateBox = new QComboBox(true, baseWidget, "Sample rate");
  sampleRateBox->insertStrList(sampleRateStr);
  //sampleRateBox->setCurrentItem(3);
  sampleRateBox->setCurrentText(gdata->settings.getString("Sound", "sampleRate"));
  baseLayout->addWidget(sampleRateBox, 0, 4);

  baseLayout->addWidget(new QLabel("Bits per sample:", baseWidget), 1, 0);
  bitsBox = new QComboBox(false, baseWidget, "bits");
  bitsBox->insertStrList(bitsBoxStr);
  //bitsBox->setCurrentItem(1);
  bitsBox->setCurrentText(gdata->settings.getString("Sound", "bitsPerSample"));
  baseLayout->addWidget(bitsBox, 1, 1);

  baseLayout->addWidget(new QLabel("Window size:", baseWidget), 1, 3);
  windowSizeBox = new QComboBox(true, baseWidget, "Window size");
  windowSizeBox->insertStrList(windowSizeBoxStr);
  windowSizeBox->setCurrentItem(5);
  baseLayout->addWidget(windowSizeBox, 1, 4);
*/
  appendWavCheckBox =      new QCheckBox("Append .wav extension if needed", baseWidget);
  rememberFolderCheckBox = new QCheckBox("Remember current folder", baseWidget);
  //appendWavCheckBox->setChecked(gdata->settings.getBool("Dialogs", "appendWav"));
  appendWavCheckBox->setChecked(gdata->qsettings->value("Dialogs/appendWav", true).toBool());
  //rememberFolderCheckBox->setChecked(gdata->settings.getBool("Dialogs", "rememberSaveFolder"));
  rememberFolderCheckBox->setChecked(gdata->qsettings->value("Dialogs/rememberSaveFolder", true).toBool());
  //baseLayout->addWidget(appendWavCheckBox, 1, 1);
  //baseLayout->addWidget(rememberFolderCheckBox, 1, 1);
  baseLayout->addSpacing(10);
  baseLayout->addWidget(appendWavCheckBox);
  baseLayout->addWidget(rememberFolderCheckBox);

  //baseLayout->setColSpacing(2, 30);
}

SaveDialog::~SaveDialog()
{
}

void SaveDialog::accept()
{
  bool remember = rememberFolderCheckBox->isChecked();
  //gdata->settings.setBool("Dialogs", "rememberSaveFolder", remember);
  gdata->qsettings->setValue("Dialogs/rememberSaveFolder", remember);
  if(remember == true) {
    const QDir *curDir = dir();
    //gdata->settings.setString("Dialogs", "saveFilesFolder", curDir->absPath());
    gdata->qsettings->setValue("Dialogs/saveFilesFolder", curDir->absPath());
    delete curDir;
  }
  bool appendWav = appendWavCheckBox->isChecked();
  //gdata->settings.setBool("Dialogs", "appendWav", appendWav);
  gdata->qsettings->setValue("Dialogs/appendWav", appendWav);
  if(appendWav == true) {
    QString s = selectedFile();
    if(!s.lower().endsWith(".wav")) { s += ".wav"; }
    setSelection(s);
  }
  Q3FileDialog::accept();
}

QString SaveDialog::getSaveWavFileName(QWidget *parent)
{
  SaveDialog d(parent);
  if(d.exec() != QDialog::Accepted) return QString();
  return d.selectedFile();
}

