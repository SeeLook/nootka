; PRODUCT_VERSION is there
!include "NSIS.definitions.nsh"

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "Nootka"
!define PRODUCT_PUBLISHER "Nootka"
!define PRODUCT_WEB_SITE "http://nootka.sf.net"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\nootka.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

SetCompressor lzma

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "picts\pack.ico"
!define MUI_UNICON "picts\pack.ico"

; Language Selection Dialog Settings
!define MUI_LANGDLL_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_LANGDLL_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "NSIS:Language"
!define MUI_LANGDLL_ALLLANGUAGES

; Image
!define MUI_HEADERIMAGE
!define MUI_WELCOMEFINISHPAGE_BITMAP "picts\logo-left.bmp"
!define MUI_HEADERIMAGE_BITMAP "picts\logo.bmp"
;!define MUI_HEADERIMAGE_RIGHT

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "gpl"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\nootka.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Czech"
!insertmacro MUI_LANGUAGE "German"
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "Polish"
!insertmacro MUI_LANGUAGE "Russian"
!insertmacro MUI_LANGUAGE "Spanish"
!insertmacro MUI_LANGUAGE "Hungarian"


; Reserve files
!insertmacro MUI_RESERVEFILE_LANGDLL
;!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "Nootka-${PRODUCT_VERSION}-Windows-Installer.exe"
InstallDir "$PROGRAMFILES\Nootka"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

LangString NelDesc ${LANG_ENGLISH} "Nootka level file"
LangString NelDesc ${LANG_CZECH} "Úroveň zkoušky programu Nootka"
LangString NelDesc ${LANG_GERMAN} "Nootka-Level Datei"
LangString NelDesc ${LANG_FRENCH} "Fichier de niveau Nootka"
LangString NelDesc ${LANG_POLISH} "Poziom egzaminu programu Nootka"
LangString NelDesc ${LANG_RUSSIAN} "Файл Нутки с уровнем"
LangString NelDesc ${LANG_SPANISH} "Fichero de nivel de Nootka"
LangString NelDesc ${LANG_HUNGARIAN} "Nootka szint fájl"

LangString NootkaAct ${LANG_ENGLISH} "Open with Nootka"
LangString NootkaAct ${LANG_CZECH} "Otevřít v Nootka"
LangString NootkaAct ${LANG_GERMAN} "Mit Nootka öffnen"
LangString NootkaAct ${LANG_FRENCH} "Ouvrir avec Nootka"
LangString NootkaAct ${LANG_POLISH} "Otwórz w programie Nootka"
LangString NootkaAct ${LANG_RUSSIAN} "Открыть в Нутке"
LangString NootkaAct ${LANG_SPANISH} "Abrir con Nootka"
LangString NootkaAct ${LANG_HUNGARIAN} "Megnyitás Nootka-val"

LangString NooDesc ${LANG_ENGLISH} "Nootka exam file"
LangString NooDesc ${LANG_CZECH} "Zkušební soubor programu Nootka"
LangString NooDesc ${LANG_GERMAN} "Nootka-Prüfungs Datei"
LangString NooDesc ${LANG_FRENCH} "Fichier d'examen Nootka"
LangString NooDesc ${LANG_POLISH} "Plik egzaminu programu Nootka"
LangString NooDesc ${LANG_RUSSIAN} "Файл Нутки с экзаменом"
LangString NooDesc ${LANG_SPANISH} "Fichero de examen de Nootka"
LangString NooDesc ${LANG_HUNGARIAN} "Nootka vizsga fájl"

Section -AssociateMime
;FILE ASSOCIATION

!include fileasoc.nsh
!insertmacro APP_ASSOCIATE "nel" "nootka.level" "$(NelDesc)" "$INSTDIR\picts\nootka-levels.ico"     "$(NootkaAct)" "$INSTDIR\nootka.exe $\"%1$\""
!insertmacro APP_ASSOCIATE "noo" "nootka.exam" "$(NooDesc)" "$INSTDIR\picts\nootka-exam.ico"     "$(NootkaAct)" "$INSTDIR\nootka.exe $\"%1$\""
SectionEnd


Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

