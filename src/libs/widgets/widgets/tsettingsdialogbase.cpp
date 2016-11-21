/***************************************************************************
 *   Copyright (C) 2011-2016 by Tomasz Bojczuk                  				   *
 *   seelook@gmail.com   					      	                                 *
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


#include "tsettingsdialogbase.h"
#include <widgets/troundedlabel.h>
#include <touch/ttouchproxy.h>
#if defined (Q_OS_ANDROID)
  #include "tmtr.h"
  #include <tmenuwidget.h>
  #include <graphics/tdropshadoweffect.h>
  #include <ttouchmenu.h>
  #include <tinitcorelib.h>
#endif
#include <QtWidgets/QtWidgets>


#if defined (Q_OS_WIN) // more time under Win to make space for icons
  #define NAVLIST_REFRESH_TIME (500)
#else
  #define NAVLIST_REFRESH_TIME (100)
#endif

/* static */
bool TsettingsDialogBase::touchEnabled() { return TtouchProxy::touchEnabled(); }


TsettingsDialogBase::TsettingsDialogBase(QWidget *parent) :
  QDialog(parent),
  m_hiPage(0), m_wiPage(0)
{
    navList = new QListWidget(this);
#if defined (Q_OS_ANDROID)
    int bSize = qBound(qRound(Tmtr::fingerPixels() * 1.1), Tmtr::longScreenSide() / 12, qRound(Tmtr::fingerPixels() * 1.6));
    navList->setIconSize(QSize(bSize, bSize));
    navList->setMaximumWidth(bSize + 10);
    QFont f = font();
    f.setPixelSize(qMin<int>(bSize / 5, fontMetrics().height()));
    navList->setFont(f);
    navList->setObjectName(QStringLiteral("navList")); // revert colors of navigation list
    navList->setStyleSheet(navList->styleSheet() + QStringLiteral(" QListWidget#navList { background: palette(text); color: palette(base); }"));
    navList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#else
    int w = fontMetrics().boundingRect(QStringLiteral("Instrument")).width();
    navList->setIconSize(QSize(w, w));
    navList->setFixedWidth(w * 1.2);
#endif
    navList->setViewMode(QListView::IconMode);
		navList->setMovement(QListView::Static);
    navList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QScroller::grabGesture(navList->viewport(), QScroller::LeftMouseButtonGesture);
    auto scroller = QScroller::scroller(navList->viewport());
    QScrollerProperties prop = scroller->scrollerProperties();
    QVariant overshootPolicy = QVariant::fromValue<QScrollerProperties::OvershootPolicy>(QScrollerProperties::OvershootAlwaysOff);
//     prop.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, overshootPolicy);
    prop.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, overshootPolicy);
    scroller->setScrollerProperties(prop);

    stackLayout = new QStackedLayout;

    hint = new TroundedLabel(this);
    hint->setFixedHeight(qRound(fontMetrics().boundingRect("A").height() * 3.5));
		hint->setMinimumWidth(fontMetrics().boundingRect("w").width() * 70);
    hint->setWordWrap(true);

    buttonBox = new QDialogButtonBox(Qt::Horizontal, this);

    QVBoxLayout *mainLay = new QVBoxLayout;
    QHBoxLayout *upLay = new QHBoxLayout;
      QVBoxLayout *navLay = new QVBoxLayout;
      navLay->setContentsMargins(0, 0, 0, 0);
      navLay->addWidget(navList);
      upLay->addLayout(navLay);
      QVBoxLayout *aLay = new QVBoxLayout;
        aLay->addLayout(stackLayout);
        aLay->addWidget(hint);
      upLay->addLayout(aLay);
      mainLay->addLayout(upLay);
      mainLay->addWidget(buttonBox);
    setLayout(mainLay);

    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    if (touchEnabled()) {
      setAttribute(Qt::WA_AcceptTouchEvents);
    }
#if defined (Q_OS_ANDROID)
    mainLay->setContentsMargins(0, 0, 0, 0);
    aLay->setContentsMargins(0, 0, 0, 0);
    stackLayout->setContentsMargins(0, 0, 0, 0);
    upLay->setContentsMargins(0, 0, 0, 0);
    navLay->setContentsMargins(0, 0, 0, 0);
    buttonBox->hide();
    hint->hide();
    showMaximized();
    menuButton = new TmenuWidget(this);
    navLay->setSpacing(0);
    navLay->addSpacing(1);
    navLay->addWidget(menuButton);
    navLay->addSpacing(1);
    connect(menuButton, &TmenuWidget::clicked, this, &TsettingsDialogBase::tapMenu);
    QTimer::singleShot(100, this, [this] { navList->setFixedWidth(navList->sizeHintForColumn(0) + 2 * navList->frameWidth());
                                           menuButton->setFixedSize(navList->width(), qRound(Tmtr::fingerPixels() * 0.7)); } );
#endif
}


void TsettingsDialogBase::addItem(const QString& label, const QString& iconPath) {
  QListWidgetItem *configButton = new QListWidgetItem(navList);
  configButton->setIcon(QIcon(iconPath));
  configButton->setText(label);
  configButton->setTextAlignment(Qt::AlignHCenter);
  configButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}


