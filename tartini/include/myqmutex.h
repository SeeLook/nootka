/***************************************************************************
                          myqmutex.h  -  description
                             -------------------
    begin                : Mon Nov 29 2004
    copyright            : (C) 2004 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef MYQMUTEX_H
#define MYQMUTEX_H
 
#if (QT_VERSION > 230)
#include <qmutex.h>
#else
#include <qthread.h>

//class Q_EXPORT QMutexLocker
class QMutexLocker
{
public:
    QMutexLocker( QMutex * );
    ~QMutexLocker();

    QMutex *mutex() const;

private:
    QMutex *mtx;

#if defined(Q_DISABLE_COPY)
    QMutexLocker( const QMutexLocker & );
    QMutexLocker &operator=( const QMutexLocker & );
#endif
};


inline QMutexLocker::QMutexLocker( QMutex *m )
    : mtx( m )
{
    if ( mtx ) mtx->lock();
}

inline QMutexLocker::~QMutexLocker()
{
    if ( mtx ) mtx->unlock();
}

inline QMutex *QMutexLocker::mutex() const
{
    return mtx;
}

#endif
#endif //ifndef
