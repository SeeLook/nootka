/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                             *
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

#ifndef TABOUT_H
#define TABOUT_H

#include "nootkamiscglobal.h"
#include <QtWidgets/qwidget.h>

class TroundedLabel;


/**
 * Widget with initial information about Nootka
 */
class NOOTKAMISC_EXPORT Tabout : public QWidget
{
  Q_OBJECT
public:
  explicit Tabout(QWidget *parent = 0);


private:
    TroundedLabel *m_aboutLab;
};

#endif // TABOUT_H
