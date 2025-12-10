/****************************************************************************
** Meta object code from reading C++ file 'simulationwidgets.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../simulationwidgets.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'simulationwidgets.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN16SimulationWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto SimulationWidget::qt_create_metaobjectdata<qt_meta_tag_ZN16SimulationWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "SimulationWidget",
        "startSimulation",
        "",
        "pauseSimulation",
        "resetSimulation",
        "addPrey",
        "count",
        "addPredator",
        "updateSimulation"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'startSimulation'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'pauseSimulation'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'resetSimulation'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'addPrey'
        QtMocHelpers::SlotData<void(int)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 6 },
        }}),
        // Slot 'addPrey'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void),
        // Slot 'addPredator'
        QtMocHelpers::SlotData<void(int)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 6 },
        }}),
        // Slot 'addPredator'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void),
        // Slot 'updateSimulation'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SimulationWidget, qt_meta_tag_ZN16SimulationWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject SimulationWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16SimulationWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16SimulationWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN16SimulationWidgetE_t>.metaTypes,
    nullptr
} };

void SimulationWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SimulationWidget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->startSimulation(); break;
        case 1: _t->pauseSimulation(); break;
        case 2: _t->resetSimulation(); break;
        case 3: _t->addPrey((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->addPrey(); break;
        case 5: _t->addPredator((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->addPredator(); break;
        case 7: _t->updateSimulation(); break;
        default: ;
        }
    }
}

const QMetaObject *SimulationWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SimulationWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16SimulationWidgetE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int SimulationWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
