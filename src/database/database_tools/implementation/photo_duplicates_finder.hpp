/*
 * Photo Broom - photos management tool.
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

#ifndef PHOTODUPLICATESFINDER_HPP
#define PHOTODUPLICATESFINDER_HPP

#include <database/iphoto_info.hpp>

namespace Database
{
    struct IDatabase;
}

class PhotoDuplicatesFinder
{
    public:
        PhotoDuplicatesFinder();
        PhotoDuplicatesFinder(const PhotoDuplicatesFinder &) = delete;
        ~PhotoDuplicatesFinder();
        PhotoDuplicatesFinder& operator=(const PhotoDuplicatesFinder &) = delete;

        void setDatabase(Database::IDatabase *);

        bool hasDuplicate(const IPhotoInfo::Ptr &) const;

    private:
        Database::IDatabase* m_database;
};

#endif // PHOTODUPLICATESFINDER_HPP
