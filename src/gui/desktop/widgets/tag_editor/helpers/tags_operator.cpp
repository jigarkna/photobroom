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

#include "tags_operator.hpp"

#include <QObject>


TagsOperator::TagsOperator(): m_photos()
{

}


void TagsOperator::operateOn(const std::vector<IPhotoInfo::Ptr>& photos)
{
    m_photos = photos;
}


Tag::TagsList TagsOperator::getTags() const
{
    // TODO: std::tie?
    struct
    {
        bool operator()(const std::pair<TagTypeInfo, TagValue> &a,
                        const std::pair<TagTypeInfo, TagValue> &b) const
        {
            if (a.first < b.first)
                return true;
            else if (a.first > b.first)
                return false;
            else
                return a.second < b.second;
        }

    } tagsComparer;

    Tag::TagsList commonTags;
    bool first = true;

    for (const auto& photo: m_photos)
    {
        const Tag::TagsList tags = photo->getTags();

        if (first)
            commonTags = tags, first = false;
        else
        {
            Tag::TagsList intersection;
            std::set_intersection(commonTags.begin(), commonTags.end(),
                                  tags.begin(), tags.end(),
                                  std::inserter(intersection, intersection.end()), tagsComparer);

            commonTags = intersection;
        }
    }

    return commonTags;
}


void TagsOperator::setTag(const TagTypeInfo& name, const TagValue& values)
{
    for (auto& photo: m_photos)
        photo->setTag(name, values);
}


void TagsOperator::setTags(const Tag::TagsList& tags)
{
    for (auto& photo: m_photos)
        photo->setTags(tags);
}


void TagsOperator::insert(const TagTypeInfo& name, const TagValue& value)
{
    //find tag for given name
    Tag::TagsList tags = getTags();
    bool updated = false;

    for(Tag::Info info: tags) // TODO: use std::find_if
    {
        if (info.getTypeInfo() == name)
        {
            const bool differs = info.value() != value;

            if (differs)
            {
                info.setValue(value);
                setTag(name, info.value());
            }

            updated = true;
            break;
        }
    }

    if (updated == false)
        setTag(name, value);
}
