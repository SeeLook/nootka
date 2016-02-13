/***************************************************************************
 *   Copyright (C) 2016 by Tomasz Bojczuk                                  *
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

#ifndef TSTATUSLABEL_H
#define TSTATUSLABEL_H

#include <widgets/troundedlabel.h>

class QTimer;

#define   STATUS    TstatusLabel::instance()

/**
 * This is a label with a role like a status bar.
 * Use its method @p messageSlot() to get status bar functionality.
 *
 * It has single instance available through @p instance()
 * defined also as a macro @p STATUS
 */
class TstatusLabel : public TroundedLabel
{

  Q_OBJECT

public:
  explicit TstatusLabel(QWidget* parent = 0);
  virtual ~TstatusLabel();

  static TstatusLabel* instance() { return m_instance; }

      /** Wraps given @p text into HTML tag <center></center> */
  static inline QString center(const QString& text) { return QLatin1String("<center>") + text + QLatin1String("</center>"); }

  void setMessage(const QString& msg);
  void setMessage(const QString& msg, int time);
  void setText(const QString& msg) { setMessage(msg); } /**< Just overrides method od @class QLabel and calls @p setMessage() */
  QString message() { return m_text; } /**< Current message text */

  void setBackground (const QColor& bg);

      /** Connect this slot to signal emitted by widgets with status message text to get this functionality. */
  void messageSlot(const QString& msg);

protected:
  void restoreMessage();

private:
  QString               m_text, m_prevText;
  QTimer               *m_messageTimer;

  bool                  m_lockStat; /**< Keeps true when it is locked by temporary message and stops any status messages in this time.*/
  QColor                m_currentBg, m_prevBg;
  static TstatusLabel  *m_instance;
};

#endif // TSTATUSLABEL_H
