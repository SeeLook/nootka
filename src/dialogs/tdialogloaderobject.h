/***************************************************************************
 *   Copyright (C) 2017-2019 by Tomasz Bojczuk                             *
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

#ifndef TDIALOGLOADEROBJECT_H
#define TDIALOGLOADEROBJECT_H

#include <QtCore/qobject.h>

/**
 * Implements logic for dialogues loader (@p DialogLoader.qml),
 * but also initializes C++ components related to exams/levels for QML.
 * Those components are not necessary when Nootka starts
 */
class TdialogLoaderObject : public QObject
{

  Q_OBJECT

  Q_PROPERTY(QString updateMessage READ updateMessage NOTIFY updateMessageChanged)

public:
  TdialogLoaderObject(QObject* parent = nullptr);
  ~TdialogLoaderObject() override;

  QString updateMessage() { return m_updateMessage; }

      /**
       * Returns translated text according to Qt @p ButtonDialogBox enumerator
       */
  Q_INVOKABLE QString stdButtonText(int role);

      /**
       * Icon image name of given @p ButtonDialogBox role
       */
  Q_INVOKABLE QString stdButtonIcon(int role);

      /**
       * icon name by button role
       */
  Q_INVOKABLE QString buttonRoleIcon(int role) const;

  Q_INVOKABLE void openFile(const QString& fileName);

  Q_INVOKABLE QString aboutQt() const;

  Q_INVOKABLE QString mainHelp() const;
  Q_INVOKABLE QString exerOrExamHelp() const;
  Q_INVOKABLE QString examHelp() const;

  Q_INVOKABLE QString getChanges() const;

  Q_INVOKABLE QString getLicense();

  static void updateCheckInBackground();

  Q_INVOKABLE void checkForUpdates();

      /**
       * Do routines related to version change, show support window or other actions.
       * Returns @p TRUE if dialog will be invoked, or @p FALSE if no action will be proceeded.
       * @p nootWin should be QObject instance of QML main window.
       * Also it stores current version in settings - it should be only place where it is done
       */
  static bool checkVersion(QObject* nootWin);

signals:
  void continueExam(const QString& examFile);
  void openLevel(const QString& levelFile);
  void updateMessageChanged();

private:
  static bool                    m_firstTime;
  QString                        m_updateMessage;

};

#endif // TDIALOGLOADEROBJECT_H
