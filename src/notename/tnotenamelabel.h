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


#include <QGraphicsView>
#include <QGraphicsTextItem>


			/**  Returns style sheet text: 
			*	background-color: rgba(red, green, blue, alpha);
			* of given color @p C  */
QString getBgColorText(const QColor &C);


//*****************************************************************************************************************
class TgraphicsStrikeItem;
/** */
class TnoteNameLabel : public QGraphicsView
{
	Q_OBJECT
public:
	
	explicit TnoteNameLabel(const QString& text, QWidget* parent = 0);
	
			/** Sets a label text. HTML format appreciated. Text is aligned to center. */
	void setText(const QString &text);
	QString text() const { return m_textItem->toHtml(); }
	
	void setFont(const QFont &font) { m_textItem->setFont(font); }
	QFont font() const { return m_textItem->font(); }
	
	void showQuestionMark(const QColor &color);
	void showStringNumber(int strNr, const QColor& color);
	void markText(const QColor &color);
	
			/** Starts painting cross over label given @p count times with @p period duration of each. */
	void blinkCross(const QColor& color);
			/** Fades out background to transparency, sets new text and fades in with new color.*/
	void crossFadeText(const QString &newText, const QColor &newBgColor, int duration = 150);
	
	void setStyleSheet(const QString &style);
	void setBackgroundColor(const QColor &color);
	
	static QString borderStyleText();
	
signals:
	void blinkingFinished();
	void crossFadeingFinished();
	
protected:
	virtual void resizeEvent(QResizeEvent* event);
	
			/** Places main text item on the center of a widget. */
	void center();
	
protected slots:
	void strikeBlinkingSlot();
	void crossFadeSlot();
	
private:
	int 											m_fadeDuration, m_fadePhase, m_alphaStepOut, m_alphaStepIn;
	QColor 										m_bgColor, m_newBgColor;
	QString   								m_newText, m_bgColorText, m_styleText;
	QGraphicsTextItem					*m_textItem;
	TgraphicsStrikeItem				*m_strikeOut;
	QGraphicsSimpleTextItem		*m_questMark, *m_stringNumber;
	
};

#endif // TNOTENAMELABEL_H
