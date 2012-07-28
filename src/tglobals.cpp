/***************************************************************************
 *   Copyright (C) 2011-2012 by Tomasz Bojczuk                             *
 *   tomaszbojczuk@gmail.com                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tglobals.h"
#include "tkeysignature.h"
#include "taudioparams.h"
#include "texamparams.h"
#include <QDir>
#include <QSettings>
#include <QCoreApplication>
#include <qset.h>
//#include <QDebug>



/*static*/
QString Tglobals::getInstPath(QString appInstPath) {
    QString p = "";
    QDir d = QDir(appInstPath);
#if defined(Q_OS_WIN32)
		p = d.path() + "/"; 				//	Windows
  #elif defined(Q_OS_LINUX)
		  d.cdUp();
		  p = d.path() + "/share/nootka/"; 	// Linux
  #else
      d.cdUp();
      p = d.path() + "/Resources/"; 		// MacOs
#endif

        return p;
}

QString Tglobals::getBGcolorText(QColor C) {
  if ( C != -1)
    return QString(
      "background-color: rgba(%1, %2, %3, %4); ")
            .arg(C.red()).arg(C.green()).arg(C.blue()).arg(C.alpha());
  else
    return QString("background-color: transparent; ");
}

QColor Tglobals::invertColor(QColor C) {
    if (C != -1)
        C.setRgb(qRgb(255-C.red(), 255-C.green(), 255-C.blue()));
    return C;
}

/*end static*/


Tglobals::Tglobals() {

    version = "0.8.3-alpha";
//    path ; Is declared in main()

    qRegisterMetaTypeStreamOperators<Ttune>("Ttune");
    qRegisterMetaType<Tnote>("Tnote");

    QCoreApplication::setOrganizationName("Nootka");
    QCoreApplication::setOrganizationDomain("nootka.sf.net");
    QCoreApplication::setApplicationName("Nootka");

#if defined(Q_OS_WIN32) // I hate mess in Win registry
    config = new QSettings(QSettings::IniFormat, QSettings::UserScope, "Nootka", "Nootka");
#else
    config = new QSettings();
#endif

    config->beginGroup("common");
        hintsEnabled = config->value("enableHints", true).toBool(); //true;
        isFirstRun = config->value("isFirstRun", true).toBool();
        lang = config->value("language", "").toString();
    config->endGroup();

//score widget settings
    config->beginGroup("score");
        SkeySignatureEnabled = config->value("keySignature", true).toBool(); //true;
        SshowKeySignName = config->value("keyName", true).toBool(); //true;
        SnameStyleInKeySign = Tnote::EnameStyle(config->value("nameStyleInKey",
                                                           (int)Tnote::e_english_Bb).toInt());
        SmajKeyNameSufix = config->value("majorKeysSufix", "").toString();
        SminKeyNameSufix = config->value("minorKeysSufix", "").toString();
	if (config->contains("pointerColor"))
	    SpointerColor = config->value("pointerColor").value<QColor>(); //-1;
	else 
	    SpointerColor = -1;
    config->endGroup();

//     TkeySignature::setNameStyle(SnameStyleInKeySign, SmajKeyNameSufix, SminKeyNameSufix);

//common for score widget and note name
    config->beginGroup("common");
        doubleAccidentalsEnabled = config->value("doubleAccidentals", true).toBool(); //true;
        showEnharmNotes = config->value("showEnaharmonicNotes", true).toBool(); //true;
        if (config->contains("enharmonicNotesColor"))
            enharmNotesColor = config->value("enharmonicNotesColor").value<QColor>(); //-1;
        else
            enharmNotesColor = -1;
        seventhIs_B = config->value("is7thNote_B", true).toBool(); //true;
    config->endGroup();
    
//note name settings    
    config->beginGroup("noteName");
        NnameStyleInNoteName = Tnote::EnameStyle(config->value("nameStyle", (int)Tnote::e_english_Bb).toInt());
        NoctaveInNoteNameFormat = config->value("octaveInName", true).toBool();
	//    NoctaveNameInNoteName = true;
    config->endGroup();

// guitar settings
    Ttune::prepareDefinedTunes();
    config->beginGroup("guitar");
        GfretsNumber = config->value("fretNumber", 19).toInt();
        GisRightHanded = config->value("rightHanded", true).toBool(); //true;
        GshowOtherPos = config->value("showOtherPos", true).toBool(); //true;
        if (config->contains("fingerColor"))
            GfingerColor = config->value("fingerColor").value<QColor>();
        else
            GfingerColor = -1;
        if (config->contains("selectedColor"))
            GselectedColor = config->value("selectedColor").value<QColor>();
        else
            GselectedColor = -1;
        QVariant tun = config->value("tune");
        if (tun.isValid())
            setTune(tun.value<Ttune>());
        else setTune(Ttune::stdTune);
        GpreferFlats = config->value("flatsPrefered", false).toBool(); //false;
    config->endGroup();

   
// Exam settings
    E = new TexamParams();
    config->beginGroup("exam");
        if (config->contains("questionColor"))
            EquestionColor = config->value("questionColor").value<QColor>();
        else {
               EquestionColor = QColor("red");
               EquestionColor.setAlpha(40);
           }
        if (config->contains("answerColor"))
            EanswerColor = config->value("answerColor").value<QColor>();
        else {
                EanswerColor = QColor("green");
               EanswerColor.setAlpha(40);
           }
        E->autoNextQuest = config->value("autoNextQuest", true).toBool();
        E->repeatIncorrect = config->value("repeatIncorrect", true).toBool();
        E->expertsAnswerEnable = config->value("expertsAnswerEnable", false).toBool();
        E->askAboutExpert = config->value("askAboutExpert", true).toBool();
        E->showHelpOnStart = config->value("showHelpOnStart", true).toBool();
        E->studentName = config->value("studentName", "").toString();
    config->endGroup();

// Sound settings
    A = new TaudioParams();
    config->beginGroup("sound");
      A->OUTenabled = config->value("outSoundEnabled", true).toBool();
      A->OUTdevName = config->value("outDeviceName", "").toString();
      A->midiEnabled = config->value("midiEnabled", false).toBool();
      A->midiPortName = config->value("midiPortName", "").toString();
      A->midiInstrNr = (unsigned char)config->value("midiInstrumentNr", 0).toInt();
      A->INenabled = config->value("inSoundEnabled", true).toBool();
      A->INdevName = config->value("inDeviceName", "").toString();
      A->isVoice = config->value("isVoice", false).toBool();
      A->noiseLevel = (qint16)config->value("noiseLevel", 70).toInt();
      A->a440diff = config->value("a440Offset", 0).toFloat();
    config->endGroup();

}

