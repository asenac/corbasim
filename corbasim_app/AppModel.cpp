// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * AppModel.cpp
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

#include "AppModel.hpp"
#include "AppController.hpp"

#include "app_adapted.hpp" // for json serializer

#include <corbasim/json/writer.hpp>
#include <corbasim/json/parser.hpp>

#include <corbasim/core/factory_fwd.hpp>

#include <corbasim/qt/ReferenceModel.hpp>

#include <fstream>
#include <dlfcn.h>

#include <iostream>

using namespace corbasim::app;

namespace corbasim 
{
namespace app 
{

class AppModelData 
{
public:
    AppModelData ()
    {
        int argc = 0;
        orb = CORBA::ORB_init (argc, NULL);

        CORBA::Object_var rootPOAObj = 
            orb->resolve_initial_references ("RootPOA");

        rootPOA = PortableServer::POA::_narrow(rootPOAObj.in());

        manager = rootPOA->the_POAManager();

        manager->activate();
    }
    
    ~AppModelData ()
    {
    }

    CORBA::ORB_var orb;
    PortableServer::POA_var rootPOA;
    PortableServer::POAManager_var manager; 
};

} // namespace app
} // namespace corbasim

AppModel::AppModel() : 
    m_ref_model(*qt::ReferenceModel::getDefaultModel()),
    m_data(new AppModelData), m_controller(NULL)
{
}

AppModel::~AppModel()
{
    // TODO close libraries
    
    servants_t::iterator it = m_servants.begin();
    servants_t::iterator end = m_servants.end();

    for (; it != end; it++) 
    {
        // Temporal - Proof of concept
        PortableServer::ObjectId_var myObjID = 
            m_data->rootPOA->servant_to_id(it->second->getServant()); 

        m_data->rootPOA->deactivate_object (myObjID);
        // End temporal
    }

    delete m_data;
}

const corbasim::gui::gui_factory_base * 
AppModel::getFactory(const QString& fqn)
{
    factories_t::iterator it = m_factories.find(fqn);

    if (it != m_factories.end())
        return it->second;

    QString lib (fqn);
    lib.replace("::","_");
    lib.prepend("libcorbasim_lib_");
    lib.append(".so");

    return loadLibrary(lib);
}

void AppModel::setController(AppController * controller)
{
    m_controller = controller;
}

void AppModel::createObjref(const corbasim::app::ObjrefConfig& cfg)
{
    QString id(cfg.id.in());

    if (m_servants.find(id) != m_servants.end() ||
            m_objrefs.find(id) != m_objrefs.end())
    {
        if (m_controller)
            m_controller->notifyError(
                    QString("Object '%1' already exists!").arg(id));
        return;
    }

    const corbasim::gui::gui_factory_base * factory = 
        getFactory(cfg.fqn.in());

    if (factory)
    {
        model::Objref_ptr obj(new model::Objref(cfg, factory));
        m_objrefs.insert(std::make_pair(id, obj));

        m_ref_model.appendItem(id, cfg.ref, factory);

        if (m_controller)
        {
            m_controller->notifyObjrefCreated(id, factory);

            // Also notifies its reference
            m_controller->notifyUpdatedReference(id, cfg.ref);
        }
    }
}

void AppModel::createServant(const corbasim::app::ServantConfig& cfg)
{
    QString id(cfg.id.in());

    if (m_servants.find(id) != m_servants.end() ||
            m_objrefs.find(id) != m_objrefs.end())
    {
        if (m_controller)
            m_controller->notifyError(
                    QString("Object '%1' already exists!").arg(id));
        return;
    }

    const corbasim::gui::gui_factory_base * factory = 
        getFactory(cfg.fqn.in());

    if (factory)
    {
        model::Servant_ptr obj(new model::Servant(cfg, factory));
        obj->setController(m_controller);
        m_servants.insert(std::make_pair(id, obj));

        // Temporal - Proof of concept

        PortableServer::ObjectId_var myObjID = 
            m_data->rootPOA->activate_object (obj->getServant());
    
        CORBA::Object_var objSrv = 
            m_data->rootPOA->servant_to_reference(obj->getServant());

        // Displaying reference
        CORBA::String_var ref = m_data->orb->object_to_string (objSrv);
        std::cout << cfg.id << ": " << ref << std::endl;

        // End temporal

        m_ref_model.appendItem(id, objSrv, factory);

        if (m_controller)
            m_controller->notifyServantCreated(id, factory);
    }
}

void AppModel::sendRequest(const QString& id,
        corbasim::event::request_ptr req)
{
    objrefs_t::iterator it = m_objrefs.find(id);
    if (it == m_objrefs.end())
    {
        servants_t::iterator it = m_servants.find(id);
        if (it == m_servants.end())
            m_controller->notifyError(
                    QString("Object '%1' not found!").arg(id));
        else
            corbasim::event::event_ptr ev (it->second->sendRequest(req));
    }
    else
    {
        corbasim::event::event_ptr ev (it->second->sendRequest(req));

        if (m_controller)
            m_controller->notifyRequestSent(id, req, ev);
    }
}

void AppModel::deleteObjref(const QString& id)
{
    if (m_objrefs.erase(id) > 0)
    {
        m_ref_model.removeItem(id);

        if (m_controller)
            m_controller->notifyObjrefDeleted(id);
    }
    else if (m_controller)
        m_controller->notifyError(
                QString("Object '%1' not found!").arg(id));
}

