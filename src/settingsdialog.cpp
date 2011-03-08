#include "settingsdialog.h"
#include "tglobals.h"
#include <QVBoxLayout>

#include <iostream>


extern Tglobals *gl;

//############# TnotationRadioGroup IMPLEMENTATION ##################
/*static*/
const QString TnotationRadioGroup::strNeder = tr("Dutch");	//nederlands in Lilypond
const QString TnotationRadioGroup::strNorsk = tr("Scandinavian");//norsk in Lilypond
const QString TnotationRadioGroup::strItal = tr("Italian");
const QString TnotationRadioGroup::strEnglish = tr("English");
const QString TnotationRadioGroup::strDeutsch = tr("German");
const QString TnotationRadioGroup::strNorskExampl = "(C# ,Db Hb)";
const QString TnotationRadioGroup::strItalExampl = "(Do# ,Reb)";
const QString TnotationRadioGroup::strDeutschExampl = "(Cis , Des ,B)";
const QString TnotationRadioGroup::strEnglishExampl = "(C# ,Db ,Bb)";
const QString TnotationRadioGroup::strNederExampl = "(Cis, Des, Bes)";


TnotationRadioGroup::TnotationRadioGroup( Tnote::Enotation _notation, QWidget * parent )
        : QGroupBox(parent)
{
    notation = _notation;
    setTitle(tr("Naming style"));
    norskButt = new QRadioButton(strNorsk+" "+strNorskExampl ,this);
    italianoButt = new QRadioButton(strItal+" "+strItalExampl,this);
    deutschButt = new QRadioButton(strDeutsch+" "+strDeutschExampl,this);
    englishButt = new QRadioButton(strEnglish+" "+strEnglishExampl,this);
    nederlButt = new QRadioButton(strNeder+" "+strNederExampl,this);
    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->addWidget(norskButt);
    lay->addWidget(italianoButt);
    lay->addWidget(deutschButt);
    lay->addWidget(englishButt);
    lay->addWidget(nederlButt);
    setLayout(lay);

    buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(norskButt);
    buttonGroup->addButton(italianoButt);
    buttonGroup->addButton(deutschButt);
    buttonGroup->addButton(englishButt);
    buttonGroup->addButton(nederlButt);

    connect (buttonGroup, SIGNAL(buttonClicked(int)) ,this, SLOT(noteNameStyleWasClicked()) );
        // 			RESTORING SETTINGS
    switch (notation)	{
        case Tnote::e_norsk_Hb : norskButt->setChecked(true); break;
        case Tnote::e_deutsch_His : deutschButt->setChecked(true); break;
        case Tnote::e_italiano_Si : italianoButt->setChecked(true); break;
        case Tnote::e_english_Bb : englishButt->setChecked(true); break;
        case Tnote::e_nederl_Bis : nederlButt->setChecked(true); break;
    }
}

TnotationRadioGroup::~ TnotationRadioGroup()
{
}


Tnote::Enotation TnotationRadioGroup::getNameStyle() {
    if (norskButt->isChecked()) return Tnote::e_norsk_Hb;
    else
        if (deutschButt->isChecked()) return Tnote::e_deutsch_His;
        else
            if (italianoButt->isChecked()) return Tnote::e_italiano_Si;
            else
                if (englishButt->isChecked()) return Tnote::e_english_Bb;
                else
                    return Tnote::e_nederl_Bis;
}

void TnotationRadioGroup::noteNameStyleWasClicked() {
   emit noteNameStyleWasChanged(getNameStyle());
}


//############# EnharmAndDblAccChBox IMPLEMENTATION ##################

EnharmAndDblAccChBox::EnharmAndDblAccChBox(bool otherEnharm, bool dblAcc, QWidget *parent) :
        QWidget(parent)
{
    QVBoxLayout *lay = new QVBoxLayout();
    otherEnharmChBox = new QCheckBox(tr("show other enharmonics variants of note"),this);
    otherEnharmChBox->setChecked(otherEnharm);
    lay->addWidget(otherEnharmChBox);
    dblAccChBox = new QCheckBox(tr("use double accidentals"),this);
    dblAccChBox->setChecked(dblAcc);
    lay->addWidget(dblAccChBox);
    setLayout(lay);
}

bool EnharmAndDblAccChBox::isEnableDblAccid() {
    return dblAccChBox->isChecked();
}

