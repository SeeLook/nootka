/***************************************************************************
 *   Copyright (C) 2012-2014 by Tomasz Bojczuk                             *
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

#ifndef TLEVELPREVIEW_H
#define TLEVELPREVIEW_H

#include <nootkacoreglobal.h>
#include <QWidget>
#include <QUrl>

class QTextBrowser;
class Tlevel;

/**
 * This class represents a preview of a given level in form of a table.
 * It displays level instrument symbol under the table as well.
 * It also detects broken instrument type in levels 
 * and when m_enableFixing is set to true (false by default) 
 * it allows to emit instrumentLevelToFix() signal when user wants to fix a level.
 */
class NOOTKACORE_EXPORT TlevelPreview : public QWidget
{

  Q_OBJECT
  
public:
  explicit TlevelPreview(QWidget *parent = 0);
  virtual ~TlevelPreview();
  
  static QString notesRangeTxt() { return tr("note range:"); }
  static QString fretsRangeTxt() { return tr("fret range:"); }

  void setLevel(Tlevel &tl);
	void setLevel(); /** Overloaded method with empty level to force empty table. */
	
			/** This method sets fixed height of QTextBrowser containing table with level preview
			 * to current document height. It removes vertical scroll*/
	void adjustToHeight();
	
			/** Controls when preview table shows instrument type to fix (enabled).
			 * When @p true - instrumentLevelToFix() signal is emitted when user clicks a link. */
	void setFixInstrEnabled(bool enabled);
	
signals:
			/** This signal is emitted when level had wrong instrument and user wants to fix it. */
	void instrumentLevelToFix();
  
protected:
			/** Paints guitar head shape under widget. */
	void paintEvent(QPaintEvent* );
	
protected slots:
	void linkToFixLevel(QUrl url);
	
	
private:
	QTextBrowser 	*m_summaryEdit;
	QString 			 m_instrText;
	bool					 m_enableFixing;

};

#endif // TLEVELPREVIEW_H
