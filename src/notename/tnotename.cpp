/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
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
    QWidget(parent),
    m_heightToSmallEmitted(false)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

// NAME LABEL
    QVBoxLayout *mainLay = new QVBoxLayout();
    mainLay->setAlignment(Qt::AlignCenter);

    m_nameLabel = new TnoteNameLabel("", this);
#if !defined(Q_OS_WIN)
    showVersionNumber();
#endif
    connect(m_nameLabel, SIGNAL(blinkingFinished()), this, SLOT(correctAnimationFinished()));
    resize();

    mainLay->addStretch(1);
    mainLay->addWidget(m_nameLabel, 0, Qt::AlignCenter);
// BUTTONS WITH NOTES TOOLBAR
    QHBoxLayout *noteLay = new QHBoxLayout();
    noteLay->addStretch(1);
    m_noteGroup =new QButtonGroup(this);
    for (int i = 0; i < 7; i++) {
        m_noteButtons[i] = new TpushButton("", this);
        noteLay->addWidget(m_noteButtons[i]);
        m_noteGroup->addButton(m_noteButtons[i], i);
    }
    noteLay->addStretch(1);
    mainLay->addLayout(noteLay);
    connect(m_noteGroup, SIGNAL(buttonClicked(int)), this, SLOT(noteWasChanged(int)));
		
		for (int i = 0; i < 8; i++)
        m_octaveButtons[i] = new TpushButton(tr(octaves[i]), this);
// ACCID BUTTONS TOOOLBAR
    m_accLay = new QHBoxLayout;		
// 			m_accLay->addStretch(1);
			m_accLay->addWidget(m_octaveButtons[0]);
			m_accLay->addStretch(2);
#if defined(Q_OS_MAC)
		QFont nf("nootka", 15, QFont::Normal);
#else
		QFont nf("nootka", 10, QFont::Normal);
#endif
    m_dblFlatButt = new TpushButton("B", this);
            m_dblFlatButt->setFont(nf);
			m_accLay->addWidget(m_dblFlatButt);
			connect(m_dblFlatButt, SIGNAL(clicked()), this, SLOT(accidWasChanged()));
    m_flatButt = new TpushButton("b", this);
            m_flatButt->setFont(nf);
			m_accLay->addWidget(m_flatButt);
			connect(m_flatButt, SIGNAL(clicked()), this, SLOT(accidWasChanged()));
    m_sharpButt = new TpushButton("#", this);
            m_sharpButt->setFont(nf);
			m_accLay->addWidget(m_sharpButt);
			connect(m_sharpButt, SIGNAL(clicked()), this, SLOT(accidWasChanged()));
    m_dblSharpButt = new TpushButton("x", this);
            m_dblSharpButt->setFont(nf);
			m_accLay->addWidget(m_dblSharpButt);
			connect(m_dblSharpButt, SIGNAL(clicked()), this, SLOT(accidWasChanged()));
    m_accLay->addStretch(2);
		m_accLay->addWidget(m_octaveButtons[7]);
// 		m_accLay->addStretch(1);
    mainLay->addLayout(m_accLay);
// OCTAVE BUTTONS TOOLBAR
    m_octaveLay = new QHBoxLayout;
    m_octaveLay->addStretch(1);
    m_octaveGroup = new QButtonGroup(this);
    for (int i = 0; i < 8; i++) {
//         m_octaveButtons[i]->setToolTip(tr(octavesFull[i]));
        m_octaveButtons[i]->setStatusTip(tr(octavesFull[i]));
				if (i > 0 && i < 7)
						m_octaveLay->addWidget(m_octaveButtons[i]);
        m_octaveGroup->addButton(m_octaveButtons[i], i);
    }
    m_octaveLay->addStretch(1);
    mainLay->addLayout(m_octaveLay);
    m_prevOctButton = -1;
		
    connect(m_octaveGroup, SIGNAL(buttonClicked(int)), this, SLOT(octaveWasChanged(int)));
    
    setLayout(mainLay);

    setStyle(gl->NnameStyleInNoteName);
    setNoteNamesOnButt(style());
    for (int i = 0; i < 3; i++) m_notes.push_back(Tnote());
    setAmbitus(gl->loString(), Tnote(gl->hiString().getChromaticNrOfNote()+gl->GfretsNumber));
    resize();
