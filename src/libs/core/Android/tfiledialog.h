/***************************************************************************
 *   Copyright (C) 2015-2020 by Tomasz Bojczuk                             *
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

#ifndef TFILEDIALOG_H
#define TFILEDIALOG_H


#include <QtCore/qglobal.h>


#if defined (Q_OS_ANDROID)

#include <QtWidgets/qdialog.h>
#include <QtWidgets/qfilesystemmodel.h>
#include <QtWidgets/qboxlayout.h>


class QListWidget;
class QListWidgetItem;
class QListView;
class QLineEdit;
class QLabel;
class QComboBox;


/**
 * Implementation of file dialog for mobile devices.
 * It is far from ideal but much better than native QFileDialog.
 * It has two static methods @p getOpenFileName and @p getSaveFileName
 * with three parameters (in contrary to native there is no caption).
 * Third @param filter may support multiple filters but only single one has been tested so far.
 * NOTICE: In contrary to @class QFileDialog @param filter takes:
 * only extension name WITHOUT any dots, parenthesis, asterisks or so.
 * Multiple extensions can should be separated by |
 * noo|nel or bz|gz|bz2
 */
class TfileDialog : public QDialog
{
  Q_OBJECT

public:
      /**
       * File dialog type: for opening or saving
       */
  enum EacceptMode { e_acceptSave, e_acceptOpen };
  Q_ENUM(EacceptMode)

  explicit TfileDialog(const QString& directory, const QString& filter, EacceptMode mode);
  ~TfileDialog() override;

  static QString getSaveFileName(const QString& directory = QString(), const QString& filter = QString());
  static QString getOpenFileName(const QString& directory = QString(), const QString& filter = QString());

  QString selectedFile() const { return m_selectedFile; }

      /**
       * Left to right by default
       */
  void setDirection(QBoxLayout::Direction dir) { m_lay->setDirection(dir); }
  EacceptMode acceptMode() { return m_acceptMode; }


protected:
  void itemClickedSlot(const QModelIndex& index);
  void menuClickedSlot(QListWidgetItem* item);
  void dirUpSlot();
  void newDirSlot();
  void performAction(); /**< Open or save */
  void createNewDir(const QString& newDir); /**< Creates new directory with name @p newDir in current model node */

    /**
     * Displays message about possibility of creating Nootka folder on external card (if exists - or internal then).
     * If user agreed - creates the directory.
     */
  void createNootkaDir();

  QListWidgetItem* addMenuItem(const QIcon& icon, const QString& text = QString());
  void updateLocationLabel();

private:
  QFileSystemModel        *m_fileModel;
  QString                  m_selectedFile;
  EacceptMode              m_acceptMode;

  QBoxLayout              *m_lay;
  QListView               *m_list;
  QListWidget             *m_menu;
  QListWidgetItem         *m_acceptItem, *m_cancelItem, *m_dirUpItem, *m_newDirItem;
  QLineEdit               *m_editName;
  QLabel                  *m_locationLab;
  QComboBox               *m_extensionCombo;

};

#else

#include <QtWidgets/qfiledialog.h>
#include "nootkacoreglobal.h"


/**
 * Provides methods to get open/save file name,
 * but it reverts default system font size for those dialogues
 * which is changed when Nootka launches to adjust to real screen size
 * when there is no High DPI scaling
 */
class NOOTKACORE_EXPORT TfileDialog
{

public:
  static QString getOpenFileName(const QString &caption = QString(), const QString &dir = QString(),
                          const QString &filter = QString(), QString *selectedFilter = nullptr, QFileDialog::Options options = 0);
  static QString getSaveFileName(const QString &caption = QString(), const QString &dir = QString(),
                          const QString &filter = QString(), QString *selectedFilter = nullptr, QFileDialog::Options options = 0);
};

#endif

#endif // TFILEDIALOG_H
