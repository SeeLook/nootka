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
 *  You should have received a copy of the GNU General Public License       *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#ifndef TNOTENAME_H
#define TNOTENAME_H


#include <QtWidgets/qwidget.h>
#include <QtWidgets/qboxlayout.h>
#include <music/tnote.h>
#include <tcolor.h>


class QGraphicsScene;
class TnameTip;
class QMenu;
class QGraphicsTextItem;
class QHBoxLayout;
class TnoteNameLabel;
class QLabel;
class QPushButton;
class TpushButton;
class QButtonGroup;


#define   NOTENAME    TnoteName::instance()


/**
 * This widget displays note name and buttons to manipulate it.
 * Since Nootka 1.1.0 it is also pop up tip managed by score.
 *
 * It has single instance available through @p instance()
 * defined also as a macro @p TOOLBAR
 */
class NOOTKACORE_EXPORT TnoteName : public QWidget
{

  Q_OBJECT

public:
  explicit TnoteName(QWidget *parent = 0);
  virtual ~TnoteName();

  static TnoteName* instance() { return m_instance; }

  static const char * const octaves[8];
  static const char * const octavesFull[8];

      /** Displays note name as menu. @p pos is desired position in screen coordinates
        * @p scoreFactor is m11() factor of a scene to automatically calculate and place the menu
        * on the right or left side of given position */
  void exec(QPoint pos, qreal scoreFactor);

      /** Sets names on buttons to given style.  Doesn't refresh note name label. */
  void setNoteNamesOnButt(Tnote::EnameStyle nameStyle);
  void setEnabledDblAccid(bool isEnabled);
  void setEnabledEnharmNotes(bool isEnabled);
  void setNoteName(Tnote note);
  void setNoteName(TnotesList& notes);
  Tnote getNoteName() {return m_notes[0]; }
  Tnote getNoteName(int index) {return m_notes[index]; }
  void resize(int fontSize = 0);
  void setAmbitus(Tnote lo, Tnote hi);
  void askQuestion(Tnote note, Tnote::EnameStyle questStyle, char strNr = 0);
  void prepAnswer(Tnote::EnameStyle answStyle);
  void setNameDisabled(bool isDisabled);
  void clearNoteName();
  void setStyle(Tnote::EnameStyle style); // Sets style. Doesn't refresh name label
  Tnote::EnameStyle style() { return m_style; } // Style used in note name

  void createNameTip(QGraphicsScene* scene);
  TnameTip* tip() { return m_tip; }
  void enableArrows(bool en); /**< Hides or shows arrow buttons on name label sides. */
  int widthForHorizontal(); /**< Estimated width for horizontal buttons layout */

  virtual QSize sizeHint() const;

      /** Marks m_nameLabel with given color. When clearNoteName() is invoked - marks are cleared. */
  void markNameLabel(const QColor& markColor);

      /** Highlights and check given accid button   */
  void forceAccidental(char accid);

      /** @p isWrong  determines kind of animation performed after invoking this method. */
  void correctName(Tnote &goodName, const QColor &color, bool isWrong = true);
  
      /** Returns given color mixed with palette base and 220 of alpha. */
  QColor prepareBgColor(const QColor &halfColor);
  
  QRectF textRect(); /**< Rectangle of note name text item */
  QPoint textPos(); /**< Position of name text in main window coordinates. */
  QRect labelRect(); /**< Name label position and size in TnoteName coordinates. */

signals:
  void noteNameWasChanged(Tnote note);
  void noteButtonClicked();
  void statusTipRequired(QString status);
  void nextNote();
  void prevNote();
  void correctingFinished(); /**< Emitted when correction animation finish */

protected:
  virtual bool event(QEvent* event);
  virtual void mousePressEvent(QMouseEvent*);

private:
  TnoteNameLabel         *m_nameLabel;
  TpushButton            *m_noteButtons[7];
  TpushButton            *m_octaveButtons[8];
  TpushButton            *m_dblFlatButt, *m_flatButt, *m_sharpButt, *m_dblSharpButt;
  QList<TpushButton*>     m_accidButtons; /**< List of buttons with accidental symbols. */
  QButtonGroup           *m_noteGroup, *m_octaveGroup;
  QPushButton            *m_nextNoteButt, *m_prevNoteButt;
  QLabel                 *m_octavesLab;
  int                     m_prevOctButton; /**< Keeps index of previous selected octave button, none if -1 */
  static                  Tnote::EnameStyle m_style;

  TnotesList              m_notes;
  short                   m_ambitMin, m_ambitMax;
  Tnote                   m_goodNote;
  int                     m_blinkingPhase;
  QMenu                  *m_menu;

  QBoxLayout             *m_buttonsLay, *m_noteLay, *m_accLay, *m_upOctaveLay, *m_loOctaveLay;
  QSize                   m_sizeHint;
  int                     m_fontSize;
  TnameTip               *m_tip;

  bool                    m_isMenu;
  QWidget                *m_menuParent;
  static TnoteName       *m_instance;

private:
  void setNoteName(char noteNr, char octNr, char accNr);
  void setNameText();

  void setButtons(const Tnote& note); /**< Sets note, accidental and octave buttons according to given note. */

      /** Presses accidental button or un-check them all if accidental none (0). */
  void checkAccidButtons(char accid);
  void uncheckAccidButtons();
  void uncheckAllButtons();

  char getSelectedAccid(); /**< Returns current state of accidental buttons converted to accidental value [-2 to 2] */
  void updateSizeHint();
  void setButtonsSize(int widthOff, int fixedH, bool skipOctaves = false); /**< Iterates all buttons and adjust their size to context (text)  */
  TpushButton* createAccidButton(const QString& accidText);

      /** Sets fixed button width to its text width + @p widthOff or reset to default when @p widthOff = 0. */
  void fixButtonWidth(int widthOff, QPushButton* butt);

private slots:
  void noteWasChanged(int noteNr);
  void accidWasChanged();
  void octaveWasChanged(int octNr);
  void correctAnimationFinished();
  void invokeBlinkingAgain();
  void prevNoteSlot() { emit prevNote(); }
  void nextNoteSlot() { emit nextNote(); }
};

#endif // TNOTENAME_H
