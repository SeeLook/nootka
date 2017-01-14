/***************************************************************************
 *   Copyright (C) 2013-2017 by Tomasz Bojczuk                             *
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


#include "tscorenote.h"
#include "tscorescene.h"
#include "tscorestaff.h"
#include "tnotecontrol.h"
#include "tscorebeam.h"
#include "tscoretie.h"
#include "tscorelines.h"
#include <music/tnote.h>
#include <tnoofont.h>
#include <QtCore/qeasingcurve.h>
#include <QtWidgets/qgraphicseffect.h>
#include <QtWidgets/qgraphicssceneevent.h>
#include <QtGui/qpainter.h>
#include <QtWidgets/qapplication.h>
#include <QtGui/qpalette.h>
#include <QtCore/qtimer.h>
#include <QtGui/qevent.h>

#include <QtCore/qdebug.h>


#define REST_Y (19.0)


static const int accCharTable[6] = { 0xe123, 0xe11a, 0x20, 0xe10e, 0xe125, 0xe116 };

char d(TscoreNote* sn) {
  QTextStream o(stdout);
  o << "   \033[01;29m[" << sn->index() << " NOTE]\033[01;00m";
  return 32; // fake
}

/*static*/
QString TscoreNote::getAccid(int accNr) {
  return accNr ? QString(QChar(accCharTable[accNr + 2])) : QString();
}


/** To avoid creating many tips - each one for every instance and waste RAM
 * this text exist as static variable
 * and TscoreNote manages itself when status tip is necessary to be displayed. */
QString TscoreNote::m_staticTip = QString();
QString TscoreNote::m_selectedTip = QString();

const qreal TscoreNote::m_rtmGapArray[5][3] = {
//  | bare | dot | triplet |
    { 5.0,   6.0,   4.5}, // whole note
    { 4.0,   5.0,   3.3}, // half note
    { 2.0,   2.5,   1.3}, // quarter note
    { 1.0,   1.5,   0.3}, // eighth note
    { 0.15,  0.5,   0.0}  // sixteenth note
};


qreal TscoreNote::space(const Trhythm& r) {
  if (r.rhythm() == Trhythm::e_none)
    return 0.0;

  int add = r.hasDot() ? 1 : (r.isTriplet() ? 2 : 0);
  return m_rtmGapArray[static_cast<int>(r.rhythm()) - 1][add];
}


//#################################################################################################
//###################              CONSTRUCTOR         ############################################
//#################################################################################################

TscoreNote::TscoreNote(TscoreScene* scene, TscoreStaff* staff, int index) :
  TscoreItem(scene),
  m_mainPosY(0.0), m_newPosY(0),
  m_accidental(0), m_newAccid(0),
  m_index(index),
  m_stringNr(0), m_stringText(0),
  m_readOnly(false), m_emptyLinesVisible(true),
  m_nameText(0),
  m_ottava(0),
  m_bgColor(-1),
  m_selected(false),
  m_touchedToMove(false), m_wasTouched(false)
{
  setStaff(staff);
  setParentItem(staff);
  setFlag(QGraphicsItem::ItemHasNoContents);
  m_height = staff->height();
  m_width = scene->isRhythmEnabled() ? 4.0 : 7.0;
//   m_width = 7.0;
  m_mainColor = qApp->palette().text().color();
  m_note = new Tnote(); // empty note with no rhythm

  m_lines = new TscoreLines(this);
  m_lines->setPos(0.6, 0.0);
  m_mainNote = new TnoteItem(scoreScene(), Trhythm(Trhythm::e_none));
  m_mainNote->setParentItem(this);
  m_mainNote->hide();
  m_mainNote->setX(3.0);

  m_mainAccid = new QGraphicsSimpleTextItem();
  m_mainAccid->setParentItem(m_mainNote);

  m_mainAccid->setFont(TnooFont(5));
  if (m_staticTip.isEmpty()) { // only when first instance of TscoreNote is constructed
      m_staticTip = tr("Click to enter a note, use horizontal scroll to change accidental.");
      m_selectedTip = QLatin1String("<br>") + tr("Right mouse button just selects a note.");
  }
  m_emptyText = new QGraphicsSimpleTextItem(QStringLiteral("n"), this);
    m_emptyText->setFont(TnooFont(8));
    m_emptyText->setZValue(1);
    QColor cc = qApp->palette().highlight().color();
    cc.setAlpha(50);
    m_emptyText->setBrush(cc);
    m_emptyText->setPen(QPen(qApp->palette().highlight().color(), 0.1));
//     m_emptyText->setScale(m_width / m_emptyText->boundingRect().width());
    m_emptyText->setPos((m_width - m_emptyText->boundingRect().width() * m_emptyText->scale()) / 2,
                        staff->upperLinePos() + 1.0 + (staff->isPianoStaff() ? 6 : 0));
    m_emptyText->hide();

  m_mainAccid->setScale(scoreScene()->accidScale());
  m_mainAccid->setPos(-2.3, - scoreScene()->accidYoffset());

  setColor(m_mainColor);
  m_mainNote->setZValue(34); // under
  m_mainAccid->setZValue(m_mainNote->zValue() - 1);
  if (staff->isPianoStaff())
    setAmbitus(40, 2);
  else
    setAmbitus(34, 2);
  connect(this, SIGNAL(statusTip(QString)), scene, SLOT(statusTipChanged(QString)));
  setRhythmEnabled(scoreScene()->isRhythmEnabled());
  checkEmptyText();
}


