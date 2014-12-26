/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
 *   tomaszbojczuk@gmail.com                                               *
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

#ifndef TPLUGINSLOADER_H
#define TPLUGINSLOADER_H

#include <QObject>
#include "tplugininterface.h"


class QWidget;
class Texam;
class QPluginLoader;

/** 
 * Common class to load Nootka plugins.
 * After constructing a loader object:
 * 1. call @p load(plugin type) and check is it loaded
 * 2. call @p init() to start a plugin
 * 3. check returned @p lastWord()
 */
class TpluginsLoader : public QObject
{
  Q_OBJECT
  
public:
  explicit TpluginsLoader(QObject* parent = 0);
  virtual ~TpluginsLoader();
  
  enum Etype {
    e_level = 0, e_settings = 1, e_analyzer = 2, e_updater = 3, e_wizard = 4
  }; /** Plugin type. */
  
  bool load(Etype pluginType); /** Loads given plugin type and returns @p TRUE when loaded properly. */
  bool init(const QString& argument = "", QWidget* parent = 0, Texam* exam = 0); /** Starts plugin routines. */
  QString& lastWord() { return m_lastWord; } /** Returns last plugin string. */
  
  TpluginObject* node() { return m_signalNode; } /** Object through which a plugin can sends signals */
  
protected:
  void pluginMessage(const QString& m) { m_lastWord = m; }
  
private:
  TpluginInterface     *m_plugInterface;
  QPluginLoader        *m_loader;
  TpluginObject        *m_signalNode;
  Etype                 m_type;
  QString               m_lastWord;
};


#endif // TPLUGINSLOADER_H