/***************************************************************************
 *   Copyright (C) 2011-2016 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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
#include "tnametip.h"
#include <tinitcorelib.h>
#include <widgets/tpushbutton.h>
#include <tnoofont.h>
#include <tscoreparams.h>
#include <QtWidgets/QtWidgets>


/**static*/
Tnote::EnameStyle TnoteName::m_style = Tnote::e_italiano_Si;
const char * const TnoteName::octaves[8] = { QT_TR_NOOP("Sub"), 	QT_TR_NOOP("Contra"), QT_TR_NOOP("Great"), QT_TR_NOOP("Small"),
				QT_TR_NOOP("1-line"), QT_TR_NOOP("2-line"), QT_TR_NOOP("3-line"), QT_TR_NOOP("4-line") };
const char * const TnoteName::octavesFull[8] = { QT_TR_NOOP("Subcontra octave"), 
					QT_TR_NOOP("Contra octave"), QT_TR_NOOP("Great octave"), QT_TR_NOOP("Small octave"),
                    QT_TR_NOOP("One-line octave"), QT_TR_NOOP("Two-line octave"),
                    QT_TR_NOOP("Three-line octave"), QT_TR_NOOP("Four-line octave") };

TnoteName* TnoteName::m_instance = nullptr;



TnoteName::TnoteName(QWidget *parent) :
	QWidget(parent),
	m_menu(0),
	m_fontSize(12),
	m_tip(0),
	m_isMenu(true)
{
  if (m_instance) {
    qDebug() << "TnoteName instance already exists";
    return;
  }

  m_instance = this;
	m_menuParent = parent;
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	setObjectName(QStringLiteral("TnoteName"));
	setStyleSheet(QStringLiteral("QWidget#TnoteName {background: transparent;}"));

// NAME LABEL
	QVBoxLayout *mainLay = new QVBoxLayout();
	mainLay->setAlignment(Qt::AlignCenter);
	mainLay->setContentsMargins(2, 2, 2, 2);
#if defined (Q_OS_MAC)
  mainLay->setSpacing(5);
#endif

	m_nextNoteButt = new QPushButton(QIcon(QWidget::style()->standardIcon(QStyle::SP_ArrowRight)), "", this);
	m_nextNoteButt->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	m_nextNoteButt->setStatusTip(tr("Next note"));
	connect(m_nextNoteButt, SIGNAL(clicked()), this, SLOT(nextNoteSlot()));
	m_prevNoteButt = new QPushButton(QIcon(QWidget::style()->standardIcon(QStyle::SP_ArrowLeft)), "", this);
	m_prevNoteButt->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	m_prevNoteButt->setStatusTip(tr("Previous note"));
	connect(m_prevNoteButt, SIGNAL(clicked()), this, SLOT(prevNoteSlot()));
	
	m_nameLabel = new TnoteNameLabel(QString(), this);
	connect(m_nameLabel, SIGNAL(blinkingFinished()), this, SLOT(correctAnimationFinished()));

	QBoxLayout *nameLay = new QBoxLayout(QBoxLayout::LeftToRight);
		nameLay->addWidget(m_prevNoteButt);
		nameLay->addWidget(m_nameLabel);
		nameLay->addWidget(m_nextNoteButt);
	mainLay->addLayout(nameLay);
// #if defined(Q_OS_ANDROID)
//   m_prevNoteButt->hide();
//   m_nextNoteButt->hide();
// #endif
	m_buttonsLay = new QBoxLayout(QBoxLayout::TopToBottom);
// BUTTONS WITH NOTES TOOLBAR
	m_noteLay = new QBoxLayout(QBoxLayout::LeftToRight);
	m_noteGroup =new QButtonGroup(this);
	for (int i = 0; i < 7; i++) {
			m_noteButtons[i] = new TpushButton(QString(), this);
						m_noteLay->addWidget(m_noteButtons[i]);
			m_noteButtons[i]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
			m_noteGroup->addButton(m_noteButtons[i], i);
	}
		m_buttonsLay->addLayout(m_noteLay);
	connect(m_noteGroup, SIGNAL(buttonClicked(int)), this, SLOT(noteWasChanged(int)));
	
// ACCID BUTTONS TOOOLBAR
	m_accLay = new QBoxLayout(QBoxLayout::LeftToRight);
	m_accLay->addStretch(2);
	m_dblFlatButt = createAccidButton(QStringLiteral("B"));
	m_flatButt = createAccidButton(QStringLiteral("b"));
	m_sharpButt = createAccidButton(QStringLiteral("#"));
	m_dblSharpButt = createAccidButton(QStringLiteral("x"));
	m_accLay->addStretch(2);
	m_buttonsLay->addLayout(m_accLay);
// OCTAVE BUTTONS TOOLBAR
	m_upOctaveLay = new QBoxLayout(QBoxLayout::LeftToRight);
	m_loOctaveLay = new QBoxLayout(QBoxLayout::LeftToRight);
#if !defined (Q_OS_ANDROID)
	m_octavesLab = new QLabel(this);
	m_octavesLab->setOpenExternalLinks(true);
	m_octavesLab->setStatusTip(tr("Click to see what <i>octaves</i> are at \"http://en.wikipedia.org/wiki/Octave\"",
															"You can change this link to article in your language. Leave quotation matks around the address!"));
	m_octavesLab->setText(QLatin1String("<a href=") + m_octavesLab->statusTip().mid(m_octavesLab->statusTip().indexOf("\"")) + QLatin1String(">") +
											tr("Octaves") + QLatin1String("</a>"));
	m_octavesLab->setStatusTip(m_octavesLab->statusTip().replace(QLatin1String("\""), QLatin1String("<b><i>")));
	m_upOctaveLay->addStretch(1);
#endif
	m_octaveGroup = new QButtonGroup(this);
	for (int i = 0; i < 8; i++) {
			m_octaveButtons[i] = new TpushButton(tr(octaves[i]), this);
			m_octaveButtons[i]->setStatusTip(tr(octavesFull[i]));
			if (i % 2) {
        if (i > 0)
            m_upOctaveLay->addStretch(1);
        m_upOctaveLay->addWidget(m_octaveButtons[i]);
        if (i == 3) {
          m_upOctaveLay->addStretch(1);
#if !defined (Q_OS_ANDROID)
          m_upOctaveLay->addWidget(m_octavesLab);
#endif
          m_upOctaveLay->addStretch(1);
        }
			} else {
        m_loOctaveLay->addWidget(m_octaveButtons[i]);
        if (i == 2)
          m_loOctaveLay->addStretch(2);
        else
          m_loOctaveLay->addStretch(1);
			}
			m_octaveGroup->addButton(m_octaveButtons[i], i);
	}
	m_buttonsLay->addLayout(m_upOctaveLay);
	m_buttonsLay->addLayout(m_loOctaveLay);
	m_prevOctButton = -1;
	
	connect(m_octaveGroup, SIGNAL(buttonClicked(int)), this, SLOT(octaveWasChanged(int)));
	
	mainLay->addLayout(m_buttonsLay);
	setLayout(mainLay);

	setStyle(Tcore::gl()->S->nameStyleInNoteName);
	setNoteNamesOnButt(style());
	for (int i = 0; i < 3; i++) m_notes.push_back(Tnote());
	setAmbitus(Tcore::gl()->loString(), Tnote(Tcore::gl()->hiString().chromatic()+Tcore::gl()->GfretsNumber));
#if defined (Q_OS_ANDROID)
//   m_upOctaveLay->addStretch(2); // no octaves link under Android
  mainLay->setContentsMargins(0, 0, 0, 0);
  mainLay->setSpacing(0);
  m_accLay->setContentsMargins(0, 2, 0, 2);
  m_accLay->setSpacing(0);
  m_upOctaveLay->setContentsMargins(0, 0, 0, 2);
  m_upOctaveLay->setSpacing(0);
  m_loOctaveLay->setContentsMargins(0, 0, 0, 0);
  m_loOctaveLay->setSpacing(0);
// #if !defined (Q_OS_ANDROID)
// 		resize((qApp->desktop()->availableGeometry().height() / 10) / 4 - 2);
#endif
}