Tglobals::~Tglobals() {
    storeSettings();
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

void Tglobals::storeSettings() {
#if defined(Q_OS_WIN32) // I hate mess in Windows registry
    QSettings sett(QSettings::IniFormat, QSettings::UserScope, "Nootka", "Nootka");
#else
    QSettings sett;
#endif
    sett.beginGroup("common");
        sett.setValue("enableHints", hintsEnabled);
        sett.setValue("isFirstRun", isFirstRun);
        sett.setValue("doubleAccidentals", doubleAccidentalsEnabled);
        sett.setValue("showEnaharmonicNotes", showEnharmNotes);
        sett.setValue("enharmonicNotesColor", enharmNotesColor);
        sett.setValue("is7thNote_B", seventhIs_B);
        sett.setValue("language", lang);
    sett.endGroup();

    sett.beginGroup("score");
        sett.setValue("keySignature", SkeySignatureEnabled);
        sett.setValue("keyName", SshowKeySignName);
        sett.setValue("nameStyleInKey", (int)SnameStyleInKeySign);
		QString majS, minS;
		if (SmajKeyNameSufix != TkeySignature::majorSufixTxt()) majS = SmajKeyNameSufix;
		else majS = ""; // default sufixes are reset to be translateable in next run
        sett.setValue("majorKeysSufix", majS);
		if (SminKeyNameSufix != TkeySignature::minorSufixTxt()) minS = SminKeyNameSufix;
		else minS = "";
        sett.setValue("minorKeysSufix", minS);
        sett.setValue("pointerColor", SpointerColor);
    sett.endGroup();
  
    sett.beginGroup("noteName");
        sett.setValue("nameStyle", (int)NnameStyleInNoteName);
        sett.setValue("octaveInName", NoctaveInNoteNameFormat);
    sett.endGroup();
    
    sett.beginGroup("guitar");
        sett.setValue("fretNumber", (int)GfretsNumber);
        sett.setValue("rightHanded", GisRightHanded);
        sett.setValue("showOtherPos", GshowOtherPos);
        sett.setValue("fingerColor", GfingerColor);
        sett.setValue("selectedColor", GselectedColor);
        sett.setValue("tune", qVariantFromValue(Gtune()));
        sett.setValue("flatsPrefered", GpreferFlats);
    sett.endGroup();

    sett.beginGroup("exam");
        sett.setValue("questionColor", EquestionColor);
        sett.setValue("answerColor", EanswerColor);
        sett.setValue("autoNextQuest", E->autoNextQuest);
        sett.setValue("repeatIncorrect", E->repeatIncorrect);
        sett.setValue("expertsAnswerEnable", E->expertsAnswerEnable);
        sett.setValue("askAboutExpert", E->askAboutExpert);
        sett.setValue("showHelpOnStart", E->showHelpOnStart);
        sett.setValue("studentName", E->studentName);
    sett.endGroup();

    sett.beginGroup("sound");
        sett.setValue("outSoundEnabled", A->OUTenabled);
        sett.setValue("outDeviceName", A->OUTdevName);
        sett.setValue("midiEnabled", A->midiEnabled);
        sett.setValue("midiPortName", A->midiPortName);
        sett.setValue("midiInstrumentNr", (int)A->midiInstrNr);
        sett.setValue("inSoundEnabled", A->INenabled);
        sett.setValue("inDeviceName", A->INdevName);
        sett.setValue("isVoice", A->isVoice);
        sett.setValue("noiseLevel", A->noiseLevel);      
        sett.setValue("a440Offset", A->a440diff);
    sett.endGroup();
}