Section "GrupaGlowna" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  SetOverwrite try
  File "nootka.exe"
  CreateDirectory "$SMPROGRAMS\Nootka"
  CreateShortCut "$SMPROGRAMS\Nootka\Nootka.lnk" "$INSTDIR\nootka.exe"
  CreateShortCut "$DESKTOP\Nootka.lnk" "$INSTDIR\nootka.exe"
  
  File "libstdc++-6.dll"
  File "libgcc_s_dw2-1.dll"
  File "libwinpthread-1.dll"
  File "libvorbis.dll"
  File "libogg.dll"
  File "libvorbisfile.dll"  
  File "libfftw3f-3.dll"
  File "libRtAudio.dll"
; Qt5 staff
  File "Qt5Core.dll"
  File "Qt5PrintSupport.dll"
  File "Qt5Network.dll"
  File "Qt5Gui.dll"
  File "Qt5Widgets.dll"
; Main Nootka libraries
  File "libNootkaMain.dll"
  File "libNootkaCore.dll"
  File "libNootkaMisc.dll"
  File "libNootkaScore.dll"
  File "libNootkaSound.dll"
  File "libNootkaWidgets.dll"
; Nootka plugins
  File "libNootkaAboutPlugin.dll"
  File "libNootkaExamPlugin.dll"
  File "libNootkaWizardPlugin.dll"
  File "libNootkaLevelPlugin.dll"
  File "libNootkaSettingsPlugin.dll"
  File "libNootkaAnalyzerPlugin.dll"
  File "libNootkaUpdaterPlugin.dll"

  File "changes"
  File "gpl"

  SetOutPath "$INSTDIR\platforms"
		File "platforms\qwindows.dll"
		
  SetOutPath "$INSTDIR\bearer"
		File "bearer\qgenericbearer.dll"
		File "bearer\qnativewifibearer.dll"
		
  SetOutPath "$INSTDIR\imageformats"
    File "imageformats\qgif.dll"
    File "imageformats\qicns.dll"
    File "imageformats\qico.dll"
    File "imageformats\qjpeg.dll"
    File "imageformats\qtiff.dll"
    File "imageformats\qwbmp.dll"
    File "imageformats\qwebp.dll"

  SetOutPath "$INSTDIR\printsupport"
		File "printsupport\windowsprintersupport.dll"
  
  SetOutPath "$INSTDIR\sounds"
		File "sounds\classical-guitar.ogg"
		File "sounds\electric-guitar.ogg"
		File "sounds\bass-guitar.ogg"
  SetOutPath "$INSTDIR\picts"
		File "picts\pack.ico"
		File "picts\questionsSettings.png"
		File "picts\stopExam.png"
		File "picts\certBg.png"
		File "picts\soundSettings.png"
		File "picts\body-electro.png"
		File "picts\nootka-levels.ico"
		File "picts\wizard-left.png"
		File "picts\score.png"
		File "picts\stamp.png"
		File "picts\exit.png"
		File "picts\exam-settings.png"
		File "picts\body-bass.png"
		File "picts\fingbg-el.png"
		File "picts\global.png"
		File "picts\chlog.png"
		File "picts\exam.png"
		File "picts\startExam.png"
		File "picts\nootka.png"
		File "picts\nootka-frame.png"
		File "picts\flags-pl.png"
		File "picts\logo.png"
		File "picts\flags-cs.png"
		File "picts\flags-hu.png"
		File "picts\help.png"
		File "picts\help-frame.png"
		File "picts\flags-ru.png"
		File "picts\flags-es.png"
		File "picts\appearance.png"
		File "picts\author.png"
		File "picts\melody-play.png"
		File "picts\nootka.ico"
		File "picts\fork.png"
		File "picts\levelsSettings.png"
		File "picts\barChart.png"
		File "picts\zoom-in.png"
		File "picts\pickup.png"
		File "picts\melody.png"
		File "picts\fullscreen.png"
		File "picts\scoreSettings.png"
		File "picts\nootka-exam.ico"
		File "picts\scr.png"
		File "picts\clear-score.png"
		File "picts\prevQuest.png"
		File "picts\flags-en.png"
		File "picts\check.png"
		File "picts\license.png"
		File "picts\support.png"
		File "picts\nootka-exam.png"
		File "picts\zoom-out.png"
		File "picts\practice.png"
		File "picts\flags-fr.png"
		File "picts\nextQuest.png"
		File "picts\flags-de.png"
		File "picts\linearChart.png"
		File "picts\systemsettings.png"
		File "picts\body.png"
		File "picts\levelCreator.png"
		File "picts\nootka-level.png"
		File "picts\melody-rec.png"
		File "picts\guitarSettings.png"
		File "picts\correct.png"
		File "picts\repeatSound.png"
		File "picts\record.png"
		File "picts\rangeSettings.png"
		File "picts\notSaved.png"
		File "picts\fingbg.png"
		File "picts\accidSettings.png"
		File "picts\charts.png"
		File "picts\donors.png"
		File "picts\qt.png"
		File "picts\melodySett.png"
  SetOutPath "$INSTDIR\lang"
		File "lang\nootka_pl.qm"
		File "lang\nootka_cs.qm"
		File "lang\nootka_fr.qm"
		File "lang\nootka_de.qm"
		File "lang\nootka_hu.qm"
		File "lang\nootka_ru.qm"
		File "lang\nootka_es.qm"
		File "lang\qtbase_de.qm"
		File "lang\qtbase_cs.qm"
		File "lang\qtbase_fr.qm"
		File "lang\qtbase_hu.qm"
		File "lang\qtbase_pl.qm"
		File "lang\qtbase_ru.qm"
		File "lang\qtbase_es.qm"
  SetOutPath "$INSTDIR\fonts"
		File "fonts\README"
		File "fonts\nootka.ttf"
