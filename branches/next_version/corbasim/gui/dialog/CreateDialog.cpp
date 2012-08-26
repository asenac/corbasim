// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * CreateDialog.cpp
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

#include "CreateDialog.hpp"
#include <corbasim/adapted.hpp>
#include <corbasim/corba_adapted.hpp>
#define CORBASIM_NO_IMPL
#include <corbasim/core/reflective.hpp>

using namespace corbasim::gui;

ObjrefCreateDialog::ObjrefCreateDialog(QWidget * parent) :
    QDialog(parent)
{
    QVBoxLayout * layout = new QVBoxLayout();

    QGridLayout * grid = new QGridLayout();

    grid->addWidget(new QLabel("Object name"), 0, 0);
    m_name = new QLineEdit();
    m_name->setObjectName("name");
    grid->addWidget(m_name, 0, 1);
 
    grid->addWidget(new QLabel("Interface"), 1, 0);
    m_fqn = new QLineEdit();
    m_fqn->setObjectName("fqn");
    grid->addWidget(m_fqn, 1, 1); 

    QGroupBox * group = new QGroupBox("Object reference");
    m_fqn->setObjectName("reference_group");
    QVBoxLayout * groupLayout = new QVBoxLayout();
    groupLayout->setMargin(0);

    m_reference = new qt::ObjrefWidget();
    m_reference->setObjectName("reference");
    groupLayout->addWidget(m_reference);
    group->setLayout(groupLayout);
    
    grid->addWidget(group, 2, 0, 1, 2);

    layout->addLayout(grid);

    // Buttons
    QDialogButtonBox * btns = new QDialogButtonBox();
    QPushButton * createButton = 
        btns->addButton("&Create", QDialogButtonBox::AcceptRole);
    QPushButton * cancelButton = 
        btns->addButton("C&ancel", QDialogButtonBox::RejectRole);
    layout->addWidget(btns);

    connect(createButton, SIGNAL(clicked()),
            this, SLOT(createClicked()));

    connect(cancelButton, SIGNAL(clicked()),
            window(), SLOT(hide()));
    // End buttons

    setLayout(layout);

    ::corbasim::core::detail::objrefvar_reflective< CORBA::Object_var > iface;
    m_validator.reset(iface.create_validator());
    m_reference->setValidator(m_validator.get());
}

ObjrefCreateDialog::~ObjrefCreateDialog()
{
}

void ObjrefCreateDialog::createClicked()
{
    ObjrefConfig cfg;

    cfg.name = m_name->text().toStdString();
    cfg.fqn = m_fqn->text().toStdString();
    cfg.reference = m_validator->get_reference();
    cfg.entry = m_reference->getNSEntry().toStdString();

    emit createObjref(cfg);
}

void ObjrefCreateDialog::hideEvent(QHideEvent* event)
{
    QWidget * parent = parentWidget();

    if (parent && dynamic_cast< QMdiSubWindow* >(parent))
        parent->hide();

    event->accept();
}

// Servant

ServantCreateDialog::ServantCreateDialog(QWidget * parent) :
    QDialog(parent)
{
    QVBoxLayout * layout = new QVBoxLayout;

    QGridLayout * grid = new QGridLayout();

    grid->addWidget(new QLabel("Servant name"), 0, 0);
    m_name = new QLineEdit();
    m_name->setObjectName("name");
    grid->addWidget(m_name, 0, 1);
 
    grid->addWidget(new QLabel("Interface"), 1, 0);
    m_fqn = new QLineEdit();
    m_fqn->setObjectName("fqn");
    grid->addWidget(m_fqn, 1, 1); 

    layout->addLayout(grid);

#if 0
    QGroupBox * group = new QGroupBox("Object reference");
    m_fqn->setObjectName("reference_group");
    QVBoxLayout * groupLayout = new QVBoxLayout();
    groupLayout->setMargin(0);

    m_reference = new qt::ObjrefWidget();
    m_reference->setObjectName("reference");
    groupLayout->addWidget(m_reference);
    group->setLayout(groupLayout);

    grid->addWidget(group, 2, 0, 1, 2);
#endif

    // Buttons
    QDialogButtonBox * btns = new QDialogButtonBox();

    QPushButton * createButton = 
        btns->addButton("&Create", QDialogButtonBox::AcceptRole);
    QPushButton * cancelButton = 
        btns->addButton("C&ancel", QDialogButtonBox::RejectRole);

    layout->addWidget(btns);

    connect(createButton, SIGNAL(clicked()),
            this, SLOT(createClicked()));

    connect(cancelButton, SIGNAL(clicked()),
            window(), SLOT(hide()));

    setLayout(layout);

    setMinimumWidth(400);
}

ServantCreateDialog::~ServantCreateDialog()
{
}

void ServantCreateDialog::createClicked()
{
    ServantConfig cfg;

    cfg.name = m_name->text().toStdString();
    cfg.fqn = m_fqn->text().toStdString();

    emit createServant(cfg);
}

void ServantCreateDialog::hideEvent(QHideEvent* event)
{
    QWidget * parent = parentWidget();

    if (parent && dynamic_cast< QMdiSubWindow* >(parent))
        parent->hide();

    event->accept();
}

