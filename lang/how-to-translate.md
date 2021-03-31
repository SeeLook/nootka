
## HOW TO TRANSLATE NOOTKA
---------------------------


#### This process is very easy and does not require any programming skills, only some time and a bit of patience.
---------------------------
#### Current translation status

[![Translation status](https://hosted.weblate.org/widgets/nootka/-/translations/multi-auto.svg)](https://hosted.weblate.org/engage/nootka/) 
<a href="https://hosted.weblate.org/engage/nootka/">
<img src="https://hosted.weblate.org/widgets/nootka/-/open-graph.png" width="300" />
</a>

---------------------------
#### A few hints at the beginning
  - Do not remove any special characters like:  
    **\n**, **%1**, **%n** or HTML tags like **&lt;br&gt;**, **&lt;whatever&gt;translatable word&lt;/whatever&gt;** or **\[STUDENT\]**.  
    Obviously you can move those characters and tags to make whole translated sentence look in proper way in your language
  - Please be responsible! Use kind words and official musical terms.  
  - Do not use some slang words even musical - Nootka is used by children and pupils.

---------------------------

##  <u>The easiest way</u>: translation with *Weblate* online service
  1. Visit <https://hosted.weblate.org/projects/nootka/translations/>
    and simply start new translation.  
    Keep in mind you will need to create free account.

  2. Your translation will be sent to Nootka repository automatically and next day or two also included in nightly builds that you can test.

  3. But if you want to check translation at spot, you need to use *Qt linguist* way.

---------------------------
## <u>The best way</u>: translation with *Qt Linguist* or *lokalize*
  1. Install Qt Linguist:  
    Linux users will prefer using package manager of their distribution. Linguist will be there.  
    For Mac and Windows users:
      - form <https://www.qt.io/download-open-source> download online installer,
        launch it and follow its instructions - select the latest version of Qt for Desktop.
        
      - But also Windows users can download standalone Qt linguist application in zip archive:  
        <https://github.com/thurask/Qt-Linguist/releases> (latest version prefferred)

     or use other software for translators that supports *.ts format,  
     i.e. [lokalize](https://apps.kde.org/en/lokalize).
      
  2. Download [Nootka language file: template-nootka_xx.ts](https://www.opencode.net/seelook/nootka/raw/master/lang/template-nootka_xx.ts)  
     remove **template-** from the file name and change that **xx** to your language code to get file name like i.e.: `nootka_en.ts`

  3. Open this file in *Qt Linguist* (or any other app) and translate.

  4. In Qt Linguist, at every moment you can generate a "language package" form menu `File->Release` (file with \*.qm extension)
     and put that file in Nootka installation directory into the `lang` folder  
     i.e.: `path_to_nootka/lang/nootka_en.qm`  
     and run the app to see the effects of the translation

  5. A few hints for *Qt Linguist*  
     - To approve translated entry you can use `[CTRL+Enter]` - it will be automatically marked as finished.
     - To copy original entry use `[CTRL+b]` and then just translate.

  6. When translation will be ready or you give up :-(, please send what you have to:  
     <seelook@gmail.com>  

---------------------------
## <u>Other possible way</u>: translation online trough *LaunchPad*
     A disadvantage of this method is that you can not easily generate translation package ( *.qm)
     to check your translation in action.

  1. You will need to have account on <https://launchpad.net/>

  2. Then visit a site: <https://translations.launchpad.net/nootka>

  3. Start translation or continue existing one.

  4. To generate Nootka-loadable file and launch Nootka with it you have to:
     - download \*.po file from site
     - convert it to \*.qm (it is necessary to have Qt installed.  
       Command line *lconvert* program is shipped with it)  
       `lconvert -i your_file.po -o nootka_languageCode.qm`
     - and put that file in Nootka installation directory into the lang folder

  5. Let us know: <seelook@gmail.com> when ready, we will fetch translation from the *LaunchPad* service.


---------------------------
    How to give some info about translator (you) you will find during translation.
---------------------------

## Have a fun !!!


