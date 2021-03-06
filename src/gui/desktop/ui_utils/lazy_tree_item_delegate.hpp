/*
 * Photo Broom - photos management tool.
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

#ifndef LAZYTREEITEMDELEGATE_HPP
#define LAZYTREEITEMDELEGATE_HPP

#include <QCache>

#include <database/group.hpp>
#include <database/idatabase.hpp>
#include "views/tree_item_delegate.hpp"

struct IImagesSource
{
    virtual ~IImagesSource() = default;

    virtual QImage image(const QModelIndex &, const QSize &) = 0;
};


class LazyTreeItemDelegate: public TreeItemDelegate
{
        Q_OBJECT

    public:
        LazyTreeItemDelegate(ImagesTreeView *);
        LazyTreeItemDelegate(const LazyTreeItemDelegate &) = delete;
        ~LazyTreeItemDelegate();

        LazyTreeItemDelegate& operator=(const LazyTreeItemDelegate &) = delete;

        void set(IImagesSource *);
        void set(Database::IDatabase *);

        // TreeItemDelegate:
        QImage getImage(const QModelIndex &, const QSize &) const override;

    private:
        typedef QCache<Group::Id, Group::Type> Cache;

        IImagesSource* m_imagesSource;
        mutable Cache m_groupCache;
        Database::IDatabase* m_db;

        Group::Type getGroupTypeFor(const Group::Id &) const;
};

#endif // LAZYTREEITEMDELEGATE_HPP
