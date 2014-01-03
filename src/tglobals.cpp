/***************************************************************************
 *   Copyright (C) 2011-2013 by Tomasz Bojczuk                             *
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
#include <ttune.h>
#include "taudioparams.h"
#include "texamparams.h"
#include <tnamestylefilter.h>
#include <tnotenamelabel.h>
#include <QDir>
#include <QSettings>
#include <QCoreApplication>
#include <QDebug>



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
  return getBgColorText(C);
}

QColor Tglobals::invertColor(QColor C) {
    if (C != -1)
        C.setRgb(qRgb(255-C.red(), 255-C.green(), 255-C.blue()));
    return C;
}

QColor Tglobals::mergeColors(QColor C1, QColor C2) {
    qreal al = iV(C1.alpha()) + iV(C2.alpha() * (1 - iV(C1.alpha())));
    return QColor(((iV(C1.red()) * iV(C1.alpha()) + iV(C2.red()) * iV(C2.alpha()) * (1 - iV(C1.alpha()))) / al) * 255,
                  ((iV(C1.green()) * iV(C1.alpha()) + iV(C2.green()) * iV(C2.alpha()) * (1 - iV(C1.alpha()))) / al) * 255,
                  ((iV(C1.blue()) * iV(C1.alpha()) + iV(C2.blue()) * iV(C2.alpha()) * (1 - iV(C1.alpha()))) / al) * 255,
                  qMin(255, (int)(255 * al)));
}

/*end static*/


