/****************************************************************************
** Meta object code from reading C++ file 'htrackview.h'
**
** Created: Thu Jun 21 21:45:53 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/htrack/htrackview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'htrackview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_HTrackView[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      18,   12,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_HTrackView[] = {
    "HTrackView\0\0value\0setPeakThreshold(int)\0"
};

const QMetaObject HTrackView::staticMetaObject = {
    { &ViewWidget::staticMetaObject, qt_meta_stringdata_HTrackView,
      qt_meta_data_HTrackView, 0 }
};

const QMetaObject *HTrackView::metaObject() const
{
    return &staticMetaObject;
}

void *HTrackView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_HTrackView))
	return static_cast<void*>(const_cast< HTrackView*>(this));
    return ViewWidget::qt_metacast(_clname);
}

int HTrackView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ViewWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setPeakThreshold((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 1;
    }
    return _id;
}
