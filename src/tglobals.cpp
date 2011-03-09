#include "tglobals.h"
#include <QColor>

Tglobals::Tglobals() {

    version = "0.1 alpha";

//score widget settings
    keySignatureEnabled = true;
    showKeySignName = true;
    nameStyleInKeySign = Tnote::e_deutsch_His;
    majKeyNameSufix = " " ; //like 'major' or 'dur'
    minKeyNameSufix = " " ; //like 'minor' or 'moll'

//common for score widget and note name
    doubleAccidentalsEnabled = true;
    showEnharmNotes = true;
    enharmNotesColor = -1;


//note name settings
    NnameStyleInNoteName = Tnote::e_deutsch_His;
    NoctaveInNoteNameFormat = true;
//    NoctaveNameInNoteName = true;

}

Tglobals::~Tglobals() {
}
