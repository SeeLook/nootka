/***************************************************************************
 *   Copyright (C) 2015-2016 by Tomasz Bojczuk                             *
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
  enum EacceptMode { e_acceptSave, e_acceptOpen }; /**< File dialog type: for opening or saving */
  Q_ENUM(EacceptMode)

  explicit TfileDialog(QWidget* parent, const QString& directory, const QString& filter, EacceptMode mode);
  ~TfileDialog();

  static QString getSaveFileName(QWidget* parent = 0, const QString& directory = QString(), const QString& filter = QString());
  static QString getOpenFileName(QWidget* parent = 0, const QString& directory = QString(), const QString& filter = QString());

  QString selectedFile() const { return m_selectedFile; }

  void setDirection(QBoxLayout::Direction dir) { m_lay->setDirection(dir); } /** Left to right by default */
  EacceptMode acceptMode() { return m_acceptMode; }


protected:
  void itemClickedSlot(const QModelIndex& index);
  void menuClickedSlot(QListWidgetItem* item);
  void dirUpSlot();
  void newDirSlot();
  void performAction(); /**< Open or save */
  void createNewDir(const QString& newDir); /**< Creates new directory with name @p newDir in current model node */

    /**< Displays message about possibility of creating Nootka folder on external card (if exists - or internal then).
     * If user agreed - creates the directory.  */
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

#endif // TFILEDIALOG_H