TscoreNote::~TscoreNote() {
  if (note()->rtm.tie() == Trhythm::e_tieEnd || note()->rtm.tie() == Trhythm::e_tieCont) {
    auto prev = prevNote();
    if (prev)
      prev->tieRemove();
  }
  if (m_tie)
    delete m_tie;

  if (staff())
    staff()->noteGoingDestroy(this);

  delete m_note;
}

//##############################################################################
//#################### PUBLIC METHODS    #######################################
//##############################################################################

Trhythm* TscoreNote::rhythm() const {
  return m_mainNote->rhythm();
}


void TscoreNote::setRhythm(const Trhythm& r) {
//   m_mainNote->setRhythm(r);
//   Trhythm old(m_note->rtm);
  m_note->setRhythm(r);
//   m_note->rtm.setTie(old.tie());
//   m_note->rtm.setStemDown(old.stemDown());
  update();
}


void TscoreNote::adjustSize() {
  m_height = staff()->height();
  m_lines->adjustLines(this);
  setColor(m_mainColor);
  if (staff()->isPianoStaff())
    m_emptyText->setPos(m_emptyText->x(), m_emptyText->y() + 6);
  else
    m_emptyText->setPos(m_emptyText->x(), m_emptyText->y() - 6);
}


void TscoreNote::setRhythmEnabled(bool rhythmEnabled) {
  if (rhythmEnabled != scoreScene()->isRhythmEnabled())
      qDebug() << d(this) << "Note has rhythms available different than scene!";

  if (rhythmEnabled) {
//       m_emptyText->setText(QString(QChar(TnooFont::getCharFromRhythm(scoreScene()->workRhythm()->weight(), true, scoreScene()->workRhythm()->isRest()))));
//       m_mainNote->setRhythm(Trhythm(scoreScene()->workRhythm()->rhythm(), !m_note->isValid()));
//       if (!m_note->isValid())
//         moveNote(15);
  } else {
        m_emptyText->setText(QStringLiteral("n"));
//         m_mainNote->setRhythm(Trhythm(Trhythm::e_none));
  }
  changeWidth();
}


void TscoreNote::setColor(const QColor& color) {
  m_mainColor = color;
  m_mainNote->setColor(color);
//   m_mainNote->setPen(Qt::NoPen); // TODO
//   m_mainNote->setBrush(QBrush(m_mainColor, Qt::SolidPattern));
  m_mainAccid->setBrush(QBrush(m_mainColor));
  m_lines->setColor(color);
  if (m_stringText)
      m_stringText->setBrush(QBrush(m_mainColor));
}


