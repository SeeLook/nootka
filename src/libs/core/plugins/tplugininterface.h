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

#ifndef TPLUGININTERFACE_H
#define TPLUGININTERFACE_H

#include <nootkacoreglobal.h>
#include <QtPlugin>

class Texam;

/** 
 * This is QObject subclass that implements two additional signals:
 * @p message(QString) and @p value(int).
 * Through it a plugin can communicate with a rest of an application.
 */
class NOOTKACORE_EXPORT TpluginObject : public QObject
{
  Q_OBJECT
  
public:
  explicit TpluginObject(QObject* parent = 0) : QObject(parent) {}
  
  void emitMessage(const QString& m) { emit message(m); }
  void emitValue(int v) { emit value(v); }
  
signals:
  void message(const QString&);
  void value(int);
  
};


/** 
 * Abstract class defining plugin interface to load extra dialog windows
 */
class NOOTKACORE_EXPORT TpluginInterface 
{
  
public:
  virtual ~TpluginInterface() {}
  
      /** This method has to be called when plugin is initialized.
       * All parameters are optional and depends on real plugin behind the abstract interface.
       * @p argument is a string forwarded to plugin.
       * @p ob - through QObject instance plugin can communicate with caller
       * @p parent - is a parent window for plugin QDialog class
       * and @p exam is pointer to existing Texam object.
       */
  virtual void init(const QString& argument = "", TpluginObject* ob = 0, QWidget* parent = 0, Texam* exam = 0) = 0;
  
      /** Implementation of this method should return QString with communicate from plugin. */
  virtual QString& lastWord() = 0;
  
};

#define TpluginInterface_iid "nootka.pluginInterface"

Q_DECLARE_INTERFACE(TpluginInterface, TpluginInterface_iid)

#endif // TPLUGININTERFACE_H
