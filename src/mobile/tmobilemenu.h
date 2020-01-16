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

#ifndef TMOBILEMENU_H
#define TMOBILEMENU_H


#include <QtQuick/qquickitem.h>


#define MOBILE_MENU (TmobileMenu::instance())


class Taction;
class TtipHandler;


/**
 * C++ logic of the main mobile menu button on the screen top-left
 * and quick access actions aka 'flying actions' around the button
 */
class TmobileMenu : public QQuickItem
{

  friend class TtipHandler;

  Q_OBJECT

  Q_PROPERTY(bool pressed READ pressed NOTIFY pressedChanged)
  Q_PROPERTY(bool showFlys READ showFlys NOTIFY showFlysChanged)
  Q_PROPERTY(QQuickItem* currentFly READ currentFly NOTIFY currentFlyChanged)
  Q_PROPERTY(Taction* pitchDetectAct READ pitchDetectAct NOTIFY actionsChanged)
  Q_PROPERTY(Taction* tempoAct READ tempoAct NOTIFY actionsChanged)
  Q_PROPERTY(Taction* fly1act READ fly1act NOTIFY fly1actChanged)
  Q_PROPERTY(Taction* fly2act READ fly2act NOTIFY fly2actChanged)
  Q_PROPERTY(Taction* fly3act READ fly3act NOTIFY fly3actChanged)
  Q_PROPERTY(Taction* fly4act READ fly4act NOTIFY fly4actChanged)
  Q_PROPERTY(Taction* fly5act READ fly5act NOTIFY fly5actChanged)


public:
  TmobileMenu(QQuickItem* parent = nullptr);
  ~TmobileMenu() override;

      /**
       * Static instance of mobile (Android) menu.
       * Single one for entire Nootka launch
       */
  static TmobileMenu* instance() { return m_instance; }

  bool pressed() const { return m_pressed; }

  bool showFlys() const { return m_showFlys; }

  QQuickItem* currentFly() { return m_currentFlyItem; }

  Taction* pitchDetectAct() { return m_pitchDetectAct; }
  Taction* tempoAct() { return m_tempoAct; }
  Taction* fly1act() { return m_1flyAct; }
  Taction* fly2act() { return m_2flyAct; }
  Taction* fly3act() { return m_3flyAct; }
  Taction* fly4act() { return m_4flyAct; }
  Taction* fly5act() { return m_5flyAct; }

  Q_INVOKABLE int fingerPixels() const;

  Q_INVOKABLE qreal flyX(int itemNr);
  Q_INVOKABLE qreal flyY(int itemNr);

signals:
  void pressedChanged();
  void clicked();
  void showFlysChanged();
  void currentFlyChanged();
  void actionsChanged();
  void flyClicked();
  void fly1actChanged();
  void fly2actChanged();
  void fly3actChanged();
  void fly4actChanged();
  void fly5actChanged();

protected:
  void mousePressEvent(QMouseEvent*) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;

      /**
       * Connected with @p Tsound::initialize() initializes flying actions,
       * connects with single/multiple note mode change signal
       */
  void init();
  void singleNoteModeSlot();

      /**
       * @p TtipHandler is very aware what is exam/exercise phase (questioning, answering, correcting)
       * and it has full access to exam executor, exam, level classes.
       * So, it calls this method to update flying menu actions
       */
  void setFlyActions(Taction* a1, Taction* a2, Taction* a3, Taction* a4, Taction* a5);

private:
  static TmobileMenu*         m_instance;
  bool                        m_pressed = false;
  bool                        m_showFlys = false;
  QQuickItem                 *m_currentFlyItem = nullptr;
  Taction                    *m_pitchDetectAct;
  Taction                    *m_tempoAct;
  Taction                    *m_1flyAct = nullptr;
  Taction                    *m_2flyAct = nullptr;
  Taction                    *m_3flyAct = nullptr;
  Taction                    *m_4flyAct = nullptr;
  Taction                    *m_5flyAct = nullptr;
};

#endif // TMOBILEMENU_H