#if defined(Q_OS_WIN)
    QTimer::singleShot(2, this, SLOT(showVersionNumber()));
#endif
}


QRectF TnoteName::textRect() {
	return m_nameLabel->textRect();
}


QPoint TnoteName::textPos() {
	return mapToParent(m_nameLabel->mapToParent(m_nameLabel->textPos()));
}


QRect TnoteName::labelRect() {
	return QRect(m_nameLabel->mapToParent(m_nameLabel->geometry().topLeft()), m_nameLabel->size());
}


void TnoteName::setNoteNamesOnButt(Tnote::EnameStyle nameStyle) {
    for (int i = 0; i < 7; i++)
        m_noteButtons[i]->setText(Tnote(i + 1, 0, 0).toText(nameStyle, false));
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
#if defined (Q_OS_MAC)
				m_dblFlatButt->setAttribute(Qt::WA_LayoutUsesWidgetRect);
				m_flatButt->setAttribute(Qt::WA_LayoutUsesWidgetRect);
				m_sharpButt->setAttribute(Qt::WA_LayoutUsesWidgetRect);
        m_dblSharpButt->setAttribute(Qt::WA_LayoutUsesWidgetRect);
#endif
    }
}


/** Variables declared here have been used nowhere so far.  */
void TnoteName::setAmbitus(Tnote lo, Tnote hi) {
    m_ambitMin = lo.getChromaticNrOfNote();
    m_ambitMax = hi.getChromaticNrOfNote();
}


int TnoteName::smallSpace() {
  if (m_noteButtons[0]->geometry().y() <= m_nameLabel->geometry().bottom())
      return (m_nameLabel->geometry().bottom() - m_noteButtons[0]->geometry().y()) + 
							(m_flatButt->geometry().y() - m_noteButtons[0]->geometry().bottom()); // plus space between
  else
      return 0;
}


//##############################################################################################
//#################################### EXAM RELATED ############################################
//##############################################################################################
QColor TnoteName::prepareBgColor(const QColor& halfColor) {
		QColor mixedColor = Tcolor::merge(halfColor, palette().window().color());
    mixedColor.setAlpha(220);
		return mixedColor;
}


void TnoteName::askQuestion(Tnote note, Tnote::EnameStyle questStyle, char strNr) {
    Tnote::EnameStyle tmpStyle = m_style;
    setStyle(questStyle);
    setNoteName(note);
		m_nameLabel->showQuestionMark(QColor(gl->EquestionColor.name()));
    if (strNr)
      m_nameLabel->showStringNumber(strNr, QColor(gl->EquestionColor.name()));
		m_nameLabel->setBackgroundColor(prepareBgColor(gl->EquestionColor));
    uncheckAllButtons();
    setStyle(tmpStyle);
}


void TnoteName::prepAnswer(Tnote::EnameStyle answStyle) {
		m_nameLabel->setBackgroundColor(prepareBgColor(gl->EanswerColor));
    setNoteNamesOnButt(answStyle);
    setNameDisabled(false);
    m_notes[0] = Tnote(0,0,0); // Reset, otherwise getNoteName() returns it
}


void TnoteName::forceAccidental(char accid) {
		if (accid) {
			checkAccidButtons(accid);
		}
}


void TnoteName::markNameLabel(QColor markColor) {
		m_nameLabel->markText(QColor(markColor.name()));
}


void TnoteName::setNameDisabled(bool isDisabled) {
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
          setAttribute(Qt::WA_TransparentForMouseEvents, true);
					for (int i = 0; i < 8; i++) {
						if (m_octaveButtons[i]->isChecked())
								m_prevOctButton = i;
						m_octaveButtons[i]->setChecked(false);
					}
// 					m_nameLabel->setBackgroundColor(prepareBgColor(palette().window().color()));
				} else { // restore last checked octave vhen becomes enabled
          setAttribute(Qt::WA_TransparentForMouseEvents, false);
					if (m_prevOctButton != -1)
							m_octaveButtons[m_prevOctButton]->setChecked(true);
					else {// or set it to small octave
						  m_octaveButtons[3]->setChecked(true);
							m_prevOctButton = 3;
					}
// 					m_nameLabel->setBackgroundColor(prepareBgColor(palette().base().color()));
				}
}


