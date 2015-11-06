/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                             *
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


#include "tlevelselector.h"
#include "tlevelsdefs.h"
#include <graphics/tnotepixmap.h>
#include <widgets/tquestionaswdg.h>
#include <level/tlevelpreview.h>
#include <texamparams.h>
#include <music/ttune.h>
#if defined (Q_OS_ANDROID)
  #include <widgets/tfiledialog.h>
#else
  #include "tfixleveldialog.h"
#endif
#include <tinitcorelib.h>
#include <QtWidgets/QtWidgets>



/*static*/

QString TlevelSelector::checkLevel(Tlevel& l) {
	QString warringText;
	if (Tcore::gl()->instrument == e_noInstrument && l.instrument != e_noInstrument)
					warringText = tr("Level is not suitable for current instrument type");
	else if (l.canBeGuitar() || (l.instrument != e_noInstrument && l.canBeSound())) {
    if (l.hiFret > Tcore::gl()->GfretsNumber || Tcore::gl()->Gtune()->stringNr() < 3 ||
        l.loNote.chromatic() < Tcore::gl()->loString().chromatic() ||
			  l.hiNote.chromatic() > Tcore::gl()->hiNote().chromatic())
					warringText = tr("Level is not suitable for current tuning and/or fret number");
  }
	return warringText;
}

/*end static*/


//##########################################################################################################
//########################################## CONSTRUCTOR ###################################################
//##########################################################################################################
TlevelSelector::TlevelSelector(QWidget *parent) :
	QWidget(parent)
{
	QLabel *levLab = new QLabel(levelFilterTxt() + QLatin1String(":"), this);
	m_levelsListWdg = new QListWidget(this);
		m_levelsListWdg->setMouseTracking(true);
    m_levelsListWdg->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
#if defined (Q_OS_ANDROID)
		m_levelsListWdg->setMinimumWidth(fontMetrics().boundingRect(QStringLiteral("W")).width() * 15);
    m_levelsListWdg->setMaximumWidth(qApp->desktop()->availableGeometry().width() * 0.45);
    m_levelsListWdg->setMaximumHeight(qApp->desktop()->availableGeometry().height() * 0.7);
    m_levelsListWdg->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_levelsListWdg->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#else
    m_levelsListWdg->setMinimumWidth(fontMetrics().boundingRect(QStringLiteral("W")).width() * 20);
    m_levelsListWdg->setMinimumHeight(fontMetrics().height() * 8); // to see four items at least
#endif
		m_levelsListWdg->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	m_loadBut = new QPushButton(tr("Load"), this);
		m_loadBut->setStatusTip(tr("Load level from file"));
		m_loadBut->setIcon(QIcon(Tcore::gl()->path + "picts/nootka-level.png"));
	m_removeButt = new QPushButton(tr("Remove"), this);
		m_removeButt->setStatusTip(TremoveLevel::removeTxt());
		m_removeButt->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
		m_removeButt->setDisabled(true);
#if !defined (Q_OS_ANDROID)
    m_removeButt->setIconSize(QSize(22, 22));
    m_loadBut->setIconSize(QSize(22, 22));
#endif

	m_levelPreview = new TlevelPreview(this);
#if !defined (Q_OS_ANDROID)
	m_levelPreview->setFixInstrEnabled(true);
#endif
	m_levelPreview->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	QHBoxLayout *mainLay = new QHBoxLayout;
	QVBoxLayout *levLay = new QVBoxLayout;
    levLay->setContentsMargins(0, 0, 0, 0);
		levLay->addWidget(levLab);
		levLay->addWidget(m_levelsListWdg);
		QHBoxLayout *buttLay = new QHBoxLayout;
			buttLay->addWidget(m_loadBut);
			buttLay->addWidget(m_removeButt);
		levLay->addLayout(buttLay);
	mainLay->addLayout(levLay);
	mainLay->addWidget(m_levelPreview);
	setLayout(mainLay);

	m_fakeLevel.name.clear();
	m_fakeLevel.desc.clear();
	findLevels();

	connect(m_levelsListWdg, SIGNAL(currentRowChanged(int)), this, SLOT(levelSelected(int)));
	connect(m_loadBut, SIGNAL(clicked()), this, SLOT(loadFromFilePrivate()));
	connect(m_levelPreview, SIGNAL(instrumentLevelToFix()), this, SLOT(fixInstrumentSlot()));
	connect(m_removeButt, SIGNAL(clicked()), this, SLOT(removeLevelSlot()));
  QScroller::grabGesture(m_levelsListWdg->viewport(), QScroller::LeftMouseButtonGesture);
}

TlevelSelector::~TlevelSelector() {
	updateRecentLevels();
}


//##########################################################################################################
//########################################## PUBLIC #######################################################
//##########################################################################################################

void TlevelSelector::levelSelected(int id) {
	if (id >= 0 && id < m_levelsListWdg->count()) {
    m_levelPreview->setLevel(m_levels[id].level);
		if (m_levels[id].file.isEmpty())
				m_removeButt->setDisabled(true);
		else
				m_removeButt->setDisabled(false);
    emit levelChanged(m_levels[id].level);
	} else // clear level preview if none
			m_levelPreview->setLevel();
}


