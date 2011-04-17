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
    keySignatureEnabled = true;
    showKeySignName = true;
    nameStyleInKeySign = Tnote::e_deutsch_His;
    majKeyNameSufix = " " ; //like 'major' or 'dur'
    minKeyNameSufix = " " ; //like 'minor' or 'moll'
    pointerColor = -1;

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
