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

#ifndef TNOTENAMELABEL_H
#define TNOTENAMELABEL_H


#include <QLabel>

QString getBgColorText(const QColor &C);

/** */
class TnoteNameLabel : public QLabel
{
	Q_OBJECT
public:
	
	explicit TnoteNameLabel(const QString& text, QWidget* parent = 0);
	
			/** Starts painting cross over label given @p count times with @p period duration of each. */
	void blinkCross(const QColor &color, int count = 2, int period = 150);
			/** Fades out background to transparency, sets new text and fades in with new color.*/
	void crossFadeText(const QString &newText, const QColor &newBgColor, int duration = 150);
	
	void setStyleSheet(const QString &style);
	void setBackgroundColor(const QColor &color);
	
	static QString borderStyleText();
	
signals:
	void blinkingFinished();
	void crossFadeingFinished();
	
protected:
	virtual void paintEvent(QPaintEvent* event);
	
protected slots:
	void strikeBlinkingSlot();
	void crossFadeSlot();
	
private:
	int 			m_period, m_count, m_currentBlink;
	int 			m_fadeDuration, m_fadePhase, m_alphaStepOut, m_alphaStepIn;
	QColor 		m_color, m_bgColor, m_newBgColor;
	QString   m_newText, m_bgColorText, m_styleText;
	
};

#endif // TNOTENAMELABEL_H
