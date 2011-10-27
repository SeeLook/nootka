/****************************************************************************
** Meta object code from reading C++ file 'htrackwidget.h'
**
** Created: Thu Jun 21 21:45:53 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/htrack/htrackwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'htrackwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_HTrackWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,
      42,   13,   13,   13, 0x05,
      77,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
     124,  110,   13,   13, 0x0a,
     154,  148,   13,   13, 0x0a,
     185,  148,   13,   13, 0x0a,
     223,  214,   13,   13, 0x0a,
     247,   13,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_HTrackWidget[] = {
    "HTrackWidget\0\0distanceAwayChanged(double)\0"
    "viewAngleHorizontalChanged(double)\0"
    "viewAngleVerticalChanged(double)\0"
    "peakThreshold\0setPeakThreshold(float)\0"
    "angle\0setViewAngleHorizontal(double)\0"
    "setViewAngleVertical(double)\0distance\0"
    "setDistanceAway(double)\0home()\0"
};

const QMetaObject HTrackWidget::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_HTrackWidget,
      qt_meta_data_HTrackWidget, 0 }
};

const QMetaObject *HTrackWidget::metaObject() const
{
    return &staticMetaObject;
}

void *HTrackWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_HTrackWidget))
	return static_cast<void*>(const_cast< HTrackWidget*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int HTrackWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: distanceAwayChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: viewAngleHorizontalChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: viewAngleVerticalChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: setPeakThreshold((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 4: setViewAngleHorizontal((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: setViewAngleVertical((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: setDistanceAway((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: home(); break;
        }
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void HTrackWidget::distanceAwayChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void HTrackWidget::viewAngleHorizontalChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void HTrackWidget::viewAngleVerticalChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
