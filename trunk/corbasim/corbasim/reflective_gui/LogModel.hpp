// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * LogModel.hpp
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

#ifndef CORBASIM_REFLECTIVE_GUI_LOGMODEL_HPP
#define CORBASIM_REFLECTIVE_GUI_LOGMODEL_HPP

#include <map>

#include <QStandardItemModel>
#include <corbasim/qt/types.hpp>
#include <corbasim/core/reflective_fwd.hpp>

namespace corbasim 
{
namespace reflective_gui
{

class LogModel : public QStandardItemModel
{
    Q_OBJECT
    Q_PROPERTY(int maxEntries READ maxEntries WRITE setMaxEntries)
public:
    LogModel(QObject * parent = 0);
    virtual ~LogModel();

    QVariant data(const QModelIndex& index, 
            int role = Qt::DisplayRole) const; 

public slots:

    void clearLog();

    int maxEntries() const;
    void setMaxEntries(int max);

    void registerInstance(const QString& id,
        const corbasim::core::interface_reflective_base * factory);
    void unregisterInstance(const QString& id);

    void inputRequest(const QString& id, 
            corbasim::event::request_ptr req,
            corbasim::event::event_ptr resp);
    void outputRequest(const QString& id, 
            corbasim::event::request_ptr req,
            corbasim::event::event_ptr resp);

protected:

    QStandardItem * append(const QString& id, 
            corbasim::event::request_ptr req,
            corbasim::event::event_ptr resp,
            bool is_in);

    int m_maxEntries;

    typedef std::map< QString, 
            core::interface_reflective_base const * > instances_t;
    instances_t m_instances;

    QIcon m_inputIcon;
    QIcon m_outputIcon;

    struct LogEntry
    {
        bool is_in_entry;
        QString id;
        corbasim::event::request_ptr req;
        corbasim::event::event_ptr resp;
    };

    QList< LogEntry > m_entries;
};

} // namespace qt
} // namespace corbasim

#endif /* CORBASIM_REFLECTIVE_GUI_LOGMODEL_HPP */
