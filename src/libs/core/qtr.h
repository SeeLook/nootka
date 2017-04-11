#ifndef QTR_H
#define QTR_H


#include <QtGui/qguiapplication.h>


/** HACK:
 * wrapper of standard Qt @p QApplication::translate method
 * to avoid parsing texts by lupdate.
 * This way translations of Qt can be used without adding them to *.ts files.
 */
inline QString qTR(const char* context, const char* key, const char* disambiguation = 0, int n = -1) {
  return QGuiApplication::translate(context, key, disambiguation, n);
}

#endif // QTR_H
