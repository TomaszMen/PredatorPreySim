/****************************************************************************
** Meta object code from reading C++ file 'sidepanel.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../sidepanel.h"
#include <QtGui/qtextcursor.h>
#include <QtGui/qscreen.h>
#include <QtCharts/qlineseries.h>
#include <QtCharts/qabstractbarseries.h>
#include <QtCharts/qvbarmodelmapper.h>
#include <QtCharts/qboxplotseries.h>
#include <QtCharts/qcandlestickseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qpieseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qboxplotseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qpieseries.h>
#include <QtCharts/qpieseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qxyseries.h>
#include <QtCharts/qxyseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qboxplotseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qpieseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qxyseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sidepanel.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN9SidePanelE_t {};
} // unnamed namespace

template <> constexpr inline auto SidePanel::qt_create_metaobjectdata<qt_meta_tag_ZN9SidePanelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "SidePanel",
        "simulationParametersChanged",
        "",
        "preyReproductionRate",
        "predatorReproductionRate",
        "foodRegenerationRate",
        "energyConsumptionRate",
        "mutationRate",
        "initialPrey",
        "initialPredators",
        "addPreyRequested",
        "count",
        "addPredatorRequested",
        "clearAllRequested",
        "updateStatistics",
        "preyCount",
        "predatorCount",
        "generation",
        "updateCharts",
        "QList<int>",
        "preyHistory",
        "predatorHistory",
        "applyParameters",
        "resetToDefaults"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'simulationParametersChanged'
        QtMocHelpers::SignalData<void(float, float, float, float, float, int, int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Float, 3 }, { QMetaType::Float, 4 }, { QMetaType::Float, 5 }, { QMetaType::Float, 6 },
            { QMetaType::Float, 7 }, { QMetaType::Int, 8 }, { QMetaType::Int, 9 },
        }}),
        // Signal 'addPreyRequested'
        QtMocHelpers::SignalData<void(int)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 11 },
        }}),
        // Signal 'addPredatorRequested'
        QtMocHelpers::SignalData<void(int)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 11 },
        }}),
        // Signal 'clearAllRequested'
        QtMocHelpers::SignalData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'updateStatistics'
        QtMocHelpers::SlotData<void(int, int, int)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 15 }, { QMetaType::Int, 16 }, { QMetaType::Int, 17 },
        }}),
        // Slot 'updateCharts'
        QtMocHelpers::SlotData<void(const QVector<int> &, const QVector<int> &)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 19, 20 }, { 0x80000000 | 19, 21 },
        }}),
        // Slot 'applyParameters'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'resetToDefaults'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SidePanel, qt_meta_tag_ZN9SidePanelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject SidePanel::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9SidePanelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9SidePanelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9SidePanelE_t>.metaTypes,
    nullptr
} };

void SidePanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SidePanel *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->simulationParametersChanged((*reinterpret_cast<std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<float>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<float>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<float>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<float>>(_a[5])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[6])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[7]))); break;
        case 1: _t->addPreyRequested((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->addPredatorRequested((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->clearAllRequested(); break;
        case 4: _t->updateStatistics((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3]))); break;
        case 5: _t->updateCharts((*reinterpret_cast<std::add_pointer_t<QList<int>>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QList<int>>>(_a[2]))); break;
        case 6: _t->applyParameters(); break;
        case 7: _t->resetToDefaults(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<int> >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (SidePanel::*)(float , float , float , float , float , int , int )>(_a, &SidePanel::simulationParametersChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (SidePanel::*)(int )>(_a, &SidePanel::addPreyRequested, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (SidePanel::*)(int )>(_a, &SidePanel::addPredatorRequested, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (SidePanel::*)()>(_a, &SidePanel::clearAllRequested, 3))
            return;
    }
}

const QMetaObject *SidePanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SidePanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9SidePanelE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int SidePanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void SidePanel::simulationParametersChanged(float _t1, float _t2, float _t3, float _t4, float _t5, int _t6, int _t7)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2, _t3, _t4, _t5, _t6, _t7);
}

// SIGNAL 1
void SidePanel::addPreyRequested(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void SidePanel::addPredatorRequested(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void SidePanel::clearAllRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