Tglobals::Tglobals() :
	m_tune(0)
{

    version = "0.9.3-alpha";
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
        useAnimations = config->value("useAnimations", true).toBool();
        lang = config->value("language", "").toString();
				instrumentToFix = config->value("instrumentToFix", -1).toInt();
    config->endGroup();

//score widget settings
    config->beginGroup("score");
        SkeySignatureEnabled = config->value("keySignature", false).toBool();
        SshowKeySignName = config->value("keyName", true).toBool(); //true;
        SnameStyleInKeySign = Tnote::EnameStyle(config->value("nameStyleInKey",
                                                           (int)Tnote::e_english_Bb).toInt());
        SmajKeyNameSufix = config->value("majorKeysSufix", "").toString();
        SminKeyNameSufix = config->value("minorKeysSufix", "").toString();
    if (config->contains("pointerColor"))
        SpointerColor = config->value("pointerColor").value<QColor>(); //-1;
    else 
        SpointerColor = -1;
				Sclef = Tclef::Etype(config->value("clef", (int)Tclef::e_treble_G_8down).toInt());
    config->endGroup();


//common for score widget and note name
    config->beginGroup("common");
        doubleAccidentalsEnabled = config->value("doubleAccidentals", false).toBool();
        showEnharmNotes = config->value("showEnaharmonicNotes", false).toBool();
        if (config->contains("enharmonicNotesColor"))
            enharmNotesColor = config->value("enharmonicNotesColor").value<QColor>(); //-1;
        else
            enharmNotesColor = -1;
        seventhIs_B = config->value("is7thNote_B", true).toBool(); //true;
    config->endGroup();
    
//note name settings    
    config->beginGroup("noteName");
        NnameStyleInNoteName = Tnote::EnameStyle(config->value("nameStyle", (int)Tnote::e_english_Bb).toInt());
				NsolfegeStyle = Tnote::EnameStyle(config->value("solfegeStyle", (int)getSolfegeStyle()).toInt());
        NoctaveInNoteNameFormat = config->value("octaveInName", true).toBool();
	//    NoctaveNameInNoteName = true;
    config->endGroup();
	// Initialize name filter
		TnameStyleFilter::setStyleFilter(&seventhIs_B, &NsolfegeStyle);

// guitar settings
    Ttune::prepareDefinedTunes();
    config->beginGroup("guitar");
				instrument = Einstrument(config->value("instrument", (int)e_classicalGuitar).toInt());
        GfretsNumber = config->value("fretNumber", 19).toInt();
        GisRightHanded = config->value("rightHanded", true).toBool(); //true;
        GshowOtherPos = config->value("showOtherPos", false).toBool();
        if (config->contains("fingerColor"))
            GfingerColor = config->value("fingerColor").value<QColor>();
        else
            GfingerColor = -1;
        if (config->contains("selectedColor"))
            GselectedColor = config->value("selectedColor").value<QColor>();
        else
            GselectedColor = -1;
        QVariant tun = config->value("tune");
        if (tun.isValid()) {
						Ttune tmpTune = tun.value<Ttune>();
            setTune(tmpTune);
				} else
						setTune(Ttune::stdTune);
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
        if (config->contains("notBadColor"))
            EnotBadColor = config->value("notBadColor").value<QColor>();
        else {
                EnotBadColor = QColor("#FF8000");
               EnotBadColor.setAlpha(40);
           }
        E->autoNextQuest = config->value("autoNextQuest", true).toBool();
        E->repeatIncorrect = config->value("repeatIncorrect", true).toBool();
        E->expertsAnswerEnable = config->value("expertsAnswerEnable", false).toBool();
        E->studentName = config->value("studentName", "").toString();
        E->examsDir = config->value("examsDir", QDir::homePath()).toString();
        E->levelsDir = config->value("levelsDir", QDir::homePath()).toString();
				E->closeWithoutConfirm = config->value("closeWithoutConfirm", false).toBool();
				E->showCorrected = config->value("showCorrected", true).toBool();
				E->correctViewDuration = config->value("durationOfCorrect", 2500).toInt();
				E->suggestExam = config->value("suggestExam", true).toBool();
				E->showHelpOnStart = config->value("showHelpOnStart", true).toBool();
				E->askAboutExpert = config->value("askAboutExpert", true).toBool();
				E->showVeryBeginHelp = config->value("showVeryBeginHelp", true).toBool();
    config->endGroup();

// Sound settings
    A = new TaudioParams();
    config->beginGroup("sound");
      A->OUTenabled = config->value("outSoundEnabled", true).toBool();
      A->OUTdevName = config->value("outDeviceName", "").toString();
      A->midiEnabled = config->value("midiEnabled", false).toBool();
      A->midiPortName = config->value("midiPortName", "").toString();
      A->midiInstrNr = (unsigned char)config->value("midiInstrumentNr", 0).toInt();
			A->audioInstrNr = qBound(1, config->value("audioInstrumentNr", 1).toInt(), 3);
      A->INenabled = config->value("inSoundEnabled", true).toBool();
      A->INdevName = config->value("inDeviceName", "").toString();
      A->isVoice = config->value("isVoice", false).toBool();
      A->minimalVol = config->value("minimalVolume", 0.4).toFloat();
			A->minDuration = config->value("minimalDuration", 0.09).toFloat();
      A->a440diff = config->value("a440Offset", 0).toFloat();
			A->range = (TaudioParams::Erange)config->value("pitchDetectRange", (int)TaudioParams::e_middle).toInt();
			A->intonation = (quint8)qBound(0, config->value("intonation", 3).toInt(), 5);
    config->endGroup();

}

Tglobals::~Tglobals() {
    storeSettings();
    delete config;
    delete E;
    delete A;
		delete m_tune;
}


void Tglobals::setTune(Ttune& t) {
		delete m_tune;
    m_tune = new Ttune(t.name, t[1], t[2], t[3], t[4], t[5], t[6]);
// creating array with guitar strings in order of their height
    char openStr[6];
    for (int i = 0; i < 6; i++) {
        m_order[i] = i;
				if (m_tune->str(i + 1).note != 0)
					openStr[i] = m_tune->str(i + 1).getChromaticNrOfNote();
				else // empty note - not such string
					openStr[i] = -120; // make it lowest
    }
      int i = 4;
      while (i > -1) {
          for (int j = i; j < 5 && openStr[m_order[j]] < openStr[m_order[j + 1]]; j++) {
              char tmp = m_order[j];
              m_order[j] = m_order[j + 1];
              m_order[j + 1] = tmp;
          }
          i--;
      }
}


Tnote Tglobals::hiString() {
		return m_tune->str(m_order[0] + 1); 
}


Tnote Tglobals::loString() {
		return m_tune->str(m_order[m_tune->stringNr() - 1] + 1);
}


Tnote::EnameStyle Tglobals::getSolfegeStyle() {
		Tnote::EnameStyle solStyle = Tnote::e_italiano_Si;
		QString ll = lang;
		if (ll == "")
			ll = QLocale::system().name();
		if (ll.contains("ru"))
			solStyle = Tnote::e_russian_Ci;
		return solStyle;
}



