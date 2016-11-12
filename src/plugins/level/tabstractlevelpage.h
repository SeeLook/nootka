/***************************************************************************
 *   Copyright (C) 2014-2016 by Tomasz Bojczuk                             *
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

#ifndef TABSTRACTLEVELWIDGET_H
#define TABSTRACTLEVELWIDGET_H

#include <touch/ttoucharea.h>


class TlevelCreatorDlg;
class Tlevel;


/**
 * This is base class for level settings widget.
 * It has static Tlevel object which is working instance of a level
 * When subclass changes some level parameter it will emit @p levelChanged()
 * connected to @p TlevelCreatorDlg to inform other widgets
 */
class TabstractLevelPage : public TtouchArea
{
  Q_OBJECT

public:
  explicit TabstractLevelPage(TlevelCreatorDlg* creator);
  virtual ~TabstractLevelPage();

  virtual void loadLevel(Tlevel* level) = 0;
  virtual void saveLevel(Tlevel* level) = 0;

signals:
  void levelChanged();

public slots:

      /** This method is invoked from 'outside' when other widget changed a level settings.
       * Implement here routines to adjust widgets to changed level parameters. */
  virtual void changed() = 0;


protected:

      /** Common level for all pages of level creator.
       *  Every page stores there its settings and sets controls according to it */
  static Tlevel* workLevel() { return m_workLevel; }

      /** Generates status tip text from given parameters
       * @p qType and @p aType are @p TQAtype::Etype values cast to int  */
  QString tableTip(const QString& tipText, int qType, int aType, int fSize);

protected slots:

      /** Connect to this slot changes of every settings widget (check boxes, other switches).
       * It will invoke @p saveLevel(m_workLevel) and emit @p levelChanged() signal.   */
  virtual void changedLocal();

private:
  static Tlevel          *m_workLevel;
  static int              m_cnt; // to properly remove Tlevel after the last instance;
};

#endif // TABSTRACTLEVELWIDGET_H
