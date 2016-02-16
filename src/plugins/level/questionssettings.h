/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                             *
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


#ifndef QUESTIONSSETTINGS_H
#define QUESTIONSSETTINGS_H


#include "tabstractlevelpage.h"

class QSpinBox;
class Tlevel;
class TlevelCreatorDlg;
class QComboBox;
class QCheckBox;
class QButtonGroup;
class QLabel;
class TkeySignComboBox;
class TquestionAsWdg;
class QRadioButton;
class QGroupBox;
class TpaintHandler;


class questionsSettings : public TabstractLevelPage
{
    Q_OBJECT
public:
  explicit questionsSettings(TlevelCreatorDlg *creator);

  virtual void loadLevel(Tlevel *level);
  virtual void saveLevel(Tlevel *level);


      /** Hides options related to guitar  */
  void hideGuitarRelated();

  void setMelodiesEnabled(bool enableMelodies); /** Switches between melodies and single note.  */

signals:
  void questSettChanged(); /** If any GUI element is changed this signal is emitted. */
  void scoreEnabled(bool); /** emitted when note in score are disabled. */
  void accidEnabled(bool); /** emitted when note in score and names are all disabled or all enabled.  */

public slots:
  void stringsCheckedSlot(bool checked);
  void singleMultiSlot(); /** Responses for switching between single note/melody mode. */
  void melodyQuestionSlot(); /** Sets appropriate boxes for melodies in single note table */
  virtual void changed();

protected:
  void paintSlot(); /** Paints lines of a table*/
  void adjustToLevel(); /** Checks values were set and locks/unlocks suitable widgets. */


private:
  QWidget 							*m_tableWdg;
  QLabel 								*m_questLab; // QLabel with 'QUESTION' text
  QLabel				 				*m_answLab; // fake QLabel to keep space
  QLabel 								*m_asSoundLab, *m_asFretLab, *m_soundNooLab, *m_qSoundNooLab;
  QLabel								*m_guitarNooLab, *m_qGuitarNooLab;
  TquestionAsWdg 				*asNoteWdg, *asNameWdg, *asFretPosWdg, *asSoundWdg;
  QCheckBox 						*styleRequiredChB, *octaveRequiredChB, *showStrNrChB;
  QCheckBox 						*lowPosOnlyChBox;
  QComboBox 						*m_intonationCombo;
  QGroupBox 						*m_singleGr, *m_melodiesGr;
  QCheckBox 						*m_playMelodyChB, *m_writeMelodyChB, *m_finishOnTonicChB;
  QSpinBox							*m_melodyLengthSpin;
  TpaintHandler         *m_paintHandler;

private slots:
  void whenParamsChanged(); /** Every element calls this when clicked. */

};

/**
 * Simple subclass of @class QWidget
 * that handles paint event end emits signal from there
 * to allow other widget paint on it.
 */
class TpaintHandler : public QWidget
{
  Q_OBJECT
public:
  TpaintHandler(QWidget* parent = 0) : QWidget(parent) {}

signals:
  void paintMe(); /** Signal from paint event. Emitting widget is available in slot method through @p sender() */

protected:
  virtual void paintEvent(QPaintEvent* e) { emit paintMe(); QWidget::paintEvent(e); }

};

#endif // QUESTIONSSETTINGS_H