void TlevelSelector::findLevels() {
  Tlevel lev = Tlevel();
// from predefined list
  QList<Tlevel> llist;
  getExampleLevels(llist);
  for (int i = 0; i < llist.size(); i++) {
      addLevel(llist[i]);
      m_levels.last().suitable = isSuitable(llist[i]);
  }
// from constructor (Master of Masters)
  addLevel(lev);
  m_levels.last().suitable = true;
// from setting file - recent load/saved levels
  QStringList recentLevels = Tcore::gl()->config->value("recentLevels").toStringList();
  for (int i = recentLevels.size()-1; i >= 0; i--) {
      QFile file(recentLevels[i]);
      if (file.exists()) {
          Tlevel level = getLevelFromFile(file);
          if (!level.name.isEmpty()) {
              addLevel(level, file.fileName());
              m_levels.last().suitable = isSuitable(level);
          } else
              recentLevels.removeAt(i);
      } else
          recentLevels.removeAt(i);
  }
  Tcore::gl()->config->setValue("recentLevels", recentLevels);
}


void TlevelSelector::addLevel(const Tlevel& lev, QString levelFile, bool check) {
  if (check && !levelFile.isEmpty()) {
    int pos = -1;
    for (int i = 0; i < m_levels.size(); i++)
      if (m_levels[i].file == levelFile) // file and level exist
          pos = i;

    QListWidgetItem *it = m_levelsListWdg->takeItem(pos);
    delete it;
    m_levels.removeAt(pos);
  }
  SlevelContener l;
  m_levelsListWdg->addItem(lev.name);
  l.level = lev;
  l.file = levelFile;
  l.item = m_levelsListWdg->item(m_levelsListWdg->count() - 1);
  l.item->setStatusTip(lev.desc);
  QFont nf("nootka", fontMetrics().boundingRect("A").height() * 2);
  nf.setBold(false);
  l.item->setIcon(QIcon(pixFromString(l.level.canBeMelody() ? "m" : "n", nf)));
  if (m_levelsListWdg->count() % 2)
    l.item->setBackground(palette().alternateBase());
  l.suitable = true;
  m_levels << l;
}


bool TlevelSelector::isSuitable(Tlevel &l) {
	QString warringText = checkLevel(l);
	if (!warringText.isEmpty()) {
			m_levels.last().item->setStatusTip("<span style=\"color: red;\">" + warringText + "</span>");
			m_levels.last().item->setForeground(QBrush(Qt::red));
			return false;
	}
	return true;
}


void TlevelSelector::disableNotSuitable() {
	for (int i = 0; i < m_levels.size(); i++)
		if (!m_levels[i].suitable) {
			m_levels[i].item->setFlags(Qt::NoItemFlags);
			m_levels[i].item->setForeground(QBrush(palette().color(QPalette::Disabled, QPalette::Text)));
		}
}


bool TlevelSelector::isSuitable() {
	if (idOfSelected() > -1 )
			return m_levels[idOfSelected()].suitable;
	else
			return false;
}


void TlevelSelector::selectLevel(int id) {
	if (id >= 0 && id < m_levelsListWdg->count())
			m_levelsListWdg->setCurrentRow(id);
}


void TlevelSelector::selectLevel() {
    m_levelsListWdg->setCurrentRow(m_levelsListWdg->count()-1);
}


void TlevelSelector::loadFromFile(QString levelFile) {
  if (levelFile.isEmpty())
#if defined (Q_OS_ANDROID)
    levelFile = TfileDialog::getOpenFileName(this, Tcore::gl()->E->levelsDir, QStringLiteral("nel"));
#else
    levelFile = QFileDialog::getOpenFileName(this, tr("Load exam's level"), Tcore::gl()->E->levelsDir, levelFilterTxt() + " (*.nel)");
#endif
  QFile file(levelFile);
  Tlevel level = getLevelFromFile(file);
  if (!level.name.isEmpty()) {
      Tcore::gl()->E->levelsDir = QFileInfo(levelFile).absoluteDir().absolutePath();
      addLevel(level, levelFile, true);
      if (isSuitable(level))
          selectLevel(); // select the last
      updateRecentLevels();
  }
}


Tlevel& TlevelSelector::getSelectedLevel() {
    if (m_levelsListWdg->currentRow() == -1 ) {
        return m_fakeLevel;
    } else
        return m_levels[m_levelsListWdg->currentRow()].level;
}


void TlevelSelector::updateRecentLevels() {
	QStringList recentLevels;
	for (int i = m_levels.size() - 1; i > 1; i--) {
		if (!m_levels[i].file.isEmpty())
			recentLevels << m_levels[i].file;
	}
	Tcore::gl()->config->setValue("recentLevels", recentLevels);
}


//##########################################################################################################
//########################################## PRIVATE #######################################################
//##########################################################################################################


void TlevelSelector::loadFromFilePrivate() {
    emit levelToLoad();
}


