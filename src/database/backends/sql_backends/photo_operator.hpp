/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2019  Michał Walenciak <Kicer86@gmail.com>
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
 */

#ifndef PHOTO_OPERATOR_HPP
#define PHOTO_OPERATOR_HPP

#include <QString>

#include <database/iphoto_operator.hpp>


struct ILogger;

namespace Database
{
    struct IBackend;
    struct ISqlQueryExecutor;

    class PhotoOperator: public IPhotoOperator
    {
        public:
            PhotoOperator(const QString &, ISqlQueryExecutor *, ILogger *, IBackend *);

            bool removePhoto(const Photo::Id &) override;
            bool removePhotos(const std::vector<IFilter::Ptr> &) override;

        private:
            QString m_connectionName;
            ISqlQueryExecutor* m_executor;
            ILogger* m_logger;
            IBackend* m_backend;
    };
}

#endif // PHOTO_OPERATOR_HPP
