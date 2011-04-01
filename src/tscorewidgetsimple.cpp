/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tscorewidgetsimple.h"
#include <QPainter>
//#include <QtSvg/QSvgRenderer>
#include "tnoteview.h"
#include "tkeysignatureview.h"
#include "tglobals.h"
#include <QDebug>

extern Tglobals *gl;


/** This is count of notes witch can be shown on the whole widget*/
const char _C = 36;

TscoreWidgetSimple::TscoreWidgetSimple(unsigned char _notesCount, QWidget *parent):
    QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    setMinimumHeight(180);
//    updateCoefficients();

    for (int i=0; i<_notesCount; i++) {
        noteViews << new TnoteView(i,this);
        m_notes.push_back(Tnote());
    }
    keySignView = 0;

    m_dblSharpBut = new QPushButton(this);
    m_dblSharpBut->setIcon(QIcon(":/picts/dblsharp.svg"));
    setButtons(m_dblSharpBut);
    m_sharpBut = new QPushButton(this);
    m_sharpBut->setIcon(QIcon(":/picts/sharp.svg"));
    setButtons(m_sharpBut);
    m_flatBut = new QPushButton(this);
    m_flatBut->setIcon(QIcon(":/picts/flat.svg"));
    setButtons(m_flatBut);
    m_dblFlatBut = new QPushButton(this);
    m_dblFlatBut->setIcon(QIcon(":/picts/dblflat.svg"));
    setButtons(m_dblFlatBut);
    QVBoxLayout *butLay = new QVBoxLayout;
    butLay->addStretch(1);
    butLay->addWidget(m_dblSharpBut);
    butLay->addWidget(m_sharpBut);
    butLay->addSpacing(5);;;
    butLay->addWidget(m_flatBut);
    butLay->addWidget(m_dblFlatBut);
    butLay->addStretch(1);
    QHBoxLayout *mainLay = new QHBoxLayout();
    mainLay->addLayout(butLay);
    mainLay->setAlignment(Qt::AlignRight);
    setLayout(mainLay);

    for (int i=0; i<7; i++) accInKeyArr[i]=0;
    setEnabledDblAccid(false);
    setHasScord(false);
    resize();

    connect(m_dblSharpBut,SIGNAL(clicked()),this,SLOT(onAcidButtonPressed()));
    connect(m_sharpBut,SIGNAL(clicked()),this,SLOT(onAcidButtonPressed()));
    connect(m_flatBut,SIGNAL(clicked()),this,SLOT(onAcidButtonPressed()));
    connect(m_dblFlatBut,SIGNAL(clicked()),this,SLOT(onAcidButtonPressed()));
    for (int i=0; i<noteViews.size(); i++) {
        connect(noteViews[i],SIGNAL(accidWasChanged(int)),this,SLOT(changeAccidButtonsState(int)));
        connect(noteViews[i],SIGNAL(noteWasClicked(int)),this,SLOT(noteWasClicked(int)));
    }

}

void TscoreWidgetSimple::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setWindow(0,0,width(),height());

    painter.setPen(QPen(palette().foreground().color()));
    painter.setBrush(QBrush(palette().base().color(),Qt::SolidPattern));
    painter.drawRoundedRect(1,1,width()-50,height()-2,coeff,coeff);
//    painter.setPen(QPen(Qt::black));
    for (int i=16; i < 26; i += 2)
        painter.drawLine(5,(i*coeff),width()-55,(i*coeff));
    painter.setFont(QFont("Emmentaler",coeff*5));
    painter.drawText(QRect(1,qRound(12.6*coeff),coeff*5,coeff*18),Qt::AlignLeft,QString(QChar(0xe1a7)));

/**
    QSvgRenderer svgrendr(QString(":/picts/clef.svg"));
    svgrendr.render(&painter,QRectF(1,12*coeff,coeff*5,coeff*18));

    Is nice to have svg support but only for this cleff it has no sence
    untill I'll prepare class for manage svg-s and use it for accidentals and so on
 */
}

void TscoreWidgetSimple::resizeEvent(QResizeEvent *) {
    resize();
}

void TscoreWidgetSimple::resize() {
    coeff = geometry().height() / _C;
    int shift = 6*coeff;
    if (m_hasScord)
        if (shift < 85) shift = 85;
    if (keySignView) {
        keySignView->setGeometry(5*coeff,0,8*coeff,height());
        keySignView->resize(coeff);
        shift = 14*coeff;
    }
    for (int i=0; i<noteViews.size(); i++) {
        noteViews[i]->setGeometry(shift+(i*6)*coeff,0,6*coeff,height());
        noteViews[i]->resize(coeff);
    }

}