void TscoreNote::selectNote(bool sel) {
  m_selected = sel;
  checkEmptyText();
}


void TscoreNote::moveNote(int posY) {
//     if (posY == 0 || !(posY >= m_ambitMax - 1 && posY <= m_ambitMin)) {
  if (posY == 0 || !(posY >= 1 && posY <= m_height - 3)) {
      if (m_mainNote->rhythm()->isRest()) {
          m_mainNote->setY(REST_Y);
          m_mainNote->show();
      } else
          hideNote();
      m_mainAccid->setText(QString());
      m_accidental = 0;
      return;
  }
  if (!m_mainNote->isVisible()) {
      m_mainNote->show();
      m_mainAccid->show();
  }

  if (note()->isRest())
      m_mainNote->setY(REST_Y);
  else if (posY != static_cast<int>(m_mainNote->y()))
      m_mainNote->setY(posY);

  m_mainPosY = posY;
  int noteNr = (56 + staff()->notePosRelatedToClef(staff()->fixNotePos(posY))) % 7;
  QString newAccid = getAccid(m_accidental);
  if (staff()->accidInKeyArray[noteNr]) {
    if (m_accidental == 0) {
        newAccid = getAccid(3); // neutral
        m_mainAccid->hide();
    } else {
        if (staff()->accidInKeyArray[noteNr] == m_accidental) {
          if (staff()->extraAccids()) // accidental from key signature in braces
            newAccid = QString(QChar(accCharTable[m_accidental + 2] + 1));
          else
            newAccid.clear(); // hide accidental
        }
    }
  }
  for (int i = index() - 1; i >= 0; i--) { // check the previous notes for accidentals
    if (staff()->noteSegment(i)->note()->note == noteNr + 1) {
        if (staff()->noteSegment(i)->note()->alter != 0 && m_accidental == 0) {
          if (newAccid.isEmpty())
            newAccid = getAccid(3); // and add neutral when some of previous notes with the same step had an accidental
        } else if (staff()->accidInKeyArray[noteNr] == m_accidental && staff()->noteSegment(i)->note()->alter != m_accidental)
            newAccid = getAccid(m_accidental); // There is already accidental in key signature but some of the previous notes had another one
      break;
    }
  }
  m_mainAccid->show();
  m_mainAccid->setText(newAccid);
  setStringPos();
  m_lines->checkLines(posY);
  checkEmptyText();
}


void TscoreNote::setNote(int notePos, int accNr, const Tnote& n) {
  m_accidental = accNr;
  *m_note = n;
  m_mainNote->setRhythm(Trhythm(n.rhythm(), notePos == 0, n.hasDot(), n.isTriplet()));
  moveNote(notePos);
  if (m_mainPosY == 0) {
    m_note->note = 0;
    m_note->alter = 0;
//     m_mainNote->setY(REST_Y);
  }
//   if (m_note->rhythm() != Trhythm::e_none)
//     setRhythm(Trhythm(n.rhythm(), m_mainPosY == 0, n.hasDot(), n.isTriplet()));
  changeWidth();
  if (m_nameText)
    showNoteName();
  checkEmptyText();
  if (beam()/* && beam()->last() == this*/)
    beam()->performBeaming();
  staff()->fit();
}


void TscoreNote::hideNote() {
  m_mainNote->hide();
  m_mainAccid->hide();
  m_lines->hideAllLines();
  m_mainPosY = 0;
  m_accidental = 0;
  m_mainNote->setY(0);
}


void TscoreNote::moveWorkNote(const QPointF& newPos) {
  QGraphicsSceneHoverEvent moveEvent(QEvent::GraphicsSceneHoverMove);
  moveEvent.setPos(newPos);
  hoverMoveEvent(&moveEvent);
}


