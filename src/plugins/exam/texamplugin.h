/***************************************************************************
 *   Copyright (C) 2016 by Tomasz Bojczuk                                  *
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

#ifndef TEXAMPLUGIN_H
#define TEXAMPLUGIN_H

#include <QtCore/qobject.h>
#include <plugins/tplugininterface.h>

class TexamExecutor;
class Tlevel;

/**
 * Exam executing plugin
 */
class TexamPlugin : public QObject, public TpluginInterface
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID TpluginInterface_iid FILE "")
  Q_INTERFACES(TpluginInterface)

public:
  virtual ~TexamPlugin();

  virtual void init(const QString& argument = QString(), TpluginObject* ob = 0, QWidget* parent = 0, Texam* exam = 0);
  virtual QString lastWord() { return QString(); } // so far unused

protected:
  void backValueSlot(int v);

private:
  TpluginObject           *m_sender;
  QWidget                 *m_parentWidget;
  TexamExecutor           *m_executor;
  Tlevel                  *m_level;
};

#endif // TEXAMPLUGIN_H
