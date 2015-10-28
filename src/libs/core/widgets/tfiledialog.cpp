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

#include "tfiledialog.h"
#include <QtWidgets/QtWidgets>


/* static */
QString TfileDialog::getSaveFileName(QWidget* parent, const QString& caption, const QString& directory, const QString& filter) {
  TfileDialog saveDialog(parent, caption, directory, filter);
  saveDialog.setAcceptMode(AcceptSave);
  if (saveDialog.exec() == QDialog::Accepted)
    return saveDialog.selectedFiles().first();
  else
    return QString();
}


QString TfileDialog::getOpenFileName(QWidget* parent, const QString& caption, const QString& directory, const QString& filter) {
  TfileDialog openDialog(parent, caption, directory, filter);
  openDialog.setAcceptMode(AcceptOpen);
  if (openDialog.exec() == QDialog::Accepted)
    return openDialog.selectedFiles().first();
  else
    return QString();
}



TfileDialog::TfileDialog(QWidget* parent, const QString& caption, const QString& directory, const QString& filter) :
  QFileDialog(parent, caption, directory, filter)
{
//   setFileMode(QFileDialog::AnyFile);
//   setOption(QFileDialog::DontUseNativeDialog);
//   setOption(QFileDialog::HideNameFilterDetails);
//   setOption(QFileDialog::DontUseCustomDirectoryIcons);

#if defined (Q_OS_ANDROID)
  showMaximized();
  layout()->setContentsMargins(0, 0, 0, 0);
#endif

  foreach(QAbstractScrollArea *a, findChildren<QAbstractScrollArea*>()) {
//     qDebug() << "scrolls" << a->objectName();
    a->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    a->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      QScroller::grabGesture(a->viewport(), QScroller::LeftMouseButtonGesture);
  }

  foreach(QWidget *w, findChildren<QWidget*>()) {
//     qDebug() << "widgets" << w->objectName();
    if (w->objectName() == QLatin1String("sidebar"))
      w->hide();
//     else if (w->objectName() == QLatin1String("lookInLabel"))
//       w->hide();
//     else if (w->objectName() == QLatin1String("lookInCombo"))
//       w->hide();
    else if (w->objectName() == QLatin1String("listModeButton"))
      w->hide();
    else if (w->objectName() == QLatin1String("detailModeButton"))
      w->hide();
    else if (w->objectName() == QLatin1String("fileTypeCombo"))
      w->hide();
    else if (w->objectName() == QLatin1String("fileTypeLabel"))
      w->hide();
    else if (w->objectName() == QLatin1String("fileTypeLabel"))
      w->hide();
  }

  foreach(QPushButton *b, findChildren<QPushButton*>()) {
//     qDebug() << "buttons" << b->objectName();
    QSize iSize(style()->pixelMetric(QStyle::PM_SmallIconSize), style()->pixelMetric(QStyle::PM_SmallIconSize));
    if (b->objectName() == QLatin1String("backButton"))
      b->setIconSize(iSize);
    else if (b->objectName() == QLatin1String("forwardButton"))
      b->setIconSize(iSize);
    else if (b->objectName() == QLatin1String("toParentButton"))
      b->setIconSize(iSize);
    else if (b->objectName() == QLatin1String("newFolderButton"))
      b->setIconSize(iSize);
  }

  foreach(QListView *l, findChildren<QListView*>()) {
//     qDebug() << "lists" << l->objectName();
//     if (l->objectName() == QLatin1String("listView")) {
      l->setFlow(QListView::TopToBottom);
      l->setDragDropMode(QAbstractItemView::NoDragDrop);
      l->setDragEnabled(false);
//       l->setIconSize(QSize(48, 48));
//       auto model = qobject_cast<QFileSystemModel*>(l->model());
//       qDebug() << l->model()->columnCount() << model->iconProvider();
//       model->setIconProvider(saveDialog.iconProvider());
//       model->removeColumns(1, 10);
//     }
  }

}
