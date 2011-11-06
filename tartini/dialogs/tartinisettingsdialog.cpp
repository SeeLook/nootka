/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/
#include <qcolordialog.h>
//Added by qt3to4:
//#include <Q3Frame>
#include <QCustomEvent>
#include "mainwindow.h"
#include "tartinisettingsdialog.h"
#include "gdata.h"
#include "audio_stream.h"
#include <QFileDialog>

TartiniSettingsDialog::TartiniSettingsDialog(QWidget *parent)
  : QDialog(parent, Qt::WDestructiveClose)
{
  setupUi(this);

  connect(okButton, SIGNAL(clicked()), this, SLOT(saveSettings()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

  //connect( listBox, SIGNAL( highlighted(int) ), stack, SLOT( raiseWidget(int) ) );
  connect( backgroundButton, SIGNAL( clicked() ), this, SLOT( getBackgroundColor() ) );
  connect( shading1Button, SIGNAL( clicked() ), this, SLOT( getShading1Color() ) );
  connect( shading2Button, SIGNAL( clicked() ), this, SLOT( getShading2Color() ) );
  connect( fileNumberOfDigits, SIGNAL( valueChanged(int) ), this, SLOT( fileNameChanged() ) );
  connect( fileGeneratingNumber, SIGNAL( valueChanged(int) ), this, SLOT( fileNameChanged() ) );
  connect( filenameGeneratingString, SIGNAL( textChanged(const QString&) ), this, SLOT( fileNameChanged() ) );
  connect( chooseTempFolderButton, SIGNAL( clicked() ), this, SLOT( changeTempFolder() ) );
  connect( noteRangeChoice, SIGNAL( activated(int) ), this, SLOT( onNoteRangeChoice(int) ) );
  connect( resetDefaultsButton, SIGNAL( clicked() ), this, SLOT( resetDefaults() ) );

  init();
}

void TartiniSettingsDialog::loadSetting(QObject *obj, const QString &group)
{
  //printf("loadSettings %s, %s, %s\n", obj->metaObject()->className(), obj->objectName().latin1(), group.latin1());
  QString key = obj->name();
  QString fullKey = group + "/" + key;

  if(obj->isA("QGroupBox")) {
    //Iterate over the groupBox's children
    //const QObjectList *widgets = obj->children();
    const QList<QObject*> &widgets = obj->children();
    //if(!widgets) return;
    //QObject *childObj;
    for(QList<QObject*>::const_iterator it = widgets.begin(); it < widgets.end(); ++it) {
      loadSetting(*it, group);
    }
  } else if(obj->isA("QLineEdit")) {
    //((QLineEdit*)obj)->setText((gdata->settings).getString(group, key));
    ((QLineEdit*)obj)->setText(gdata->qsettings->value(fullKey).toString());
  } else if(obj->isA("QComboBox")) {
    //((QComboBox*)obj)->setCurrentText((gdata->settings).getString(group, key));
    ((QComboBox*)obj)->setCurrentText(gdata->qsettings->value(fullKey).toString());
  } else if(obj->isA("QPushButton") && ((QPushButton*)obj)->isToggleButton()) {
    //((QPushButton*)obj)->setOn((gdata->settings).getBool(group, key));
    ((QPushButton*)obj)->setOn(gdata->qsettings->value(fullKey).toBool());
  } else if(obj->isA("QCheckBox")) {
    //((QCheckBox*)obj)->setChecked((gdata->settings).getBool(group, key));
    ((QCheckBox*)obj)->setChecked(gdata->qsettings->value(fullKey).toBool());
  } else if(obj->isA("QSpinBox")) {
    //((QSpinBox*)obj)->setValue((gdata->settings).getInt(group, key));
    ((QSpinBox*)obj)->setValue(gdata->qsettings->value(fullKey).toInt());
  } else if(obj->isA("QFrame")) {
    QColor color;
    //color.setNamedColor((gdata->settings).getString(group, key));
    color.setNamedColor(gdata->qsettings->value(fullKey).toString());
    ((QFrame*)obj)->setPaletteBackgroundColor(color);
  }
}

void TartiniSettingsDialog::init()
{
  // Go through all the categories on the left, and insert all the preferences we can into the fields.
  // Combo boxes must be done separately.
  soundInput->clear();
  soundInput->insertStringList(AudioStream::getInputDeviceNames());
  soundOutput->clear();
  soundOutput->insertStringList(AudioStream::getOutputDeviceNames());
  
  QString group;
  //QObject *obj;
  //const QObjectList *widgets;
  //Iterate over all groups
  for (int i = 0; i < tabWidget->count(); i++) {
    //Iterate over all widgets in the current group and load their settings
    group = tabWidget->tabText(i);
    //const QList<QObject*> &widgets = stack->widget(i)->children();
    const QList<QObject*> &widgets = tabWidget->widget(i)->children();
    for(QList<QObject*>::const_iterator it=widgets.begin(); it < widgets.end(); ++it) {
      loadSetting(*it, group);
    }
  }

/*  #ifdef LINUX
  soundInput->setEditable(true);
  soundOutput->setEditable(true);
  #endif
*/
  checkAnalysisEnabled();
}

QString TartiniSettingsDialog::getPath(const QString initialPath)
{
  //return Q3FileDialog::getExistingDirectory(initialPath, this, "getDirectory", "Choose a directory", true, false);
  return QFileDialog::getExistingDirectory(this, "Choose a directory", initialPath);
}

/*
void TartiniSettingsDialog::changeOpenFolder()
{
  if(openFilesFolder) {
    QString path = getPath(openFilesFolder->text());
    if (path != "") openFilesFolder->setText(QDir::convertSeparators(path));
  }
}

void TartiniSettingsDialog::changeSaveFolder()
{
  if(saveFilesFolder) {
    QString path = getPath(saveFilesFolder->text());
    if (path != "") saveFilesFolder->setText(QDir::convertSeparators(path));
  }
}
*/
void TartiniSettingsDialog::changeTempFolder()
{
  if(tempFilesFolder) {
    QString path = getPath(tempFilesFolder->text());
    if (path != "") tempFilesFolder->setText(QDir::convertSeparators(path));
  }
}

/*
void TartiniSettingsDialog::openFilesTextChanged(const QString &s)
{
  if(bindOpenSaveFolders->isOn() && s != saveFilesFolder->text())
    saveFilesFolder->setText(s);
}

void TartiniSettingsDialog::saveFilesTextChanged(const QString &s)
{
  if(bindOpenSaveFolders->isOn() && openFilesFolder->text() != s)
    openFilesFolder->setText(s);
}

void TartiniSettingsDialog::toggleBindFolder(bool state)
{
  if(state) {
    bindOpenSaveFolders->setPixmap(QPixmap::fromMimeSource("chain_closed.png"));
    if(openFilesFolder->text().isEmpty()) {
      openFilesFolder->setText(saveFilesFolder->text());
    } else {
      saveFilesFolder->setText(openFilesFolder->text());
    }
  } else {
    bindOpenSaveFolders->setPixmap(QPixmap::fromMimeSource("chain_open.png"));
  }
}
*/
void TartiniSettingsDialog::fileNameChanged()
{
  QString filename;
  int digits = fileNumberOfDigits->value();
  if(digits == 0) {
    filename.sprintf("%s.wav", filenameGeneratingString->text().latin1());
  } else {
    filename.sprintf("%s%0*d.wav", filenameGeneratingString->text().latin1(), digits, fileGeneratingNumber->value());
  }
  filenameExample->setText(filename);
}

void TartiniSettingsDialog::getBackgroundColor()
{
  QColor color = QColorDialog::getColor(theBackgroundColor->paletteBackgroundColor());
  if(color.isValid()) {
    theBackgroundColor->setPaletteBackgroundColor(color);
  }
}

void TartiniSettingsDialog::getShading1Color()
{
  QColor color = QColorDialog::getColor(shading1Color->paletteBackgroundColor());
  if(color.isValid()) {
    shading1Color->setPaletteBackgroundColor(color);
  }
}

void TartiniSettingsDialog::getShading2Color()
{
  QColor color = QColorDialog::getColor(shading2Color->paletteBackgroundColor());
  if(color.isValid()) {
    shading2Color->setPaletteBackgroundColor(color);
  }
}

void TartiniSettingsDialog::saveSetting(QObject *obj, const QString group)
{
  QString key = obj->name();
  QString fullKey = group + "/" + key;

  if(obj->isA("QGroupBox")) {
    //Iterate over the groupBox's children
    //const QObjectList *widgets = obj->children();
    const QList<QObject*> &widgets = obj->children();
    //if(!widgets) return;
    //QObject *childObj;
    for(QList<QObject*>::const_iterator it=widgets.begin(); it < widgets.end(); ++it) {
      saveSetting(*it, group);
    }
  } else if(obj->isA("QLineEdit")) {
    //(gdata->settings).setString(group, key, ((QLineEdit*)obj)->text());
    gdata->qsettings->setValue(fullKey, ((QLineEdit*)obj)->text());
  } else if(obj->isA("QComboBox")) {
    //(gdata->settings).setString(group, key, ((QComboBox*)obj)->currentText());
    gdata->qsettings->setValue(fullKey, ((QComboBox*)obj)->currentText());
  } else if(obj->isA("QPushButton") && ((QPushButton*)obj)->isToggleButton()) {
    //(gdata->settings).setBool(group, key, ((QPushButton*)obj)->isOn());
    gdata->qsettings->setValue(fullKey, ((QPushButton*)obj)->isOn());
  } else if(obj->isA("QCheckBox")) {
    //(gdata->settings).setBool(group, key, ((QCheckBox*)obj)->isChecked());
    gdata->qsettings->setValue(fullKey, ((QCheckBox*)obj)->isChecked());
  } else if(obj->isA("QSpinBox")) {
    //(gdata->settings).setInt(group, key, ((QSpinBox*)obj)->value());
    gdata->qsettings->setValue(fullKey, ((QSpinBox*)obj)->value());
  } else if(obj->isA("QFrame")) {
    //(gdata->settings).setString(group, key, ((QFrame*)obj)->paletteBackgroundColor().name());
    gdata->qsettings->setValue(fullKey, ((QFrame*)obj)->paletteBackgroundColor().name());
  }
}

void TartiniSettingsDialog::saveSettings()
{
  // Go through all the categories on the left, and save all the preferences we can from the fields.
  // Combo boxes must be done separately.
  QString group;
  //QObject *obj;
  //const QObjectList *widgets;
  //Iterate over all the groups
  for(int i = 0; i < tabWidget->count(); i++) {
    //Iterate over all widgets in the current group and save their settings
    group = tabWidget->tabText(i);
    const QList<QObject*> &widgets = tabWidget->widget(i)->children();
    for(QList<QObject*>::const_iterator it=widgets.begin(); it < widgets.end(); ++it) {
      saveSetting(*it, group);
    }
  }
  
  //(gdata->settings).save();
  gdata->qsettings->sync();
	
  QApplication::postEvent(mainWindow, new QCustomEvent(SETTINGS_CHANGED));
	
  TartiniSettingsDialog::accept();

}

void TartiniSettingsDialog::checkAnalysisEnabled()
{
  //QObjectList *widgets = stack->widget(0)->queryList("QComboBox", "noteRangeChoice", false, true);
  //QComboBox *noteRangeChoice = (QComboBox*)widgets->first();
  QComboBox *noteRangeChoice = tabWidget->widget(0)->findChild<QComboBox *>("noteRangeChoice");
  myassert(noteRangeChoice);
  
  int choice = noteRangeChoice->currentItem();
  
  //widgets = stack->widget(2)->queryList("QGroupBox", "bufferSizeGroupBox", false, true);
  //QGroupBox *bufferSizeGroupBox = (QGroupBox*)widgets->first();
  QGroupBox *bufferSizeGroupBox = tabWidget->widget(2)->findChild<QGroupBox*>("bufferSizeGroupBox");
  myassert(bufferSizeGroupBox);
  //widgets = stack->widget(2)->queryList("QGroupBox", "stepSizeGroupBox", false, true);
  //QGroupBox *stepSizeGroupBox = (QGroupBox*)widgets->first();
  QGroupBox *stepSizeGroupBox = tabWidget->widget(2)->findChild<QGroupBox*>("stepSizeGroupBox");
  myassert(stepSizeGroupBox);
  
  if(choice == 0) {
    bufferSizeGroupBox->setEnabled(true);
    stepSizeGroupBox->setEnabled(true);
  } else {
    bufferSizeGroupBox->setEnabled(false);
    stepSizeGroupBox->setEnabled(false);
  }
}

void TartiniSettingsDialog::onNoteRangeChoice(int choice)
{
  //QObjectList *widgets = stack->widget(2)->queryList("QSpinBox", "bufferSizeValue", false, true);
  //QSpinBox *bufferSizeValue = (QSpinBox*)widgets->first();
  QSpinBox *bufferSizeValue = tabWidget->widget(2)->findChild<QSpinBox*>("bufferSizeValue");
  myassert(bufferSizeValue);
  //widgets = stack->widget(2)->queryList("QComboBox", "bufferSizeUnit", false, true);
  //QComboBox *bufferSizeUnit = (QComboBox*)widgets->first();
  QComboBox *bufferSizeUnit = tabWidget->widget(2)->findChild<QComboBox*>("bufferSizeUnit");
  myassert(bufferSizeUnit);
  //widgets = stack->widget(2)->queryList("QCheckBox", "bufferSizeRound", false, true);
  //QCheckBox *bufferSizeRound = (QCheckBox*)widgets->first();
  QCheckBox *bufferSizeRound = tabWidget->widget(2)->findChild<QCheckBox*>("bufferSizeRound");
  myassert(bufferSizeRound);
  //widgets = stack->widget(2)->queryList("QSpinBox", "stepSizeValue", false, true);
  //QSpinBox *stepSizeValue = (QSpinBox*)widgets->first();
  QSpinBox *stepSizeValue = tabWidget->widget(2)->findChild<QSpinBox*>("stepSizeValue");
  myassert(stepSizeValue);
  //widgets = stack->widget(2)->queryList("QComboBox", "stepSizeUnit", false, true);
  //QComboBox *stepSizeUnit = (QComboBox*)widgets->first();
  QComboBox *stepSizeUnit = tabWidget->widget(2)->findChild<QComboBox*>("stepSizeUnit");
  myassert(stepSizeUnit);
  //widgets = stack->widget(2)->queryList("QCheckBox", "stepSizeRound", false, true);
  //QCheckBox *stepSizeRound = (QCheckBox*)widgets->first();
  QCheckBox *stepSizeRound = tabWidget->widget(2)->findChild<QCheckBox*>("stepSizeRound");
  myassert(stepSizeRound);

  switch(choice) {
    case 1:
        bufferSizeValue->setValue(96);
        stepSizeValue->setValue(48);
      break;
    case 2:
        bufferSizeValue->setValue(48);
        stepSizeValue->setValue(24);
      break;
    case 3:
        bufferSizeValue->setValue(24);
        stepSizeValue->setValue(12);
      break;
    case 4:
        bufferSizeValue->setValue(12);
        stepSizeValue->setValue(6);
      break;
  }
  if(choice > 0) {
    bufferSizeUnit->setCurrentText("milli-seconds");
    stepSizeUnit->setCurrentText("milli-seconds");
    bufferSizeRound->setChecked(true);
    stepSizeRound->setChecked(true);
  }
  checkAnalysisEnabled();
}

/*
void TartiniSettingsDialog::setUnknownstToDefault(Settings *settings)
{
  settings->setBool("General", "bindOpenSaveFolders", true);
  settings->setString("General", "tempFilesFolder", QDir::convertSeparators(QDir::currentDirPath()));
  settings->setString("General", "filenameGeneratingString", "Untitled");
  settings->setInt("General", "fileGeneratingNumber", 1);
  settings->setInt("General", "fileNumberOfDigits", 2);
  settings->setInt("Display", "fastUpdateSpeed", 75);
  settings->setInt("Display", "slowUpdateSpeed", 150);
  settings->setString("General", "noteRangeChoice", "Notes F1 and higher - Cello, Guitar, Bass Clarinet, General sounds ...");
  
  settings->setBool("View", "autoFollow", true);
  settings->setBool("View", "backgroundShading", true);

#ifndef LINUX
  settings->setString("Sound", "soundInput", "Default Input Device");   // Use default device
  settings->setString("Sound", "soundOutput", "Default Output Device"); // Use default device
#else
  settings->setString("Sound", "soundInput", "/dev/dsp");
  settings->setString("Sound", "soundOutput", "/dev/dsp");
#endif
  settings->setString("Sound", "numberOfChannels", "Mono");
  settings->setInt("Sound", "sampleRate", 44100);
  settings->setInt("Sound", "bitsPerSample", 16);
  settings->setBool("Sound", "muteOutput", true);
  
  settings->setInt("Analysis", "bufferSizeValue", 48);
  settings->setString("Analysis", "bufferSizeUnit", "milli-seconds");
  settings->setBool("Analysis", "bufferSizeRound", true);
  
  settings->setInt("Analysis", "stepSizeValue", 24);
  settings->setString("Analysis", "stepSizeUnit", "milli-seconds");
  settings->setBool("Analysis", "stepSizeRound", true);
  settings->setBool("Analysis", "doingHarmonicAnalysis", true);
  settings->setBool("Analysis", "doingFreqAnalysis", true);
  settings->setBool("Analysis", "doingEqualLoudness", true);
  settings->setBool("Analysis", "doingAutoNoiseFloor", true);
  settings->setString("Analysis", "analysisType", "MPM");
  settings->setInt("Analysis", "thresholdValue", 93);

  settings->setString("Display", "theBackgroundColor", "#BBCDE2");
  settings->setString("Display", "shading1Color", "#BBCDEF");
  settings->setString("Display", "shading2Color", "#CBCDE2");
  settings->setBool("Display", "displayBackgroundShading", true);
  
  settings->setBool("Dialogs", "rememberOpenFolder", true);
  settings->setString("Dialogs", "openFilesFolder", QDir::convertSeparators(QDir::currentDirPath()));
  settings->setBool("Dialogs", "rememberSaveFolder", true);
  settings->setString("Dialogs", "saveFilesFolder", QDir::convertSeparators(QDir::currentDirPath()));
  settings->setBool("Dialogs", "appendWav", true);
}
*/

#define SetIfMissing(key, value) \
  if(!s->contains(key)) s->setValue(key, value)

void TartiniSettingsDialog::setUnknownsToDefault(QSettings *s)
{
  SetIfMissing("General/bindOpenSaveFolders", true);
  SetIfMissing("General/tempFilesFolder", QDir::convertSeparators(QDir::currentDirPath()));
  SetIfMissing("General/filenameGeneratingString", "Untitled");
  SetIfMissing("General/fileGeneratingNumber", 1);
  SetIfMissing("General/fileNumberOfDigits", 2);
  SetIfMissing("General/noteRangeChoice", "Notes F1 and higher - Cello, Guitar, Bass Clarinet, General sounds ...");

  SetIfMissing("View/autoFollow", false);
  SetIfMissing("View/backgroundShading", true);
  SetIfMissing("View/freqA", 440);

  SetIfMissing("Sound/soundInput", "Default");
  SetIfMissing("Sound/soundOutput", "Default");
/*
#ifndef LINUX
  SetIfMissing("Sound/soundInput", "Default Input Device");
  SetIfMissing("Sound/soundOutput", "Default Output Device");
#else
  SetIfMissing("Sound/soundInput", "/dev/dsp");
  SetIfMissing("Sound/soundOutput", "/dev/dsp");
#endif
*/
  SetIfMissing("Sound/numberOfBuffers", 4);
  SetIfMissing("Sound/numberOfChannels", "Mono");
  SetIfMissing("Sound/sampleRate", 44100);
  SetIfMissing("Sound/bitsPerSample", 16);
  SetIfMissing("Sound/muteOutput", true);

  SetIfMissing("Analysis/bufferSizeValue", 48);
  SetIfMissing("Analysis/bufferSizeUnit", "milli-seconds");
  SetIfMissing("Analysis/bufferSizeRound", true);
  SetIfMissing("Analysis/stepSizeValue", 24);
  SetIfMissing("Analysis/stepSizeUnit", "milli-seconds");
  SetIfMissing("Analysis/stepSizeRound", true);
  SetIfMissing("Analysis/doingHarmonicAnalysis", true);
  SetIfMissing("Analysis/doingFreqAnalysis", true);
  SetIfMissing("Analysis/doingEqualLoudness", true);
  SetIfMissing("Analysis/doingAutoNoiseFloor", true);
  SetIfMissing("Analysis/doingDetailedPitch", true);
  SetIfMissing("Analysis/analysisType", "MPM");
  SetIfMissing("Analysis/thresholdValue", 93);

  SetIfMissing("Display/fastUpdateSpeed", 75);
  SetIfMissing("Display/slowUpdateSpeed", 150);
  SetIfMissing("Display/theBackgroundColor", "#BBCDE2");
  SetIfMissing("Display/shading1Color", "#BBCDEF");
  SetIfMissing("Display/shading2Color", "#CBCDE2");
  SetIfMissing("Display/displayBackgroundShading", true);
  SetIfMissing("Display/useTopLevelWidgets", false);

  SetIfMissing("Dialogs/rememberOpenFolder", true);
  SetIfMissing("Dialogs/openFilesFolder", QDir::convertSeparators(QDir::currentDirPath()));
  SetIfMissing("Dialogs/rememberSaveFolder", true);
  SetIfMissing("Dialogs/saveFilesFolder", QDir::convertSeparators(QDir::currentDirPath()));
  SetIfMissing("Dialogs/appendWav", true);

  SetIfMissing("Advanced/showMeanVarianceBars", false);
  SetIfMissing("Advanced/savingMode", "Ask when closing unsaved files (normal)");
  SetIfMissing("Advanced/vibratoSineStyle", false);
  SetIfMissing("Advanced/mouseWheelZooms", false);
}

void TartiniSettingsDialog::resetDefaults()
{
  gdata->qsettings->clear();
  //setDefaults(&gdata->settings);
  //gdata->settings.save();
  setUnknownsToDefault(gdata->qsettings);
  gdata->qsettings->sync();
  init();
}
