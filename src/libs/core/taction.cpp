/***************************************************************************
 *   Copyright (C) 2017-2020 by Tomasz Bojczuk                             *
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

#include "taction.h"
#include "tpath.h"

#include <QtQml/qqmlcomponent.h>
#include <QtGui/qguiapplication.h>
#include <QtGui/qpalette.h>

#include <QtCore/qdebug.h>


Taction::Taction(QObject* parent) :
  QObject(parent)
{
  m_bgColor = qApp->palette().base().color();
  m_bgColor.setAlpha(0);
}


Taction::Taction(const QString& txt, const QString& ico, QObject* parent, bool isEnabled) :
  QObject(parent),
  m_enabled(isEnabled),
  m_iconTag(ico),
  m_text(txt)
{
  m_bgColor = qApp->palette().base().color();
  m_bgColor.setAlpha(0);
}


Taction::~Taction() {}


QString Taction::icon() const { return m_iconTag.isEmpty() ? QString() : Tpath::pix(m_iconTag); }


void Taction::setIconTag(const QString& ic) {
  if (ic != m_iconTag) {
    m_iconTag = ic;
    emit iconChanged();
  }
}


void Taction::setText(const QString& t) {
  if (t != m_text) {
    m_text = t;
    emit textChanged();
  }
}


void Taction::setTip(const QString& t, int pos) {
  if (t != m_tip) {
    m_tip = t;
    m_tipPos = static_cast<quint8>(pos);
    emit tipChanged();
  }
}


void Taction::trigger() {
  if (m_enabled)
    emit triggered();
}


void Taction::setCheckable(bool ch) {
  if (ch != m_checkable) {
    m_checkable = ch;
    emit checkableChanged();
  }
}


void Taction::setChecked(bool ch) {
  if (ch != m_checked) {
    m_checked = ch;
    emit checkedChanged();
  }
}


void Taction::setShortcut(QObject* s) {
  m_shortcut = s;
  if (m_shortcut) {
    connect(m_shortcut, SIGNAL(activated()), this, SLOT(triggerSlot()));
    m_shortcut->setProperty("enabled", m_enabled);
  }
}


void Taction::setEnabled(bool e) {
  if (e != m_enabled) {
    m_enabled = e;
    if (m_shortcut)
      m_shortcut->setProperty("enabled", m_enabled);
    emit enabledChanged();
  }
}


void Taction::setBgColor(const QColor& bgC) {
  if (bgC != m_bgColor) {
    m_bgColor = bgC;
    emit bgColorChanged();
  }
}



QString Taction::key() const {
  return m_shortcut ? m_shortcut->property("nativeText").toString() : QString();
}


void Taction::createQmlShortcut(QQmlComponent* qmlComp, const char* keySequence) {
  if (m_shortcut) {
    qDebug() << "[Taction] name:" << m_text << "has shortcut already! Ignored!";
    return;
  }
  if (keySequence) { // for empty key sequence, QML component data will be used
    std::string d = "import QtQuick 2.9; Shortcut { sequence: ";
    d.append(keySequence);
    d.append(" }");
    qmlComp->setData(d.c_str(), QUrl());
  }
  auto sc = qmlComp->create(qmlContext(parent()));
  if (sc) {
      sc->setParent(this);
      setShortcut(sc);
  } else
      qDebug() << "[Taction] Can't create shortcut for" << keySequence;
}