void TsettingsDialogBase::addPage(QWidget* page) {
  stackLayout->addWidget(page);
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################
#if !defined (Q_OS_ANDROID)
void TsettingsDialogBase::hackSize() {
  int currIndex = stackLayout->currentIndex();
  if (m_wiPage) {
    stackLayout->setCurrentWidget(m_wiPage);
    setMinimumWidth(qMin(width() + m_wiPage->horizontalScrollBar()->maximum(), qApp->desktop()->availableGeometry().width()));
  }
  if (m_hiPage) {
    stackLayout->setCurrentWidget(m_hiPage);
    if (m_hiPage->verticalScrollBar()->maximum()) {
      if (height() + m_hiPage->verticalScrollBar()->maximum() > qApp->desktop()->availableGeometry().height()) {
        showMaximized();
        convertStatusTips();
        hint->hide();
      } else {
        setMinimumHeight(height() + m_hiPage->verticalScrollBar()->maximum());
      }
    }
  }
  stackLayout->setCurrentIndex(currIndex);
}


/**
 * Adjust @p navList width according to vertical scroll bar visibility
 * to keep icons and text always fully visible
 */
void TsettingsDialogBase::resizeEvent(QResizeEvent* event) {
  QDialog::resizeEvent(event);
  QTimer::singleShot(NAVLIST_REFRESH_TIME, this, [this] {
    navList->setFixedWidth(navList->sizeHintForColumn(0) + 2 * navList->frameWidth() +
    (navList->verticalScrollBar()->isVisible() ? navList->verticalScrollBar()->width() : 0));
  });
}

#endif


/* TODO: check it under Win then remove
void TsettingsDialogBase::fitSize() {
  if (qApp->desktop()->availableGeometry().height() <= 600) {
#if defined (Q_OS_WIN)
    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);
    setMinimumSize(qApp->desktop()->availableGeometry().width() - 100, qApp->desktop()->availableGeometry().height() - 70);
#endif
    useScrollArea();
    convertStatusTips();
    connect(stackLayout, SIGNAL(currentChanged(int)), this, SLOT(convertStatusTips()));
  }
  navList->setFixedWidth(navList->sizeHintForColumn(0) + 2 * navList->frameWidth() +
          (navList->verticalScrollBar()->isVisible() ? navList->verticalScrollBar()->width() : 0));
}
*/

void TsettingsDialogBase::convertStatusTips() {
  QList<QWidget*> allWidgets = findChildren<QWidget*>();
  foreach(QWidget *w, allWidgets) {
    if (!w->statusTip().isEmpty()) {
      w->setToolTip(w->statusTip());
      w->setStatusTip(QString());
    }
  }
}


QAction* TsettingsDialogBase::actionFromButton(QPushButton* b, QMenu* parentMenu) {
  return new QAction(b->icon(), b->text(), parentMenu);
}


#if defined (Q_OS_ANDROID)
void TsettingsDialogBase::tapMenu() {
  m_touchMenu = new TtouchMenu(this);
  m_touchMenu->setGraphicsEffect(new TdropShadowEffect());
  for (int i = 0; i < buttonBox->buttons().size(); ++i) {
    QAction *buttonAction = new QAction(buttonBox->buttons()[i]->icon(), buttonBox->buttons()[i]->text(), m_touchMenu);
    buttonAction->setData((i));
    m_touchMenu->addAction(buttonAction);
  }
  QAction *menuAction = m_touchMenu->exec(QPoint(navList->width(), height() -m_touchMenu->sizeHint().height()), QPoint(navList->width(), height()));
  int actionNumber = menuAction ? menuAction->data().toInt() : -1;
  delete m_touchMenu; // delete menu before performing its action
  if (actionNumber != -1)
    buttonBox->buttons()[actionNumber]->click();
}


void TsettingsDialogBase::closeEvent(QCloseEvent *event) {
  if (m_touchMenu) {
      event->ignore();
      m_touchMenu->close();
  } else
      event->accept();
}


void TsettingsDialogBase::markChanges(QWidget *container) {
  for(QCheckBox *chB : container->findChildren<QCheckBox*>())
    connect(chB, &QCheckBox::clicked, menuButton, &TmenuWidget::animate);
  for(QGroupBox *grB : container->findChildren<QGroupBox*>())
    connect(grB, &QGroupBox::clicked, menuButton, &TmenuWidget::animate);
  for(QPushButton *butt : container->findChildren<QPushButton*>())
    connect(butt, &QPushButton::clicked, menuButton, &TmenuWidget::animate);
  for(QRadioButton *radio : container->findChildren<QRadioButton*>())
    connect(radio, &QCheckBox::clicked, menuButton, &TmenuWidget::animate);
  for(QComboBox *combo : container->findChildren<QComboBox*>())
    connect(combo, SIGNAL(currentIndexChanged(int)), menuButton, SLOT(animate()));
  for(QSpinBox *spin : container->findChildren<QSpinBox*>())
    connect(spin, SIGNAL(valueChanged(int)), menuButton, SLOT(animate()));
}

#endif


void TsettingsDialogBase::openHelpLink(const QString& hash) {
  QDesktopServices::openUrl(QUrl(QString("http://nootka.sourceforge.net/index.php?L=%1&C=doc#" + hash).
    arg(QString(qgetenv("LANG")).left(2).toLower()), QUrl::TolerantMode));
}


bool TsettingsDialogBase::event(QEvent *event) {
#if defined (Q_OS_ANDROID)
  if (event->type() == QEvent::KeyRelease) {
    QKeyEvent *ke = static_cast<QKeyEvent*>(event);
    if (ke->key() == Qt::Key_Menu)
      QTimer::singleShot(10, this, [this]{ tapMenu(); });
  }
#else
  if (event->type() == QEvent::StatusTip) {
      QStatusTipEvent *se = static_cast<QStatusTipEvent *>(event);
      hint->setText(QLatin1String("<center>") + se->tip() + QLatin1String("</center>"));
  }
#endif
  return QDialog::event(event);
}



