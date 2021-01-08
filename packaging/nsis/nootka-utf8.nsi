; PRODUCT_VERSION is there
!include "NSIS.definitions.nsh"

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "Nootka"
!define PRODUCT_PUBLISHER "Nootka"
!define PRODUCT_WEB_SITE "https://nootka.sourceforge.io"
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
!insertmacro MUI_LANGUAGE "Slovenian"
!insertmacro MUI_LANGUAGE "Italian"


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
LangString NelDesc ${LANG_SLOVENIAN} "Nootka stopenjska datoteka"
LangString NelDesc ${LANG_ITALIAN} "File del livello di Nootka"

LangString NootkaAct ${LANG_ENGLISH} "Open with Nootka"
LangString NootkaAct ${LANG_CZECH} "Otevřít v Nootka"
LangString NootkaAct ${LANG_GERMAN} "Mit Nootka öffnen"
LangString NootkaAct ${LANG_FRENCH} "Ouvrir avec Nootka"
LangString NootkaAct ${LANG_POLISH} "Otwórz w programie Nootka"
LangString NootkaAct ${LANG_RUSSIAN} "Открыть в Нутке"
LangString NootkaAct ${LANG_SPANISH} "Abrir con Nootka"
LangString NootkaAct ${LANG_HUNGARIAN} "Megnyitás Nootka-val"
LangString NootkaAct ${LANG_SLOVENIAN} "Odpri z Nootko"
LangString NootkaAct ${LANG_ITALIAN} "Apri con Nootka"

LangString NooDesc ${LANG_ENGLISH} "Nootka exam file"
LangString NooDesc ${LANG_CZECH} "Zkušební soubor programu Nootka"
LangString NooDesc ${LANG_GERMAN} "Nootka-Prüfungs Datei"
LangString NooDesc ${LANG_FRENCH} "Fichier d'examen Nootka"
LangString NooDesc ${LANG_POLISH} "Plik egzaminu programu Nootka"
LangString NooDesc ${LANG_RUSSIAN} "Файл Нутки с экзаменом"
LangString NooDesc ${LANG_SPANISH} "Fichero de examen de Nootka"
LangString NooDesc ${LANG_HUNGARIAN} "Nootka vizsga fájl"
LangString NooDesc ${LANG_SLOVENIAN} "Nootka izpitna datoteka"
LangString NooDesc ${LANG_ITALIAN} "File per gli esami di Nootka"

Section -AssociateMime
;FILE ASSOCIATION

!include fileasoc.nsh
!insertmacro APP_ASSOCIATE "nel" "nootka.level" "$(NelDesc)" "$INSTDIR\picts\nootka-levels.ico"     "$(NootkaAct)" "$INSTDIR\nootka.exe $\"%1$\""
!insertmacro APP_ASSOCIATE "noo" "nootka.exam" "$(NooDesc)" "$INSTDIR\picts\nootka-exam.ico"     "$(NootkaAct)" "$INSTDIR\nootka.exe $\"%1$\""
SectionEnd


Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

Section "MainGroup" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  SetOverwrite try
  File "nootka.exe"
  CreateDirectory "$SMPROGRAMS\Nootka"
  CreateShortCut "$SMPROGRAMS\Nootka\Nootka.lnk" "$INSTDIR\nootka.exe"
  CreateShortCut "$DESKTOP\Nootka.lnk" "$INSTDIR\nootka.exe"

  File "*.dll"

  File "changes"
  File "gpl"
  File "qt.conf"

  SetOutPath "$INSTDIR\bearer"
    File "bearer\*.dll"

  SetOutPath "$INSTDIR\imageformats"
    File "imageformats\*.dll"

  SetOutPath "$INSTDIR\platforms"
    File "platforms\qwindows.dll"

  SetOutPath "$INSTDIR\qmltooling"
    File "qmltooling\*.dll"

  SetOutPath "$INSTDIR"
    File /r "Qt"
    File /r /x "*.qmlc" "QtGraphicalEffects"
    File /r /x "*.qmlc" "QtQml"
    File /r /x "*.qmlc" "QtQuick"
    File /r /x "*.qmlc" "QtQuick.2"

;  SetOutPath "$INSTDIR\printsupport"
;    File "printsupport\windowsprintersupport.dll"
  
  SetOutPath "$INSTDIR\sounds"
    File "sounds\*.ogg"
    File "sounds\*.raw48-16"

  SetOutPath "$INSTDIR\picts"
    File "picts\*.ico"
    File "picts\*.png"
  SetOutPath "$INSTDIR\picts\flags"
    File "picts\flags\*.png"
  SetOutPath "$INSTDIR\picts\pane"
    File "picts\pane\*.png"

  SetOutPath "$INSTDIR\lang"
    File "lang\*.qm"

  SetOutPath "$INSTDIR\fonts"
    File "fonts\*.*"

  SetOutPath "$INSTDIR\levels"
    File /nonfatal /a /r "levels\*"

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
  LangString UninstallMess ${LANG_SLOVENIAN} "Ste prepričani, da bi res radi odstranili Nootko in še vse ostale njene komponente?"
  LangString UninstallMess ${LANG_ITALIAN} "Vuoi davvero rimuovere Nootka e tutti i suoi componenti?"

Function un.onInit
!insertmacro MUI_UNGETLANGUAGE
   MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "$(UninstallMess)" IDYES +2
   Abort
FunctionEnd

Section Uninstall
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\nootka.exe"

  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\sounds\*.*"
  Delete "$INSTDIR\changes"
  Delete "$INSTDIR\gpl"
  Delete "$INSTDIR\qt.conf"
  Delete "$INSTDIR\picts\*.*"
  Delete "$INSTDIR\lang\*.*"
  Delete "$INSTDIR\fonts\*.*"
  Delete "$INSTDIR\platforms\*.*"
  Delete "$INSTDIR\bearer\*.*"
  Delete "$INSTDIR\imageformats\*.*"
  Delete "$INSTDIR\qmltooling\*.*"
;  Delete "$INSTDIR\printsupport\*.*"

  Delete "$SMPROGRAMS\Nootka\Uninstall.lnk"
  Delete "$SMPROGRAMS\Nootka\Website.lnk"
  Delete "$DESKTOP\Nootka.lnk"
  Delete "$SMPROGRAMS\Nootka\Nootka.lnk"

  RMDir "$SMPROGRAMS\Nootka"
  RMDir "$INSTDIR\sounds"
  RMDir  /r "$INSTDIR\picts"
  RMDir  /r "$INSTDIR\levels"
  RMDir "$INSTDIR\lang"
  RMDir "$INSTDIR\fonts"
  RMDir "$INSTDIR\platforms"
  RMDir "$INSTDIR\bearer"
  RMDir "$INSTDIR\printsupport"
  RMDir "$INSTDIR\imageformats"
  RMDir "$INSTDIR\qmltooling"
  RMDir  /r "$INSTDIR\Qt"
  RMDir  /r "$INSTDIR\QtGraphicalEffects"
  RMDir  /r "$INSTDIR\QtQml"
  RMDir  /r "$INSTDIR\QtQuick"
  RMDir  /r "$INSTDIR\QtQuick.2"
  RMDir "$INSTDIR"

  !insertmacro APP_UNASSOCIATE "nel" "nootka.level"
  !insertmacro APP_UNASSOCIATE "noo" "nootka.exam"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd
