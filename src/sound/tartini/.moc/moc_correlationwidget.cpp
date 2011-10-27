/****************************************************************************
** Meta object code from reading C++ file 'correlationwidget.h'
**
** Created: Thu Jun 21 21:45:54 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/correlation/correlationwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'correlationwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_CorrelationWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      24,   19,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CorrelationWidget[] = {
    "CorrelationWidget\0\0mode\0setAggregateMode(int)\0"
};

const QMetaObject CorrelationWidget::staticMetaObject = {
    { &DrawWidget::staticMetaObject, qt_meta_stringdata_CorrelationWidget,
      qt_meta_data_CorrelationWidget, 0 }
};

const QMetaObject *CorrelationWidget::metaObject() const
{
    return &staticMetaObject;
}

void *CorrelationWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CorrelationWidget))
	return static_cast<void*>(const_cast< CorrelationWidget*>(this));
    return DrawWidget::qt_metacast(_clname);
}

int CorrelationWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DrawWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setAggregateMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 1;
    }
    return _id;
}
