/***************************************************************************
 *   Copyright (C) 2011-2016 by Tomasz Bojczuk                             *
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


#include "tglobalsettings.h"
#include "tcolorbutton.h"
#include <tinitcorelib.h>
#include <plugins/tpluginsloader.h>
#include <QtWidgets>


TglobalSettings::TglobalSettings(QWidget *parent) :
        QWidget(parent)
{
  QVBoxLayout *lay = new QVBoxLayout();
  lay->setAlignment(Qt::AlignCenter);
  lay->addStretch(1);
  lay->addStretch(1);
  QHBoxLayout *langLay = new QHBoxLayout;
  langLay->addStretch(1);
  QLabel *langLab = new QLabel(tr("Application language"), this);
  langLay->addWidget(langLab);
  langLay->addStretch(1);
  m_langCombo = new QComboBox(this);
  langLay->addWidget(m_langCombo);
  langLay->addStretch(1);
  m_langCombo->setStatusTip(tr("Select a language.<br><span style=\"color: red;\">To take effect, this requires restarting the application!</span>"));
  m_langList[QString()] = tr("default");
  m_langList[QStringLiteral("cs")] = QString::fromUtf8("český");
  m_langList[QStringLiteral("de")] = QStringLiteral("deutsch");
  m_langList[QStringLiteral("en")] = QStringLiteral("english");
  m_langList[QStringLiteral("es")] = QString::fromUtf8("español");
  m_langList[QStringLiteral("fr")] = QString::fromUtf8("français");
  m_langList[QStringLiteral("pl")] = QStringLiteral("polski");
  m_langList[QStringLiteral("ru")] = QString::fromUtf8("русский");
  QMapIterator<QString, QString> i(m_langList);
  int id = 0;
  while (i.hasNext()) {
    i.next();
    m_langCombo->addItem(QIcon(Tcore::gl()->path + QLatin1String("picts/flags-") + i.key() + QLatin1String(".png")), i.value());
    if (i.key() == Tcore::gl()->lang)
      m_langCombo->setCurrentIndex(id);
    id++;
  }
  m_langCombo->insertSeparator(1);
  lay->addLayout(langLay);
  lay->addStretch(1);
  
  QGroupBox *updateBox = new QGroupBox(this);
  QVBoxLayout *upLay = new QVBoxLayout;
  m_updateButton = new QPushButton(tr("Check for updates"), this);
  upLay->addWidget(m_updateButton);
  m_updateLabel = new QLabel(" ", this);
  upLay->addWidget(m_updateLabel);
  updateBox->setLayout(upLay);
  lay->addWidget(updateBox);
  lay->addStretch(1);
  
	m_pluginLoader = new TpluginsLoader(this);
  if (m_pluginLoader->load(TpluginsLoader::e_updater)) {
    connect(m_updateButton, SIGNAL(clicked()), this, SLOT(updateSlot()));
    connect(m_pluginLoader->node(), &TpluginObject::message, this, &TglobalSettings::processOutputSlot);
  } else 
    updateBox->hide();
  
  lay->addStretch(1);
  m_restAllDefaultsBut = new QPushButton(tr("Restore all default settings"), this);
  m_restAllDefaultsBut->setStatusTip(warringResetConfigTxt());
  lay->addWidget(m_restAllDefaultsBut, 0 , Qt::AlignCenter);
  
  setLayout(lay);
  
  connect(m_restAllDefaultsBut, SIGNAL(clicked(bool)), this, SLOT(restoreRequired()));
}


void TglobalSettings::saveSettings() {
	QMapIterator<QString, QString> i(m_langList);
	while (i.hasNext()) {
		i.next();
		if (m_langCombo->currentText() == i.value()) {
			Tcore::gl()->lang = i.key();
			break;
		}
	}
}


void TglobalSettings::restoreDefaults() {
	
}


void TglobalSettings::restoreRequired() {
	if (QMessageBox::warning(this, tr("Restore all default settings"), warringResetConfigTxt(), QMessageBox::Ok, QMessageBox::Abort) == QMessageBox::Ok )
			emit restoreAllDefaults();
}


void TglobalSettings::updateSlot() {
	m_pluginLoader->init("", this);
  m_updateButton->setDisabled(true);
}


void TglobalSettings::processOutputSlot(QString output) {
	m_updateLabel->setText(output);
}



