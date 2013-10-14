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

#ifndef THELPDIALOGBASE_H
#define THELPDIALOGBASE_H

#include <QDialog>
#include <QCheckBox>

class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QTextEdit;

/** This is base template class for help dialogues. 
 * It contains QTextEdit field to fill with help text
 * and QCheckBox which is optional. 
 * showCheckBox() method controls it.
 * By default there is OK button connected to 'accept' dialog code,
 * but @p showButtons() method can change it. */
class ThelpDialogBase : public QDialog
{

public:
  explicit ThelpDialogBase(QWidget* parent = 0, Qt::WindowFlags f = 0);
  virtual ~ThelpDialogBase() {};
  
      /** Pointer to QTextEdit. */
  QTextEdit* helpText() { return m_helpText; }
  
      /** Displays check box at the bottom of the window. */
  void showCheckBox(const QString &label);
      /** Control buttons. OK button is shown by default so when @p withOk = false is destroyed.
       * Buttons are responsible for returning dialog codes:
       * 'OK' button - accepted, 'Cancel' button - rejected  */
  void showButtons(bool withOk, bool withCancel);
    
  bool isDoNotShowChecked() { m_doNotShowChB ? m_doNotShowChB->isChecked() : false; }
  
private:
  QTextEdit       *m_helpText;
  QCheckBox       *m_doNotShowChB;
  QPushButton     *m_OkButton, *m_cancelButton;
  QVBoxLayout     *m_lay;
  QHBoxLayout     *m_buttonsLay;

};

#endif // THELPDIALOGBASE_H
