/***************************************************************************
 *   Copyright (C) 2014-2016 by Tomasz Bojczuk                             *
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
#include <QtCore/qplugin.h>

namespace Torders {
      /**< Enumeration messages sends between @class TexamPlugin and @class MainWindow with demands:
       * @p e_examFailed - when exam could not be started
       * @p e_examFinished - when exam was finished
       * @p e_examAskCreator - when start exam dialog wants to open level creator
       * @p e_examSingle - switch to single note mode
       * @p e_examMultiple - switch to multiple note mode
       * @p e_examResize - update sizes of main window widgets
       * @p e_examDisconnect - remove signal/slot connections between widgets - executor will handle them itself
       * @p e_examConnect - connect main window widgets again
       * Orders sent to plugin
       * @p e_examAllowClose - when exam is pending and executor checked that it is allowed
       * @p e_examSettings - when settings dialog window is going to be opened
       */
  enum Eexam {
    e_examFailed = 0,     //when exam could not be started
    e_examFinished = 1,   // when exam was finished
    e_examAskCreator = 2, // when start exam dialog wants to open level creator
    e_examSingle = 3,     // switch to single note mode
    e_examMultiple = 4,   // switch to multiple note mode
    e_examResize = 5,     // update sizes of main window widgets
    e_examDisconnect = 6, // remove signal/slot connections between widgets - executor will handle them itself
    e_examConnect = 7,    // connect main window widgets again
    e_examAllowClose = 8, // when exam is pending and executor checked that it is allowed
    // demands sends to exam executor
    e_examClose = 100,     // when main window is going to close during exam/exercise
    e_examSettings = 101,  // when settings dialog window is going to be opened
  };

      /** Enumeration with results returned by settings plugin
      * @p e_settingsAccept - settings were accepted
      * @p e_canceled - settings canceled
      * @p e_reset - demand for reset all settings
      */
  enum Esettings {
    e_settingsAccept = 0,   // settings were accepted
    e_settingsCancel = 1, // settings canceled
    e_settingsReset = 2     // demand for reset all settings
  };
}

class Texam;

/**
 * This is QObject subclass that transports signals between plugin and its caller
 * @p message(QString) and @p value(int) are sent from plugin
 * @p backMessage and @p backValue are sent to plugin
 * Through it a plugin can communicate with a rest of an application.
 */
class NOOTKACORE_EXPORT TpluginObject : public QObject
{
  Q_OBJECT

public:
  explicit TpluginObject(QObject* parent = 0) : QObject(parent) {}

  void emitMessage(const QString& m) { emit message(m); }
  void emitValue(int v) { emit value(v); }

  void emitBackMessage(const QString& m) { emit backMessage(m); }
  void emitBackValue(int v) { emit backValue(v); }

signals:
  void message(const QString&);
  void value(int);
  void backMessage(const QString&);
  void backValue(int);
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
  virtual void init(const QString& argument = QString(), TpluginObject* ob = 0, QWidget* parent = 0, Texam* exam = 0) = 0;

      /** Implementation of this method should return QString with communicate from plugin. */
  virtual QString lastWord() = 0;

};

#define TpluginInterface_iid "nootka.pluginInterface"

Q_DECLARE_INTERFACE(TpluginInterface, TpluginInterface_iid)

#endif // TPLUGININTERFACE_H
