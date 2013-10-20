/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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

#ifndef TPIXMAPRADIO_H
#define TPIXMAPRADIO_H

#include <qt4/QtGui/QWidget>
#include <QBoxLayout>

class TroundedLabel;
class QLabel;
class QRadioButton;


/** This is QRadioButton with image on defined side and 
 * text which is QLabel and can be placed under a widget.
 * Default layout direction is Left-To-Right and order is pixmap-radio-label
 * It has QRadioButton available through radio() and 
 * setDirection controls vertical/horizontal direction
 * and inner widgets order can be changed by setOrder() 
 */
class TpixmapRadio : public QWidget
{

	Q_OBJECT
	
public:
	
	enum Eorder {
		e_labelRadioPix,
		e_pixRadioLabel, 
		e_radioLabelPix,
	};
	
	explicit TpixmapRadio(const QString &imgHtml, const QString &label, QWidget* parent = 0);
	
			/** Sets the order of inner widgets */
	void setOrder(TpixmapRadio::Eorder order);
	
	QRadioButton* radio() { return m_radio; }
			/** QLabel with pixmap in HTML syntax.  */
	TroundedLabel* pix() { return m_pixLabel; }
	QLabel* label() { return m_label; }
	QBoxLayout* lay() { return m_layout; }
	
			/** Shortcut to QBoxLayout::setDirection() */
	void setDirection(QBoxLayout::Direction dir) { m_layout->setDirection(dir); }
	
	
protected slots:
			/** Make pixmap label looks like disabled when radio is unchecked. */
	void radioCheched();
	
private:
	QRadioButton			*m_radio;
	QLabel						*m_label;
	TroundedLabel			*m_pixLabel;
	QBoxLayout				*m_layout;
	Eorder						m_oreder;
	

};

#endif // TPIXMAPRADIO_H

