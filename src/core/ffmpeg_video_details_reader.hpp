/*
 * Tool for reading data from FFMpeg output
 * Copyright (C) 2017  Michał Walenciak <Kicer86@gmail.com>
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

#ifndef FFMPEGVIDEODETAILSREADER_HPP
#define FFMPEGVIDEODETAILSREADER_HPP

#include <QString>

class FFMpegVideoDetailsReader
{
    public:
        FFMpegVideoDetailsReader(const QString& ffmpegPath);
        FFMpegVideoDetailsReader(const FFMpegVideoDetailsReader &) = delete;
        FFMpegVideoDetailsReader(FFMpegVideoDetailsReader &&) = delete;

        FFMpegVideoDetailsReader& operator=(const FFMpegVideoDetailsReader &) = delete;
        FFMpegVideoDetailsReader& operator=(FFMpegVideoDetailsReader &&) = delete;

        virtual ~FFMpegVideoDetailsReader() = default;

    private:
        const QString m_ffmpegPath;
};

#endif // FFMPEGVIDEODETAILSREADER_HPP
