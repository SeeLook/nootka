/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                             *
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
#include "nootkaconfig.h"
#include <music/tkeysignature.h>
#include <music/ttune.h>
#include <taudioparams.h>
#include <texamparams.h>
#include <tscoreparams.h>
#include <music/tnamestylefilter.h>
#include "tpath.h"
#include "tlayoutparams.h"
#include "tinitcorelib.h"
#include <QDir>
#include <QSettings>
#include <QCoreApplication>
#include <QDebug>


QString tmpConfigFile() {
	return QDir::tempPath() + "/nootkaTmp.conf";
}

/*static*/
QString& Tglobals::path = Tpath::main;

QString Tglobals::getInstPath(QString appInstPath) {
    QString p = "";
    QDir d = QDir(appInstPath);
#if defined (Q_OS_WIN)
		p = d.path() + "/"; 				//	Windows
  #elif defined (Q_OS_ANDROID)
//    p = "assets:/";
    p = ":/";
  #elif defined (Q_OS_LINUX)
		  d.cdUp();
		  p = d.path() + "/share/nootka/"; 	// Linux
  #elif defined (Q_OS_MAC)
      d.cdUp();
      p = d.path() + "/Resources/"; 		// MacOs
#endif

	return p;
}

/*end static*/


Tglobals::Tglobals(bool fromTemp) :
	m_tune(0)
{
	version = NOOTKA_VERSION;
//    path ; Is declared in main()

	qRegisterMetaTypeStreamOperators<Ttune>("Ttune");
	qRegisterMetaType<Tnote>("Tnote");

  QCoreApplication::setOrganizationName("Nootka");
  QCoreApplication::setOrganizationDomain("nootka.sf.net");
  if (qApp->applicationName() != "Nootini")
    QCoreApplication::setApplicationName("Nootka");

	E = new TexamParams();
	A = new TaudioParams();
	S = new TscoreParams();
	L = new TlayoutParams();
	
#if defined(Q_OS_WIN32) || defined(Q_OS_MAC) // I hate mess in Win registry
	config = new QSettings(QSettings::IniFormat, QSettings::UserScope, "Nootka", qApp->applicationName());
#else
	config = new QSettings();
#endif
	loadSettings(config);
	
	if (Tcore::gl() == 0)
		Tcore::setGlobals(this);
	else {
		qDebug() << "Tglobals instance has already existed. Application is terminating!";
		exit(109);
	}
}

Tglobals::~Tglobals() {
	storeSettings(config);
	delete E;
	delete A;
	delete S;
	delete L;
	delete m_tune;
	delete config;
	Tcore::reset();
}


//##########################################################################################
//#######################         PUBLIC         ###########################################
//##########################################################################################

void Tglobals::dumpToTemp() {
/*#if defined(Q_OS_WIN32) // I hate mess in Win registry
	QSettings tmpSett(tmpConfigFile(), QSettings::IniFormat);
#else
	QSettings tmpSett(tmpConfigFile(), QSettings::NativeFormat);
#endif
	storeSettings(&tmpSett);
	tmpSett.beginGroup("temp");
			tmpSett.setValue("nootkaPath", path); // other apps has to be able find the resources
	tmpSett.endGroup();*/
}


bool Tglobals::grabFromTemp() {
/*	if (QFileInfo::exists(tmpConfigFile())) {
		#if defined(Q_OS_WIN32) // I hate mess in Win registry
			QSettings tmpSett(tmpConfigFile(), QSettings::IniFormat);
		#else
			QSettings tmpSett(tmpConfigFile(), QSettings::NativeFormat);
		#endif
			loadSettings(&tmpSett);
			tmpSett.beginGroup("temp");
					path = tmpSett.value("nootkaPath", "").toString();
			tmpSett.endGroup();
			QFile::remove(tmpConfigFile());
			if (path != "")
				return true;
	}
		qDebug() << "Can not read from temp settings file!";*/
		return false;
}


