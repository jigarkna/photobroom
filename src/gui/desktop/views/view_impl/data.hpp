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

#ifndef DATA_HPP
#define DATA_HPP

#include <functional>
#include <memory>
#include <vector>

#include <QRect>
#include <QModelIndex>

#include "model_index_info.hpp"
#include "view_data_set.hpp"

struct IConfiguration;
class APhotoInfoModel;


class Data: public QObject
{
    public:
        Data();
        Data(const Data &) = delete;

        ~Data();
        Data& operator=(const Data &) = delete;

        void set(APhotoInfoModel *);

        void setSpacing(int);
        void setImageMargin(int);
        void setThumbnailDesiredHeight(int);

        const ModelIndexInfo& get(const QModelIndex &) const;
        ModelIndexInfo& get(const QModelIndex &);
        bool has(const QModelIndex &) const;

        QModelIndex get(const QPoint &) const;
        bool isImage(const QModelIndex &) const;
        QSize getImageSize(const QModelIndex &) const;
        QSize getThumbnailSize(const QModelIndex &) const;
        std::vector<QModelIndex> findInRect(const QRect &) const;

        bool isExpanded(const QModelIndex &) const;
        bool isVisible(const QModelIndex &) const;

        const QAbstractItemModel* getQtModel() const;

        void for_each(const std::function<void(ModelIndexInfo &)> &);

        std::size_t size() const;

        int getSpacing() const;
        int getImageMargin() const;
        int getThumbnailDesiredHeight() const;

        IConfiguration* getConfig();

        //getting siblings
        QModelIndex getRightOf(const QModelIndex &) const;
        QModelIndex getLeftOf(const QModelIndex &) const;
        QModelIndex getTopOf(const QModelIndex &) const;
        QModelIndex getBottomOf(const QModelIndex &) const;

        QModelIndex getFirst(const QModelIndex &) const;
        QModelIndex getLast(const QModelIndex &) const;

        void modelReset();
        void rowsAboutToBeRemoved(const QModelIndex & , int , int);
        void rowsRemoved(const QModelIndex &, int, int);
        void rowsInserted(const QModelIndex & , int , int);

    private:
        typedef ViewDataSet<ModelIndexInfo> ModelIndexInfoSet;

        std::unique_ptr<ModelIndexInfoSet> m_itemData;
        APhotoInfoModel* m_model;
        IConfiguration* m_configuration;
        int m_spacing;
        int m_margin;
        int m_thumbHeight;

        std::vector<QModelIndex> findInRect(const QRect &, const QModelIndex &) const;
};

#endif // DATA_HPP