SectionEnd

Section -AdditionalIcons
  SetOutPath $INSTDIR
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\Nootka\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\Nootka\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\nootka.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\nootka.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd


   LangString UninstallMess ${LANG_ENGLISH} "Do You really want to remove $(^Name) and all its components?"
   LangString UninstallMess ${LANG_GERMAN} "Möchtest du wirklich Nootka und all seine Komponenten entfernen?"
   LangString UninstallMess ${LANG_CZECH} "Opravdu chcete odstranit Nootku a vsechny její soueásti?"
   LangString UninstallMess ${LANG_FRENCH} "Voulez-vous vraiment supprimer Nootka et tous ses composants?"
   LangString UninstallMess ${LANG_POLISH} "Czy rzeczywiście chcesz usunąć Nootkę i jej składniki?"
   LangString UninstallMess ${LANG_RUSSIAN} "Вы действительно хотите удалить Нутку и все компоненты программы?"
   LangString UninstallMess ${LANG_SPANISH} "¿Quiere realmente desinstalar Nootka y todos sus componentes?"
   LangString UninstallMess ${LANG_HUNGARIAN} "Biztos, hogy törli Nootka-t minden komponensével együtt?"

Function un.onInit
!insertmacro MUI_UNGETLANGUAGE
   MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "$(UninstallMess)" IDYES +2
   Abort
FunctionEnd

Section Uninstall
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\nootka.exe"

