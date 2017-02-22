#include <QtCore/qdebug.h>
#include <QtCore/qelapsedtimer.h>

/**
 * @p CHECKTIME macro measures time spent on function/method
 * Usage:
 * CHECKTIME(
 *  (...)
 *  code to perform
 * (...)
 * )
 */
#define CONCAT_(x,y) x##y
#define CONCAT(x,y) CONCAT_(x,y)
#define CHECKTIME(x)  \
    QElapsedTimer CONCAT(sb_, __LINE__); \
    CONCAT(sb_, __LINE__).start(); \
    x \
    qDebug() << __FILE__ << __FUNCTION__ << ":" << __LINE__ << " Elapsed time: " <<  CONCAT(sb_, __LINE__).nsecsElapsed() / 1000000.0 << " ms.";


