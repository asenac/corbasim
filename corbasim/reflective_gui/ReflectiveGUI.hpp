// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * ReflectiveGUI.hpp
 * Copyright (C) Cátedra SAES-UMU 2011 <catedra-saes-umu@listas.um.es>
 *
 * CORBASIM is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CORBASIM is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CORBASIM_GUI_REFLECTIVEGUI_HPP
#define CORBASIM_GUI_REFLECTIVEGUI_HPP

#include <QtGui>
#include <corbasim/core/reflective_fwd.hpp>
#include <corbasim/qt/ObjrefWidget.hpp>
#include <corbasim/qt/MultiFileSelectionWidget.hpp>

#ifdef CORBASIM_USE_QTSCRIPT
#include <QtScript>
#endif /* CORBASIM_USE_QTSCRIPT*/

namespace corbasim 
{
namespace reflective_gui 
{

class ReflectiveWidgetBase
{
protected:

    ReflectiveWidgetBase(core::reflective_base const * reflective);

public:

    virtual ~ReflectiveWidgetBase();

    core::reflective_base const * getReflective() const;

    virtual void toHolder(core::holder& holder) = 0;
    virtual void fromHolder(core::holder& holder) = 0;

protected:

    core::reflective_base const * m_reflective;
};

class AlternativesWidget : public QWidget, public ReflectiveWidgetBase
{
    Q_OBJECT
public:
    AlternativesWidget(core::reflective_base const * reflective,
            QWidget * parent = 0);
    virtual ~AlternativesWidget();
 
    virtual void toHolder(core::holder& holder);
    virtual void fromHolder(core::holder& holder);

    void addAlternative(const QString& id,
            const QString& toolTip,
            ReflectiveWidgetBase * widget);

protected slots:

    void changeWidget(int);

protected:

    QButtonGroup m_group;
    QStackedWidget * m_stack;
    QVBoxLayout * m_btnLayout;

    std::vector< ReflectiveWidgetBase * > m_widgets;
};

class FloatWidget : public QDoubleSpinBox, public ReflectiveWidgetBase
{
    Q_OBJECT
public:
    FloatWidget(core::reflective_base const * reflective,
            QWidget * parent = 0);
    virtual ~FloatWidget();
 
    virtual void toHolder(core::holder& holder);
    virtual void fromHolder(core::holder& holder);
   
};

class IntegerWidget : public QSpinBox, public ReflectiveWidgetBase
{
    Q_OBJECT
public:
    IntegerWidget(core::reflective_base const * reflective,
            QWidget * parent = 0);
    virtual ~IntegerWidget();

    virtual void toHolder(core::holder& holder);
    virtual void fromHolder(core::holder& holder);
    
};

class StringWidget : public QLineEdit, public ReflectiveWidgetBase
{
    Q_OBJECT
public:
    StringWidget(core::reflective_base const * reflective,
            QWidget * parent = 0);
    virtual ~StringWidget();

    virtual void toHolder(core::holder& holder);
    virtual void fromHolder(core::holder& holder);

};

class EnumWidget : public QComboBox, public ReflectiveWidgetBase
{
    Q_OBJECT
public:
    EnumWidget(core::reflective_base const * reflective,
            QWidget * parent = 0);
    virtual ~EnumWidget();

    virtual void toHolder(core::holder& holder);
    virtual void fromHolder(core::holder& holder);

};

class BoolWidget : public QCheckBox, public ReflectiveWidgetBase
{
    Q_OBJECT
public:
    BoolWidget(core::reflective_base const * reflective,
            QWidget * parent = 0);
    virtual ~BoolWidget();
 
    virtual void toHolder(core::holder& holder);
    virtual void fromHolder(core::holder& holder);
   
};

class StructWidget : public QWidget, public ReflectiveWidgetBase
{
    Q_OBJECT
public:
    StructWidget(core::reflective_base const * reflective,
            QWidget * parent = 0);
    virtual ~StructWidget();
 
    virtual void toHolder(core::holder& holder);
    virtual void fromHolder(core::holder& holder);

protected:

    std::vector< ReflectiveWidgetBase * > m_widgets;
};

class SequenceWidget : public QWidget, public ReflectiveWidgetBase
{
    Q_OBJECT
public:
    SequenceWidget(core::reflective_base const * reflective,
            QWidget * parent = 0);
    virtual ~SequenceWidget();

    virtual void toHolder(core::holder& holder);
    virtual void fromHolder(core::holder& holder);

protected slots:

    void lengthChanged(int);
    void indexChanged(int);

protected:

    int m_old_idx;

    QSpinBox * m_sbLength;
    QSpinBox * m_sbCurrentIndex;

    ReflectiveWidgetBase * m_slice;
    QWidget * m_slice_widget;

    core::holder m_holder;
};

#if 0
class ArrayWidget : public QWidget, public ReflectiveWidgetBase
{
    Q_OBJECT
public:
    ArrayWidget(core::reflective_base const * reflective,
            QWidget * parent = 0);
    virtual ~ArrayWidget();

    virtual void toHolder(core::holder& holder);
    virtual void fromHolder(core::holder& holder);

protected slots:

    void indexChanged(int);

protected:

    QSpinBox * m_sbCurrentIndex;

    ReflectiveWidgetBase * m_slice;
    QWidget * m_slice_widget;

    core::holder m_holder;
};
#endif

class ObjrefvarWidget : public qt::ObjrefWidget, public ReflectiveWidgetBase
{
    Q_OBJECT
public:
    ObjrefvarWidget(core::reflective_base const * reflective,
            QWidget * parent = 0);
    virtual ~ObjrefvarWidget();
 
    virtual void toHolder(core::holder& holder);
    virtual void fromHolder(core::holder& holder);
};

class FilesWidget : public qt::MultiFileSelectionWidget, 
    public ReflectiveWidgetBase
{
    Q_OBJECT
public:
    FilesWidget(core::reflective_base const * reflective,
            QWidget * parent = 0);
    virtual ~FilesWidget();
 
    virtual void toHolder(core::holder& holder);
    virtual void fromHolder(core::holder& holder);
};

class OperationInputForm : public QWidget
{
    Q_OBJECT
public:
    OperationInputForm(
            core::operation_reflective_base const * reflective,
            QWidget * parent = 0);
    virtual ~OperationInputForm();

    core::operation_reflective_base const * getReflective() const;

    event::request_ptr createRequest();

    void setValue(event::request_ptr req);

    void dragEnterEvent(QDragEnterEvent *event);

    void dropEvent(QDropEvent *event);

public slots:    

#ifdef CORBASIM_USE_QTSCRIPT
    void reloadScript();
#endif /* CORBASIM_USE_QTSCRIPT*/


protected:

    core::operation_reflective_base const * m_reflective;

    std::vector< ReflectiveWidgetBase * > m_widgets;

#ifdef CORBASIM_USE_QTSCRIPT
    // widgets
    QTabWidget * m_tabs;
    QPlainTextEdit * m_code;

    QScriptEngine m_engine;
    QScriptValue m_thisObject;
    QScriptValue m_initFunc;
    QScriptValue m_preFunc;
    QScriptValue m_postFunc;

#endif /* CORBASIM_USE_QTSCRIPT*/

};

} // namespace reflective_gui
} // namespace corbasim

#endif /* CORBASIM_GUI_REFLECTIVEGUI_HPP */

