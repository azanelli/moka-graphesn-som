/****************************************************************************
** Meta object code from reading C++ file 'mokawindow.h'
**
** Created: Fri Mar 22 10:18:42 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/gui/mokawindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mokawindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_gui__MokaWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x08,
      43,   34,   16,   16, 0x08,
      72,   65,   16,   16, 0x08,
      96,   34,   16,   16, 0x08,
     120,   34,   16,   16, 0x08,
     145,   65,   16,   16, 0x08,
     172,   34,   16,   16, 0x08,
     199,   16,   16,   16, 0x08,
     229,   16,   16,   16, 0x08,
     266,   16,   16,   16, 0x08,
     303,   16,   16,   16, 0x08,
     343,  335,   16,   16, 0x08,
     384,   16,   16,   16, 0x08,
     408,  400,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_gui__MokaWindow[] = {
    "gui::MokaWindow\0\0closeGMMViewer()\0"
    "filepath\0loadModelEnd(QString)\0reason\0"
    "loadModelError(QString)\0loadModelStart(QString)\0"
    "loadUnitInfoEnd(QString)\0"
    "loadUnitInfoError(QString)\0"
    "loadUnitInfoStart(QString)\0"
    "on_act_view_model_triggered()\0"
    "on_btn_open_molecules_dock_clicked()\0"
    "on_btn_molecules_calculate_clicked()\0"
    "on_btn_molecules_open_clicked()\0visible\0"
    "on_dck_molecules_visibilityChanged(bool)\0"
    "showGMMViewer()\0row,col\0showUnitInfo(uint,uint)\0"
};

void gui::MokaWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MokaWindow *_t = static_cast<MokaWindow *>(_o);
        switch (_id) {
        case 0: _t->closeGMMViewer(); break;
        case 1: _t->loadModelEnd((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->loadModelError((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->loadModelStart((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->loadUnitInfoEnd((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->loadUnitInfoError((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->loadUnitInfoStart((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: _t->on_act_view_model_triggered(); break;
        case 8: _t->on_btn_open_molecules_dock_clicked(); break;
        case 9: _t->on_btn_molecules_calculate_clicked(); break;
        case 10: _t->on_btn_molecules_open_clicked(); break;
        case 11: _t->on_dck_molecules_visibilityChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->showGMMViewer(); break;
        case 13: _t->showUnitInfo((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData gui::MokaWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject gui::MokaWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_gui__MokaWindow,
      qt_meta_data_gui__MokaWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &gui::MokaWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *gui::MokaWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *gui::MokaWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_gui__MokaWindow))
        return static_cast<void*>(const_cast< MokaWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int gui::MokaWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
