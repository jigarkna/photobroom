/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "database_builder.hpp"

#include <cassert>
#include <fstream>
#include <map>
#include <memory>

#include <core/ilogger.hpp>
#include <core/ilogger_factory.hpp>
#include <plugins/plugin_loader.hpp>

#include "async_database.hpp"
#include "photo_info_cache.hpp"
#include "idatabase_plugin.hpp"
#include "idatabase.hpp"
#include "ibackend.hpp"
#include "photo_info.hpp"
#include "project_info.hpp"

//TODO: cleanup this file!


namespace Database
{

    const char* databaseLocation = "Database::Backend::DataLocation";

    struct Builder::Impl
    {
        Impl(const Impl &) = delete;

        Impl& operator=(const Impl &) = delete;

        IPluginLoader* pluginLoader;
        ILoggerFactory* m_logger_factory;
        IConfiguration* m_configuration;

        Impl(): pluginLoader(nullptr), m_logger_factory(nullptr), m_configuration(nullptr)
        {}

    };


    Builder::Builder(): m_impl(new Impl)
    {

    }


    Builder::~Builder()
    {

    }


    void Builder::set(IPluginLoader* pluginLoader)
    {
        m_impl->pluginLoader = pluginLoader;
    }


    void Builder::set(ILoggerFactory* logger)
    {
        m_impl->m_logger_factory = logger;
    }


    void Builder::set(IConfiguration* configuration)
    {
        m_impl->m_configuration = configuration;
    }


    std::unique_ptr<IDatabase> Builder::get(const ProjectInfo& info)
    {
        Database::IPlugin* plugin = m_impl->pluginLoader->getDBPlugin(info.backendName);
        assert(plugin);

        auto logger = m_impl->m_logger_factory->get("Database");

        std::unique_ptr<IBackend> backend = plugin->constructBackend(m_impl->m_configuration, logger.get());

        auto cache = std::make_unique<PhotoInfoCache>(logger.get());
        auto database = std::make_unique<AsyncDatabase>(std::move(backend), std::move(cache), logger.get());

        // TODO: added due to bug in clang: http://stackoverflow.com/questions/36752678/clang-returning-stdunique-ptr-with-type-conversion
        return std::move(database);
    }

}
