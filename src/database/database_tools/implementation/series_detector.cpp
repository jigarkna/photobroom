
/*
 * Tool for series detection
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

#include "../series_detector.hpp"

#include <QDateTime>

#include <core/iexif_reader.hpp>
#include <ibackend.hpp>


namespace
{
    qint64 timestamp(const Photo::Data& data)
    {
        qint64 timestamp = -1;

        const auto dateIt = data.tags.find(TagNameInfo(BaseTagsList::Date));

        if (dateIt != data.tags.end())
        {
            const QDate date = dateIt->second.getDate();
            const auto timeIt = data.tags.find(TagNameInfo(BaseTagsList::Time));
            const QTime time = timeIt != data.tags.end()? timeIt->second.getTime(): QTime();
            const QDateTime dateTime(date, time);

            timestamp = dateTime.toMSecsSinceEpoch();
        }

        return timestamp;
    }
}


SeriesDetector::SeriesDetector(Database::IBackend* backend, IExifReader* exif):
    m_backend(backend), m_exifReader(exif)
{

}


std::vector<SeriesDetector::Detection> SeriesDetector::listDetections() const
{
    std::vector<Detection> result;

    const auto photos = m_backend->getAllPhotos();

    std::map<qint64, std::tuple<int, Photo::Id>> sequences_by_time;

    for (const Photo::Id& id: photos)
    {
        const Photo::Data data = m_backend->getPhoto(id);
        const std::optional<std::any> seq = m_exifReader->get(data.path, IExifReader::TagType::SequenceNumber);

        if (seq)
        {
            const std::any& rawData = *seq;
            const int seqNum = std::any_cast<int>(rawData);
            const qint64 time = timestamp(data);

            if (time != -1)
                sequences_by_time.emplace(time, std::make_tuple(seqNum, id));
        }
    }

    result = process(sequences_by_time);

    return result;
}


std::vector<SeriesDetector::Detection> SeriesDetector::process(const std::map<qint64, std::tuple<int, Photo::Id>>& data) const
{
    int expected_seq = 0;
    std::vector<Photo::Id> group;

    std::vector<Detection> results;

    for(auto it = data.cbegin(); it != data.cend(); ++it)
    {
        const int seqNum = std::get<0>(it->second);
        const Photo::Id& ph_id = std::get<1>(it->second);

        if (seqNum != expected_seq || std::next(it) == data.cend())  // sequenceNumber does not match expectations? finish/skip group
        {
            if (group.empty() == false)
            {
                Detection detection;
                detection.type = Group::Type::Animation;
                detection.members = group;

                results.push_back(detection);
            }

            group.clear();
            expected_seq = 0;
        }

        if (seqNum == expected_seq)         // sequenceNumber matches expectations? begin/continue group
        {
            group.push_back(ph_id);
            expected_seq++;
        }
    }

    return results;
}
