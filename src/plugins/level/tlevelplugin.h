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

#ifndef TLEVELPLUGIN_H
#define TLEVELPLUGIN_H


#include <plugins/tplugininterface.h>


class TlevelCreatorDlg;


/**
 * This plugin loads the level creator.
 * @p args given in @p init() method can contain path to level file
 * @p lastWord() returns level creator communicate with level name to start exam or exercise
 */
class TlevelPlugin : public QObject, public TpluginInterface
{

  Q_OBJECT
  Q_PLUGIN_METADATA(IID TpluginInterface_iid FILE "")
  Q_INTERFACES(TpluginInterface)

public:
  virtual ~TlevelPlugin();

      /** For level plugin @p ob and @p exam are unused */
  virtual void init(const QString& argument = QString(), TpluginObject* ob = 0, QWidget* parent = 0, Texam* exam = 0);

  virtual QString lastWord() { return m_lastWord; }

private:
  TlevelCreatorDlg        *m_creator;
  QString                  m_lastWord;

};


#endif // TLEVELPLUGIN_H