TnoteName::~TnoteName()
{
  m_instance = nullptr;
}

//####################################################################################################
//########################################## PUBLIC  #################################################
//####################################################################################################

void TnoteName::exec(QPoint pos, qreal scoreFactor) {
#if defined (Q_OS_ANDROID)
  if (!m_menu) {
    m_menu = new QMenu(m_menuParent);
    m_menu->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *menuLay = new QVBoxLayout;
    menuLay->setContentsMargins(0, 0, 0, 0);
    setParent(m_menu);
    menuLay->addWidget(this);
    m_menu->setLayout(menuLay);
    m_menu->resize(sizeHint());
  }
  show();
  if (pos.x() > m_menuParent->width() / 2)
    pos.setX(pos.x() - sizeHint().width() - 11.5 * scoreFactor);
  m_menu->exec(pos);
  hide();
//   setParent(m_menuParent);
#else
	tip()->adjustSize();
	show();
	QPointF posF(pos.x(), pos.y());
	if (pos.x() > m_menuParent->width() / 2) // move tip left to the note if note is on the right side of a score
		posF.setX(pos.x() - tip()->boundingRect().width() - 8.5 * scoreFactor);
	if (pos.y() > m_menuParent->height() - tip()->boundingRect().height() - 10) // keep tip inside window borders
		posF.setY(m_menuParent->height() - tip()->boundingRect().height() - 10);
	tip()->setPos(posF);
	QTimer::singleShot(100, tip(), SLOT(show()));
#endif
}


