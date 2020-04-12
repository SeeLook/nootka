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

#include "tfiledialog.h"
#include "tmtr.h"

#if defined (Q_OS_ANDROID)
#include <qtr.h>
#include <tpath.h>
#include <Android/tandroid.h>
#include <QtWidgets/QtWidgets>


class TiconProvider : public QFileIconProvider
{

public:

  TiconProvider(QFileIconProvider *realProv) :
    QFileIconProvider(),
    m_realProvider(realProv)
  {
  }

  virtual QIcon icon(const QFileInfo& info) const {
    if (info.isDir())
      return QIcon(QLatin1String(":/mobile/dir.png"));
    else if (info.suffix().contains(QLatin1String("nel")))
      return QIcon(Tpath::img("nootka-level"));
    else if (info.suffix().contains(QLatin1String("noo")))
      return QIcon(Tpath::img("nootka-exam"));
    else if (info.suffix().contains(QLatin1String("xml"))) // either *.xml or *.musicxml
      return QIcon(Tpath::img("melody"));
    else
      return m_realProvider->icon(info);
  }

  virtual QIcon icon(IconType type) const {
    switch (type) {
      case Folder:
        return qApp->style()->standardIcon(QStyle::SP_DirIcon);
      default:
        return qApp->style()->standardIcon(QStyle::SP_FileIcon);
    }
  }

private:
  QFileIconProvider           *m_realProvider;
};

//#################################################################################################
//###################     class   TnewDirMessage       ############################################
//#################################################################################################
/**
 * Subclass of QDialog to get name of new directory
 */
class TnewDirMessage : public QDialog
{

public:
  explicit TnewDirMessage(QWidget* parent = nullptr) :
    QDialog(parent)
  {
    auto label = new QLabel(qTR("QFileDialog", "Create New Folder"), this);
    m_edit = new QLineEdit(this);
    m_edit->setPlaceholderText(qTR("QFileSystemModel", "Name"));
    m_edit->setMinimumWidth(qMin<int>(Tmtr::longScreenSide() / 3, fontMetrics().width(QStringLiteral("w")) * 20));

    QSize iconS(Tmtr::fingerPixels() * 0.7, Tmtr::fingerPixels() * 0.7);
    m_createButt = new QPushButton(QIcon(QLatin1String(":/mobile/newDir.png")),
                               qTR("QFileDialog", "&New Folder").replace(QLatin1String("&"), QString()),
                               this);
      m_createButt->setIconSize(iconS);
      m_createButt->setDisabled(true);
      m_createButt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
      m_createButt->setDefault(true);
    auto cancelButt = new QPushButton(style()->standardIcon(QStyle::SP_DialogCancelButton),
                                  qTR("QPlatformTheme", "Cancel"),
                                  this);
      cancelButt->setIconSize(iconS);
      cancelButt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto lay = new QVBoxLayout;
      lay->addWidget(label);
      lay->addWidget(m_edit);
      lay->addSpacing(Tmtr::fingerPixels() / 2);
      auto buttonLay = new QHBoxLayout;
        buttonLay->addWidget(m_createButt);
        buttonLay->addWidget(cancelButt);
        buttonLay->setContentsMargins(0, 0, 0, 0);
      lay->addLayout(buttonLay);
      lay->setContentsMargins(0, 0, 0, 0);
    setLayout(lay);

    connect(m_createButt, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButt, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_edit, &QLineEdit::textChanged, this, &TnewDirMessage::textChangedSlot);
  }

  QString getName() { return m_edit->text(); }

  static QString dirName(QWidget* parent = 0) {
    TnewDirMessage newDir(parent);
    if (newDir.exec() == Accepted && !newDir.getName().isEmpty())
      return newDir.getName();
    else
      return QString();
  }

protected:
  void textChangedSlot(const QString& t) {  m_createButt->setDisabled(t.isEmpty()); }

private:
  QLineEdit     *m_edit;
  QPushButton   *m_createButt;
};



/* static */
QString TfileDialog::getOpenFileName(const QString& directory, const QString& filter) {
  TfileDialog openDialog(directory, filter, TfileDialog::e_acceptOpen);
  if (openDialog.exec() == QFileDialog::Accepted)
    return openDialog.selectedFile();
  else
    return QString();
}


