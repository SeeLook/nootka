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
#include <tglobals.h>
#include <widgets/tpushbutton.h>
#include <QtWidgets>


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
    QWidget()
{
		m_menu = new QMenu(parent);
		setParent(m_menu);
		m_menu->setStyleSheet("background-color: palette(window)");
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

// NAME LABEL
    QVBoxLayout *mainLay = new QVBoxLayout();
    mainLay->setAlignment(Qt::AlignCenter);

    m_nameLabel = new TnoteNameLabel("", this);
		m_nameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(m_nameLabel, SIGNAL(blinkingFinished()), this, SLOT(correctAnimationFinished()));


    mainLay->addStretch(1);
    mainLay->addWidget(m_nameLabel);
// BUTTONS WITH NOTES TOOLBAR
    QHBoxLayout *noteLay = new QHBoxLayout();
    noteLay->addStretch(1);
    m_noteGroup =new QButtonGroup(this);
    for (int i = 0; i < 7; i++) {
        m_noteButtons[i] = new TpushButton("", this);
        noteLay->addWidget(m_noteButtons[i]);
				m_noteButtons[i]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        m_noteGroup->addButton(m_noteButtons[i], i);
    }
    noteLay->addStretch(1);
    mainLay->addLayout(noteLay);
    connect(m_noteGroup, SIGNAL(buttonClicked(int)), this, SLOT(noteWasChanged(int)));
		
// ACCID BUTTONS TOOOLBAR
    QHBoxLayout *accLay = new QHBoxLayout;
		accLay->addStretch(1);
#if defined(Q_OS_MAC)
		QFont nf("nootka", 15, QFont::Normal);
#else
		QFont nf("nootka", 10, QFont::Normal);
#endif
    m_dblFlatButt = new TpushButton("B", this);
            m_dblFlatButt->setFont(nf);
			accLay->addWidget(m_dblFlatButt);
			connect(m_dblFlatButt, SIGNAL(clicked()), this, SLOT(accidWasChanged()));
    m_flatButt = new TpushButton("b", this);
            m_flatButt->setFont(nf);
			accLay->addWidget(m_flatButt);
			connect(m_flatButt, SIGNAL(clicked()), this, SLOT(accidWasChanged()));
    m_sharpButt = new TpushButton("#", this);
            m_sharpButt->setFont(nf);
			accLay->addWidget(m_sharpButt);
			connect(m_sharpButt, SIGNAL(clicked()), this, SLOT(accidWasChanged()));
    m_dblSharpButt = new TpushButton("x", this);
            m_dblSharpButt->setFont(nf);
			accLay->addWidget(m_dblSharpButt);
			connect(m_dblSharpButt, SIGNAL(clicked()), this, SLOT(accidWasChanged()));
    accLay->addStretch(1);
    mainLay->addLayout(accLay);
// OCTAVE BUTTONS TOOLBAR        
    QHBoxLayout *upOctaveLay = new QHBoxLayout;
		QHBoxLayout *loOctaveLay = new QHBoxLayout;
    QLabel *octavesLab = new QLabel("<a href=\"http://en.wikipedia.org/wiki/Octave\">" + tr("octaves") + ":</a>", this);
    octavesLab->setOpenExternalLinks(true);
    octavesLab->setStatusTip(tr("Click to see explanation of 'octave' at <b>en.wikipedia.org/wiki/Octave</b>", "You can change a link to article in your language"));
    upOctaveLay->addWidget(octavesLab);
    m_octaveGroup = new QButtonGroup(this);
    for (int i = 0; i < 8; i++) {
        m_octaveButtons[i] = new TpushButton(tr(octaves[i]), this);
        m_octaveButtons[i]->setStatusTip(tr(octavesFull[i]));
				if (i % 2) {
            if (i > 1)
              upOctaveLay->addStretch(1);
						upOctaveLay->addWidget(m_octaveButtons[i]);
				} else {
						loOctaveLay->addWidget(m_octaveButtons[i]);
						if (i < 7)
							loOctaveLay->addStretch(1);
				}
        m_octaveGroup->addButton(m_octaveButtons[i], i);
    }
//     upOctaveLay->addStretch(1);
    mainLay->addLayout(upOctaveLay);
		mainLay->addLayout(loOctaveLay);
    m_prevOctButton = -1;
		
    connect(m_octaveGroup, SIGNAL(buttonClicked(int)), this, SLOT(octaveWasChanged(int)));
    
    setLayout(mainLay);

    setStyle(gl->NnameStyleInNoteName);
    setNoteNamesOnButt(style());
    for (int i = 0; i < 3; i++) m_notes.push_back(Tnote());
    setAmbitus(gl->loString(), Tnote(gl->hiString().getChromaticNrOfNote()+gl->GfretsNumber));

}


TnoteName::~TnoteName()
{
	setParent(0); // release TnoteName to be able delete m_menu
	delete m_menu;
}

/** @p scoreFactor parameter is used in resizeEvent() method 
 * because TnoteName gets its size only after the menu invokes exec() */ 
void TnoteName::exec(QPoint pos, qreal scoreFactor) {
	m_scoreFactor = scoreFactor;
	m_menu->exec(pos);
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
    }
}


/** Variables declared here have been used nowhere so far.  */
void TnoteName::setAmbitus(Tnote lo, Tnote hi) {
    m_ambitMin = lo.getChromaticNrOfNote();
    m_ambitMax = hi.getChromaticNrOfNote();
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
		return 0; // no accidentals selected
}


void TnoteName::resizeEvent(QResizeEvent* ) {
	m_menu->resize(size());
	if (m_menu->geometry().x() > qApp->desktop()->availableGeometry().width() / 2)
				m_menu->move(m_menu->pos().x() - width() - 11.5 * m_scoreFactor, m_menu->pos().y());
	// Move note name menu on the left screen side to alow to see edited note
}


bool TnoteName::event(QEvent* event) {
  if (gl->hintsEnabled && event->type() == QEvent::StatusTip) {
      QStatusTipEvent *se = static_cast<QStatusTipEvent *>(event);
      emit statusTipRequired(se->tip());
  }
      return QWidget::event(event);
}


//##############################################################################################
//#################################### PRIVATE SLOTS ###########################################
//##############################################################################################
void TnoteName::noteWasChanged(int noteNr) {
		if (m_notes[0].note) {
			if (m_notes[0].note != noteNr+1) //unchecked only if previous was different
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