void Tglobals::loadSettings(QSettings* cfg) {
	cfg->beginGroup("common");
			isFirstRun = cfg->value("isFirstRun", true).toBool();
			useAnimations = cfg->value("useAnimations", true).toBool();
			lang = cfg->value("language", "").toString();
			instrumentToFix = cfg->value("instrumentToFix", -1).toInt();
	cfg->endGroup();

//score widget settings
	cfg->beginGroup("score");
			S->keySignatureEnabled = cfg->value("keySignature", false).toBool();
// S->keySignatureEnabled = true;
			S->showKeySignName = cfg->value("keyName", true).toBool(); //true;
			S->nameStyleInKeySign = Tnote::EnameStyle(cfg->value("nameStyleInKey",
																													(int)Tnote::e_english_Bb).toInt());
			S->majKeyNameSufix = cfg->value("majorKeysSufix", "").toString();
			S->minKeyNameSufix = cfg->value("minorKeysSufix", "").toString();
			if (cfg->contains("pointerColor"))
					S->pointerColor = cfg->value("pointerColor").value<QColor>(); //-1;
			else 
					S->pointerColor = -1;
			S->clef = Tclef::Etype(cfg->value("clef", (int)Tclef::e_treble_G_8down).toInt());
			S->isSingleNoteMode = cfg->value("singleNoteMode", false).toBool();
			S->tempo = cfg->value("tempo", 120).toInt();
			S->scoreScale = cfg->value("scoreScale", 1.0).toReal();
	cfg->endGroup();


//common for score widget and note name
	cfg->beginGroup("common");
			S->doubleAccidentalsEnabled = cfg->value("doubleAccidentals", false).toBool();
// S->doubleAccidentalsEnabled = true;
			S->showEnharmNotes = cfg->value("showEnaharmonicNotes", false).toBool();
			if (!S->isSingleNoteMode) // enharmonically equal notes can be enabled only in single note mode
					S->showEnharmNotes = false;
			if (cfg->contains("enharmonicNotesColor"))
					S->enharmNotesColor = cfg->value("enharmonicNotesColor").value<QColor>(); //-1;
			else
					S->enharmNotesColor = -1;
			S->seventhIs_B = cfg->value("is7thNote_B", true).toBool(); //true;
	cfg->endGroup();
	
//note name settings    
	cfg->beginGroup("noteName");
			S->nameStyleInNoteName = Tnote::EnameStyle(cfg->value("nameStyle", (int)Tnote::e_english_Bb).toInt());
			S->solfegeStyle = Tnote::EnameStyle(cfg->value("solfegeStyle", (int)getSolfegeStyle()).toInt());
			S->octaveInNoteNameFormat = cfg->value("octaveInName", true).toBool();
			S->namesOnScore = cfg->value("namesOnScore", true).toBool();
			S->nameColor = cfg->value("namesColor", QColor(0, 80, 80)).value<QColor>();
	cfg->endGroup();
// Initialize name filter
	TnameStyleFilter::setStyleFilter(&S->seventhIs_B, &S->solfegeStyle);

// guitar settings
	Ttune::prepareDefinedTunes();
	cfg->beginGroup("guitar");
			instrument = Einstrument(cfg->value("instrument", (int)e_classicalGuitar).toInt());
			GfretsNumber = cfg->value("fretNumber", 19).toInt();
			GisRightHanded = cfg->value("rightHanded", true).toBool(); //true;
			GshowOtherPos = cfg->value("showOtherPos", false).toBool();
			if (cfg->contains("fingerColor"))
					GfingerColor = cfg->value("fingerColor").value<QColor>();
			else
					GfingerColor = QColor(255, 0, 127, 200); // nice pink
			if (cfg->contains("selectedColor"))
					GselectedColor = cfg->value("selectedColor").value<QColor>();
			else
					GselectedColor = QColor(51, 153, 255); // nice blue as default
			QVariant tun = cfg->value("tune");
			if (tun.isValid()) {
					Ttune tmpTune = tun.value<Ttune>();
					setTune(tmpTune);
			} else
					setTune(Ttune::stdTune);
			GpreferFlats = cfg->value("flatsPrefered", false).toBool(); //false;
			QList<QVariant> fretsList;
			fretsList << 5 << 7 << 9 << "12!" << 15 << 17;
			GmarkedFrets = cfg->value("dotsOnFrets", fretsList).toList();
	cfg->endGroup();

	
// Exam settings
	cfg->beginGroup("exam");
			if (cfg->contains("questionColor"))
					EquestionColor = cfg->value("questionColor").value<QColor>();
			else {
							EquestionColor = QColor("red");
							EquestionColor.setAlpha(40);
					}
			if (cfg->contains("answerColor"))
					EanswerColor = cfg->value("answerColor").value<QColor>();
			else {
							EanswerColor = QColor("green");
							EanswerColor.setAlpha(40);
					}
			if (cfg->contains("notBadColor"))
					EnotBadColor = cfg->value("notBadColor").value<QColor>();
			else {
							EnotBadColor = QColor("#FF8000");
							EnotBadColor.setAlpha(40);
					}
			E->autoNextQuest = cfg->value("autoNextQuest", false).toBool();
			E->repeatIncorrect = cfg->value("repeatIncorrect", true).toBool();
			E->expertsAnswerEnable = cfg->value("expertsAnswerEnable", false).toBool();
			E->studentName = cfg->value("studentName", "").toString();
			E->examsDir = cfg->value("examsDir", QDir::homePath()).toString();
			E->levelsDir = cfg->value("levelsDir", QDir::homePath()).toString();
			E->closeWithoutConfirm = cfg->value("closeWithoutConfirm", false).toBool();
			E->showCorrected = cfg->value("showCorrected", true).toBool();
			E->mistakePreview = cfg->value("mistakePreview", 3000).toInt();
			E->correctPreview = cfg->value("correctPreview", 3000).toInt();
			E->questionDelay = cfg->value("questionDelay", 150).toInt();
			E->suggestExam = cfg->value("suggestExam", true).toBool();
			E->afterMistake = (TexamParams::EafterMistake)cfg->value("afterMistake", (int)TexamParams::e_continue).toInt();
			E->showNameOfAnswered = cfg->value("showNameOfAnswered", true).toBool();
			E->showWrongPlayed = cfg->value("showWrongPlayed", false).toBool();
			E->showHelpOnStart = cfg->value("showHelpOnStart", true).toBool();
			E->askAboutExpert = cfg->value("askAboutExpert", true).toBool();
			E->showVeryBeginHelp = cfg->value("showVeryBeginHelp", true).toBool();
	cfg->endGroup();

// Sound settings
	cfg->beginGroup("sound");
		A->JACKorASIO = cfg->value("JACKorASIO", false).toBool();
		A->OUTenabled = cfg->value("outSoundEnabled", true).toBool();
		A->OUTdevName = cfg->value("outDeviceName", "").toString();
		A->midiEnabled = cfg->value("midiEnabled", false).toBool();
		A->midiPortName = cfg->value("midiPortName", "").toString();
		A->midiInstrNr = (unsigned char)cfg->value("midiInstrumentNr", 0).toInt();
		A->audioInstrNr = qBound(1, cfg->value("audioInstrumentNr", 1).toInt(), 3);
		A->INenabled = cfg->value("inSoundEnabled", true).toBool();
		A->INdevName = cfg->value("inDeviceName", "").toString();
		A->detectMethod = qBound(0, cfg->value("detectionMethod", 2).toInt(), 2); // MPM modified cepstrum
		A->minimalVol = cfg->value("minimalVolume", 0.4).toFloat();
		A->minDuration = cfg->value("minimalDuration", 0.15).toFloat();
		A->a440diff = cfg->value("a440Offset", 0).toFloat();
		A->intonation = (quint8)qBound(0, cfg->value("intonation", 3).toInt(), 5);
		A->forwardInput = cfg->value("forwardInput", false).toBool();
		A->playDetected = false; //cfg->value("playDetected", false).toBool();
		A->equalLoudness = cfg->value("equalLoudness", true).toBool();
    A->minSplitVol = cfg->value("minVolumeToSplit", 10.0).toReal();
    A->skipStillerVal = cfg->value("skipStillerThan", 80.0).toReal();
	cfg->endGroup();
	
	cfg->beginGroup("layout");
		L->toolBarAutoHide = cfg->value("toolBarAutoHide", false).toBool();
		L->iconTextOnToolBar = Qt::ToolButtonStyle(cfg->value("iconTextOnToolBar", 3).toInt());
		L->hintsBarEnabled = cfg->value("hintsBarEnabled", true).toBool();
		L->soundViewEnabled = cfg->value("soundViewEnabled", true).toBool();
		L->guitarEnabled = cfg->value("guitarEnabled", true).toBool();
	cfg->endGroup();
}


