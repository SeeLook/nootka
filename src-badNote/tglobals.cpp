#include "tglobals.h"
//#include <iostream>

Tglobals::Tglobals() {

    version = "0.1 alpha";


    doubleAccidentalsEnabled = true;
    keySignatureEnabled = true;

    showEnharmNotes = true;
    showKeySignName = true;
    nameStyleInKeySign = Tnote::e_deutsch_His;
    majKeyNameSufix = " " ; //like 'major' or 'dur'
    minKeyNameSufix = " " ; //like 'minor' or 'moll'

}

Tglobals::~Tglobals() {
//    std::cout << "Saving settings not implemented yet" << std::endl;
}
