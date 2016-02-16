/***************************************************************************
 *   Copyright (C) 2012-2015 by Tomasz Bojczuk                             *
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



#ifndef TSUPPORTNOOTKA_H
#define TSUPPORTNOOTKA_H


#include <QtWidgets/qdialog.h>
#include <touch/ttoucharea.h>


/**
 * Dialog displaying ways how to give support for Nootka
 */
class TsupportNootka : public TtouchArea
{
  Q_OBJECT

public:
  explicit TsupportNootka(QWidget *parent = 0);
};


/**
 * Wraps @class TsupportNootka into standalone dialog window
 */
class TsupportStandalone : public QDialog
{
  Q_OBJECT

public:
  explicit TsupportStandalone(QWidget *parent = 0);
};

#endif // TSUPPORTNOOTKA_H
