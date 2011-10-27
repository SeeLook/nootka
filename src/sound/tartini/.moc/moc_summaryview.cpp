/****************************************************************************
** Meta object code from reading C++ file 'summaryview.h'
**
** Created: Thu Jun 21 21:45:43 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/summary/summaryview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'summaryview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_SummaryView[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_SummaryView[] = {
    "SummaryView\0"
};

const QMetaObject SummaryView::staticMetaObject = {
    { &ViewWidget::staticMetaObject, qt_meta_stringdata_SummaryView,
      qt_meta_data_SummaryView, 0 }
};

const QMetaObject *SummaryView::metaObject() const
{
    return &staticMetaObject;
}

void *SummaryView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SummaryView))
	return static_cast<void*>(const_cast< SummaryView*>(this));
    return ViewWidget::qt_metacast(_clname);
}

int SummaryView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ViewWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