QString TfileDialog::getSaveFileName(const QString& directory, const QString& filter) {
  TfileDialog saveDialog(directory, filter, TfileDialog::e_acceptSave);
  if (saveDialog.exec() == QFileDialog::Accepted)
    return saveDialog.selectedFile();
  else
    return QString();
}


//#################################################################################################
//#################################################################################################
//###################           class TfileDialog      ############################################
//#################################################################################################
//#################################################################################################
TfileDialog::TfileDialog(const QString& directory, const QString& filter, EacceptMode mode) :
  QDialog(nullptr),
  m_acceptMode(mode),
  m_newDirItem(nullptr)
{
  showMaximized();

// left side menu
  m_menu = new QListWidget(this);
  m_menu->setIconSize(QSize(60, 60));
  m_menu->setMaximumWidth(80);
  m_menu->setViewMode(QListView::IconMode);
  m_menu->setMovement(QListView::Static);
  m_menu->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_menu->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  int bSize = qBound<int>(Tmtr::fingerPixels() * 1.1, Tmtr::longScreenSide() / 12, Tmtr::fingerPixels() * 1.6);
  m_menu->setIconSize(QSize(bSize, bSize));
  m_menu->setMaximumWidth(bSize + 10);
  QFont f = font();
  f.setPixelSize(qMin<int>(bSize / 5, fontMetrics().height()));
  m_menu->setFont(f);
  m_menu->setObjectName(QLatin1String("fileMenu")); // revert colors
  m_menu->setStyleSheet(m_menu->styleSheet() + QLatin1String(" QListWidget#fileMenu { background: palette(base); color: palette(text); }"));
  QScroller::grabGesture(m_menu->viewport(), QScroller::LeftMouseButtonGesture);

  QStringList filters = filter.split(QLatin1String("|"));

  QIcon fileIcon;
  if (filters.size() == 1) { // if filter has only one entry - check is it *.nel or *.noo and use its icon
    if (filters.first().contains(QLatin1String("nel")))
      fileIcon = QIcon(Tpath::img("nootka-level"));
    else if (filters.first().contains(QLatin1String("noo")))
      fileIcon = QIcon(Tpath::img("nootka-exam"));
  }
  if (filters.size() != 1 || fileIcon.isNull()) { // if more or none filters or other file types use standard icons
    if (mode == e_acceptSave)
      fileIcon = QIcon(Tpath::img("save"));
    else
      fileIcon = QIcon(Tpath::img("open"));
  }
  m_acceptItem = addMenuItem(fileIcon,
                mode == e_acceptSave ? qTR("QShortcut", "Save") : qTR("QShortcut", "Open"));

  QString space = QLatin1String(" ");
  QString newLine = QLatin1String("\n");
  m_dirUpItem = addMenuItem(QIcon(QLatin1String(":/mobile/dirUp.png")),
                            qTR("QFileDialog", "Parent Directory").replace(space, newLine));
  if (mode == e_acceptSave)
    m_newDirItem = addMenuItem(QIcon(QLatin1String(":/mobile/newDir.png")),
                             qTR("QFileDialog", "&New Folder").replace(space, newLine).replace(QLatin1String("&"), QString()));
  if (Tandroid::getAPIlevelNr() < 19) // display SD card shortcut only below Kitkat
    addMenuItem(QIcon(QLatin1String(":/mobile/card.png")), tr("Memory card").replace(space, newLine));
  m_cancelItem = addMenuItem(QIcon(Tpath::img("exit")), qTR("QShortcut", "Close"));

// upper location label, file name edit, extension combo
  m_locationLab = new QLabel(this);
  m_locationLab->setAlignment(Qt::AlignRight);
  m_locationLab->setFixedWidth(qApp->fontMetrics().height() * 7);

  m_editName = new QLineEdit(this);
  m_editName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  if (mode == e_acceptOpen)
    m_editName->setReadOnly(true);

  m_extensionCombo = new QComboBox(this);
  if (!filter.isEmpty()) {
    int filterCharsLen = 0, longestId = 0;
    for (int f = 0; f < filters.size(); ++f) {
      if (filters[f].length() > filterCharsLen) {
        filterCharsLen = filters[f].length();
        longestId = f;
      }
    }
    m_extensionCombo->setFixedWidth(qApp->fontMetrics().boundingRect(filters[longestId]).width() + qApp->fontMetrics().height() * 1.5);
  }

// file list
  m_list = new QListView(this);
  int is = Tmtr::fingerPixels();
  m_list->setIconSize(QSize(is, is));
  m_list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  m_list->setSelectionBehavior(QAbstractItemView::SelectItems);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  m_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  QScroller::grabGesture(m_list->viewport(), QScroller::LeftMouseButtonGesture);

// Layout
  m_lay = new QBoxLayout(QBoxLayout::LeftToRight);
  m_lay->addWidget(m_menu);
    auto innLay = new QVBoxLayout;
    auto fileLay = new QHBoxLayout;
      fileLay->addWidget(m_locationLab);
      fileLay->addWidget(m_editName);
      fileLay->addWidget(m_extensionCombo);
    innLay->addLayout(fileLay);
    innLay->addWidget(m_list);
  m_lay->addLayout(innLay);
  setLayout(m_lay);
  innLay->setContentsMargins(0, m_lay->contentsMargins().top(), 0, 0);
  m_lay->setContentsMargins(0, 0, m_lay->contentsMargins().right(), 0);

// Determine root path from given parameter 'directory'
  m_fileModel = new QFileSystemModel(this);
  QFileInfo fi(directory);
  // Directory rather exists (checked at the very beginning) - if not it is treated as a file
  if (fi.isDir())
    m_fileModel->setRootPath(directory);
  else {
    m_fileModel->setRootPath(fi.absolutePath());
    m_editName->setText(fi.fileName());
  }
  updateLocationLabel();

// Set filters
  if (filters.size()) {
    for(int i = 0; i < filters.size(); ++i) {
      filters[i].prepend(QLatin1String("."));
      m_extensionCombo->addItem(filters[i]);
      filters[i].prepend(QLatin1String("*"));
    }
    m_extensionCombo->setCurrentIndex(0);
    m_fileModel->setNameFilters(filters);
    m_fileModel->setNameFilterDisables(false);
  }

  m_list->setModel(m_fileModel);
  m_list->setRootIndex(m_fileModel->index(m_fileModel->rootPath()));

  m_fileModel->setIconProvider(new TiconProvider(m_fileModel->iconProvider()));

  connect(m_menu, &QListWidget::itemClicked, this, &TfileDialog::menuClickedSlot);
  connect(m_list, &QListView::clicked, this, &TfileDialog::itemClickedSlot);

// Adjust width of menu list according to widest text
  QTimer::singleShot(100, [=] { m_menu->setFixedWidth(m_menu->sizeHintForColumn(0) + 2 * m_menu->frameWidth()); });

  QString externalStorage = Tandroid::getExternalPath();
  if (mode == e_acceptSave && externalStorage == m_fileModel->rootPath()) {
    // Ask to create Nootka folder but only when file dialog is called with external storage path (first launch)
    if (!externalStorage.isEmpty() && !QFileInfo::exists(externalStorage + QLatin1String("/Nootka")))
      QTimer::singleShot(200, [=] { createNootkaDir(); });
  }
}