void TscoreWidgetSimple::setButtons(QPushButton *button) {
    button->setFixedSize(40,40);
    button->setIconSize(QSize(28,28));
    button->setCheckable(true);
}

void TscoreWidgetSimple::onAcidButtonPressed() {
    int ac;
    if (sender() == m_flatBut) {
        m_sharpBut->setChecked(false);
        m_dblSharpBut->setChecked(false);
        m_dblFlatBut->setChecked(false);
        if (m_flatBut->isChecked()) ac = -1;
        else ac = 0;
    } else {
        if (sender() == m_sharpBut) {
           m_flatBut->setChecked(false);
           m_dblFlatBut->setChecked(false);
           m_dblSharpBut->setChecked(false);
           if (m_sharpBut->isChecked()) ac = 1;
           else ac = 0;
       } else {
           if (sender() == m_dblFlatBut)   {
                m_sharpBut->setChecked(false);
                m_dblSharpBut->setChecked(false);
                m_flatBut->setChecked(false);
                if (m_dblFlatBut->isChecked()) ac = -2;
                else ac = 0;
            } else {
                m_sharpBut->setChecked(false);
                m_flatBut->setChecked(false);
                m_dblFlatBut->setChecked(false);
                if (m_dblSharpBut->isChecked()) ac = 2;
                else ac = 0;
            }
       }
      }
    for (int i=0; i<noteViews.size(); i++)
        noteViews[i]->setAccidText(ac);
}

void TscoreWidgetSimple::changeAccidButtonsState(int accNr) {
    m_dblSharpBut->setChecked(false);
    m_sharpBut->setChecked(false);
    m_flatBut->setChecked(false);
    m_dblFlatBut->setChecked(false);
    for (int i=0; i<noteViews.size(); i++)
        noteViews[i]->setAccidText(accNr);
    switch (accNr) {
    case -2: m_dblFlatBut->setChecked(true); break;
    case -1: m_flatBut->setChecked(true); break;
    case 1: m_sharpBut->setChecked(true); break;
    case 2: m_dblSharpBut->setChecked(true); break;
    }
}

void TscoreWidgetSimple::noteWasClicked(int index) {
    m_notes[index] = Tnote((39 - noteViews[index]->notePos())%7+1,(39 - noteViews[index]->notePos())/7 - 2,noteViews[index]->accidental());
    emit noteHasChanged(index, m_notes[index]);
}

void TscoreWidgetSimple::onKeySignatureChanged() {
    for (int i=0; i<noteViews.size(); i++) {
        if (noteViews[i]->notePos())
            noteViews[i]->moveNote(noteViews[i]->notePos());
    }
}

void TscoreWidgetSimple::setEnabledDblAccid(bool isEnabled) {
    if (isEnabled) {
        m_dblSharpBut->show();
        m_dblFlatBut->show();
        dblAccidFuse = 2;
    }
    else {
        m_dblSharpBut->hide();
        m_dblFlatBut->hide();
        dblAccidFuse = 1;
        changeAccidButtonsState(0);// set no accidental for all nottes & unset buttons
    }
}

void TscoreWidgetSimple::setEnableKeySign(bool isEnabled) {
    if (bool(keySignView) != isEnabled) {
        if (isEnabled) {
            keySignView = new TkeySignatureView(this);
            connect(keySignView, SIGNAL(keySignWasChanged()),
                    this,SLOT(onKeySignatureChanged()));
        }
        else {
            delete keySignView;
            keySignView = 0;
        }
        for (int i=0; i<7; i++) accInKeyArr[i]=0;
    }
    resize();
}

char TscoreWidgetSimple::keySignature() {
    if (keySignView) return keySignView->keySignature();
    else return 0;
}

void TscoreWidgetSimple::refreshKeySignNameStyle() {
    if (keySignView) keySignView->setNameStyle();
}

void TscoreWidgetSimple::setNote(int index, Tnote note) {
    m_notes[index] = note;
    if (note.note) {
        noteViews[index]->setNote(getNotePos(note), note.acidental);
    } else
        clearNote(index);
}

void TscoreWidgetSimple::clearNote(int index) {
    noteViews[index]->hideNote();
    m_notes[index] = Tnote(0,0,0);
}

void TscoreWidgetSimple::setAmbitus(Tnote lo, Tnote hi, int index) {
    noteViews[index]->setAmbitus(getNotePos(lo)+1,getNotePos(hi));
}

void TscoreWidgetSimple::setAmbitus(Tnote lo, Tnote hi) {
    for (int i=0; i<noteViews.size(); i++)
        setAmbitus(lo,hi,i);
}

int TscoreWidgetSimple::getNotePos(Tnote note) {
    int np = 26 - (note.octave*7 + note.note);
    return np;
}
