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
    nameLabel->setAlignment(Qt::AlignCenter);

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

    QVBoxLayout *accLay = new QVBoxLayout;
    accLay->addStretch(1);
    dblFlatButt = new QPushButton(this);
    dblFlatButt->setIcon(QIcon(":/picts/dblflat.svg"));
    dblFlatButt->setCheckable(true);
    accLay->addWidget(dblFlatButt);
    flatButt = new QPushButton(this);
    flatButt->setIcon(QIcon(":/picts/flat.svg"));
    flatButt->setCheckable(true);
    accLay->addWidget(flatButt);
    sharpButt = new QPushButton(this);
    sharpButt->setIcon(QIcon(":/picts/sharp.svg"));
    sharpButt->setCheckable(true);
    accLay->addWidget(sharpButt);
    dblSharpButt = new QPushButton(this);
    dblSharpButt->setIcon(QIcon(":/picts/dblsharp.svg"));
    dblSharpButt->setCheckable(true);
    accLay->addWidget(dblSharpButt);
    accLay->addStretch(1);
    mainLay->addLayout(accLay);
    setLayout(mainLay);

    setNoteNamesOnButt(gl->NnameStyleInNoteName);


}

void TnoteName::setNoteNamesOnButt(Tnote::Enotation nameStyle) {
    for (int i=0; i<7; i++) {
        noteButtons[i]->setText(QString::fromStdString(Tnote(i+1,0,0).getName(nameStyle,false)));
    }
}

void TnoteName::resizeEvent(QResizeEvent *) {
    nameLabel->setFixedSize(width(),height()/2-5);
    nameLabel->setFont(QFont(nameLabel->font().family(),nameLabel->height()/3.5,50));
}
