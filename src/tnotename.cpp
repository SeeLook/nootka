#include "tnotename.h"
#include "tglobals.h"
#include <QtGui>

extern Tglobals *gl;

/**static*/
const QString TnoteName::octaves[6] = {tr("Contra"),tr("Great"),tr("Small"),
                tr("1-line"),tr("2-line"),tr("3-line") };
const QString TnoteName::octavesFull[6] = {tr("Contra octave"), tr("Great octave"), tr("Small octave"), tr("One-line octave"), tr("Two-line octave"),tr("Three-line octave") };


TnoteName::TnoteName(QWidget *parent) :
    QWidget(parent)
{
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);

// NAME LABEL
    QVBoxLayout *mainLay = new QVBoxLayout();
    mainLay->setAlignment(Qt::AlignCenter);

    nameLabel = new QLabel("Note name",this);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setStyleSheet(QString("background: %1").arg(palette().base().color().name()));

    mainLay->addWidget(nameLabel);
// BUTTONS WITH NOTES TOOLBAR
    QHBoxLayout *noteLay = new QHBoxLayout();
    noteLay->addStretch(1);
    noteGroup =new QButtonGroup(this);
    for (int i=0; i<7; i++) {
        noteButtons[i] = new QPushButton(this);
        noteButtons[i]->setCheckable(true);
        noteLay->addWidget(noteButtons[i]);
        noteGroup->addButton(noteButtons[i]);
    }
    noteLay->addStretch(1);
    mainLay->addLayout(noteLay);
    connect(noteGroup, SIGNAL(buttonClicked(int)), this, SLOT(noteWasChanged(int)));
// ACCID BUTTONS TOOOLBAR
    QHBoxLayout *accLay = new QHBoxLayout;
    accLay->addStretch(1);
    dblFlatButt = new QPushButton(this);
    dblFlatButt->setIcon(QIcon(":/picts/dblflat.svg"));
    dblFlatButt->setCheckable(true);
    accLay->addWidget(dblFlatButt);
    connect(dblFlatButt, SIGNAL(clicked()), this, SLOT(accidWasChanged()));
    flatButt = new QPushButton(this);
    flatButt->setIcon(QIcon(":/picts/flat.svg"));
    flatButt->setCheckable(true);
    accLay->addWidget(flatButt);
    connect(flatButt, SIGNAL(clicked()), this, SLOT(accidWasChanged()));
    sharpButt = new QPushButton(this);
    sharpButt->setIcon(QIcon(":/picts/sharp.svg"));
    sharpButt->setCheckable(true);
    accLay->addWidget(sharpButt);
    connect(sharpButt, SIGNAL(clicked()), this, SLOT(accidWasChanged()));
    dblSharpButt = new QPushButton(this);
    dblSharpButt->setIcon(QIcon(":/picts/dblsharp.svg"));
    dblSharpButt->setCheckable(true);
    accLay->addWidget(dblSharpButt);
    connect(dblSharpButt, SIGNAL(clicked()), this, SLOT(accidWasChanged()));
    accLay->addStretch(1);
    mainLay->addLayout(accLay);
// OCTAVE BUTTONS TOOLBAR
    QHBoxLayout * octLay = new QHBoxLayout;
    octLay->addStretch(1);
    octaveGroup = new QButtonGroup(this);
    for (int i=0; i<6; i++) {
        octaveButtons[i] = new QPushButton(octaves[i],this);
        octaveButtons[i]->setToolTip(octavesFull[i]);
        octaveButtons[i]->setCheckable(true);
        octLay->addWidget(octaveButtons[i]);
        octaveGroup->addButton(octaveButtons[i]);
    }
    octLay->addStretch(1);
    mainLay->addLayout(octLay);
    connect(octaveGroup, SIGNAL(buttonClicked(int)), this, SLOT(octaveWasChanged(int)));

    setLayout(mainLay);

    setNoteNamesOnButt(gl->NnameStyleInNoteName);
    octaveButtons[2]->setChecked(true);
    for (int i=0; i<3; i++) m_notes.push_back(Tnote());


}

void TnoteName::setNoteNamesOnButt(Tnote::Enotation nameStyle) {
    for (int i=0; i<7; i++) {
        noteButtons[i]->setText(QString::fromStdString(Tnote(i+1,0,0).getName(nameStyle,false)));
    }
}

void TnoteName::paintEvent(QPaintEvent *) {
    nameLabel->setFixedSize(width(),height()/2-5);
    nameLabel->setFont(QFont(nameLabel->font().family(),nameLabel->height()/3.5,50));
}

// private setNoteName method
void TnoteName::setNoteName(char noteNr, char octNr, char accNr) {
    if (noteNr) {
        m_notes[0] = tnote(noteNr,octNr,accNr);
        setNameText();
    }
}

// public setNoteName methods
void TnoteName::setNoteName(Tnote note) {

}

void TnoteName::setNoteName(TnotesList notes) {

}

void TnoteName::setEnableDoubleAccidentals(bool isEnabled) {

}

void TnoteName::noteWasChanged(int noteNr) {
    setNoteName(noteNr+1, m_notes[0].octave, m_notes[0].acidental);

}

void TnoteName::accidWasChanged() {
    char ac;
    if (sender() == dblFlatButt) {
        flatButt->setChecked(false);
        sharpButt->setChecked(false);
        dblSharpButt->setChecked(false);
        if (dblFlatButt->isChecked()) ac = -2;
        else ac = 0;
    } else {
        if (sender() == flatButt) {
            dblFlatButt->setChecked(false);
            sharpButt->setChecked(false);
            dblSharpButt->setChecked(false);
            if (flatButt->isChecked()) ac = -1;
            else ac = 0;
    } else {
        if (sender() == sharpButt) {
            flatButt->setChecked(false);
            dblFlatButt->setChecked(false);
            dblSharpButt->setChecked(false);
            if (sharpButt->isChecked()) ac = 1;
            else ac = 0;
        } else {
            flatButt->setChecked(false);
            sharpButt->setChecked(false);
            dblFlatButt->setChecked(false);
            if (dblSharpButt->isChecked()) ac = 2;
            else ac = 0;
          }
      }
    }
    setNoteName(m_notes[0].note, m_notes[0].octave, ac);
}

void TnoteName::octaveWasChanged(int octNr) {
    setNoteName(m_notes[0].note, octNr-3, m_notes[0].acidental);
}

QString TnoteName::noteToRichText(Tnote note) {
    return QString::fromStdString(note.getName(gl->NnameStyleInNoteName,false));
}

void TnoteName::setButtons() {



}

