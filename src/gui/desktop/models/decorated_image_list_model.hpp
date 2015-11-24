/*
 * Extension for ImageListModel which will gather extra info about inserted photo paths
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

#ifndef DECORATEDIMAGELISTMODEL_HPP
#define DECORATEDIMAGELISTMODEL_HPP

#include "image_list_model.hpp"

class DecoratedImageListModel: public ImageListModel
{
    public:
        DecoratedImageListModel(QObject *);
        DecoratedImageListModel(const DecoratedImageListModel &) = delete;
        ~DecoratedImageListModel();
        DecoratedImageListModel& operator=(const DecoratedImageListModel &) = delete;

        void insert(const QString &);

        // QAbstractItemModel interface:
        QVariant data(const QModelIndex& index, int role) const override;
};

#endif // DECORATEDIMAGELISTMODEL_HPP
