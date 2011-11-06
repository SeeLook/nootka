#ifndef MYQT_H
#define MYQT_H

#include <QColor>

/** Gives a color between a and b, using ratio (a number between 0 and 1)
*/
extern QColor colorBetween(QColor a, QColor b, double ratio);

#endif
