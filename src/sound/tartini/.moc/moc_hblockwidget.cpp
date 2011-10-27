/****************************************************************************
** Meta object code from reading C++ file 'hblockwidget.h'
**
** Created: Thu Jun 21 21:45:45 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/hblock/hblockwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'hblockwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_HBlockWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_HBlockWidget[] = {
    "HBlockWidget\0"
};

const QMetaObject HBlockWidget::staticMetaObject = {
    { &DrawWidget::staticMetaObject, qt_meta_stringdata_HBlockWidget,
      qt_meta_data_HBlockWidget, 0 }
};

const QMetaObject *HBlockWidget::metaObject() const
{
    return &staticMetaObject;
}

void *HBlockWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_HBlockWidget))
	return static_cast<void*>(const_cast< HBlockWidget*>(this));
    return DrawWidget::qt_metacast(_clname);
}

int HBlockWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DrawWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