void TscoreNote::markNote(QColor blurColor) {
  if (blurColor == -1) {
//     m_mainNote->setPen(Qt::NoPen); TODO: check it
    m_mainNote->setGraphicsEffect(0);
  } else {
//     m_mainNote->setPen(QPen(blurColor, 0.2));
    QGraphicsDropShadowEffect *bluredPen = new QGraphicsDropShadowEffect();
    bluredPen->setBlurRadius(10);
    bluredPen->setColor(QColor(blurColor.name()));
    bluredPen->setOffset(0.5, 0.5);
    m_mainNote->setGraphicsEffect(bluredPen /*TdropShadowEffect(blurColor)*/);
  }
  update();
}


TscoreNote* TscoreNote::nextNote() {
  TscoreNote* next = nullptr; // find next note first
  if (this == staff()->lastNote()) { // take first note from the next staff
      auto st = staff()->nextStaff();
      if (st)
        next = st->firstNote();
  } else // or just next one
      next = staff()->noteSegment(index() + 1);
  return next;
}


TscoreNote* TscoreNote::prevNote() {
  if (staff()->count()) {
      TscoreNote* prev = nullptr; // find next note first
      if (this == staff()->firstNote()) { // take first note from the next staff
          auto st = staff()->prevStaff();
          if (st)
            prev = st->lastNote();
      } else // or just next one
          prev = staff()->noteSegment(index() - 1);
      return prev;
  }
  return nullptr;
}


qreal TscoreNote::space() {
  return space(note()->rtm);
}


QGraphicsLineItem* TscoreNote::stem() {
  return m_mainNote->stem();
}


void TscoreNote::setString(int realNr) {
  if (!m_stringText) {
        m_stringText = new QGraphicsSimpleTextItem();
        m_stringText->setFont(TnooFont(5));
        m_stringText->setBrush(QBrush(m_mainColor));
        m_stringText->setParentItem(this);
        m_stringText->setZValue(-1);
    }
    m_stringText->setText(QString("%1").arg(realNr));
    m_stringText->setScale(5.0 / m_stringText->boundingRect().width());
    m_stringNr = realNr;
    setStringPos();
}


void TscoreNote::removeString() {
    if (m_stringText) {
        delete m_stringText;
        m_stringText = 0;
    }
    m_stringNr = 0;
}


void TscoreNote::setReadOnly(bool ro) {
  setAcceptHoverEvents(!ro);
  m_readOnly = ro;
  m_emptyLinesVisible = !ro;
  checkEmptyText();
}

/** If @dropShadowColor is not set and m_nameText has already existed,
 * previously color remained. */
void TscoreNote::showNoteName(const QColor& dropShadowColor) {
  bool setColor = false;
  if (!m_nameText) {
    m_nameText = new QGraphicsTextItem();
    m_nameText->setDefaultTextColor(m_mainColor);
    m_nameText->setParentItem(this);
    m_nameText->setZValue(10);
    m_nameText->setAcceptHoverEvents(false);
    setColor = true;
  }
  if (dropShadowColor != -1)
    setColor = true;
  if (setColor) {
    QGraphicsDropShadowEffect *dropEff = new QGraphicsDropShadowEffect();
    if (dropShadowColor == -1)
      dropEff->setColor(scoreScene()->nameColor());
    else
      dropEff->setColor(dropShadowColor);
    dropEff->setOffset(0.7, 0.7);
    dropEff->setBlurRadius(5.0);
    m_nameText->setGraphicsEffect(dropEff);
  }
  if (m_note->note) {
      m_nameText->setHtml(m_note->toRichText());
      m_nameText->setScale(8.0 / m_nameText->boundingRect().height());
      if (m_nameText->boundingRect().width() * m_nameText->scale() > boundingRect().width())
          m_nameText->setScale(boundingRect().width() / (m_nameText->boundingRect().width()));
      m_nameText->setPos((8.0 - m_nameText->boundingRect().width() * m_nameText->scale()) * 0.75, /*yy);*/
              notePos() > staff()->upperLinePos() ?
                    notePos() - (m_nameText->boundingRect().height() + 2.0) * m_nameText->scale(): // above note
                    notePos() + m_mainNote->boundingRect().height()); // below note
      m_nameText->show();
  } else
      m_nameText->hide();
}


