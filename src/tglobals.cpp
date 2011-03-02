#include "tglobals.h"
//#include <iostream>

Tglobals::Tglobals() {

    version = "0.1 alpha";

//score widget settings
    keySignatureEnabled = true;
    showKeySignName = true;
    nameStyleInKeySign = Tnote::e_deutsch_His;
    majKeyNameSufix = " " ; //like 'major' or 'dur'
    minKeyNameSufix = " " ; //like 'minor' or 'moll'

//common with score widget and note name
    doubleAccidentalsEnabled = true;
    showEnharmNotes = true;

//note name settings
    nameStyleInNoteName = Tnote::e_deutsch_His;

}

Tglobals::~Tglobals() {
//    std::cout << "Saving settings not implemented yet" << std::endl;
}