bool EnharmAndDblAccChBox::isEnableOtherEnharmNotes() {
    return otherEnharmChBox->isChecked();
}


//############# ScoreSetttings IMPLEMENTATION ##################
/*static*/
const QString ScoreSettings::forExample = tr("for example");
const QString ScoreSettings::showKeySigName = tr("show names of key signature");

ScoreSettings::ScoreSettings(QWidget *parent) :
    QWidget(parent)
{

    m_workStyle = gl->nameStyleInKeySign;
    QVBoxLayout *mainLay = new QVBoxLayout();
    enablKeySignCh = new QCheckBox(tr("enable key signature"),this);
    enablKeySignCh->setChecked(gl->keySignatureEnabled);
    mainLay->addWidget(enablKeySignCh);
    QHBoxLayout *nameLay = new QHBoxLayout();
    enablKeyNameGr = new QGroupBox(showKeySigName,this);
    enablKeyNameGr->setCheckable(true);
    enablKeyNameGr->setChecked(gl->showKeySignName);
    enablKeyNameGr->setDisabled(!gl->keySignatureEnabled);

    nameStyleGr = new TnotationRadioGroup(gl->nameStyleInKeySign,this);
    nameLay->addWidget(nameStyleGr);

    nameExtGr = new QGroupBox(tr("Naming extension"));

    QVBoxLayout *majLay = new QVBoxLayout();
    majExtLab = new QLabel(tr("in major keys:"),this);
    majLay->addWidget(majExtLab,0,Qt::AlignCenter);
    majEdit = new QLineEdit(gl->majKeyNameSufix,this);
    majEdit->setMaxLength(10);
    majEdit->setAlignment(Qt::AlignCenter);
    majLay->addWidget(majEdit,0,Qt::AlignCenter);
    majExampl = new QLabel(this);
    majExampl->setAlignment(Qt::AlignCenter);
    majLay->addWidget(majExampl,0,Qt::AlignCenter);
    majLay->addStretch(1);

    QVBoxLayout *minLay = new QVBoxLayout();
    minExtLab = new QLabel(tr("in minor keys:"));
    minLay->addWidget(minExtLab,0,Qt::AlignCenter);
    minEdit = new QLineEdit(gl->minKeyNameSufix,this);
    minEdit->setMaxLength(10);
    minEdit->setAlignment(Qt::AlignCenter);
    minLay->addWidget(minEdit,0,Qt::AlignCenter);
    minExampl = new QLabel(this);
    minExampl->setAlignment(Qt::AlignCenter);
    minLay->addWidget(minExampl,0,Qt::AlignCenter);
    minLay->addStretch(1);

    QHBoxLayout *nameExtLay = new QHBoxLayout();
    nameExtLay->addLayout(majLay);
    nameExtLay->addLayout(minLay);
    nameExtGr->setLayout(nameExtLay);

    nameLay->addWidget(nameExtGr);

    enablKeyNameGr->setLayout(nameLay);
    mainLay->addWidget(enablKeyNameGr);

    setLayout(mainLay);

    connect(enablKeySignCh, SIGNAL(toggled(bool)), this, SLOT(enableKeySignGroup(bool)));
    connect(nameStyleGr, SIGNAL(noteNameStyleWasChanged(Tnote::Enotation)), this, SLOT(nameStyleWasChanged(Tnote::Enotation)));
    connect(majEdit ,SIGNAL(textChanged(QString)), this, SLOT(majorExtensionChanged()));
    connect(minEdit ,SIGNAL(textChanged(QString)), this, SLOT(minorExtensionChanged()));
//    connect(this, SIGNAL(accepted()), this, SLOT(saveSettings()));
    majExampl->setText(getMajorExample(m_workStyle));
    minExampl->setText(getMinorExample(m_workStyle));
}

void ScoreSettings::enableKeySignGroup(bool enable) {
    enablKeyNameGr->setDisabled(!enable);
}

void ScoreSettings::majorExtensionChanged() {
    majExampl->setText(getMajorExample(m_workStyle));
}

void ScoreSettings::minorExtensionChanged() {
    minExampl->setText(getMinorExample(m_workStyle));
}

