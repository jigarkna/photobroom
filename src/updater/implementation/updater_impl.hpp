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

#ifndef UPDATERIMPL_HPP
#define UPDATERIMPL_HPP

#include <memory>

#include <QObject>
#include <QNetworkReply>

class QNetworkAccessManager;
class QJsonDocument;

namespace GitHub
{
    struct AConnection;
    class Request;
}

class UpdaterImpl : public QObject
{
        Q_OBJECT

    public:
        UpdaterImpl();
        UpdaterImpl(const UpdaterImpl &) = delete;
        ~UpdaterImpl();

        UpdaterImpl& operator=(const UpdaterImpl &) = delete;

    public slots:
        void checkVersion();

    private:
        std::unique_ptr<QNetworkAccessManager> m_manager;
        std::unique_ptr<GitHub::AConnection> m_connection;
        std::unique_ptr<GitHub::Request> m_request;

        QString releaseVersion(const QJsonObject &) const;
};

#endif // UPDATERIMPL_HPP
