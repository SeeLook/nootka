/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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

#ifndef TFILEDIALOG_H
#define TFILEDIALOG_H

#include <QtWidgets/qfiledialog.h>


/**
 *
 */
class TfileDialog : public QFileDialog
{
public:
  TfileDialog(QWidget* parent, const QString& caption, const QString& directory, const QString& filter);

  static QString getSaveFileName(QWidget* parent, const QString& caption, const QString& directory, const QString& filter);
  static QString getOpenFileName(QWidget* parent, const QString& caption, const QString& directory, const QString& filter);
};

#endif // TFILEDIALOG_H
