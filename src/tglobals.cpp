#include "tglobals.h"
#include "QApplication"
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

Tglobals::Tglobals() {

    version = "0.5 alpha";
//    path ; Is declared in mainWindow constructor

//score widget settings
    SkeySignatureEnabled = true;
    SshowKeySignName = true;
    SnameStyleInKeySign = Tnote::e_english_Bb;
    SmajKeyNameSufix = " " ; //like 'major' or 'dur'
    SminKeyNameSufix = " " ; //like 'minor' or 'moll'
    SpointerColor = -1;

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


}

Tglobals::~Tglobals() {
}
