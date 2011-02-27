#include <QtGui/QApplication>
#include "nutoukmainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NutoukMainWindow w;
    w.show();

    return a.exec();
}
