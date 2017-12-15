/***************************************************************************
 *   Copyright (C) 2017 by Tomasz Bojczuk                                  *
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

#include <QtCore/qdebug.h>


Taction::Taction(QObject* parent) :
  QObject(parent)
{
}


Taction::Taction(const QString& txt, const QString& ico, QObject* parent, bool isEnabled) :
  QObject(parent),
  m_enabled(isEnabled),
  m_iconTag(ico),
  m_text(txt)
{
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


void Taction::setTip(const QString& t) {
  if (t != m_tip) {
    m_tip = t;
    emit tipChanged();
  }
}


void Taction::trigger() {
  if (m_enabled)
    emit triggered();
}


void Taction::setCheckable(bool ch) {
  m_checkable = ch;
}


void Taction::setChecked(bool ch) {
  if (ch != m_checked) {
    m_checked = ch;
    emit checkedChanged();
  }
}


void Taction::setShortcut(QObject* s) {
  m_shortcut = s;
}


void Taction::setEnabled(bool e) {
  if (e != m_enabled) {
    m_enabled = e;
    emit enabledChanged();
  }
}


QString Taction::key() {
  return m_shortcut ? m_shortcut->property("nativeText").toString() : QString(); }