void TscoreNote::removeNoteName() {
  delete m_nameText;
  m_nameText = 0;
}


void TscoreNote::setAmbitus(int lo, int hi) {
  m_ambitMin = qBound(2, lo, (int)m_height - 3);
  m_ambitMax = qBound(2, hi, (int)m_height - 3);
}


      /** When note duration is longer than shortest rhythm in current staff
       *  a space (gap) is added to visually represent note duration as a space between notes */
qreal TscoreNote::rightX() {
//   int dur = m_mainNote->rhythm()->duration();
  return x() + m_width + staff()->gapFactor() * space(note()->rtm);
//   return x() + m_width + (dur > staff()->shortestRhythm() ? staff()->gapFactor() * (((dur / staff()->shortestRhythm()) - 1.0)) : 0.0);
}


void TscoreNote::update() {
//   if (note()->rtm != *m_mainNote->rhythm())
//   qDebug() << d(this) << "UPDATE";
  m_mainNote->setRhythm(note()->rtm);
  QGraphicsItem::update();
}


void TscoreNote::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  Q_UNUSED(option)
  Q_UNUSED(widget)
//     paintBackground(painter, Qt::yellow);
  if (m_bgColor != -1) {
//       paintBackground(painter, m_bgColor);
    QPointF center(m_width / 2.0, m_mainPosY + 1.0);
    if (m_mainPosY == 0)
      center.setY(staff()->upperLinePos() + 4.0);
    QRadialGradient gr(center, 10.0);
    QColor c1 = m_bgColor;
    c1.setAlpha(40);
    QColor c2 = m_bgColor;
    c2.setAlpha(80);
    gr.setColorAt(0.0, c1);
    gr.setColorAt(0.9, c1);
    gr.setColorAt(0.95, c2);
    gr.setColorAt(1.0, Qt::transparent);
    painter->setBrush(gr);
    painter->setPen(Qt::NoPen);
    painter->drawRect(0.0, qMax(center.y() - 10.0, 0.0), m_width, qMin(center.y() + 10.0, m_height));
  }
  // for debug - index number, tie, group number, beam
  painter->setPen(Qt::red);
  QFont f(qApp->font());
  f.setPointSize(1);
  painter->setFont(f);
  painter->drawText(QRectF(0.0, 6.0, width(), 1.5), Qt::AlignCenter, QString::number(index()));
  painter->drawText(QRectF(0.0, 9.0, width(), 1.5), Qt::AlignCenter,
                    (beam() ? "B " : "") + QString::number(m_group) + (note()->rtm.stemDown() ? " \\/" : " /\\"));
  if (note()->rtm.tie()) {
    Trhythm::Etie t = note()->rtm.tie();
    painter->drawText(QRectF(0.0, 7.5, width(), 1.5), Qt::AlignCenter,
                      QString("T%1").arg(t == Trhythm::e_tieStart ? "s" : (t == Trhythm::e_tieCont ? "c" : "e")));
  }

  if (scoreScene()->currentNote() == this && m_touchedToMove) {
    painter->setPen(Qt::NoPen);
    QColor workBg(Qt::darkBlue);
    workBg.setAlpha(20);
    painter->setBrush(QBrush(workBg));
    painter->drawRect(boundingRect());
  }
  if (m_emptyLinesVisible && !m_selected && m_mainPosY == 0 && m_mainNote->rhythm()->rhythm() == Trhythm::e_none && !hasCursor()) {
    QColor emptyNoteColor;
//     if (m_mainNote->pen().style() == Qt::NoPen) // TODO: check it
    if (!m_mainNote->graphicsEffect())
      emptyNoteColor = qApp->palette().highlight().color();
    else
      emptyNoteColor = m_mainNote->color(); //m_mainNote->pen().color();
    emptyNoteColor.setAlpha(120);
    painter->setPen(QPen(emptyNoteColor, 0.4, Qt::SolidLine, Qt::RoundCap));
    painter->drawLine(QLineF(0.5, staff()->upperLinePos() - 1.0, m_width - 0.5, staff()->upperLinePos() - 2.0));
    qreal loLine = staff()->isPianoStaff() ? staff()->lowerLinePos() : staff()->upperLinePos();
    painter->drawLine(QLineF(0.5, loLine + 10.0, m_width - 0.5, loLine + 9.0));
  }
}


