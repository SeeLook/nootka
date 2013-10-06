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


#include "tnotename.h"
#include "tnotenamelabel.h"
#include "tglobals.h"
#include "tpushbutton.h"
#include <QtGui>


extern Tglobals *gl;


/**static*/
Tnote::EnameStyle TnoteName::m_style = Tnote::e_italiano_Si;
const char * const TnoteName::octaves[8] = { QT_TR_NOOP("Sub"), 	QT_TR_NOOP("Contra"), QT_TR_NOOP("Great"), QT_TR_NOOP("Small"),
				QT_TR_NOOP("1-line"), QT_TR_NOOP("2-line"), QT_TR_NOOP("3-line"), QT_TR_NOOP("4-line") };
const char * const TnoteName::octavesFull[8] = { QT_TR_NOOP("Subcontra octave"), 
					QT_TR_NOOP("Contra octave"), QT_TR_NOOP("Great octave"), QT_TR_NOOP("Small octave"),
                    QT_TR_NOOP("One-line octave"), QT_TR_NOOP("Two-line octave"),
                    QT_TR_NOOP("Three-line octave"), QT_TR_NOOP("Four-line octave") };

//#######################################################################################################
//#################################### PUBLIC ###########################################################
//#######################################################################################################

TnoteName::TnoteName(QWidget *parent) :
    QWidget(parent)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

// NAME LABEL
    QVBoxLayout *mainLay = new QVBoxLayout();
    mainLay->setAlignment(Qt::AlignCenter);

    m_nameLabel = new TnoteNameLabel("<b><span style=\"font-size: 24px; color: green;\">" +
                           gl->version + "</span></b>",this);
    m_nameLabel->setAlignment(Qt::AlignCenter);
		connect(m_nameLabel, SIGNAL(blinkingFinished()), this, SLOT(correctFadeAnimation()));
    resize();

    mainLay->addStretch(1);
    mainLay->addWidget(m_nameLabel, 0, Qt::AlignCenter);
// BUTTONS WITH NOTES TOOLBAR
    QHBoxLayout *noteLay = new QHBoxLayout();
    noteLay->addStretch(1);
    m_noteGroup =new QButtonGroup(this);
    for (int i=0; i<7; i++) {
        m_noteButtons[i] = new TpushButton("", this);
        noteLay->addWidget(m_noteButtons[i]);
        m_noteGroup->addButton(m_noteButtons[i],i);
    }
    noteLay->addStretch(1);
    mainLay->addLayout(noteLay);
    connect(m_noteGroup, SIGNAL(buttonClicked(int)), this, SLOT(noteWasChanged(int)));
		
		for (int i = 0; i < 8; i++)
        m_octaveButtons[i] = new TpushButton(tr(octaves[i]), this);
// ACCID BUTTONS TOOOLBAR
    QHBoxLayout *accLay = new QHBoxLayout;
		
		accLay->addStretch(1);
		accLay->addWidget(m_octaveButtons[0]);		
    accLay->addStretch(2);
    m_dblFlatButt = new TpushButton("B", this);
    #if defined(Q_OS_MAC)
     dblFlatButt->setFont(QFont("nootka", 15, QFont::Normal));
    #else
     m_dblFlatButt->setFont(QFont("nootka", 10, QFont::Normal));
    #endif
    accLay->addWidget(m_dblFlatButt);
    connect(m_dblFlatButt, SIGNAL(clicked()), this, SLOT(accidWasChanged()));
    m_flatButt = new TpushButton("b", this);
    #if defined(Q_OS_MAC)
     flatButt->setFont(QFont("nootka", 15, QFont::Normal));
    #else
     m_flatButt->setFont(QFont("nootka", 10, QFont::Normal));
    #endif
    accLay->addWidget(m_flatButt);
    connect(m_flatButt, SIGNAL(clicked()), this, SLOT(accidWasChanged()));
    m_sharpButt = new TpushButton("#", this);
    #if defined(Q_OS_MAC)
     sharpButt->setFont(QFont("nootka", 15, QFont::Normal));
    #else
     m_sharpButt->setFont(QFont("nootka", 10, QFont::Normal));
    #endif
    accLay->addWidget(m_sharpButt);
    connect(m_sharpButt, SIGNAL(clicked()), this, SLOT(accidWasChanged()));
    m_dblSharpButt = new TpushButton("x", this);
    #if defined(Q_OS_MAC)
     dblSharpButt->setFont(QFont("nootka", 15, QFont::Normal));
    #else
     m_dblSharpButt->setFont(QFont("nootka", 10, QFont::Normal));
    #endif
    accLay->addWidget(m_dblSharpButt);
    connect(m_dblSharpButt, SIGNAL(clicked()), this, SLOT(accidWasChanged()));
    accLay->addStretch(2);
		accLay->addWidget(m_octaveButtons[7]);
		accLay->addStretch(1);
    mainLay->addLayout(accLay);
