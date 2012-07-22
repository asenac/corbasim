// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * DumpTool.cpp
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

#include "DumpTool.hpp"
#include <corbasim/qt/SortableGroup.hpp>
#include <corbasim/gui/utils.hpp>
#include <QHBoxLayout>
#include <QTreeView>

#include <iostream>

using namespace corbasim::gui;

DumpProcessor::DumpProcessor(const QString& id,
        const gui::ReflectivePath_t path, 
        const Config& config) :
    gui::RequestProcessor(id, path), m_config(config)
{

}

DumpProcessor::~DumpProcessor()
{
}

void DumpProcessor::process(event::request_ptr req, 
        core::reflective_base const * ref,
        core::holder hold)
{
    // TODO save
}

// Reflective plot

Dumper::Dumper(const QString& id,
        core::operation_reflective_base const * reflective,
        const QList< int >& path, 
        QWidget * parent) :
    QWidget(parent), m_id(id), m_reflective(reflective), m_path(path)
{
    QGridLayout * layout = new QGridLayout();

    int row = 0;

    m_filePrefix = new QLineEdit();
    QHBoxLayout * prefixLayout = new QHBoxLayout();
    QPushButton * browse = new QPushButton("&Browse");
    prefixLayout->addWidget(m_filePrefix);
    prefixLayout->addWidget(browse);
    layout->addWidget(new QLabel("File prefix"), row, 0);
    layout->addLayout(prefixLayout, row++, 1);

    m_multipleFiles = new QCheckBox();
    m_multipleFiles->setChecked(true);
    layout->addWidget(new QLabel("Multiple files"), row, 0);
    layout->addWidget(m_multipleFiles, row++, 1);
    
    m_suffixLength = new QSpinBox();
    m_suffixLength->setRange(1, 10); 
    m_suffixLength->setValue(4);
    layout->addWidget(new QLabel("Suffix length"), row, 0);
    layout->addWidget(m_suffixLength, row++, 1);

    m_format = new QComboBox();
    m_format->addItem("Binary (*.bin)");
    m_format->addItem("Text (*.txt)");
    m_format->addItem("JSON (*.json)");
    layout->addWidget(new QLabel("Format"), row, 0);
    layout->addWidget(m_format, row++, 1);

    // start and stop button
    m_startStopButton = new QPushButton("&Start/stop");
    m_startStopButton->setCheckable(true);
    QHBoxLayout * startStopLayout = new QHBoxLayout();
    QSpacerItem * spacer = new QSpacerItem(40, 20, 
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    startStopLayout->addItem(spacer);
    startStopLayout->addWidget(m_startStopButton);
    layout->addLayout(startStopLayout, row++, 1);

    setLayout(layout);

    QObject::connect(m_startStopButton,
            SIGNAL(clicked(bool)),
            this, SLOT(doStart(bool)));

    QObject::connect(browse, SIGNAL(clicked()),
            this, SLOT(browse()));

    QString defaultFile(getFieldName(reflective, path));
    defaultFile += "-";
    m_filePrefix->setText(defaultFile);
}

Dumper::~Dumper()
{
}

void Dumper::browse()
{
    const QString file = QFileDialog::getSaveFileName(this, 
            "Select a file...", ".");

    m_filePrefix->setText(file);
}

corbasim::core::operation_reflective_base const * 
Dumper::getReflective() const
{
    return m_reflective;
}

void Dumper::doStart(bool start)
{
    reset();

    if (start)
    {
        const QString& file (m_filePrefix->text());

        if (file.isEmpty())
        {
            m_startStopButton->setChecked(false);
        }
        else
        {
            const DumpProcessor::Config config = {
                m_multipleFiles->isChecked(),
                file,
                static_cast< DumpProcessor::Format >(m_format->currentIndex()),
                m_suffixLength->value()
            };
            
            DumpProcessor * processor =
                new DumpProcessor(m_id, m_path, config);

            m_processor.reset(processor);
        }
    }
}

void Dumper::reset()
{
    if (m_processor)
    {
        emit removeProcessor(m_processor);
        m_processor.reset();
    }
}

DumpTool::DumpTool(QWidget * parent) :
    QWidget(parent)
{
    QHBoxLayout * layout = new QHBoxLayout(this);

    // TODO splitter

    // Model view
    QTreeView * view = new QTreeView(this);
    view->setModel(&m_model);
    layout->addWidget(view);
    view->setMaximumWidth(300);
    view->setColumnWidth(0, 210);

    // Plots
    m_group = new qt::SortableGroup(this);
    m_group->setDelete(false);
    layout->addWidget(m_group);

    setLayout(layout);

    // widget signals
    QObject::connect(m_group, 
            SIGNAL(deleteRequested(corbasim::qt::SortableGroupItem *)),
            this, 
            SLOT(deleteRequested(corbasim::qt::SortableGroupItem *)));

    // connect model signals 
    QObject::connect(&m_model, 
            SIGNAL(checked(const QString&,
                    core::interface_reflective_base const *,
                    const QList< int >&)),
            this,
            SLOT(createDumper(const QString&,
                    core::interface_reflective_base const *,
                    const QList< int >&)));
    QObject::connect(&m_model, 
            SIGNAL(unchecked(const QString&,
                    core::interface_reflective_base const *,
                    const QList< int >&)),
            this,
            SLOT(deleteDumper(const QString&,
                    core::interface_reflective_base const *,
                    const QList< int >&)));
    
    setMinimumSize(650, 400);
}

DumpTool::~DumpTool()
{
}

void DumpTool::registerInstance(const QString& name,
        core::interface_reflective_base const * reflective)
{
    m_model.registerInstance(name, reflective);
}

void DumpTool::unregisterInstance(const QString& name)
{
    // Maps
    map_t::iterator it = m_map.begin();
    for(; it != m_map.end(); it++)
    {
        if (name == it->first.first)
        {
            for (int i = 0; i < it->second.size(); i++) 
            {
                Dumper * plot = it->second[i];

                m_inverse_map.erase(plot);
                m_model.uncheck(it->first.first, plot->getPath());

            }

            m_map.erase(it);
        }
    }

    m_model.unregisterInstance(name);
}

void DumpTool::createDumper(const QString& id, 
        core::interface_reflective_base const * reflective,
        const QList< int >& path)
{
    core::operation_reflective_base const * op =
        reflective->get_reflective_by_index(path.front());

    Dumper * plot = new Dumper(id, op, path);

    const key_t key(id, op->get_tag());
    m_map[key].push_back(plot);
    m_inverse_map[plot] = key;

    qt::SortableGroupItem * item = 
        new qt::SortableGroupItem(plot, m_group);

    item->showDetails();

    const QString title(gui::getFieldName(op, path));
    item->setTitle(id + "." + title);

    m_group->appendItem(item);

    // connect with the processor
    QObject::connect(plot, 
            SIGNAL(addProcessor(
                    corbasim::gui::RequestProcessor_ptr)),
            gui::getDefaultInputRequestController(),
            SLOT(addProcessor(
                    corbasim::gui::RequestProcessor_ptr)));
    QObject::connect(plot, 
            SIGNAL(removeProcessor(
                    corbasim::gui::RequestProcessor_ptr)),
            gui::getDefaultInputRequestController(),
            SLOT(removeProcessor(
                    corbasim::gui::RequestProcessor_ptr)));
}

void DumpTool::deleteDumper(const QString& id, 
        core::interface_reflective_base const * reflective,
        const QList< int >& path)
{
    core::operation_reflective_base const * op =
        reflective->get_reflective_by_index(path.front());

    const key_t key(id, op->get_tag());
    QList< Dumper * >& list = m_map[key];

    for (int i = 0; i < list.size(); i++) 
    {
        Dumper * plot = list[i];
        if (plot->getPath() == path)
        {
            list.removeAt(i);
            m_inverse_map.erase(plot);
            m_group->deleteItem(
                    qobject_cast< qt::SortableGroupItem * >
                        (plot->parent()));

            // Notify to the processor
            plot->reset();
            break;
        }
    }
}

void DumpTool::deleteRequested(qt::SortableGroupItem* item)
{
    Dumper * plot = 
        qobject_cast< Dumper * >(item->getWidget());

    if (plot)
    {
        inverse_map_t::iterator it = m_inverse_map.find(plot);

        if (it != m_inverse_map.end())
        {
            const key_t key(it->second);
            m_map[key].removeAll(plot);

            // notify to model
            m_model.uncheck(key.first, plot->getPath());

            m_inverse_map.erase(it);
        }
    }
    m_group->deleteItem(item);
}


