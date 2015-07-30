/*
 * Flat list model with lazy image load.
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

#ifndef IMAGELISTMODELPRIVATE_HPP
#define IMAGELISTMODELPRIVATE_HPP

#include <deque>

#include <QPixmap>

#include <OpenLibrary/putils/ts_queue.hpp>


class ITaskExecutor;

struct Info
{
    QString path;
    QPixmap pixmap;
    QString filename;

    Info(const QString& p): path(p), pixmap(), filename() {}
};


class ImageListModelPrivate
{
    public:
        ImageListModelPrivate(ImageListModel* q);
        virtual ~ImageListModelPrivate();

        std::deque<Info> m_data;
        ITaskExecutor* m_taskExecutor;

    private:
        class ImageListModel* const q;
};

#endif // IMAGELISTMODELPRIVATE_HPP
