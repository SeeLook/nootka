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

#ifndef TPITCHBUTTON_H
#define TPITCHBUTTON_H

#include <QAbstractButton>


/**
 * This is button like image with Nootka font glyph
 * used to switch audio input and intonation.
 * It will display text given in constructor - Nootka font glyph
 */
class TpitchButton : public QAbstractButton
{

public:
	TpitchButton(const QString& text, QWidget* parent = 0);

	virtual QSize sizeHint() const;

protected:
	virtual void paintEvent(QPaintEvent* );
	virtual void enterEvent(QEvent* );
	virtual void leaveEvent(QEvent* );
	virtual void resizeEvent(QResizeEvent*);

private:
	bool				m_entered;
};

#endif // TPITCHBUTTON_H
