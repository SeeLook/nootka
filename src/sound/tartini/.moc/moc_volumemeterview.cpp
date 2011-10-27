/****************************************************************************
** Meta object code from reading C++ file 'volumemeterview.h'
**
** Created: Thu Jun 21 21:45:51 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/volumemeter/volumemeterview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'volumemeterview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_VolumeMeterView[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_VolumeMeterView[] = {
    "VolumeMeterView\0"
};

const QMetaObject VolumeMeterView::staticMetaObject = {
    { &ViewWidget::staticMetaObject, qt_meta_stringdata_VolumeMeterView,
      qt_meta_data_VolumeMeterView, 0 }
};

const QMetaObject *VolumeMeterView::metaObject() const
{
    return &staticMetaObject;
}

void *VolumeMeterView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VolumeMeterView))
	return static_cast<void*>(const_cast< VolumeMeterView*>(this));
    return ViewWidget::qt_metacast(_clname);
}

int VolumeMeterView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ViewWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
