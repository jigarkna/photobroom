/*
 * Factory for QCompleter for particular tag type
 * Copyright (C) 2016  Michał Walenciak <MichalWalenciak@gmail.com>
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

#include "completer_factory.hpp"

#include <QCompleter>

#include <utils/tag_value_model.hpp>


CompleterFactory::CompleterFactory(): m_tagValueModels()
{

}


CompleterFactory::~CompleterFactory()
{

}


QCompleter* CompleterFactory::createCompleter(const TagNameInfo& info)
{
    auto it = m_tagValueModels.find(info.getType());

    if (it == m_tagValueModels.end())
    {
        auto model = std::make_unique<TagValueModel>(info);

        auto iit = m_tagValueModels.insert( std::make_pair(info.getType(), std::move(model)) );

        it = iit.first;
    }

    QCompleter* result = new QCompleter(it->second.get());
    return result;
}