void Tglobals::setTune(Ttune& t) {
		delete m_tune;
    m_tune = new Ttune(t.name, t[1], t[2], t[3], t[4], t[5], t[6]);
// creating array with guitar strings in order of their height
    char openStr[6];
    for (int i = 0; i < 6; i++) {
        m_order[i] = i;
				if (m_tune->str(i + 1).note != 0)
					openStr[i] = m_tune->str(i + 1).chromatic();
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



void Tglobals::storeSettings(QSettings* cfg) {
	cfg->beginGroup("common");
			cfg->setValue("isFirstRun", isFirstRun);
			cfg->setValue("useAnimations", useAnimations);
			cfg->setValue("doubleAccidentals", S->doubleAccidentalsEnabled);
			cfg->setValue("showEnaharmonicNotes", S->showEnharmNotes);
			cfg->setValue("enharmonicNotesColor", S->enharmNotesColor);
			cfg->setValue("is7thNote_B", S->seventhIs_B);
			cfg->setValue("language", lang);
			cfg->setValue("instrumentToFix", instrumentToFix);
	cfg->endGroup();

	cfg->beginGroup("score");
			cfg->setValue("keySignature", S->keySignatureEnabled);
			cfg->setValue("keyName", S->showKeySignName);
			cfg->setValue("nameStyleInKey", (int)S->nameStyleInKeySign);
	QString majS, minS;
	if (S->majKeyNameSufix != TkeySignature::majorSufixTxt()) majS = S->majKeyNameSufix;
	else majS = ""; // default suffixes are reset to be translatable in next run
			cfg->setValue("majorKeysSufix", majS);
	if (S->minKeyNameSufix != TkeySignature::minorSufixTxt()) minS = S->minKeyNameSufix;
	else minS = "";
			cfg->setValue("minorKeysSufix", minS);
			cfg->setValue("pointerColor", S->pointerColor);
			cfg->setValue("clef", (int)S->clef);
			cfg->setValue("singleNoteMode", S->isSingleNoteMode);
			cfg->setValue("tempo", S->tempo);
			cfg->setValue("scoreScale", S->scoreScale);
	cfg->endGroup();

	cfg->beginGroup("noteName");
			cfg->setValue("nameStyle", (int)S->nameStyleInNoteName);
			cfg->setValue("octaveInName", S->octaveInNoteNameFormat);
			cfg->setValue("solfegeStyle", S->solfegeStyle);
			cfg->setValue("namesOnScore", S->namesOnScore );
			cfg->setValue("namesColor", S->nameColor);
	cfg->endGroup();
	
	cfg->beginGroup("guitar");
			cfg->setValue("instrument", (int)instrument);
			cfg->setValue("fretNumber", (int)GfretsNumber);
			cfg->setValue("rightHanded", GisRightHanded);
			cfg->setValue("showOtherPos", GshowOtherPos);
			cfg->setValue("fingerColor", GfingerColor);
			cfg->setValue("selectedColor", GselectedColor);
			cfg->setValue("tune", qVariantFromValue(*Gtune()));
			cfg->setValue("flatsPrefered", GpreferFlats);
			cfg->setValue("dotsOnFrets", GmarkedFrets);
	cfg->endGroup();

	cfg->beginGroup("exam");
			cfg->setValue("questionColor", EquestionColor);
			cfg->setValue("answerColor", EanswerColor);
			cfg->setValue("notBadColor", EnotBadColor);
			cfg->setValue("autoNextQuest", E->autoNextQuest);
			cfg->setValue("repeatIncorrect", E->repeatIncorrect);
			cfg->setValue("showCorrected", E->showCorrected);
			cfg->setValue("expertsAnswerEnable", E->expertsAnswerEnable);
			cfg->setValue("studentName", E->studentName);
			cfg->setValue("examsDir", E->examsDir);
			cfg->setValue("levelsDir", E->levelsDir);
			cfg->setValue("closeWithoutConfirm", E->closeWithoutConfirm);
			cfg->setValue("mistakePreview", E->mistakePreview);
			cfg->setValue("correctPreview", E->correctPreview);
			cfg->setValue("questionDelay", E->questionDelay);
			cfg->setValue("suggestExam", E->suggestExam);
			cfg->setValue("afterMistake", (int)E->afterMistake);
			cfg->setValue("showNameOfAnswered", E->showNameOfAnswered);
			cfg->setValue("showWrongPlayed", E->showWrongPlayed);
			cfg->setValue("askAboutExpert", E->askAboutExpert);
			cfg->setValue("showHelpOnStart", E->showHelpOnStart);
			cfg->setValue("showVeryBeginHelp", E->showVeryBeginHelp);
	cfg->endGroup();

	cfg->beginGroup("sound");
			cfg->setValue("JACKorASIO", A->JACKorASIO);
			cfg->setValue("outSoundEnabled", A->OUTenabled);
			cfg->setValue("outDeviceName", A->OUTdevName);
			cfg->setValue("midiEnabled", A->midiEnabled);
			cfg->setValue("midiPortName", A->midiPortName);
			cfg->setValue("midiInstrumentNr", (int)A->midiInstrNr);
			cfg->setValue("audioInstrumentNr", (int)A->audioInstrNr);
			cfg->setValue("inSoundEnabled", A->INenabled);
			cfg->setValue("inDeviceName", A->INdevName);
			cfg->setValue("detectionMethod", A->detectMethod);
			cfg->setValue("minimalVolume", A->minimalVol);
			cfg->setValue("minimalDuration", A->minDuration);
			cfg->setValue("a440Offset", A->a440diff);
			cfg->setValue("intonation", A->intonation);
			cfg->setValue("forwardInput", A->forwardInput);
// 			cfg->setValue("playDetected", A->playDetected);
      cfg->setValue("equalLoudness", A->equalLoudness);
      cfg->setValue("minVolumeToSplit", A->minSplitVol);
      cfg->setValue("skipStillerThan", A->skipStillerVal);
	cfg->endGroup();
	
	cfg->beginGroup("layout");
			cfg->setValue("toolBarAutoHide", L->toolBarAutoHide);
			cfg->setValue("iconTextOnToolBar", (int)L->iconTextOnToolBar);
			cfg->setValue("hintsBarEnabled", L->hintsBarEnabled);
			cfg->setValue("soundViewEnabled", L->soundViewEnabled);
			cfg->setValue("guitarEnabled", L->guitarEnabled);
	cfg->endGroup();
}