; File list generated by cpack -G NSIS
  Delete "$INSTDIR\libstdc++-6.dll"
  Delete "$INSTDIR\libgcc_s_dw2-1.dll"
  Delete "$INSTDIR\libwinpthread-1.dll"
  Delete "$INSTDIR\Qt5Widgets.dll"
  Delete "$INSTDIR\sounds\classical-guitar.ogg"
  Delete "$INSTDIR\sounds\electric-guitar.ogg"
  Delete "$INSTDIR\sounds\bass-guitar.ogg"
  Delete "$INSTDIR\libfftw3f-3.dll"
  Delete "$INSTDIR\changes"
  Delete "$INSTDIR\picts\pack.ico"
  Delete "$INSTDIR\picts\questionsSettings.png"
  Delete "$INSTDIR\picts\stopExam.png"
  Delete "$INSTDIR\picts\certBg.png"
  Delete "$INSTDIR\picts\soundSettings.png"
  Delete "$INSTDIR\picts\body-electro.png"
  Delete "$INSTDIR\picts\nootka-levels.ico"
  Delete "$INSTDIR\picts\wizard-left.png"
  Delete "$INSTDIR\picts\score.png"
  Delete "$INSTDIR\picts\stamp.png"
  Delete "$INSTDIR\picts\exit.png"
  Delete "$INSTDIR\picts\exam-settings.png"
  Delete "$INSTDIR\picts\body-bass.png"
  Delete "$INSTDIR\picts\fingbg-el.png"
  Delete "$INSTDIR\picts\global.png"
  Delete "$INSTDIR\picts\chlog.png"
  Delete "$INSTDIR\picts\exam.png"
  Delete "$INSTDIR\picts\startExam.png"
  Delete "$INSTDIR\picts\nootka.png"
  Delete "$INSTDIR\picts\nootka-frame.png"
  Delete "$INSTDIR\picts\flags-pl.png"
  Delete "$INSTDIR\picts\flags-hu.png"
  Delete "$INSTDIR\picts\logo.png"
  Delete "$INSTDIR\picts\flags-cs.png"
  Delete "$INSTDIR\picts\help.png"
  Delete "$INSTDIR\picts\help-frame.png"
  Delete "$INSTDIR\picts\flags-ru.png"
  Delete "$INSTDIR\picts\flags-es.png"
  Delete "$INSTDIR\picts\appearance.png"
  Delete "$INSTDIR\picts\author.png"
  Delete "$INSTDIR\picts\melody-play.png"
  Delete "$INSTDIR\picts\nootka.ico"
  Delete "$INSTDIR\picts\fork.png"
  Delete "$INSTDIR\picts\levelsSettings.png"
  Delete "$INSTDIR\picts\barChart.png"
  Delete "$INSTDIR\picts\zoom-in.png"
  Delete "$INSTDIR\picts\pickup.png"
  Delete "$INSTDIR\picts\melody.png"
  Delete "$INSTDIR\picts\fullscreen.png"
  Delete "$INSTDIR\picts\scoreSettings.png"
  Delete "$INSTDIR\picts\nootka-exam.ico"
  Delete "$INSTDIR\picts\scr.png"
  Delete "$INSTDIR\picts\clear-score.png"
  Delete "$INSTDIR\picts\prevQuest.png"
  Delete "$INSTDIR\picts\flags-en.png"
  Delete "$INSTDIR\picts\check.png"
  Delete "$INSTDIR\picts\license.png"
  Delete "$INSTDIR\picts\support.png"
  Delete "$INSTDIR\picts\nootka-exam.png"
  Delete "$INSTDIR\picts\zoom-out.png"
  Delete "$INSTDIR\picts\practice.png"
  Delete "$INSTDIR\picts\flags-fr.png"
  Delete "$INSTDIR\picts\nextQuest.png"
  Delete "$INSTDIR\picts\flags-de.png"
  Delete "$INSTDIR\picts\linearChart.png"
  Delete "$INSTDIR\picts\systemsettings.png"
  Delete "$INSTDIR\picts\body.png"
  Delete "$INSTDIR\picts\levelCreator.png"
  Delete "$INSTDIR\picts\nootka-level.png"
  Delete "$INSTDIR\picts\melody-rec.png"
  Delete "$INSTDIR\picts\guitarSettings.png"
  Delete "$INSTDIR\picts\correct.png"
  Delete "$INSTDIR\picts\repeatSound.png"
  Delete "$INSTDIR\picts\record.png"
  Delete "$INSTDIR\picts\rangeSettings.png"
  Delete "$INSTDIR\picts\notSaved.png"
  Delete "$INSTDIR\picts\fingbg.png"
  Delete "$INSTDIR\picts\accidSettings.png"
  Delete "$INSTDIR\picts\charts.png"
  Delete "$INSTDIR\picts\donors.png"
  Delete "$INSTDIR\picts\qt.png"
  Delete "$INSTDIR\picts\melodySett.png"
  Delete "$INSTDIR\Qt5Core.dll"
  Delete "$INSTDIR\gpl"
  Delete "$INSTDIR\lang\qtbase_cs.qm"
  Delete "$INSTDIR\lang\qtbase_de.qm"
  Delete "$INSTDIR\lang\qtbase_fr.qm"
  Delete "$INSTDIR\lang\qtbase_pl.qm"
  Delete "$INSTDIR\lang\qtbase_hu.qm"
  Delete "$INSTDIR\lang\qtbase_ru.qm"
  Delete "$INSTDIR\lang\qtbase_es.qm"
  Delete "$INSTDIR\lang\nootka_pl.qm"
  Delete "$INSTDIR\lang\nootka_cs.qm"
  Delete "$INSTDIR\lang\nootka_fr.qm"
  Delete "$INSTDIR\lang\nootka_de.qm"
  Delete "$INSTDIR\lang\nootka_hu.qm"
  Delete "$INSTDIR\lang\nootka_ru.qm"
  Delete "$INSTDIR\lang\nootka_es.qm"
  Delete "$INSTDIR\libvorbis.dll"
  Delete "$INSTDIR\libogg.dll"
  Delete "$INSTDIR\fonts\README"
  Delete "$INSTDIR\fonts\nootka.ttf"
  Delete "$INSTDIR\libvorbisfile.dll"
  Delete "$INSTDIR\libRtAudio.dll"
  Delete "$INSTDIR\platforms\qwindows.dll"
  Delete "$INSTDIR\bearer\qgenericbearer.dll"
  Delete "$INSTDIR\bearer\qnativewifibearer.dll"
  Delete "$INSTDIR\imageformats\qgif.dll"
  Delete "$INSTDIR\imageformats\qicns.dll"
  Delete "$INSTDIR\imageformats\qico.dll"
  Delete "$INSTDIR\imageformats\qjpeg.dll"
  Delete "$INSTDIR\imageformats\qtiff.dll"
  Delete "$INSTDIR\imageformats\qwbmp.dll"
  Delete "$INSTDIR\imageformats\qwebp.dll"
  Delete "$INSTDIR\printsupport\windowsprintersupport.dll"

  Delete "$INSTDIR\libNootkaMain.dll"
  Delete "$INSTDIR\libNootkaCore.dll"
  Delete "$INSTDIR\libNootkaMisc.dll"
  Delete "$INSTDIR\libNootkaScore.dll"
  Delete "$INSTDIR\libNootkaSound.dll"
  Delete "$INSTDIR\libNootkaWidgets.dll"
  Delete "$INSTDIR\libNootkaAboutPlugin.dll"
  Delete "$INSTDIR\libNootkaExamPlugin.dll"
  Delete "$INSTDIR\libNootkaWizardPlugin.dll"
  Delete "$INSTDIR\libNootkaLevelPlugin.dll"
  Delete "$INSTDIR\libNootkaSettingsPlugin.dll"
  Delete "$INSTDIR\libNootkaAnalyzerPlugin.dll"
  Delete "$INSTDIR\libNootkaUpdaterPlugin.dll"

  Delete "$INSTDIR\Qt5PrintSupport.dll"
  Delete "$INSTDIR\Qt5Network.dll"
  Delete "$INSTDIR\Qt5Gui.dll"

  Delete "$SMPROGRAMS\Nootka\Uninstall.lnk"
  Delete "$SMPROGRAMS\Nootka\Website.lnk"
  Delete "$DESKTOP\Nootka.lnk"
  Delete "$SMPROGRAMS\Nootka\Nootka.lnk"

  RMDir "$SMPROGRAMS\Nootka"
  RMDir "$INSTDIR\sounds"
  RMDir "$INSTDIR\picts"
  RMDir "$INSTDIR\lang"
  RMDir "$INSTDIR\fonts"
  RMDir "$INSTDIR\platforms"
  RMDir "$INSTDIR\bearer"
  RMDir "$INSTDIR\printsupport"
  RMDir "$INSTDIR\imageformats"
  RMDir "$INSTDIR"
  
  !insertmacro APP_UNASSOCIATE "nel" "nootka.level"
  !insertmacro APP_UNASSOCIATE "noo" "nootka.exam"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd
