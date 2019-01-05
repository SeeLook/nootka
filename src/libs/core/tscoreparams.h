/***************************************************************************
 *   Copyright (C) 2014-2019 by Tomasz Bojczuk                             *
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

#ifndef TSCOREPARAMS_H
#define TSCOREPARAMS_H

#include "nootkacoreglobal.h"
#include "music/tnote.h"
#include "music/tclef.h"

#include <QtGui/qcolor.h>


/**
 * This is set of parameters related to score and note names
 */
class NOOTKACORE_EXPORT TscoreParams
{
public:

  TscoreParams() : clef(Tclef::defaultType) {}

//============ score widget settings =============================================================
       /**
        * if true shows other similar (enharmonic) notes on the staff:
        * for C# - Db, for G - Fx and Abb. In Tnotename widget also.
        */
  bool keySignatureEnabled;
  bool showKeySignName; /**< default true */
  Tnote::EnameStyle nameStyleInKeySign;

       /**
        * Convention is:
        * if keyNameSuffix == " " constructor of Tglobals sets its default and
        * constructor of TkeySignatureView sets translatable value "major" and "minor"
        * otherwise it is overridden by loading settings
        * if keyNameSuffix == "" user prefers without suffix.
        * If keyNameSuffix has some text -  is translated by user himself
        */
  QString majKeyNameSufix;
  QString minKeyNameSufix;
  QColor pointerColor;
  bool rhythmsEnabled;
  Tclef::EclefType& clef; /**< preferred clef - treble by default, reference to static Tclef::defaultType */
  bool isSingleNoteMode; /**< score mode (single or multi) */
  qreal scoreScale; /**< score scale - user preferred staff size */
  int tempo; /**< playback tempo */
  QString lastXmlDir;

//============= common with score widget and note name ==========================================
  bool doubleAccidentalsEnabled; /**< default @p TRUE */
  bool showEnharmNotes; /**< default @p TRUE */

       /**
        * On the very beginning it is -1 and then it is set in TscoreWidget constructor
        * as inversion of highlight color from palette() and put to TnoteName,
        * otherwise is taken from saved settings.
        */
  QColor enharmNotesColor;
  bool seventhIs_B; /**< To determine note names - default @p TRUE */


//======== note name settings ===================================================================
  Tnote::EnameStyle nameStyleInNoteName;
  bool octaveInNoteNameFormat; /**< default @p TRUE */
  Tnote::EnameStyle solfegeStyle; /**< e_italiano_Si is default */
  bool namesOnScore; /**< show/hide note names on the score */
  QColor nameColor; /**< color of note name highlight */

      /**
       * Determines whether to use International Pitch Notation,
       * when octaves are numbered from 0 (-3 here, sub contra octave).
       * By default it is @p FALSE
       */
  bool scientificOctaves;

};

#endif // TSCOREPARAMS_H
