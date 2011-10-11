// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * factory_fwd.hpp
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

#ifndef CORBASIM_CORE_FACTORY_FWD_HPP
#define CORBASIM_CORE_FACTORY_FWD_HPP

#include <vector>
#include <string>
#include <map>
#include <corbasim/event.hpp>

namespace corbasim 
{
namespace core 
{
class request_serializer_base;
} // namespace core

namespace core 
{

struct operation_factory_base
{
    virtual const char * get_name() const = 0;
    virtual void to_json(event::request* req, std::string& str) const = 0;
    virtual event::request* from_json(const std::string& str) const = 0;
    virtual ~operation_factory_base();
};

struct factory_base
{
    unsigned int operation_count() const;
    operation_factory_base * get_factory_by_index(unsigned int idx) const;
    operation_factory_base * get_factory_by_name(
            const std::string& name) const;
    operation_factory_base * get_factory_by_tag(tag_t tag) const;

    virtual ~factory_base();
    virtual core::request_serializer_base * get_serializer() const = 0;

    void insert_factory(const std::string& name,
            tag_t tag, operation_factory_base * factory);

    // Data
    typedef std::vector< operation_factory_base * > factories_t;
    factories_t m_factories;

    typedef std::map< std::string, operation_factory_base * > 
        factories_by_name_t;
    factories_by_name_t m_factories_by_name;

    typedef std::map< tag_t, operation_factory_base * > 
        factories_by_tag_t;
    factories_by_tag_t m_factories_by_tag;
};

} // namespace core
} // namespace corbasim

#endif /* CORBASIM_CORE_FACTORY_FWD_HPP */
