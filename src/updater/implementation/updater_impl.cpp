/*
 * Updater's private implementation
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
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

#include "updater_impl.hpp"

#include <cassert>
#include <set>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "github_api/github_api.hpp"
#include "github_api/aconnection.hpp"
#include "github_api/request.hpp"

#include "config.hpp"

#include "version.hpp"


UpdaterImpl::UpdaterImpl(): m_manager(new QNetworkAccessManager), m_connection(nullptr), m_request(nullptr)
{
    GitHubApi api;
    api.set(m_manager.get());

    m_connection = api.connect("8e47abda51d9e3515acf5c22c8278204d5206610");
    m_request.reset( new GitHub::Request(m_connection.get()) );
}


UpdaterImpl::~UpdaterImpl()
{

}


void UpdaterImpl::checkVersion()
{
    const Version currentVersion = Version::fromString(PHOTO_BROOM_VERSION);
    const QJsonDocument& doc = m_request->getReleases("Kicer86", "photobroom");
    std::set<Version> versions;

    QJsonArray releases = doc.array();
    for(const QJsonValueRef& release_ref: releases)
    {
        assert(release_ref.isObject());
        const QJsonObject release = release_ref.toObject();

        const QString releaseVer = releaseVersion(release);
        const Version ver = Version::fromTagName(releaseVer);

        versions.insert(ver);
    }

    if (versions.empty() == false)
    {
        const Version& onlineVersion = *versions.rbegin();
        if (onlineVersion > currentVersion)
        {

        }
    }
}


QString UpdaterImpl::releaseVersion(const QJsonObject& release) const
{
    QString result;

    auto draft_it = release.find("draft");
    const bool is_draft = draft_it->toBool(false);

    auto prerelease_it = release.find("prerelease");
    const bool is_prerelease = prerelease_it->toBool(false);

    if (is_draft == false && is_prerelease == false)
    {
        auto tag_name_it = release.find("tag_name");
        const QString tag_name = tag_name_it->toString();

        result = tag_name;
    }

    return result;
}
