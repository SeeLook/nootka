/****************************************************************************
** Meta object code from reading C++ file 'ledindicator.h'
**
** Created: Thu Jun 21 21:45:37 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/ledindicator.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ledindicator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_LEDIndicator[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_LEDIndicator[] = {
    "LEDIndicator\0"
};

const QMetaObject LEDIndicator::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_LEDIndicator,
      qt_meta_data_LEDIndicator, 0 }
};

const QMetaObject *LEDIndicator::metaObject() const
{
    return &staticMetaObject;
}

void *LEDIndicator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LEDIndicator))
	return static_cast<void*>(const_cast< LEDIndicator*>(this));
    return QWidget::qt_metacast(_clname);
}

int LEDIndicator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
