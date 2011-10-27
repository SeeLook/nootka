/****************************************************************************
** Meta object code from reading C++ file 'hcircleview.h'
**
** Created: Thu Jun 21 21:45:47 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/hcircle/hcircleview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'hcircleview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_HCircleView[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_HCircleView[] = {
    "HCircleView\0"
};

const QMetaObject HCircleView::staticMetaObject = {
    { &ViewWidget::staticMetaObject, qt_meta_stringdata_HCircleView,
      qt_meta_data_HCircleView, 0 }
};

const QMetaObject *HCircleView::metaObject() const
{
    return &staticMetaObject;
}

void *HCircleView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_HCircleView))
	return static_cast<void*>(const_cast< HCircleView*>(this));
    return ViewWidget::qt_metacast(_clname);
}

int HCircleView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ViewWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