bool TscoreNote::rhythmChanged() const {
  return *m_mainNote->rhythm() != m_note->rtm;
}


void TscoreNote::setX(qreal x) {
  bool xChanged = QGraphicsItem::x() != x;
  QGraphicsItem::setX(x);
  if (xChanged) {
    if (m_beam && m_beam->last() == this) // when this note is the last one in a beam - update beam
      m_beam->drawBeam();
    if (note()->rtm.tie() == Trhythm::e_tieCont || note()->rtm.tie() == Trhythm::e_tieEnd) {
      auto prev = prevNote();
      if (prev->tie())
        prev->tie()->updateLength();
      else // TODO: delete it
        qDebug() << d(this) << "BOOOOOOOOM! No tie in previous note of" << index();
    }
  }
}



void TscoreNote::setPos(const QPointF& pos) {
  bool xChanged = QGraphicsItem::x() == pos.x();
  QGraphicsItem::setPos(pos);
  if (xChanged) {
    if (m_beam && m_beam->last() == this) // when this note is the last one in a beam - update beam
      m_beam->performBeaming();
  }
}

//#################################################################################################
//########################################## PROTECTED   ##########################################
//#################################################################################################


void TscoreNote::mousePressEvent(QGraphicsSceneMouseEvent* event) {
/*
  if (scoreScene()->workPosY()) { // edit mode
    if (event->button() == Qt::LeftButton) {
        m_newAccid = scoreScene()->currentAccid();
        m_newPosY = scoreScene()->workRhythm()->isRest() ? 0 : scoreScene()->workPosY();
        qreal widthDiff = 0.0;
        if (scoreScene()->isRhythmEnabled()) {
            Trhythm::Etie oldTie = note()->rtm.tie();
            if (note()->rtm != *scoreScene()->workRhythm()) {
              note()->rtm.setRhythm(*scoreScene()->workRhythm());
              qDebug() << d(this) << "rhythm changed" << rhythmChanged();
              if (!pitchChanged() && !accidChanged()) // copy tie state
                note()->rtm.setTie(oldTie);
            } else {
              note()->rtm.setStemDown(scoreScene()->workRhythm()->stemDown());
              qDebug() << d(this) << "rhythm the same";
            }
            qreal newWidth = 4.0;
            if (m_newAccid)
              newWidth += 2.0; // when removed - width remains smaller
            if (newWidth > m_width || newWidth < m_width) { // newWidth != m_width
              widthDiff =  newWidth - m_width;
              qDebug() << d(this) << "Note width differs" << m_width << newWidth << widthDiff;
            }
        }
        if (pitchChanged() || rhythmChanged() || accidChanged() || widthDiff != 0.0) {
            if ((pitchChanged() || accidChanged())) {
              if (note()->rtm.tie()) {
                if (note()->rtm.tie() == Trhythm::e_tieEnd || note()->rtm.tie() == Trhythm::e_tieCont) {
                    auto prev = prevNote();
                    if (prev)
                      prev->tieRemove();
                }
                tieRemove();
              }
            }
            emit noteGoingToChange(this);
            staff()->prepareNoteChange(this);
        }
//         m_mainNote->setRhythm(*m_newRhythm);
        m_accidental = m_newAccid;
        moveNote(scoreScene()->workPosY());
        changeWidth();
        staff()->onNoteClicked(m_index);
        emit noteWasClicked(m_index);
        if (m_nameText)
          showNoteName();
        update();
    } else if (event->button() == Qt::RightButton) {
        if (!isReadOnly() && staff()->selectableNotes()) {
            setBackgroundColor(qApp->palette().highlight().color());
            emitNoteWasSelected();
            update();
        }
    }
  } else { // read only mode
    if (event->button() == Qt::LeftButton)
      emit roNoteClicked(this, event->pos());
    else if (event->button() == Qt::RightButton)
      emit roNoteSelected(this, event->pos());
  }
*/
}



