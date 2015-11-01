

#include "tfiledialog.h"
#include "tmtr.h"
#include <tpath.h>
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
      return qApp->style()->standardIcon(QStyle::SP_DirIcon);
    else if (info.suffix().contains(QLatin1String("nel")))
      return QIcon(Tpath::img("nootka-level"));
    else if (info.suffix().contains(QLatin1String("noo")))
      return QIcon(Tpath::img("nootka-exam"));
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


class TnewDirMessage : public QDialog
{

public:
  explicit TnewDirMessage(QWidget* parent = 0) :
    QDialog(parent)
  {
    auto label = new QLabel(QApplication::translate("QFileDialog", "Create New Folder"), this);
    m_edit = new QLineEdit(this);
    m_edit->setPlaceholderText(QApplication::translate("QFileDialog", "Directory:"));
    auto box = new QDialogButtonBox(this);
    auto okButt = box->addButton(QDialogButtonBox::Ok);
    okButt->setIcon(style()->standardIcon(QStyle::QStyle::SP_FileDialogNewFolder));
    auto cancelButt = box->addButton(QDialogButtonBox::Cancel);
    cancelButt->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));

    auto lay = new QVBoxLayout;
    lay->addWidget(label);
    lay->addWidget(m_edit);
    lay->addWidget(box);
    setLayout(lay);

    connect(box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(box, &QDialogButtonBox::rejected, this, &QDialog::reject);
  }

  QString getName() { return m_edit->text(); }

  static QString dirName(QWidget* parent = 0) {
    TnewDirMessage newDir(parent);
    if (newDir.exec() == Accepted && !newDir.getName().isEmpty())
      return newDir.getName();
    else
      return QString();
  }

private:
  QLineEdit           *m_edit;
};



/* static */
QString TfileDialog::getOpenFileName(QWidget* parent, const QString& directory, const QString& filter) {
  TfileDialog openDialog(parent, directory, filter, TfileDialog::e_acceptOpen);
  if (openDialog.exec() == QFileDialog::Accepted)
    return openDialog.selectedFile();
  else
    return QString();
}


QString TfileDialog::getSaveFileName(QWidget* parent, const QString& directory, const QString& filter) {
  TfileDialog saveDialog(parent, directory, filter, TfileDialog::e_acceptSave);
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
TfileDialog::TfileDialog(QWidget *parent, const QString& directory, const QString& filter, EacceptMode mode) :
  QDialog(parent),
  m_acceptMode(mode)
{
  showMaximized();

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
//   m_menu->setObjectName("fileMenu"); // revert colors
//   m_menu->setStyleSheet(m_menu->styleSheet() + " QListWidget#fileMenu { background: palette(text); color: palette(base); }");
  QScroller::grabGesture(m_menu->viewport(), QScroller::LeftMouseButtonGesture);

  if (mode == e_acceptSave)
    m_acceptItem = addMenuItem(style()->standardIcon(QStyle::SP_DialogSaveButton), QApplication::translate("QShortcut", "Save"));
  else
    m_acceptItem = addMenuItem(style()->standardIcon(QStyle::SP_DialogOpenButton), QApplication::translate("QShortcut", "Open"));

  m_dirUpItem = addMenuItem(style()->standardIcon(QStyle::SP_ArrowUp));
  m_newDirItem = addMenuItem(style()->standardIcon(QStyle::QStyle::SP_FileDialogNewFolder));
  m_cancelItem = addMenuItem(style()->standardIcon(QStyle::QStyle::SP_DialogCloseButton), QApplication::translate("QShortcut", "Close"));

  m_locationLab = new QLabel(this);
  m_locationLab->setAlignment(Qt::AlignRight);
  m_locationLab->setFixedWidth(Tmtr::longScreenSide() / 3);

  m_editName = new QLineEdit(this);
  m_editName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);;
  if (mode == e_acceptOpen)
    m_editName->setReadOnly(true);

  m_extensionCombo = new QComboBox(this);
  m_list = new QListView(this);
  int is = style()->pixelMetric(QStyle::PM_SmallIconSize) * 1.1;
  m_list->setIconSize(QSize(is, is));
  m_list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  m_list->setSelectionBehavior(QAbstractItemView::SelectItems);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  m_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  QScroller::grabGesture(m_list->viewport(), QScroller::LeftMouseButtonGesture);

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
  m_lay->setContentsMargins(0, m_lay->contentsMargins().top(), m_lay->contentsMargins().right(), 0);

  m_fileModel = new QFileSystemModel(this);

  QFileInfo fi(directory);
  if (fi.isDir())
    m_fileModel->setRootPath(directory);
  else {
    m_fileModel->setRootPath(fi.absolutePath());
    m_editName->setText(fi.fileName());
  }
  updateLocationLabel();

  QStringList filters = filter.split(QLatin1String("|"));
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
        QMessageBox::warning(this, QString(), QApplication::translate("QFileSystemModel", "Invalid filename"));
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
            QApplication::translate("QFileDialog", "%1 already exists.\nDo you want to replace it?").arg(m_selectedFile)) != QMessageBox::Ok) {
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
  QString newDir = TnewDirMessage::dirName(this);
  if (!newDir.isEmpty())
    m_fileModel->mkdir(m_list->rootIndex(), newDir);
}


QListWidgetItem* TfileDialog::addMenuItem(const QIcon& icon, const QString& text) {
  QListWidgetItem *menuButton = new QListWidgetItem(m_menu);
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


