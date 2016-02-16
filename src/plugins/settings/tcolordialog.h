/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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

#ifndef TCOLORDIALOG_H
#define TCOLORDIALOG_H

#include <QtWidgets/qdialog.h>


class QSpinBox;
class TcolorPreview;
namespace color_widgets {
  class ColorWheel;
}


/**
 * This is replacement of QColorDialog for Android and other mobile.
 * So far, @class QColorDialog is not adjusted for touch screens.
 * Usage:
 * 1. Create dialog with given initial color.
 * 2. Call @p exec() to invoke the dialog.
 * 3. Check was dialog accepted.
 * 4. Get selected color through @p selectedColor().
 */
class TcolorDialog : public QDialog
{
  Q_OBJECT

public:
  explicit TcolorDialog(const QColor& initial, QWidget* parent = 0);
  QColor selectedColor();
  void setColor(const QColor& c);

protected slots:
  void colorChanged(const QColor& c);
  void spinValueChanged();

private:
  QSpinBox                        *m_redSpin, *m_greenSpin, *m_blueSpin;
  QPushButton                     *m_acceptBut, *m_cancelButt;
  TcolorPreview                   *m_colorPreview;
  color_widgets::ColorWheel       *m_wheel;
};

#endif // TCOLORDIALOG_H