//##########################################################################################################
//####################################         PRIVATE     #################################################
//##########################################################################################################

void TscoreNote::setStringPos() {
  if (m_stringText) {
    qreal yy = staff()->upperLinePos() + 9.0; // below the staff
    if (m_mainPosY > staff()->upperLinePos() + 4.0)
        yy = staff()->upperLinePos() - 7.0; // above the staff
    m_stringText->setPos(m_width + 0.5 - m_stringText->boundingRect().width() * m_stringText->scale(), yy);
  }
}



void TscoreNote::checkEmptyText() {
  if (!scoreScene()->isRhythmEnabled()) {
    if (!isReadOnly() && staff()->selectableNotes() && !m_selected && m_mainPosY == 0)
      m_emptyText->show();
    else
      m_emptyText->hide();
  }
}


qreal TscoreNote::estimateWidth(const Tnote& n) {
  qreal w = 4.0;
  if (n.rhythm() == Trhythm::e_none)
    w = 7.0;
  else {
    if (n.alter != 0)
      w += 2.0;
    if (n.hasDot() || (n.rtm.weight() > 4 && n.rtm.beam() == Trhythm::e_noBeam))
      w += 1.0;
  }
  return w;
}


void TscoreNote::emitNoteWasSelected() {
  staff()->onNoteSelected(m_index);
  emit noteWasSelected(m_index);
}


/**
 * Note width:
 * 4 - base (minimal) width of bare note (no accidental, no flag)
 * 5 - eighth/sixteenth flag or a dot
 * 6 - when note has accidental (neutral as well)
 * 7 - an accidental and a flag or a dot (maximal width)
 */
void TscoreNote::changeWidth() {
  qreal oldWidth = m_width;
  qreal newWidth = 7.0;
  if (scoreScene()->isRhythmEnabled()) {
    newWidth = 4.0;
    if (!m_mainAccid->text().isEmpty()) {
        newWidth += 2.0;
        m_mainNote->setX(2.5);
        m_lines->setX(2.1);
    } else {
        m_mainNote->setX(0.5);
        m_lines->setX(0.1);
    }
    if (m_mainNote->rhythm()->weight() > 4 && m_mainNote->rhythm()->beam() == Trhythm::e_noBeam)
      newWidth += 1.0;
  }
  if (newWidth != oldWidth) {
      qDebug() << d(this) << "Note changed width" << newWidth;
      prepareGeometryChange();
      m_width = newWidth;
//       staff()->noteChangedWidth(index());
  }
}


void TscoreNote::tieWithNext() {
  if (!m_tie)
    m_tie = TscoreTie::check(this);
  else
    qDebug() << d(this) << "has already tie";
}


void TscoreNote::tieRemove() {
  if (m_tie) {
    delete m_tie; // it will set this note tie to none
    m_tie = nullptr;
  }
}





/*
void TscoreNote::checkOctavation() {
  bool notPossible = false;
      if (pos < m_ambitMax) {
        m_ottava = -1;
        pos += 7;
        if (pos < m_ambitMax) {
          m_ottava = -2;
          pos += 7;
          if (pos < m_ambitMax)
            notPossible = true;
        }
      } else {
        m_ottava = 1;
        pos -= 7;
        if (pos > m_ambitMin) {
          m_ottava = 2;
          pos -= 7;
          if (pos > m_ambitMin)
            notPossible = true;
        }
      }
      if (notPossible) {
        m_ottava = 0;
        m_mainPosY = 0;
        hideNote();
        return;
      }
      qDebug() << "octavation required" << (int)m_ottava;
} */




