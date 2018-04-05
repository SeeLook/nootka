/***************************************************************************
 *   Copyright (C) 2013-2018 by Tomasz Bojczuk                             *
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


#ifndef THELPDIALOGBASE_H
#define THELPDIALOGBASE_H


#include <QtWidgets/qdialog.h>
#include <QtWidgets/qcheckbox.h>
#include <QtWidgets/qtextbrowser.h>
#include <QtCore/qpointer.h>


class TtextBrowser : public QTextBrowser
{
public:
  explicit TtextBrowser(QWidget* parent = nullptr) : QTextBrowser(parent) {}

  QVariant loadResource(int type, const QUrl & name) override;
};

class QDialogButtonBox;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;


/**
 * This is base template class for help dialogues. 
 * It contains QTextEdit field to fill with help text
 * and QCheckBox which is optional. 
 * showCheckBox() method controls it.
 * By default there is OK button connected to 'accept' dialog code,
 * but @p showButtons() method can change it. 
 */
class ThelpDialogBase : public QDialog
{

  Q_OBJECT

public:
  explicit ThelpDialogBase(QWidget* parent = nullptr, Qt::WindowFlags f = 0);
  virtual ~ThelpDialogBase();

      /**
       * Pointer to QTextEdit.
       */
  QTextEdit* helpText() { return m_helpText; }

      /**
       * Displays check box at the bottom of the window with given description. 
       * @p state reference will keep this check state.
       */
  void showCheckBox(const QString& label, bool* state);

      /**
       * Overridden method with universal text 'always show this help window'
       */
  void showCheckBox(bool* state) { showCheckBox(showHelpWindowTxt(), state); }

      /**
       * Control buttons. OK button is shown by default so when @p withOk = false is destroyed.
       * Buttons are responsible for returning dialog codes:
       * 'OK' button - accepted, 'Cancel' button - rejected 
       */
  void showButtons(bool withOk, bool withCancel);

  bool isDoNotShowChecked() { if (m_checkBox) return m_checkBox->isChecked(); else return false; }

      /**
       * Shortcut to global pixToHtml function. @p imageName is only filename without extension and path
       */
  static QString pix(const char* imageName, int height = 32);

      /**
       * Path to Nootka resources
       */
  static QString& path() { return m_path; }

      /**
       * Universal button text with context:
       * 'always show this help window'
       */
  static QString showHelpWindowTxt() { return tr("always show this help window"); }

      /**
       * Returns HTML tag p (paragraph) with right aligned text: Open online documentation linked to Nootka site documentation with given hash
       */
  static QString onlineDocP(const QString& hash);

protected:
  QDialogButtonBox* buttonBox() { return m_buttonBox; }

private:
  TtextBrowser             *m_helpText;
  QCheckBox                *m_checkBox;
  QPointer<QPushButton>     m_OkButton, m_cancelButton;
  QDialogButtonBox         *m_buttonBox;
  QVBoxLayout              *m_lay;
  bool                     *m_stateOfChB;
  static QString            m_path;

};

#endif // THELPDIALOGBASE_H
