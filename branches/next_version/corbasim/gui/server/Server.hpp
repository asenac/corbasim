// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * Server.hpp
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

#ifndef CORBASIM_GUI_SERVER_HPP
#define CORBASIM_GUI_SERVER_HPP

#include <corbasim/gui/export.hpp>
#include <corbasim/gui/types.hpp>
#include <corbasim/gui/Model.hpp>
#include <corbasim/gui/item/LogModel.hpp>
#include <corbasim/gui/item/ApplicationLogModel.hpp>
#include <corbasim/gui/item/InstanceModel.hpp>

#include <QtGui>

namespace corbasim 
{
namespace gui 
{

class FilteredLogView;
class OperationSequenceTool;
class SenderSequenceTool;
class DumpTool;

class CORBASIM_GUI_DECLSPEC Server : public QMainWindow
{
    Q_OBJECT
public:
    Server(QWidget * parent = 0);
    virtual ~Server();

public slots:

    // Notificaciones del controlador

    void objrefCreated(Objref_ptr objref);
    void objrefDeleted(ObjectId id);

    void servantCreated(Objref_ptr servant);
    void servantDeleted(ObjectId id);

    void displayError(const QString& err);
    void displayMessage(const QString& msg);

    void selectedOperation(Objref_ptr, OperationDescriptor_ptr);

signals:

    void createObjref(const ObjrefConfig&);
    void deleteObjref(ObjectId);

protected slots:

protected:

    LogModel m_logModel;
    ApplicationLogModel m_appLogModel;
    InstanceModel m_instanceModel;

    ObjrefRepository m_objrefs;

    QTabWidget * m_tabs;

    FilteredLogView * m_view;
    OperationSequenceTool * m_seqTool;
    SenderSequenceTool * m_senderSeqTool;
    DumpTool * m_dumpInput;

    int m_seqIdx;
    int m_senderIdx;
};

} // namespace gui
} // namespace corbasim

#endif /* CORBASIM_GUI_SERVER_HPP */

