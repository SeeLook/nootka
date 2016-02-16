/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                  				   *
 *   seelook@gmail.com        						                                 *
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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#ifndef TSETTINGSDIALOGBASE_H
#define TSETTINGSDIALOGBASE_H

#include <QDialog>
#include <nootkacoreglobal.h>

class QDialogButtonBox;
class QAbstractScrollArea;
class TroundedLabel;
class QListWidget;
class QStackedLayout;
class QMenu;
#if defined (Q_OS_ANDROID)
class TmenuWidget;
class TtouchMenu;
#endif

/**
 * This is base class for settings dialogues.
 * It contains navigation list to switch widgets (pages) on
 * QStackedLayout widget, and @class TroundedLabel @p hint
 * that shows status tips captured by handling @p event()
 * It has hidden "default" button - @p defaultBut
*/

class NOOTKACORE_EXPORT TsettingsDialogBase : public QDialog
{
  Q_OBJECT

public:
  explicit TsettingsDialogBase(QWidget *parent = 0);

      /** Adds icon with label text to config pages list (@p navList - left pane) */
  void addItem(const QString& label, const QString& iconPath);

      /** Adds widget (page) to stacked layout (@p stackLayout - right pane) */
  void addPage(QWidget* page);

      /** Open online documentation (http://nootka.sourceforge.net/index.php?C=doc) */
  QString helpButtonTipText() { return tr("Open online documentation") + "<br>(http://nootka.sourceforge.net/index.php?C=doc)"; }

  static bool touchEnabled(); /** @p TRUE when touch is enabled */

protected:
  bool event(QEvent *event);

  QListWidget  					   	*navList;
  QStackedLayout  					*stackLayout;
  QPushButton     					*cancelBut, *okBut, *defaultBut;
  TroundedLabel          		*hint;
  QDialogButtonBox					*buttonBox; /** Bottom layout with buttons */
#if defined (Q_OS_ANDROID)
  TmenuWidget               *menuButton;
#endif

#if !defined (Q_OS_ANDROID)
    /** HACK: Avoiding using scroll bars when there is enough space on a desktop
     * All settings pages rather inherit from @class TscrollArea and have scrolls.
     * It works out of a box on mobile, where settings dialog is maximized.
     * All derivative classes on desktop require to set
     * @p setWidesttPage() and @p setHighestPage() (hard-coded)
     * Then call @p hackSize() at the end of derivative  constructor
     * with small delay (10ms).
     * It switches to those pages, obtains maximum of scroll-bar
     * and adds it to dialog window size, or sets window maximized if no enough space.
     */
  void hackSize();
  void setWidesttPage(QAbstractScrollArea* page) { m_wiPage = page; }
  void setHighestPage(QAbstractScrollArea* page) { m_hiPage = page; }
#endif

//   void fitSize();
#if defined (Q_OS_ANDROID)
  virtual void tapMenu(); /** Displays menu created from @p navList context and @p buttonBox contex */
#endif
  void convertStatusTips(); /** Transforms all status tip texts into tool tips. */

  void openHelpLink(const QString& hash); /** calls QDesktopServices::openUrl with Nootka site doc at given @p hash */

      /** Creates action with given button icon and text, emulating button click. */
  QAction* actionFromButton(QPushButton* b, QMenu* parentMenu);

private:
  QAbstractScrollArea       *m_hiPage, *m_wiPage;

};
#endif // TSETTINGSDIALOGBASE_H
