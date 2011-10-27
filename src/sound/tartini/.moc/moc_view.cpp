/****************************************************************************
** Meta object code from reading C++ file 'view.h'
**
** Created: Thu Jun 21 21:45:50 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../global/view.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'view.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_View[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      33,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      19,    6,    5,    5, 0x05,
      40,    6,    5,    5, 0x05,
      61,    5,    5,    5, 0x05,
      77,   75,    5,    5, 0x05,
     104,   75,    5,    5, 0x05,
     131,   75,    5,    5, 0x05,
     157,   75,    5,    5, 0x05,
     182,   75,    5,    5, 0x05,
     208,   75,    5,    5, 0x05,
     231,   75,    5,    5, 0x05,
     252,   75,    5,    5, 0x05,
     294,  273,    5,    5, 0x05,
     330,   75,    5,    5, 0x05,
     354,   75,    5,    5, 0x05,

 // slots: signature, parameters, type, tag, flags
     378,    5,    5,    5, 0x0a,
     389,    5,    5,    5, 0x0a,
     404,    5,    5,    5, 0x0a,
     419,    5,    5,    5, 0x0a,
     431,    5,    5,    5, 0x0a,
     448,    5,    5,    5, 0x0a,
     465,   75,    5,    5, 0x0a,
     491,   75,    5,    5, 0x0a,
     514,   75,    5,    5, 0x0a,
     536,  534,    5,    5, 0x0a,
     556,   75,    5,    5, 0x0a,
     588,  579,    5,    5, 0x0a,
     615,  534,    5,    5, 0x0a,
     647,  638,    5,    5, 0x0a,
     674,    5,    5,    5, 0x0a,
     697,    5,  692,    5, 0x0a,
     710,    5,    5,    5, 0x0a,
     730,    5,    5,    5, 0x0a,
     757,    5,  692,    5, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_View[] = {
    "View\0\0currentTime_\0onSlowUpdate(double)\0"
    "onFastUpdate(double)\0viewChanged()\0x\0"
    "scrollableYChanged(double)\0"
    "currentTimeChanged(double)\0"
    "viewBottomChanged(double)\0"
    "viewWidthChanged(double)\0"
    "viewHeightChanged(double)\0"
    "viewOffsetChanged(int)\0zoomXChanged(double)\0"
    "zoomYChanged(double)\0leftView_,rightView_\0"
    "timeViewRangeChanged(double,double)\0"
    "logZoomXChanged(double)\0logZoomYChanged(double)\0"
    "doUpdate()\0doSlowUpdate()\0doFastUpdate()\0"
    "newUpdate()\0nextFastUpdate()\0"
    "nextSlowUpdate()\0setCurrentTimeRaw(double)\0"
    "setCurrentTime(double)\0changeViewX(double)\0"
    "y\0changeViewY(double)\0setZoomFactorX(double)\0"
    "x,fixedX\0setZoomFactorX(double,int)\0"
    "setZoomFactorY(double)\0y,fixedY\0"
    "setZoomFactorY(double,int)\0doAutoFollowing()\0"
    "bool\0autoFollow()\0setAutoFollow(bool)\0"
    "setBackgroundShading(bool)\0"
    "backgroundShading()\0"
};

const QMetaObject View::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_View,
      qt_meta_data_View, 0 }
};

const QMetaObject *View::metaObject() const
{
    return &staticMetaObject;
}

void *View::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_View))
	return static_cast<void*>(const_cast< View*>(this));
    return QObject::qt_metacast(_clname);
}

int View::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: onSlowUpdate((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: onFastUpdate((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: viewChanged(); break;
        case 3: scrollableYChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: currentTimeChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: viewBottomChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: viewWidthChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: viewHeightChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 8: viewOffsetChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: zoomXChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 10: zoomYChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 11: timeViewRangeChanged((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 12: logZoomXChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 13: logZoomYChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 14: doUpdate(); break;
        case 15: doSlowUpdate(); break;
        case 16: doFastUpdate(); break;
        case 17: newUpdate(); break;
        case 18: nextFastUpdate(); break;
        case 19: nextSlowUpdate(); break;
        case 20: setCurrentTimeRaw((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 21: setCurrentTime((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 22: changeViewX((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 23: changeViewY((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 24: setZoomFactorX((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 25: setZoomFactorX((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 26: setZoomFactorY((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 27: setZoomFactorY((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 28: doAutoFollowing(); break;
        case 29: { bool _r = autoFollow();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 30: setAutoFollow((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 31: setBackgroundShading((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 32: { bool _r = backgroundShading();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        }
        _id -= 33;
    }
    return _id;
}

// SIGNAL 0
void View::onSlowUpdate(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void View::onFastUpdate(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void View::viewChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void View::scrollableYChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void View::currentTimeChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void View::viewBottomChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void View::viewWidthChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void View::viewHeightChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void View::viewOffsetChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void View::zoomXChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void View::zoomYChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void View::timeViewRangeChanged(double _t1, double _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void View::logZoomXChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void View::logZoomYChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}