QString ScoreSettings::getMajorExample(Tnote::Enotation nameStyle) {
    Tnote noteE = Tnote(3,0,0);
    Tnote noteBflat = Tnote(7,0,-1);
    QString S;
    if (majEdit->text().isEmpty()) S = "";
      else S = "-"+majEdit->text();
      return forExample + "<br><b>" + QString::fromStdString(noteE.getName(nameStyle,false)) + S +
              "<br>" + QString::fromStdString(noteBflat.getName(nameStyle,false)) + S + "</b>";
}

QString ScoreSettings::getMinorExample(Tnote::Enotation nameStyle) {
    Tnote noteCsharp = Tnote(1,0,1);
    Tnote noteG = Tnote(5,0,0);
    QString S;
    if (minEdit->text().isEmpty()) S = "";
      else S = "-"+minEdit->text();
      return forExample + "<br><b>" +
              QString::fromStdString(noteCsharp.getName(nameStyle,false)).toLower()+ S + "<br>" +
              QString::fromStdString(noteG.getName(nameStyle,false)).toLower() + S + "</b>";
}


void ScoreSettings::nameStyleWasChanged(Tnote::Enotation nameStyle) {
    m_workStyle = nameStyle;
    majExampl->setText(getMajorExample(m_workStyle));
    minExampl->setText(getMinorExample(m_workStyle));
}

void ScoreSettings::saveSettings() {
    gl->keySignatureEnabled = enablKeySignCh->isChecked();
    if (gl->keySignatureEnabled) { //changed only if key signature is enabled
        gl->majKeyNameSufix = majEdit->text();
        gl->minKeyNameSufix = minEdit->text();
        gl->nameStyleInKeySign = nameStyleGr->getNameStyle();
        gl->showKeySignName = enablKeyNameGr->isChecked();
    }
}

//############# SettingsDialog IMPLEMENTATION ##################
SettingsDialog::SettingsDialog(QWidget *parent) :
        QDialog(parent)
{
    setWindowTitle("Nootka - "+tr("programs settings"));

    QVBoxLayout *mainLay = new QVBoxLayout;
    QHBoxLayout *contLay = new QHBoxLayout;
    navList = new QListWidget(this);
    navList->setIconSize(QSize(64,64));
    navList->setFixedWidth(130);
    navList->addItem(tr("Global"));
    navList->item(0)->setIcon(QIcon(":/picts/global.svg"));
//    navList->item(0)->setSizeHint(QSize(70,120));
//    navList->item(0)->setTextAlignment(Qt::AlignHCenter);
    navList->addItem(tr("Score"));
    navList->item(1)->setIcon(QIcon(":/picts/scoreSettings.svg"));
    navList->addItem(tr("Note names"));
    navList->item(2)->setIcon(QIcon(":/picts/nameSettings.svg"));
    contLay->addWidget(navList);

    m_globalSett = new EnharmAndDblAccChBox(gl->showEnharmNotes, gl->doubleAccidentalsEnabled);
    m_scoreSett = new ScoreSettings();


    stackLayout = new QStackedLayout;
    stackLayout->addWidget(m_globalSett);
    stackLayout->addWidget(m_scoreSett);
    contLay->addLayout(stackLayout);

    mainLay->addLayout(contLay);

    QHBoxLayout *butLay = new QHBoxLayout();
    okBut = new QPushButton(tr("OK"),this);
    butLay->addStretch(1);
    butLay->addWidget(okBut);
    butLay->addStretch(1);
    cancelBut = new QPushButton(tr("cancel"),this);
    butLay->addWidget(cancelBut);
    butLay->addStretch(1);
    mainLay->addLayout(butLay);

    setLayout(mainLay);

    connect(navList, SIGNAL(currentRowChanged(int)), stackLayout, SLOT(setCurrentIndex(int)));
    connect(this, SIGNAL(accepted()), this, SLOT(saveSettings()));
    connect(cancelBut, SIGNAL(clicked()), this, SLOT(reject()));
    connect(okBut, SIGNAL(clicked()), this, SLOT(accept()));

    navList->setCurrentRow(0);
}

void SettingsDialog::saveSettings() {
    m_scoreSett->saveSettings();
    //    gl->doubleAccidentalsEnabled = enhAndDblAcc->isEnableDblAccid();
    //    gl->showEnharmNotes = enhAndDblAcc->isEnableOtherEnharmNotes();
}
