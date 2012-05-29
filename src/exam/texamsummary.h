/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk                                  *
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

#ifndef TEXAMSUMMARY_H
#define TEXAMSUMMARY_H

#include <QDialog>

class Texam;


/** When @param cont is true On button is shown text 'continue' */
class TexamSummary : public QDialog
{
  Q_OBJECT
public:
  TexamSummary(Texam *exam, QString &path, bool cont = false, QWidget *parent = 0);
  virtual ~TexamSummary();
  
private:
  Texam *m_exam;
};

#endif // TEXAMSUMMARY_H