void Tglobals::storeSettings() {
    config->beginGroup("common");
        config->setValue("enableHints", hintsEnabled);
        config->setValue("isFirstRun", isFirstRun);
        config->setValue("useAnimations", useAnimations);
        config->setValue("doubleAccidentals", doubleAccidentalsEnabled);
        config->setValue("showEnaharmonicNotes", showEnharmNotes);
        config->setValue("enharmonicNotesColor", enharmNotesColor);
        config->setValue("is7thNote_B", seventhIs_B);
        config->setValue("language", lang);
				config->setValue("instrumentToFix", instrumentToFix);
    config->endGroup();

    config->beginGroup("score");
        config->setValue("keySignature", SkeySignatureEnabled);
        config->setValue("keyName", SshowKeySignName);
        config->setValue("nameStyleInKey", (int)SnameStyleInKeySign);
		QString majS, minS;
		if (SmajKeyNameSufix != TkeySignature::majorSufixTxt()) majS = SmajKeyNameSufix;
		else majS = ""; // default sufixes are reset to be translateable in next run
        config->setValue("majorKeysSufix", majS);
		if (SminKeyNameSufix != TkeySignature::minorSufixTxt()) minS = SminKeyNameSufix;
		else minS = "";
        config->setValue("minorKeysSufix", minS);
        config->setValue("pointerColor", SpointerColor);
				config->setValue("clef", (int)Sclef);
    config->endGroup();
  
    config->beginGroup("noteName");
        config->setValue("nameStyle", (int)NnameStyleInNoteName);
        config->setValue("octaveInName", NoctaveInNoteNameFormat);
				config->setValue("solfegeStyle", NsolfegeStyle);
    config->endGroup();
    
    config->beginGroup("guitar");
				config->setValue("instrument", (int)instrument);
        config->setValue("fretNumber", (int)GfretsNumber);
        config->setValue("rightHanded", GisRightHanded);
        config->setValue("showOtherPos", GshowOtherPos);
        config->setValue("fingerColor", GfingerColor);
        config->setValue("selectedColor", GselectedColor);
        config->setValue("tune", qVariantFromValue(*Gtune()));
        config->setValue("flatsPrefered", GpreferFlats);
    config->endGroup();

    config->beginGroup("exam");
        config->setValue("questionColor", EquestionColor);
        config->setValue("answerColor", EanswerColor);
        config->setValue("notBadColor", EnotBadColor);
        config->setValue("autoNextQuest", E->autoNextQuest);
        config->setValue("repeatIncorrect", E->repeatIncorrect);
        config->setValue("expertsAnswerEnable", E->expertsAnswerEnable);
        config->setValue("studentName", E->studentName);
        config->setValue("examsDir", E->examsDir);
        config->setValue("levelsDir", E->levelsDir);
				config->setValue("closeWithoutConfirm", E->closeWithoutConfirm);
				config->setValue("durationOfCorrect", E->correctViewDuration);
				config->setValue("suggestExam", E->suggestExam);
				config->setValue("askAboutExpert", E->askAboutExpert);
        config->setValue("showHelpOnStart", E->showHelpOnStart);
				config->setValue("showVeryBeginHelp", E->showVeryBeginHelp);
    config->endGroup();

    config->beginGroup("sound");
        config->setValue("outSoundEnabled", A->OUTenabled);
        config->setValue("outDeviceName", A->OUTdevName);
        config->setValue("midiEnabled", A->midiEnabled);
        config->setValue("midiPortName", A->midiPortName);
        config->setValue("midiInstrumentNr", (int)A->midiInstrNr);
				config->setValue("audioInstrumentNr", (int)A->audioInstrNr);
        config->setValue("inSoundEnabled", A->INenabled);
        config->setValue("inDeviceName", A->INdevName);
        config->setValue("isVoice", A->isVoice);
        config->setValue("minimalVolume", A->minimalVol);
				config->setValue("minimalDuration", A->minDuration);
        config->setValue("a440Offset", A->a440diff);
				config->setValue("pitchDetectRange", (int)A->range);
				config->setValue("intonation", A->intonation);
    config->endGroup();
}
