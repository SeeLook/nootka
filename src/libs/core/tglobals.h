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


#ifndef TGLOBALS_H
#define TGLOBALS_H

#include <nootkacoreglobal.h>
#include <QtCore/qstring.h>
#include <QtGui/qcolor.h>
#include <music/tnote.h>
#include <music/tinstrument.h>


class TlayoutParams;
class TscoreParams;
class Ttune;
class QSettings;
class TexamParams;
class TaudioParams;

class NOOTKACORE_EXPORT Tglobals
{

public:

      /** If @p true, setting are loaded from temporary config file */
  Tglobals();
  ~Tglobals();

      /** This method return application install path - path from where Nootka was started. */
  static QString getInstPath(QString appInstPath);
  static QString& path; /**< Reference to Tpath::main - Nootka resources path */

  void storeSettings(QSettings* cfg); /**< Saves settings stored in @p cfg */
  void loadSettings(QSettings* cfg); /**< Loads Nootka settings from file to @p cfg */

  QString version;
  bool isFirstRun; /**< to show or skip first run wizard*/
  QString lang; /**< Application language. If empty - selected from system info*/
  QSettings *config; /**< Pointer to QSettings instance of Nootka */
  bool useAnimations; /**< to show GUI animations. */
  bool enableTouch; /**< When true, touch events are propagated */

        /** Due to bug in version 0.8.95 it keeps value of user declared instrument to fix incorrect levels
         * or -1 to display a dialog for user to declare it.  */
  int instrumentToFix;

        /** Let's have a convention:
        * globals settings for @class TnoteName will started from 'N' letter
        * for @class TscoreWidget and @class TscoreWidgetSimple
        * and for guitar (@class TfingerBoard) from 'G' letter.
        * For sound and exam there are pointers to appropirate classes with theirs parameters. */

  //============ score settings =============================================================
  TscoreParams *S; /**< Score parameters */


    /** Guessing solfege name style from current locale setting. F.e.: ru is e_russian_Ci */
  Tnote::EnameStyle getSolfegeStyle();

  //============ guitar settings =============================================================
  Einstrument instrument; /**< Type of instrument - classical guitar default */
  unsigned char GfretsNumber; /**< default 19 */
  bool GisRightHanded; /**< default true */
  bool GshowOtherPos; /**< Shows other possibilities of note (sound) on the fretboard (default true) */
  QColor GfingerColor; /**< rules the same like in S->enharmNotesColor */
  QColor GselectedColor;

      /** Actual tune of the guitar also with information about strings number
       * available by Ttune::stringsNr() */
  Ttune *Gtune() { return m_tune; }
  void setTune(Ttune &t);

      /** It returns real string number (0 - 5) when @param strNr
       * is sorted number from highest (0) to lowest (5) */
  qint8 strOrder(qint8 strNr) { return m_order[strNr]; }
  Tnote hiString(); /**< Returns the highest (usually first - 0) string. */
  Tnote loString(); /**< Returns the lowest (usually last) string */

      /** The highest available note in current tune with current fret number */
  Tnote hiNote() { return Tnote(hiString().chromatic() + GfretsNumber); }
  Tnote loNote() { return loString(); } // the same as loString()

      /** It says witch accidentals are preferred while user clicks guitar
       * and note is calculated. Default are sharps*/
  bool GpreferFlats; // default false
  QList<QVariant> GmarkedFrets;


  TexamParams *E; /**< Exam parameters */
  QColor EquestionColor; // red
  QColor EanswerColor; // green
  QColor EnotBadColor; // #ff8000 (orange)

  TaudioParams *A;  /** Audio parameters */
  TlayoutParams *L; /**< Main window Layout params. */

private:
  Ttune *m_tune; /**< current guitar tune */
  qint8 m_order[6]; /**< Strings order is determined in @param setTune() method */

};
#endif // TGLOBALS_H

