#ifndef NUTOUKMAINWINDOW_H
#define NUTOUKMAINWINDOW_H

//#include <QtGui/QMainWindow>
//#include <tscorewidgetsimple.h>
#include <tscorewidget.h>

class NutoukMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    NutoukMainWindow(QWidget *parent = 0);
    ~NutoukMainWindow();


public slots:
    void whenEnableDblAccStateChanged(bool isEnabled);

private:
    TscoreWidget *m_score;
    QMenu *m_actMenu, *m_scoreMenu;
    QAction *showOherNotesAct, *useDblAccidAct;

    void createMenus();
    void createActions();

};

#endif // NUTOUKMAINWINDOW_H