// OCTAVE BUTTONS TOOLBAR
    QHBoxLayout * octLayRow1 = new QHBoxLayout;
// 		QHBoxLayout * octLayRow2 = new QHBoxLayout;
    octLayRow1->addStretch(1);
// 		octLayRow2->addStretch(1);
    m_octaveGroup = new QButtonGroup(this);
    for (int i = 0; i < 8; i++) {
//         octaveButtons[i] = new TpushButton(tr(octaves[i]), this);
        m_octaveButtons[i]->setToolTip(tr(octavesFull[i]));
        m_octaveButtons[i]->setStatusTip(m_octaveButtons[i]->toolTip());
				if (i > 0 && i < 7)
					octLayRow1->addWidget(m_octaveButtons[i]);
// 				if (i % 2) { // upper: 1, 3, 5, 7
// 					octLayRow1->addWidget(octaveButtons[i]);
// 					octLayRow1->addStretch(1);
// 				} else { // lower: 0, 2, 4, 6
// 					octLayRow2->addWidget(octaveButtons[i]);
// 					octLayRow2->addStretch(1);
// 				}
        m_octaveGroup->addButton(m_octaveButtons[i], i);
    }
    octLayRow1->addStretch(1);
// 		octLayRow2->addStretch(3);
    mainLay->addLayout(octLayRow1);
// 		mainLay->addLayout(octLayRow2);
    m_prevOctButton = -1;
		
    connect(m_octaveGroup, SIGNAL(buttonClicked(int)), this, SLOT(octaveWasChanged(int)));
    
//     mainLay->addSpacing(5);
    setLayout(mainLay);

    setStyle(gl->NnameStyleInNoteName);
    setNoteNamesOnButt(style());
    for (int i = 0; i < 3; i++) m_notes.push_back(Tnote());
    setAmbitus(gl->loString(),
               Tnote(gl->hiString().getChromaticNrOfNote()+gl->GfretsNumber));
    resize();

}

void TnoteName::setNoteNamesOnButt(Tnote::EnameStyle nameStyle) {
    for (int i = 0; i < 7; i++) {
        m_noteButtons[i]->setText(Tnote(i + 1, 0, 0).toText(nameStyle, false));
    }
}

void TnoteName::setStyle(Tnote::EnameStyle style) {
    m_style = style;
		Tnote::defaultStyle = style;
}


void TnoteName::setNoteName(Tnote note) {
	if (m_notes[0].note) { // uncheck current note button
		m_noteButtons[m_notes[0].note - 1]->setChecked(false);
	}
	if (m_prevOctButton != -1) 
		m_octaveButtons[m_prevOctButton]->setChecked(false);
	if (note.note) {
			m_notes[0] = note;
			setButtons(note);
	} else {
			m_notes[0] = Tnote(0,0,0);
			m_notes[1] = Tnote(0,0,0);
			m_notes[2] = Tnote(0,0,0);
// 	m_prevOctButton = -1;
	}
	setNameText();
}


