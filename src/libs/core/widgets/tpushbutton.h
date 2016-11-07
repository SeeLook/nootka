/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
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
 *  You should have received a copy of the GNU General Public License       *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#ifndef TPUSHBUTTON_H
#define TPUSHBUTTON_H

#include <nootkacoreglobal.h>
#include <QPushButton>

/** This is replacement for standard QPushButton.
 * The aim is to override setChecked() and isChecked() method
 * to be friendly with Bespin style witch looks bad with check-able buttons
*/
class NOOTKACORE_EXPORT TpushButton : public QPushButton
{
  Q_OBJECT
public:
  TpushButton(const QString& text = QString(), QWidget *parent = 0);
  void setChecked(bool isChecked);
  bool isChecked() { return m_Ichecked; }

    /** Sets colors for checked button.
     * It has influence on all buttons in the application.   */
  static void setCheckColor(QColor background = Qt::black, QColor text = Qt::white);

    /** Sets colors for this instance of checked button. */
  void setThisColors(QColor background = Qt::black, QColor text = Qt::white);


private:
  bool m_Ichecked;
  static QColor m_textColor;
  static QColor m_backColor;
  QColor m_textThis;
  QColor m_backThis;


};

#endif // TPUSHBUTTON_H