void TnoteName::createNameTip(QGraphicsScene* scene) {
	if (!m_tip) {
		m_tip = new TnameTip(this);
		scene->addItem(m_tip);
		m_tip->hide();
	}
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
	if (note.isValid()) {
			m_notes[0] = note;
			setButtons(note);
	} else {
			m_notes[0] = Tnote(0,0,0);
			m_notes[1] = Tnote(0,0,0);
			m_notes[2] = Tnote(0,0,0);
			if (m_isMenu) { // clear TnoteName state for every empty note when pop up
				uncheckAllButtons();
				if (m_prevOctButton != -1) // restore previously selected octave button
					m_octaveButtons[m_prevOctButton]->setChecked(true);
				else // or select small octave one
					m_octaveButtons[note.octave + 3]->setChecked(true);
			}
// 	m_prevOctButton = -1;
	}
	setNameText();
}


void TnoteName::setNoteName(TnotesList& notes) {
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
    m_fontSize = fontSize;
		QFont f = QFont(m_noteButtons[0]->font().family());
		f.setPixelSize(fontSize);
		for (int i = 0; i < 7; i++)
				m_noteButtons[i]->setFont(f);
		for (int i = 0; i < 8; i++)
				m_octaveButtons[i]->setFont(f);
#if !defined (Q_OS_ANDROID)
    m_octavesLab->setFont(f);
#endif
		f = QFont(m_dblFlatButt->font().family());
		f.setPointSize(fontSize);
		QFontMetrics fMetr(f);
		qreal fact = ((qreal)fontSize / (qreal)fMetr.boundingRect("b").height());
#if !defined (Q_OS_ANDROID)
		fact *= 1.2;
#endif
		f.setPointSize(f.pointSize() * fact);
		for (int i = 0; i < m_accidButtons.size(); ++i)
			m_accidButtons[i]->setFont(f);
		int widthOffset = 0;
#if defined (Q_OS_ANDROID)
    int nameHfactor = 4;
    if (m_isMenu) {
			widthOffset = 30;
      nameHfactor = 3;
#else
    int nameHfactor = 5;
    if (m_isMenu) {
#if defined (Q_OS_MAC)
      widthOffset = 30;
#else
      widthOffset = 15;
#endif
      nameHfactor = 4;
#endif
    }
		setButtonsSize(widthOffset, fontSize * 1.5);
		m_nameLabel->setFixedHeight(fontSize * nameHfactor);
		m_nextNoteButt->setFixedHeight(m_nameLabel->height());
		m_prevNoteButt->setFixedHeight(m_nameLabel->height());
#if defined (Q_OS_ANDROID)
    m_nextNoteButt->setFixedWidth(m_nameLabel->height() * 0.6);
    m_prevNoteButt->setFixedWidth(m_nameLabel->height() * 0.6);
#endif
		updateSizeHint();
		adjustSize();
		if (tip())
			tip()->adjustSize();
	}
}