void AppModel::deleteServant(const QString& id)
{
    servants_t::iterator it = m_servants.find(id);

    if (it != m_servants.end())
    {
        // Temporal - Proof of concept
        PortableServer::ObjectId_var myObjID = 
            m_data->rootPOA->servant_to_id(it->second->getServant()); 

        m_data->rootPOA->deactivate_object (myObjID);
        // End temporal
    }

    if (m_servants.erase(id) > 0)
    {
        m_ref_model.removeItem(id);

        if (m_controller)
            m_controller->notifyServantDeleted(id);
    }
    else if (m_controller)
        m_controller->notifyError(
                QString("Object '%1' not found!").arg(id));
}

void AppModel::saveFile(const QString& file)
{
    Configuration cfg;

    // Objrefs
    {
        cfg.objects.length(m_objrefs.size());
        objrefs_t::const_iterator it = m_objrefs.begin();
        objrefs_t::const_iterator end = m_objrefs.end();

        for (int i = 0; it != m_objrefs.end(); ++it, i++)
            cfg.objects[i] = it->second->getConfig();
    }

    // Servants
    {
        cfg.servants.length(m_servants.size());
        servants_t::const_iterator it = m_servants.begin();
        servants_t::const_iterator end = m_servants.end();

        for (int i = 0; it != m_servants.end(); ++it, i++)
            cfg.servants[i] = it->second->getConfig();
    }

    std::string file_ (file.toStdString());
    std::ofstream ofs (file_.c_str());

    // convert to JSON and save
    try {
        json::write(ofs, cfg);

        if (m_controller)
            m_controller->notifyMessage(
                    QString("Configuration saved into file '%1'").arg(file));

    } catch (...) {
        if (m_controller)
            m_controller->notifyError(
                    QString("Error saving file '%1'").arg(file));
    }
}

void AppModel::loadFile(const QString& file)
{
    // clear current config
    clearConfig();

    char * buffer = NULL;
    size_t length = 0;

    Configuration cfg;

    try {
        std::string file_ (file.toStdString());
        std::ifstream ifs (file_.c_str());

        // get length of file:
        ifs.seekg (0, std::ios::end);
        length = ifs.tellg();
        ifs.seekg (0, std::ios::beg);

        // allocate memory:
        buffer = new char [length];

        // read data as a block:
        ifs.read (buffer, length);
        ifs.close();

        // parse JSON
        json::parse(cfg, buffer, length);

        if (m_controller)
            m_controller->notifyMessage(
                    QString("Configuration loaded from file '%1'").arg(file));

        // process cfg

        // Objects
        for (unsigned int i = 0; i < cfg.objects.length(); i++) 
            createObjref(cfg.objects[i]);

        // Servants
        for (unsigned int i = 0; i < cfg.servants.length(); i++) 
            createServant(cfg.servants[i]);

    } catch (...) {
    }

    delete [] buffer;
}

void AppModel::loadDirectory(const QString& path)
{
    QDir d(path);
    QStringList filters;
    filters << "libcorbasim_lib_*.so";
    d.setNameFilters(filters);

    const QFileInfoList files = d.entryInfoList(QDir::Files);
    int count = files.count();

    for (int i = 0; i < count; i++) 
    {
        loadLibrary(files[i].absoluteFilePath());
    }
}

const corbasim::gui::gui_factory_base * 
AppModel::loadLibrary(const QString& file)
{
    std::string str(file.toStdString());

    typedef const corbasim::gui::gui_factory_base *(*get_factory_t)();

    void * handle = dlopen(str.c_str(), RTLD_NOW);

    if (!handle)
    {
        if (m_controller)
            m_controller->notifyError(
                    QString("Library '%1' not found!").arg(file));
        return NULL;
    }

    QFileInfo info(file);
    QString lib(info.baseName());
    lib.remove(0, 3); // lib
    // basename has no extension
    // lib.truncate(lib.length() - 3); // .so
    str = lib.toStdString();
   
    get_factory_t get_factory = (get_factory_t) dlsym(handle,
            str.c_str());

    if (!get_factory)
    {
        dlclose (handle);

        if (m_controller)
            m_controller->notifyError(
                    QString("Symbol '%1' not found!").arg(lib));
        return NULL;
    }

    const corbasim::gui::gui_factory_base * factory = get_factory();
    
    if (factory)
    {
        const QString fqn(factory->get_core_factory()->get_fqn());

        m_libraries.insert(std::make_pair(fqn, handle));
        m_factories.insert(std::make_pair(fqn, factory));

        // I don't like this solution because I need to set the list
        // for each new fqn, but...
        m_fqns << fqn;
        m_fqns.removeDuplicates();
        m_fqns_model.setStringList(m_fqns);

        if (m_controller)
            m_controller->notifyMessage(
                    QString("New library loaded '%1' for '%2'")
                    .arg(file).arg(fqn));
    }
    else
    {
        // Impossible is nothing!
        if (m_controller)
            m_controller->notifyError(
                    QString("Erroneus library '%1'!").arg(lib));

        dlclose(handle);
    }

    return factory;
}

void AppModel::clearConfig()
{
   // TODO
}

void AppModel::updateReference(const QString& id,
        const CORBA::Object_var& ref)
{
    objrefs_t::iterator it = m_objrefs.find(id);

    if (it == m_objrefs.end())
    {
        if (m_controller)
            m_controller->notifyError(
                    QString("Object '%1' not found!").arg(id));
    }
    else
    {
        CORBA::Object_var new_ref = it->second->updateReference(ref.in());

        if (CORBA::is_nil(new_ref) && m_controller)
            m_controller->notifyError(
                    QString("Invalid reference for '%1'!").arg(id));

        m_ref_model.appendItem(id, new_ref, NULL);

        if (m_controller)
            m_controller->notifyUpdatedReference(id, new_ref);
    }
}

QAbstractItemModel * AppModel::getFQNModel()
{
    return &m_fqns_model;
}

QAbstractItemModel * AppModel::getReferenceModel()
{
    return &m_ref_model;
}