void TnoteName::setNoteName(TnotesList notes) {
    TnotesList::iterator it = notes.begin();
    ++it;
    if (it != notes.end())
        m_notes[1] = *(it);
    else m_notes[1] = Tnote(0,0,0);
    ++it;
    if (it != notes.end())
        m_notes[2] = *(it);
    else m_notes[2] = Tnote(0,0,0);
    setNoteName(notes[0]);
}


void TnoteName::setEnabledDblAccid(bool isEnabled) {
    if (isEnabled) {
        m_dblFlatButt->show();
        m_dblSharpButt->show();
    } else {
        m_dblFlatButt->hide();
        m_dblSharpButt->hide();
        m_notes[2] = Tnote(0,0,0);
        setNameText();
    }
}


void TnoteName::setEnabledEnharmNotes(bool isEnabled) {
    if (!isEnabled) {
        m_notes[1] = Tnote(0,0,0);
        m_notes[2] = Tnote(0,0,0);
        setNameText();
    }
}


void TnoteName::resize(int fontSize) {    
    if (fontSize) {
        QFont f = QFont(m_noteButtons[0]->font().family());
        f.setPixelSize(fontSize);
        for (int i = 0; i < 7; i++) {
            m_noteButtons[i]->setFont(f);
        }
        for (int i = 0; i < 8; i++) {
            m_octaveButtons[i]->setFont(f);
        }
        f = QFont(m_dblFlatButt->font().family());
        f.setPointSize(fontSize);
        QFontMetrics fMetr(f);
        qreal fact = ((qreal)fontSize / (qreal)fMetr.boundingRect("b").height()) * 1.4;
        f.setPointSize(f.pointSize() * fact);
        m_dblFlatButt->setFont(f);
        m_flatButt->setFont(f);
        m_sharpButt->setFont(f);
        m_dblSharpButt->setFont(f);
    }
}

void TnoteName::setAmbitus(Tnote lo, Tnote hi) {
    m_ambitMin = lo.getChromaticNrOfNote();
    m_ambitMax = hi.getChromaticNrOfNote();
}


//##############################################################################################
//#################################### EXAM RELATED ############################################
//##############################################################################################
QColor TnoteName::prepareBgColor(const QColor& halfColor) {
		QColor mixedColor = gl->mergeColors(halfColor, palette().window().color());
    mixedColor.setAlpha(220);
		return mixedColor;
}


void TnoteName::askQuestion(Tnote note, Tnote::EnameStyle questStyle, char strNr) {
    Tnote::EnameStyle tmpStyle = m_style;
    setStyle(questStyle);
    setNoteName(note);
    QString sN = "";
    if (strNr) sN = QString("  %1").arg((int)strNr);
    m_nameLabel->setText(m_nameLabel->text() +
                       QString(" <span style=\"color: %1; font-family: nootka;\">?%2</span>").arg(gl->EquestionColor.name()).arg(sN));
		m_nameLabel->setBackgroundColor(prepareBgColor(gl->EquestionColor));
    uncheckAllButtons();
    setStyle(tmpStyle);
}


void TnoteName::prepAnswer(Tnote::EnameStyle answStyle) {
		m_nameLabel->setBackgroundColor(prepareBgColor(gl->EanswerColor));
    setNoteNamesOnButt(answStyle);
    setNameDisabled(false);
// 		if (backNote.acidental) {
//         QString accTxt = QString(" <sub><i><span style=\"color: %1;\">(%2)</span></i></sub>").arg(gl->GfingerColor.name()).arg(QString::fromStdString(signsAcid[backNote.acidental + 2]));
//         m_nameLabel->setText(m_nameLabel->text() + accTxt);
//         checkAccidButtons(backNote.acidental);
//     }
    m_notes[0] = Tnote(0,0,0); // Reset, otherwise getNoteName() returns it
}


