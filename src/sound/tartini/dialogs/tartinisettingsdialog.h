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
#include <QDialog>
#include "ui_settingsdialog.h"
//#include "settings.h"
#include <QSettings>

class TartiniSettingsDialog : public QDialog, private Ui_SettingsDialog {
  Q_OBJECT

public:
  TartiniSettingsDialog(QWidget *parent = 0);

  void loadSetting(QObject *obj, const QString &group);
  void init();

public slots:
  QString getPath(const QString initialPath);
  void changeTempFolder();
  void fileNameChanged();
  void getBackgroundColor();
  void getShading1Color();
  void getShading2Color();
  void saveSetting(QObject *obj, const QString group);
  void saveSettings();
  void checkAnalysisEnabled();
  void onNoteRangeChoice(int choice);
  static void setUnknownsToDefault(QSettings *s);
  void resetDefaults();
};