void TnoteName::setButtonsSize(int widthOff, int fixedH, bool skipOctaves) {
#if defined (Q_OS_ANDROID)
  fixedH *= 1.2;
#elif defined (Q_OS_MAC)
  fixedH *= 1.3;
#endif
	for (int i = 0; i < 7; i++) {
		m_noteButtons[i]->setFixedHeight(fixedH);
		fixButtonWidth(widthOff, m_noteButtons[i]);
	}
	if (!skipOctaves) {
		for (int i = 0; i < 8; i++) {
			m_octaveButtons[i]->setFixedHeight(fixedH);
			fixButtonWidth(widthOff, m_octaveButtons[i]);
		}
	}
#if defined (Q_OS_MAC)
  fixedH *= 1.2;
#endif
	for (int i = 0; i < m_accidButtons.size(); ++i) {
		m_accidButtons[i]->setFixedHeight(fixedH);
		fixButtonWidth(widthOff, m_accidButtons[i]);
	}
}


void TnoteName::fixButtonWidth(int widthOff, QPushButton* butt) {
	if (widthOff)
			butt->setFixedWidth(butt->fontMetrics().width(butt->text()) + widthOff);
		else {
			butt->setMinimumWidth(0);
			butt->setMaximumWidth(QWIDGETSIZE_MAX);
		}
}


/** Variables declared here have been used nowhere so far.  */
void TnoteName::setAmbitus(Tnote lo, Tnote hi) {
    m_ambitMin = lo.chromatic();
    m_ambitMax = hi.chromatic();
}


void TnoteName::enableArrows(bool en) {
	m_isMenu = en;
	if (en) {
		m_prevNoteButt->show();
		m_nextNoteButt->show();
		if (tip())
			tip()->wrapNoteName();
	} else {
		tip()->unwrapNoteName();
		m_prevNoteButt->hide();
		m_nextNoteButt->hide();
    if (m_menu) {
      setParent(m_menuParent);
      delete m_menu;
      m_menu = 0;
    }
	}
	resize(m_fontSize);
}


int TnoteName::widthForHorizontal() {
	int w = 0;
	for (int i = 0; i < 7; i++)
		w += m_noteButtons[i]->width();
	w += m_noteLay->spacing() * 9;
	return w;
}


QSize TnoteName::sizeHint() const {
	return m_sizeHint;
}


//##############################################################################################
//#################################### EXAM RELATED ############################################
//##############################################################################################
QColor TnoteName::prepareBgColor(const QColor& halfColor) {
	QColor mixedColor = Tcolor::merge(halfColor, qApp->palette().window().color());
	mixedColor.setAlpha(220);
	return mixedColor;
}


void TnoteName::askQuestion(Tnote note, Tnote::EnameStyle questStyle, char strNr) {
	Tnote::EnameStyle tmpStyle = m_style;
	setStyle(questStyle);
	setNoteName(note);
	m_nameLabel->showQuestionMark(QColor(Tcore::gl()->EquestionColor.name()));
	if (strNr)
		m_nameLabel->showStringNumber(strNr, QColor(Tcore::gl()->EquestionColor.name()));
	m_nameLabel->setBackgroundColor(prepareBgColor(Tcore::gl()->EquestionColor));
	uncheckAllButtons();
	setStyle(tmpStyle);
}


void TnoteName::prepAnswer(Tnote::EnameStyle answStyle) {
	m_nameLabel->setBackgroundColor(prepareBgColor(Tcore::gl()->EanswerColor));
	setNoteNamesOnButt(answStyle);
	setNameDisabled(false);
	m_notes[0] = Tnote(0,0,0); // Reset, otherwise getNoteName() returns it
}


void TnoteName::forceAccidental(char accid) {
	if (accid) {
		checkAccidButtons(accid);
	}
}


void TnoteName::markNameLabel(const QColor& markColor) {
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
//           setAttribute(Qt::WA_TransparentForMouseEvents, true);
		for (int i = 0; i < 8; i++) {
			if (m_octaveButtons[i]->isChecked())
					m_prevOctButton = i;
			m_octaveButtons[i]->setChecked(false);
		}
	} else { // restore last checked octave when becomes enabled
//           setAttribute(Qt::WA_TransparentForMouseEvents, false);
		if (m_prevOctButton != -1)
				m_octaveButtons[m_prevOctButton]->setChecked(true);
		else { // or set it to small octave
				m_octaveButtons[3]->setChecked(true);
				m_prevOctButton = 3;
		}
	}
}


