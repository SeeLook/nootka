#include "tnotename.h"
#include "tglobals.h"
#include <QtGui>

extern Tglobals *gl;

/**static*/
const QString TnoteName::octaves[6] = {tr("Contra"),tr("Great"),tr("Small"),
                tr("One-line"),tr("Two-line"),tr("Three-line") };


TnoteName::TnoteName(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout();
    mainLay->setAlignment(Qt::AlignCenter);

    nameLabel = new QLabel("NUTA",this);
    mainLay->addWidget(nameLabel);

    QHBoxLayout *noteLay = new QHBoxLayout();
    noteLay->addStretch(1);
    noteGroup =new QButtonGroup(this);
    for (int i=0; i<7; i++) {
        noteButtons[i] = new QPushButton(this);
        noteLay->addWidget(noteButtons[i]);
        noteGroup->addButton(noteButtons[i]);
    }
    noteLay->addStretch(1);
    mainLay->addLayout(noteLay);
    setNoteNamesOnButt(gl->nameStyleInNoteName);

    mainLay->addStretch(1);
    setLayout(mainLay);


}

void TnoteName::setNoteNamesOnButt(Tnote::Enotation nameStyle) {
    for (int i=0; i<7; i++) {
        noteButtons[i]->setText(QString::fromStdString(Tnote(i+1,0,0).getName(nameStyle,false)));
    }
}