void TnoteName::clearNoteName() {
    setNoteName(Tnote());
		m_nameLabel->setBackgroundColor(prepareBgColor(palette().base().color()));
}


void TnoteName::correctName(Tnote& goodName, const QColor& color, bool isWrong) {
		m_goodNote = goodName;
		m_blinkingPhase = 0;
		if (isWrong) {
			m_nameLabel->blinkCross(QColor(color.name()));
		} else
			m_nameLabel->blinkingText(2);
}



//##############################################################################################
//#################################### PRIVATE #################################################
//##############################################################################################

void TnoteName::setNameText() {
    if (m_notes[0].note) {
				QString txt = "<big>" + m_notes[0].toRichText(gl->NoctaveInNoteNameFormat) + "</big>";
        if (m_notes[1].note) {
            txt = txt + QString("  <small style=\"color: %1\">(").arg(gl->enharmNotesColor.name()) + 
							m_notes[1].toRichText(gl->NoctaveInNoteNameFormat);
            if (m_notes[2].note)
                txt = txt + "  " + m_notes[2].toRichText(gl->NoctaveInNoteNameFormat);
            txt = txt + ")</small>";
        }
        m_nameLabel->setText(txt);
    } else 
				m_nameLabel->setText("");
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
    m_nameLabel->setFixedSize(width() * 0.95, parentWidget()->height() / 9 );
	// determine to show all octave buttons in single row or put the first and the last in accidentals row
		int allButtWidth = 0;
		for (int i = 0; i < 8; i++) // total width of buttons
        allButtWidth += m_octaveButtons[i]->width();
		allButtWidth += (m_octaveButtons[3]->geometry().left() - m_octaveButtons[2]->geometry().right()) * 10; // plus space between
		if (allButtWidth < width() - 10) { // all octave buttons in single row (-10 is Qt styles quirk)
			if (m_octaveLay->count() < 9) { // only once, when not inserted yet
				m_accLay->removeWidget(m_octaveButtons[0]);
				m_accLay->removeWidget(m_octaveButtons[7]);
				m_octaveLay->insertWidget(1, m_octaveButtons[0]);
				m_octaveLay->insertWidget(m_octaveLay->count() - 1, m_octaveButtons[7]);
			}
		} else { // sub-contra and 4-line buttons with accidentals
			if (m_octaveLay->count() > 8) { // only once, when already inserted
				m_octaveLay->removeWidget(m_octaveButtons[0]);
				m_octaveLay->removeWidget(m_octaveButtons[7]);
				m_accLay->insertWidget(0, m_octaveButtons[0]);
				m_accLay->insertWidget(m_accLay->count(), m_octaveButtons[7]);
			}
		}
		if (m_noteButtons[0]->geometry().y() <= m_nameLabel->geometry().bottom()) {
			if (!m_heightToSmallEmitted) {
				QTimer::singleShot(10, this, SLOT(emitSmallHeight()));
				m_heightToSmallEmitted = true;
			}
		} else
				m_heightToSmallEmitted = false;
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


void TnoteName::correctAnimationFinished() {
	// This slot is invoked by m_nameLabel child object which is deleted.
	// It cases crashes. It is safer to call it with delay and outside
		QTimer::singleShot(1, this, SLOT(invokeBlinkingAgain()));
}


void TnoteName::invokeBlinkingAgain() {
	disconnect(m_nameLabel, SIGNAL(throwingFinished()), this, SLOT(correctAnimationFinished()));
	if (m_notes[0] != m_goodNote) {
		markNameLabel(QColor(gl->EanswerColor.name()));
		m_nameLabel->thrownText(m_goodNote.toRichText(), 150, 150);
	}
}


void TnoteName::emitSmallHeight() {
		emit heightTooSmall();
}


void TnoteName::showVersionNumber() {
    QColor C = palette().text().color();
    C.setAlpha(30);
    C = prepareBgColor(C);
    m_nameLabel->setText(QString("<b><big><span style=\"color: %1;\">").arg(C.name()) +
                         gl->version + "</span></big></b>");
}