void TnoteName::forceAccidental(char accid) {
		if (accid) {
			QString accTxt = QString(" <sub><i><span style=\"color: %1;\">(%2)</span></i></sub>").arg(gl->GfingerColor.name()).arg(QString::fromStdString(signsAcid[accid + 2]));
					m_nameLabel->setText(m_nameLabel->text() + accTxt);
			checkAccidButtons(accid);
		}
}


void TnoteName::markNameLabel(QColor markColor) {
		m_nameLabel->setBackgroundColor(prepareBgColor(markColor));
		m_nameLabel->setText(QString("<span style=\"color: %1; \">").arg(markColor.name()) + m_nameLabel->text() + "</span>");
}


void TnoteName::setNameDisabled(bool isDisabled) {
//     if (isDisabled) {
        uncheckAllButtons();
        for (int i = 0; i < 7; i++)
            m_noteButtons[i]->setDisabled(isDisabled);
        for (int i = 0; i < 8; i++)
            m_octaveButtons[i]->setDisabled(isDisabled);
        m_dblFlatButt->setDisabled(isDisabled);
        m_flatButt->setDisabled(isDisabled);
        m_sharpButt->setDisabled(isDisabled);
        m_dblSharpButt->setDisabled(isDisabled);
				if (isDisabled) { // uncheck when disabled
					for (int i = 0; i < 8; i++) {
						if (m_octaveButtons[i]->isChecked())
								m_prevOctButton = i;
						m_octaveButtons[i]->setChecked(false);
					}
				} else { // restore last checked octave vhen becomes enabled
					if (m_prevOctButton != -1)
							m_octaveButtons[m_prevOctButton]->setChecked(true);
					else {// or set it to small octave
						  m_octaveButtons[3]->setChecked(true);
							m_prevOctButton = 3;
					}
				}
}


void TnoteName::clearNoteName() {
    setNoteName(Tnote());
		m_nameLabel->setBackgroundColor(prepareBgColor(palette().base().color()));
}


void TnoteName::correctName(Tnote& goodName, const QColor& color) {
		m_goodNote = goodName;
		m_blinkingPhase = 0;
		m_nameLabel->blinkCross(QColor(color.name()));
}



//##############################################################################################
//#################################### PRIVATE #################################################
//##############################################################################################

void TnoteName::setNameText() {
    if (m_notes[0].note) {
				QString txt = m_notes[0].toRichText();
        if (m_notes[1].note) {
            txt = txt + QString("  <span style=\"font-size: %1px; color: %2\">(").arg(m_nameLabel->font().pointSize()-2).arg(gl->enharmNotesColor.name()) + m_notes[1].toRichText();
            if (m_notes[2].note)
                txt = txt + "  " + m_notes[2].toRichText();
            txt = txt + ")</span>";
        }
        m_nameLabel->setText(txt);
    } else 
				m_nameLabel->setText("");;
}


void TnoteName::setNoteName(char noteNr, char octNr, char accNr) {
    m_notes[0] = Tnote(noteNr, octNr, accNr);
    if (noteNr) {
        if (gl->showEnharmNotes) {
            TnotesList enharmList = m_notes[0].getTheSameNotes(gl->doubleAccidentalsEnabled);
            TnotesList::iterator it = enharmList.begin();
            ++it;
            if (it != enharmList.end())
                m_notes[1] = *(it);
            else m_notes[1] = Tnote();
            ++it;
            if (it != enharmList.end())
                m_notes[2] = *(it);
            else m_notes[2] = Tnote();
        }
        setNameText();
        emit noteNameWasChanged(m_notes[0]);
    }
}


void TnoteName::setButtons(Tnote note) {
    m_noteButtons[note.note-1]->setChecked(true);
		checkAccidButtons(note.acidental);
    if (note.octave >= -3 && note.octave <= 4) {
			if (m_octaveButtons[note.octave + 3]->isEnabled()) { // check octave button only when is enabled
					m_octaveButtons[note.octave + 3]->setChecked(true);
					m_prevOctButton = note.octave + 3;
			}
		}
}


