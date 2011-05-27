#include "tglobals.h"
#include "tkeysignature.h"
//#include "QApplication"
#include "QDir"



/*static*/
QString Tglobals::getInstPath(QString appInstPath) {
    QString p = "";
#if defined(Q_OS_LINUX)
    QDir d = QDir(appInstPath);
    d.cdUp();
    p = d.path()+"/share/nootka/"; //Linux
#endif
    return p;
}

QString Tglobals::getBGcolorText(QColor C) {
  if ( C != -1)
    return QString(
      "background-color: rgba(%1, %2, %3, %4); ").arg(C.red()).arg(C.green()).arg(C.blue()).arg(C.alpha());
  else
    return QString("background-color: transparent; ");
}


Tglobals::Tglobals() {

    version = "0.6 beta";
//    path ; Is declared in mainWindow constructor

//score widget settings
    SkeySignatureEnabled = true;
    SshowKeySignName = true;
    SnameStyleInKeySign = Tnote::e_english_Bb;
    SmajKeyNameSufix = QObject::tr("major");
    SminKeyNameSufix = QObject::tr("minor");
    SpointerColor = -1;

    TkeySignature::setNameStyle(SnameStyleInKeySign, SmajKeyNameSufix, SminKeyNameSufix);

//common for score widget and note name
    doubleAccidentalsEnabled = true;
    showEnharmNotes = true;
    enharmNotesColor = -1;
    seventhIs_B = true;




//note name settings
    NnameStyleInNoteName = Tnote::e_english_Bb;
    NoctaveInNoteNameFormat = true;
//    NoctaveNameInNoteName = true;

// guitar settings
   GfretsNumber = 19;
   GisRightHanded = true;
   GshowOtherPos = true;
   GfingerColor = -1;
   GselectedColor = -1;
   Gtune = Ttune::stdTune;
   GpreferFlats = false;
   
// Exam settings
    EquestionColor = QColor("red");
    EquestionColor.setAlpha(40);
    EanswerColor = QColor("green");
    EanswerColor.setAlpha(40);
    EautoNextQuest = false;
    ErepeatIncorrect = true;

}

Tglobals::~Tglobals() {
}
