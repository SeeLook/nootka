/***************************************************************************
 *   Copyright (C) 2013-2018 by Tomasz Bojczuk                             *
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

#ifndef TMAINHELP_H
#define TMAINHELP_H


#include <QtCore/qobject.h>


/**
 * This is help text displayed in first run wizard and in help dialog
 * Static methods give access to some excerpt texts.
 * When Nootka launches first time, this object is registered for QML and available through @p HELP context
 */
class TmainHelp : public QObject
{

  Q_OBJECT

public:
  explicit TmainHelp(QObject *parent = nullptr) : QObject(parent) {}
  ~TmainHelp();

      /**
       * Whole text of II. paragraph (II. Exercises and exams)
       */
  static QString exerciseAndExamText();

      /**
       * You will learn by answering questions. To answer, you can play, sing, put in the name of a note, and so on.
       */
  static QString youWillLearnText();

      /**
       * During exercising %1 the program will be your understanding and friendly teacher - 
       * it will show you corrected answers if you miss.
       */
  static QString duringExercisingText();

      /**
       * During exams %1 Nootka will be your strict and &quot;old school&quot; master.
       * Any mistake will be penalized with additional questions...
       * When you pass an exam you got a certificate!
       */
  static QString duringExamsText();

  Q_INVOKABLE static QString mainHelp();

};

#endif // TMAINHELP_H
