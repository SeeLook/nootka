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

QColor Tglobals::invertColor(QColor C) {
    /** @todo */
}

/*end static*/


Tglobals::Tglobals() {

    version = "0.6 beta";
//    path ; Is declared in mainWindow constructor
    hintsEnabled = true;

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
//   Gtune = Ttune::stdTune;
   setTune(Ttune::stdTune);
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

void Tglobals::setTune(Ttune t) {
    m_tune = t;
// creating array with guitar strings in order of their height
    char openStr[6];
    for (int i=0; i<6; i++) {
        m_order[i] = i;
        openStr[i] = m_tune[i+1].getChromaticNrOfNote();
    }
      int i = 4;
      while (i > -1) {
          for (int j=i; j < 5 && openStr[m_order[j]] < openStr[m_order[j+1]]; j++) {
              char tmp = m_order[j];
              m_order[j] = m_order[j+1];
              m_order[j+1] = tmp;
          }
          i--;
      }
}


