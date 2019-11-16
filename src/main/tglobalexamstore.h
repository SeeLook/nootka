/***************************************************************************
 *   Copyright (C) 2013-2019 by Tomasz Bojczuk                             *
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

#ifndef TGLOBALSTORE_H
#define TGLOBALSTORE_H


#include <music/tnote.h>
#include <music/tinstrument.h>
#include <music/ttune.h>
#include <music/tclef.h>


class Tlevel;
class Tglobals;


/**
 * Simple class which grabs global settings overwritten by exam,
 * stores them and restores when exam is finished. 
 */
class TglobalExamStore
{

public:

  TglobalExamStore(Tglobals* globals);

      /**
       * Grabs exam necessary settings from Tglobals except:
       * @p tune and @p fretsNumber which have to be set separately.
       */
  void storeSettings();
  void restoreSettings();

      /**
       * Sets initial values to Tglobals from given level.
       */
  void prepareGlobalsToExam(const Tlevel &level);

  bool                          showEnharmNotes;
  bool                          showKeySignName;
  bool                          showOtherPos;
  Tnote::EnameStyle             nameStyleInNoteName;
  bool                          useDblAccids;
  bool                          useKeySign;
  Ttune                         tune;
  bool                          octaveInName;
  char                          fretsNumber;
  Tclef                         clef;
  Tinstrument::Etype            instrument;
  quint8                        intonation;
  int                           playbackInstr;
  bool                          isSingleNoteMode;
  bool                          namesOnScore;
  bool                          OUTenabled,  INenabled;
  bool                          showSoundView, showGuitar;
  bool                          enableRhythms;
  int                           quantization;

private:
  Tglobals *m_globals;

};

#endif // TGLOBALSTORE_H
