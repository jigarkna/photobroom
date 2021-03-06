/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2016  Michał Walenciak <Kicer86@gmail.com>
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

#include "selection_extractor.hpp"

#include <cassert>

#include <QItemSelectionModel>

#include "models/db_data_model.hpp"


SelectionExtractor::SelectionExtractor():
    m_selectionModel(nullptr),
    m_photosModel(nullptr)
{
}


SelectionExtractor::~SelectionExtractor()
{
}


void SelectionExtractor::set(QItemSelectionModel* selectionModel)
{
    if (m_selectionModel != nullptr)
        m_selectionModel->disconnect(this);

    m_selectionModel = selectionModel;
    connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &SelectionExtractor::selectionChanged);
}


void SelectionExtractor::set(DBDataModel* model)
{
    m_photosModel = model;
}


std::vector<Photo::Data> SelectionExtractor::getSelection() const
{
    std::vector<Photo::Data> result;

    QItemSelection selection = m_selectionModel->selection();

    for (const QItemSelectionRange& range : selection)
    {
        QModelIndexList idxList = range.indexes();

        for (const QModelIndex& idx : idxList)
            if (m_photosModel->isLeaf(idx))
            {
                const Photo::Data& photo = m_photosModel->getPhoto(idx);

                result.push_back(photo);
            }
    }

    return result;
}