void TnoteName::uncheckAllButtons() {
    uncheckAccidButtons();
    m_noteGroup->setExclusive(false);
    for (int i = 0; i < 7; i++)
        m_noteButtons[i]->setChecked(false);
//     for (int i = 0; i < 8; i++)
//         octaveButtons[i]->setChecked(false);
// 		m_prevOctButton = -1;
    m_noteGroup->setExclusive(true);
}


void TnoteName::uncheckAccidButtons() {
    m_dblFlatButt->setChecked(false);
    m_flatButt->setChecked(false);
    m_sharpButt->setChecked(false);
    m_dblSharpButt->setChecked(false);
}


void TnoteName::checkAccidButtons(char accid) {
		uncheckAccidButtons();
		switch (accid) {
				case -1 : m_flatButt->setChecked(true); break;
				case 1 : m_sharpButt->setChecked(true); break;
				case -2 : m_dblFlatButt->setChecked(true); break;
				case 2 : m_dblSharpButt->setChecked(true); break;
    }
}


char TnoteName::getSelectedAccid() {
		if (m_flatButt->isChecked())
			return -1;
		if (m_sharpButt->isChecked())
			return 1;
		if (m_dblFlatButt->isChecked())
			return -2;
		if (m_dblSharpButt->isChecked())
			return 2;
		return 0; // no accide selected
}



void TnoteName::resizeEvent(QResizeEvent* ) {
    m_nameLabel->setFixedSize(width() * 0.9, parentWidget()->height() / 9 );
    QFont f(QFont(m_nameLabel->font().family(), qRound(m_nameLabel->height() * 0.55), 50));
    QFontMetrics fMetr(f);
    qreal fact = (m_nameLabel->height() * 0.95) / fMetr.boundingRect("A").height();
    f.setPointSize(f.pointSize() * fact);
    m_nameLabel->setFont(f);
    m_nameLabel->setText(m_nameLabel->text());
}

//##############################################################################################
//#################################### PRIVATE SLOTS ###########################################
//##############################################################################################
void TnoteName::noteWasChanged(int noteNr) {
		if (m_notes[0].note) {
			if (m_notes[0].note != noteNr+1) //uncheck only if previous was different
				m_noteButtons[m_notes[0].note-1]->setChecked(false);
		} 
		m_noteButtons[noteNr]->setChecked(true);
		if (m_octaveGroup->checkedId() == -1 && m_prevOctButton == -1) {
			m_octaveButtons[3]->setChecked(true);
			m_prevOctButton = 3;
		}
		setNoteName(noteNr + 1, m_prevOctButton - 3, getSelectedAccid());
		emit noteButtonClicked();
}


void TnoteName::accidWasChanged() {
  if (sender() != m_flatButt)
    m_flatButt->setChecked(false);
  if (sender() != m_sharpButt)
    m_sharpButt->setChecked(false);
  if (sender() != m_dblSharpButt)
    m_dblSharpButt->setChecked(false);
  if (sender() != m_dblFlatButt)
    m_dblFlatButt->setChecked(false);
// 	char ac;
  TpushButton *button = static_cast<TpushButton *>(sender());
  button->setChecked(!button->isChecked());
	setNoteName(m_notes[0].note, m_prevOctButton - 3, getSelectedAccid());
}


void TnoteName::octaveWasChanged(int octNr) { // octNr is button nr in the group
  if (octNr != m_prevOctButton && m_prevOctButton != -1)
    m_octaveButtons[m_prevOctButton]->setChecked(false);
  m_prevOctButton = octNr;
  m_octaveButtons[octNr]->setChecked(true);
	setNoteName(m_notes[0].note, octNr - 3, getSelectedAccid());
}


void TnoteName::correctFadeAnimation() {
	m_nameLabel->crossFadeText(m_goodNote.toRichText(m_style), prepareBgColor(gl->EanswerColor), 300);
}


