/****************************************************************************
** Meta object code from reading C++ file 'gmmviewer.h'
**
** Created: Fri Mar 22 10:18:50 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/gui/gmmviewer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gmmviewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_gui__GMMViewer[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,
      38,   30,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      62,   15,   15,   15, 0x08,
      87,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_gui__GMMViewer[] = {
    "gui::GMMViewer\0\0closeViewer()\0row,col\0"
    "showUnitInfo(uint,uint)\0"
    "on_btn_zoom_in_clicked()\0"
    "on_btn_zoom_out_clicked()\0"
};

void gui::GMMViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        GMMViewer *_t = static_cast<GMMViewer *>(_o);
        switch (_id) {
        case 0: _t->closeViewer(); break;
        case 1: _t->showUnitInfo((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        case 2: _t->on_btn_zoom_in_clicked(); break;
        case 3: _t->on_btn_zoom_out_clicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData gui::GMMViewer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject gui::GMMViewer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_gui__GMMViewer,
      qt_meta_data_gui__GMMViewer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &gui::GMMViewer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *gui::GMMViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *gui::GMMViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_gui__GMMViewer))
        return static_cast<void*>(const_cast< GMMViewer*>(this));
    return QWidget::qt_metacast(_clname);
}

int gui::GMMViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void gui::GMMViewer::closeViewer()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void gui::GMMViewer::showUnitInfo(uint _t1, uint _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
