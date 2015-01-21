/*
 * Managing projects
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PROJECTMANAGER_HPP
#define PROJECTMANAGER_HPP

#include "project_utils_export.h"

#include "iproject_manager.hpp"


class QString;

namespace Database
{
    struct IBuilder;
}
class IConfiguration;


class PROJECT_UTILS_EXPORT ProjectManager: public IProjectManager
{
    public:
        ProjectManager();
        ProjectManager(const ProjectManager &) = delete;
        virtual ~ProjectManager();

        void set(Database::IBuilder *);
        void set(IConfiguration *);

        ProjectManager& operator=(const ProjectManager &) = delete;

        ProjectInfo new_prj(const QString &, const Database::IPlugin *) override;
        std::deque<ProjectInfo> listProjects() override;
        std::unique_ptr<IProject> open(const ProjectInfo &) override;
        bool save(const IProject *) override;
        bool remove(const ProjectInfo &) override;

    private:
        Database::IBuilder* m_dbBuilder;
        IConfiguration* m_configuration;
        
        QString getPrjStorage() const;
        ProjectInfo get(const QString& id) const;
        QString getUniqueId() const;

        void initNewProjectDatabase(const ProjectInfo &);
};

#endif // PROJECTMANAGER_HPP