TfileDialog::~TfileDialog()
{
  delete m_fileModel;
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TfileDialog::itemClickedSlot(const QModelIndex& index) {
  if (m_fileModel->isDir(index)) {
      m_list->setRootIndex(index);
      m_fileModel->setRootPath(m_fileModel->filePath(index));
      updateLocationLabel();
      if (acceptMode() == e_acceptOpen) {
        m_editName->setText(QString());
        m_selectedFile.clear(); // reset previously selected file
      }
  } else {
      m_selectedFile = m_fileModel->fileInfo(index).absoluteFilePath();
      m_editName->setText(QFileInfo(m_selectedFile).fileName());
  }
}


void TfileDialog::performAction() {
  if (acceptMode() == e_acceptSave) {
    if (m_editName->text().isEmpty()) {
        QMessageBox::warning(this, QString(), qTR("QFileSystemModel", "Invalid filename"));
        return;
    } else {
      m_selectedFile = m_fileModel->rootPath() + QLatin1String("/") + m_editName->text();
      QFileInfo fi(m_selectedFile);
      bool addExt = false; // Add current file extension
      if (!fi.suffix().isEmpty()) {
        if (m_extensionCombo->currentIndex() > -1 && m_extensionCombo->currentText() != m_selectedFile.right(4))
            addExt = true;
      } else
          addExt = true;
      if (addExt) {
        m_selectedFile.append(m_extensionCombo->currentText());
        fi.setFile(m_selectedFile);
      }
      if (fi.exists()) {
        if (QMessageBox::question(this, QString(),
            qTR("QFileDialog", "%1 already exists.\nDo you want to replace it?").arg(m_selectedFile)) != QMessageBox::Ok) {
                m_selectedFile.clear();
                return;
        }
      }
    }
  }
  QDialog::accept();
}


void TfileDialog::menuClickedSlot(QListWidgetItem* item) {
  if (item == m_cancelItem)
    reject();
  else if (item == m_acceptItem)
    performAction();
  else if (item == m_dirUpItem)
    dirUpSlot();
  else if (item == m_newDirItem)
    newDirSlot();
  else if (m_menu->currentRow() == 3) {
    itemClickedSlot(m_fileModel->index(Tandroid::getExternalPath()));
  }
}


void TfileDialog::dirUpSlot() {
  QDir dir(m_fileModel->rootDirectory());
  if (dir.cdUp()) {
    m_fileModel->setRootPath(dir.absolutePath());
    m_list->setRootIndex(m_fileModel->index(dir.absolutePath()));
    updateLocationLabel();
  }
}


void TfileDialog::newDirSlot() {
  createNewDir(TnewDirMessage::dirName(this));
}


void TfileDialog::createNewDir(const QString& newDir) {
  if (!newDir.isEmpty()) {
    auto dirIndex = m_fileModel->mkdir(m_list->rootIndex(), newDir);
    if (dirIndex.isValid())
        itemClickedSlot(dirIndex);
    else
        QMessageBox::warning(this,
                  m_newDirItem->text(), qTR("QFtp", "Creating directory failed:\n%1").arg(newDir));
  }
}


QListWidgetItem* TfileDialog::addMenuItem(const QIcon& icon, const QString& text) {
  auto menuButton = new QListWidgetItem(m_menu);
  menuButton->setIcon(icon);
  if (!text.isEmpty())
    menuButton->setText(text);
  menuButton->setTextAlignment(Qt::AlignHCenter);
  menuButton->setFlags(Qt::ItemIsEnabled | Qt::ItemNeverHasChildren);
  return menuButton;
}


void TfileDialog::updateLocationLabel() {
  m_locationLab->setText(fontMetrics().elidedText(m_fileModel->rootPath() + QLatin1String("/"),
                                                  Qt::ElideMiddle, m_locationLab->width(), Qt::TextShowMnemonic));
}


void TfileDialog::createNootkaDir() {
  if (QMessageBox::question(this, QString(),
      tr("Directory named <b>Nootka</b> will be created in<br>%1<br>Application files will be written there.").arg(m_fileModel->rootPath()))
      == QMessageBox::Yes)
      createNewDir(QLatin1String("Nootka"));
}

#else


QString TfileDialog::getOpenFileName(const QString& caption, const QString& dir, const QString& filter, QString* selectedFilter, QFileDialog::Options options) {
  QString fN;
  auto f = qApp->font();
  qApp->setFont(Tmtr::systemFont);
  fN = QFileDialog::getOpenFileName(nullptr, caption, dir, filter, selectedFilter, options);
  qApp->setFont(f);
  return fN;
}


QString TfileDialog::getSaveFileName(const QString& caption, const QString& dir, const QString& filter, QString* selectedFilter, QFileDialog::Options options) {
  QString fN;
  auto f = qApp->font();
  qApp->setFont(Tmtr::systemFont);
  fN = QFileDialog::getSaveFileName(nullptr, caption, dir, filter, selectedFilter, options);
  qApp->setFont(f);
  return fN;
}



#endif