void TnoteName::clearNoteName() {
	setNoteName(Tnote());
	m_nameLabel->setBackgroundColor(prepareBgColor(qApp->palette().base().color()));
}


void TnoteName::correctName(Tnote& goodName, const QColor& color, bool isWrong) {
		m_goodNote = goodName;
		m_blinkingPhase = 0;
		if (isWrong) {
			m_nameLabel->blinkCross(QColor(color.name()));
		} else
			m_nameLabel->blinkingText(2);
}

//####################################################################################################
//########################################## EVENTS  #################################################
//####################################################################################################

bool TnoteName::event(QEvent* event) {
  if (event->type() == QEvent::StatusTip) {
      QStatusTipEvent *se = static_cast<QStatusTipEvent *>(event);
      emit statusTipRequired(se->tip());
  }
	return QWidget::event(event);
}

/** Current purpose of this event re-implementation is to stop mouse click forwarding to under-laying score. */
void TnoteName::mousePressEvent(QMouseEvent*) {}


//##############################################################################################
//#################################### PRIVATE #################################################
//##############################################################################################

void TnoteName::setNameText() {
	if (m_notes[0].note) {
		QString txt = "<big>" + m_notes[0].toRichText(Tcore::gl()->S->octaveInNoteNameFormat) + "</big>";
		if (m_notes[1].note) {
				txt = txt + QString("  <small style=\"color: %1\">(").arg(Tcore::gl()->S->enharmNotesColor.name()) +
					m_notes[1].toRichText(Tcore::gl()->S->octaveInNoteNameFormat);
				if (m_notes[2].note)
						txt = txt + "  " + m_notes[2].toRichText(Tcore::gl()->S->octaveInNoteNameFormat);
				txt = txt + ")</small>";
		}
		m_nameLabel->setText(txt);
	} else 
		m_nameLabel->setText("");
}


void TnoteName::setNoteName(char noteNr, char octNr, char accNr) {
	m_notes[0] = Tnote(noteNr, octNr, accNr);
	if (noteNr) {
		if (Tcore::gl()->S->showEnharmNotes && !m_isMenu) {
				TnotesList enharmList = m_notes[0].getTheSameNotes(Tcore::gl()->S->doubleAccidentalsEnabled);
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


void TnoteName::setButtons(const Tnote& note) {
	m_noteButtons[note.note-1]->setChecked(true);
	checkAccidButtons(note.alter);
	if (note.octave >= -3 && note.octave <= 4) {
		if (m_octaveButtons[note.octave + 3]->isEnabled()) { // check octave button only when it is enabled
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


void TnoteName::updateSizeHint() {
	int fixW = widthForHorizontal() + m_noteLay->spacing() * 2;
#if !defined(Q_OS_ANDROID)
	if (m_nextNoteButt->isVisible())
		fixW += m_nextNoteButt->width() * 2;
#endif
	m_sizeHint.setWidth(fixW);
	int fixH = m_nameLabel->height() + m_flatButt->height() * 4 + m_noteLay->spacing() * 2;
	m_sizeHint.setHeight(fixH);
}


TpushButton* TnoteName::createAccidButton(const QString& accidText) {
#if defined(Q_OS_MAC)
	TnooFont nf(15);
#else
	TnooFont nf(10);
#endif
  TpushButton *accidButt = new TpushButton(accidText, this);
	accidButt->setFont(nf);
	m_accLay->addWidget(accidButt);
	m_accLay->addStretch(1);
	connect(accidButt, &TpushButton::clicked, this, &TnoteName::accidWasChanged);
	m_accidButtons << accidButt;
	return accidButt;
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
	// It causes crashes. It is safer to call it with delay and outside
		QTimer::singleShot(1, this, SLOT(invokeBlinkingAgain()));
}


void TnoteName::invokeBlinkingAgain() {
	disconnect(m_nameLabel, SIGNAL(throwingFinished()), this, SLOT(correctAnimationFinished()));
	if (m_notes[0] != m_goodNote) {
		markNameLabel(QColor(Tcore::gl()->EanswerColor.name()));
		m_nameLabel->thrownText(m_goodNote.toRichText(), 150, 150);
	}
	emit correctingFinished();
}




