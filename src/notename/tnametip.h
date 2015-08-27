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

#ifndef TNAMETIP_H
#define TNAMETIP_H

#include <graphics/tgraphicstexttip.h>
#include <QGraphicsProxyWidget>


class QVBoxLayout;
class TnoteName;


/** 
 * This is wrapper for @p TnoteName to squeeze it into graphics widget.
 * It is @class TgraphicsTextTip derivative with window color background.
 * 
 * @class TnoteName is created by @class TmainScore 
 * but its size is managed from @class MainWindow during re-sizing.
 * @class TnameTip instance exists inside TnoneName as a @p tip() method
 * but it is created from @class TmainView which keeps graphics scene for it.
 * @class TmainScore calls @p exec() of TnoteName with desired position
 * and tip appears.
 * 
 * @class TnameTip does nothing in single note mode 
 * when TnoteName is ordinary widget on main Nootka window.
 */
class TnameTip : public TgraphicsTextTip
{

	Q_OBJECT
public:
	TnameTip(TnoteName* noteName);
	
	virtual ~TnameTip();
	
	void wrapNoteName(); /** It takes note name inside again. */
	void unwrapNoteName(); /** Sets wrapped widget to 0 */
	
	void adjustSize();

	virtual QRectF boundingRect() const;

public slots:
	void hide() { m_cursorEntered = false; QGraphicsTextItem::hide(); }
	void show();
	void showTimeExpired();
	void enteredSlot() { m_cursorEntered = true; }
	
protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) {} /** Grab click events */
	
private:
	QGraphicsProxyWidget							*m_proxy;
	QWidget														*m_widget;
	QVBoxLayout 											*m_lay;
	TnoteName													*m_name;
	QTimer														*m_hideTimer;
	bool															 m_cursorEntered;
};

#endif // TNAMETIP_H