Tlevel TlevelSelector::getLevelFromFile(QFile &file) {
    Tlevel level;
    level.name.clear();;
    if (file.open(QIODevice::ReadOnly)) {
         QDataStream in(&file);
         in.setVersion(QDataStream::Qt_5_2);
         quint32 lv; // level version identifier
         in >> lv;
				 bool wasLevelValid = true;
				 bool wasLevelFile = true;
				 if (Tlevel::levelVersionNr(lv) == 1 || Tlevel::levelVersionNr(lv) == 2) // *.nel with binary data
						wasLevelValid = getLevelFromStream(in, level, lv); // *.nel in XML
				 else if (Tlevel::levelVersionNr(lv) == 3) {
					 Tlevel::EerrorType er;
					 QXmlStreamReader xml(in.device());
					 if (!xml.readNextStartElement()) // open first XML node
						 er = Tlevel::e_noLevelInXml;
					 else 
						er = level.loadFromXml(xml);
					 switch (er) {
						 case Tlevel::e_levelFixed:
								wasLevelValid = false; break;
						 case Tlevel::e_noLevelInXml:
						 case Tlevel::e_otherError:
							 wasLevelFile = false; break;
						 default:
							 break;
					 }
				 } else
							wasLevelFile = false;
				 file.close();
				 if (!wasLevelFile) {
						QMessageBox::critical(this, QString(), tr("File: %1 \n is not Nootka level file!").arg(file.fileName()));
						level.name.clear();
						return level;
				 } else if (!wasLevelValid)
             QMessageBox::warning(0, QString(), tr("Level file\n %1 \n was corrupted and repaired!\n Check please, if its parameters are as expected.").arg(file.fileName()));
    } else {
				if (!file.fileName().isEmpty()) // skip empty file names (ignored by user)
					Tlevel::fileIOerrorMsg(file, this);
    }
    return level;
}


void TlevelSelector::fixInstrumentSlot() {
#if !defined (Q_OS_ANDROID)
	if (fixLevelInstrument(m_levels[m_levelsListWdg->currentRow()].level, m_levels[m_levelsListWdg->currentRow()].file, 
						Tcore::gl()->instrumentToFix, this)) {
			if (!Tlevel::saveToFile(m_levels[m_levelsListWdg->currentRow()].level, m_levels[m_levelsListWdg->currentRow()].file))
					qDebug() << "Failed when writing fixed level to:" << m_levels[m_levelsListWdg->currentRow()].file;
			else
					m_levelPreview->setLevel(m_levels[m_levelsListWdg->currentRow()].level);
	}
#endif
}


void TlevelSelector::removeLevelSlot() {
	if (m_levelsListWdg->currentRow() == -1)
		return;
	QPointer<TremoveLevel> removeDialog = new TremoveLevel(m_levels[idOfSelected()].level.name,	m_levels[idOfSelected()].file, this);
	if (removeDialog->exec() == QDialog::Accepted) {
		int selId = idOfSelected();
		m_levelsListWdg->setCurrentRow(-1);
		m_levels.removeAt(selId);
		QListWidgetItem *toTrash = m_levelsListWdg->takeItem(selId);
		delete toTrash;
		updateRecentLevels();
		emit levelChanged(m_fakeLevel);
		m_removeButt->setDisabled(true);
	}
}



//%%%%%%%%%%%%%%%%%%%%%%%%%% TremoveLevel %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

TremoveLevel::TremoveLevel(const QString& levelName, const QString& fileName, QWidget* parent) : 
	QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint),
	m_levelFile(fileName)
{
		setWindowTitle(removeTxt().replace("<b>", QString()).replace("</b>", QString()));
		QLabel *removeLab = new QLabel(removeTxt(levelName), this);
		m_deleteChB = new QCheckBox(tr("Also delete level file:"), this);
		QLabel *fNameLab = new QLabel(QLatin1String("<b>") + fileName + QLatin1String("</b>"), this);
		
		QDialogButtonBox *stdButtons = new QDialogButtonBox(this);
		QPushButton *removeButton = stdButtons->addButton(tr("Remove"), QDialogButtonBox::AcceptRole);
			removeButton->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
		QPushButton *cancelButton =	stdButtons->addButton(QDialogButtonBox::Cancel);
			cancelButton->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
			
		QVBoxLayout *lay = new QVBoxLayout;
			lay->addWidget(removeLab, 0, Qt::AlignCenter);
			lay->addSpacing(20);
			lay->addWidget(m_deleteChB, 0, Qt::AlignCenter);
			lay->addWidget(fNameLab, 0, Qt::AlignCenter);
			lay->addSpacing(20);
			lay->addWidget(stdButtons, 0, Qt::AlignCenter);
		setLayout(lay);
			
		connect(this, SIGNAL(accepted()), this, SLOT(acceptedSlot()));
		connect(stdButtons, SIGNAL(accepted()), this, SLOT(accept()));
		connect(stdButtons, SIGNAL(rejected()), this, SLOT(reject()));
}


void TremoveLevel::acceptedSlot() {
		if (m_deleteChB->isChecked()) {
			QFile levF(m_levelFile);
			if (!levF.remove())
				qDebug() << "Can't remove level file" << m_levelFile ;
		}
}






