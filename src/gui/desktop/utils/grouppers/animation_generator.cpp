/*
 * tool for generating gif file from many images
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

#include "animation_generator.hpp"

#include <cmath>

#include <QFile>
#include <QMovie>
#include <QLabel>
#include <QProcess>
#include <QProgressBar>
#include <QRegExp>
#include <QTextStream>

#include <core/cross_thread_call.hpp>
#include <system/system.hpp>

namespace
{
    template<typename T>
    void append(QStringList& list, const T& arg)
    {
        list << arg;
    }

    template<typename T, typename ...Args>
    void append(QStringList& list, const T& arg, Args... args)
    {
        list << arg;
        append(list, args...);
    }

    template<typename ...Args>
    bool execute(const QString& executable, Args... args)
    {
        QStringList arguments;
        append(arguments, args...);

        QProcess pr;
        pr.start(executable, arguments);
        const bool status = pr.waitForFinished(-1);

        return status;
    }

    struct GifGenerator: ITaskExecutor::ITask
    {
        GifGenerator(const AnimationGenerator::Data& data, const QString& location, const std::function<void(const QString &)>& doneCallback):
            m_data(data),
            m_location(location),
            m_doneCallback(doneCallback)
        {
        }

        std::string name() const override
        {
            return "GifGenerator";
        }

        void perform() override
        {
            // stabilize?
            QStringList images_to_be_used;

            if (m_data.stabilize)
            {
                // https://groups.google.com/forum/#!topic/hugin-ptx/gqodoTgAjbI
                // http://wiki.panotools.org/Panorama_scripting_in_a_nutshell

                // generate .pto file
                const QString pto_file_path = System::getTempFilePath() + ".pto";
                execute("pto_gen",
                        "-o", pto_file_path,
                        m_data.photos);

                // find control points
                execute("cpfind",
                        "--fullscale",
                        "-o", pto_file_path,
                        pto_file_path );

                // generate stabilized images
                QFile pto_file(pto_file_path);
                pto_file.open(QIODevice::ReadOnly);
                QTextStream pto_stream(&pto_file);

                std::vector<std::vector<QPoint>> distances;
                while(pto_stream.atEnd() == false)
                {
                    const QString line = pto_stream.readLine();

                    QRegExp regex("^c n0 N(\\d+) x([\\d\\.]+) y([\\d\\.]+) X([\\d\\.]+) Y([\\d\\.]+) t.+$");
                    if (regex.indexIn(line) != -1)
                    {
                        const int right_img = regex.cap(1).toUInt();
                        const double left_x = regex.cap(2).toDouble();
                        const double left_y = regex.cap(3).toDouble();
                        const double right_x = regex.cap(4).toDouble();
                        const double right_y = regex.cap(5).toDouble();

                        const QPoint distance(left_x - right_x, left_y - right_y);

                        while (distances.size() < static_cast<unsigned int>(right_img + 1))
                            distances.push_back(std::vector<QPoint>());

                        distances[right_img].push_back(distance);
                    }
                }

                std::vector<QPoint> avg_points(distances.size());;
                for(std::size_t i = 1; i < distances.size(); i++)
                {
                    std::vector<QPoint>& points = distances[i];
                    std::sort(points.begin(), points.end(), [](const QPoint& lhs, const QPoint& rhs)
                    {
                        const double lhsLength = std::sqrt(std::pow(lhs.x(), 2) + std::pow(lhs.y(), 2));
                        const double rhsLength = std::sqrt(std::pow(rhs.x(), 2) + std::pow(rhs.y(), 2));

                        return lhsLength < rhsLength;
                    });

                    // save average point
                    const QPoint avg_point = points[points.size() / 2];
                    avg_points[i] = avg_point;
                }

                const double max_x = std::max_element(avg_points.begin(), avg_points.end(), [](const QPoint& lhs, const QPoint& rhs)
                {
                    return lhs.x() < rhs.x();
                })->x();

                const double max_y = std::max_element(avg_points.begin(), avg_points.end(), [](const QPoint& lhs, const QPoint& rhs)
                {
                    return lhs.y() < rhs.y();
                })->y();

                const double min_x = std::min_element(avg_points.begin(), avg_points.end(), [](const QPoint& lhs, const QPoint& rhs)
                {
                    return lhs.x() < rhs.x();
                })->x();

                const double min_y = std::min_element(avg_points.begin(), avg_points.end(), [](const QPoint& lhs, const QPoint& rhs)
                {
                    return lhs.y() < rhs.y();
                })->y();

                for(std::size_t i = 0; i < avg_points.size(); i++)
                {
                    QImage img(m_data.photos[i]);
                    const QSize img_size = img.size();

                    const QPoint& own_offset = avg_points[i];
                    const QPoint lt(-own_offset.x() + max_x, -own_offset.y() + max_y);
                    const QPoint rb(-own_offset.x() + img_size.width() + min_x, -own_offset.y() + img_size.height() + min_y);
                    const QRect crop(lt, rb);

                    QImage cropped = img.copy(crop);

                    const QString new_img_path = System::getTempFilePath();
                    cropped.save(new_img_path, "JPEG");

                    images_to_be_used.push_back(new_img_path);
                }
            }
            else
                images_to_be_used = m_data.photos;

            // generate gif
            const int last_photo_delay = (m_data.delay / 1000.0) * 100 + (1 / m_data.fps * 100);
            const QStringList all_but_last = images_to_be_used.mid(0, images_to_be_used.size() - 1);
            const QString last = images_to_be_used.last();

            execute("convert",
                    "-delay", QString::number(1/m_data.fps * 100),   // convert fps to 1/100th of a second
                    all_but_last,
                    "-delay", QString::number(last_photo_delay),
                    last,
                    "-auto-orient",
                    "-loop", "0",
                    "-resize", QString::number(m_data.scale) + "%",
                    m_location);

            m_doneCallback(m_location);
        }

        AnimationGenerator::Data m_data;
        QString m_location;
        std::function<void(const QString &)> m_doneCallback;
    };
}


///////////////////////////////////////////////////////////////////////////////


AnimationGenerator::AnimationGenerator(ITaskExecutor* executor, const std::function<void(QWidget *, const QString &)>& callback):
    m_callback(callback),
    m_movie(),
    m_baseSize(),
    m_executor(executor)
{
}


AnimationGenerator::~AnimationGenerator()
{

}


void AnimationGenerator::generatePreviewWidget(const Data& data)
{
    if (m_movie.get() != nullptr)
        m_movie->stop();

    m_baseSize = QSize();

    const QString location = System::getTempFilePath() + ".gif";

    using namespace std::placeholders;
    std::function<void(const QString &)> doneFun = std::bind(&AnimationGenerator::done, this, _1);
    auto doneCallback = make_cross_thread_function(this, doneFun);

    auto task = std::make_unique<GifGenerator>(data, location, doneCallback);
    m_executor->add(std::move(task));

    QProgressBar* progress = new QProgressBar;
    progress->setRange(0, 0);

    m_callback(progress, QString());
}


void AnimationGenerator::scalePreview(double scale)
{
    if (m_movie.get() != nullptr)
    {
        if (m_baseSize.isValid() == false)
            m_baseSize = m_movie->frameRect().size();

        const double scaleFactor = scale/100;
        QSize size = m_baseSize;
        size.rheight() *= scaleFactor;
        size.rwidth() *= scaleFactor;

        m_movie->setScaledSize(size);
    }
}


void AnimationGenerator::done(const QString& location)
{
    m_movie = std::make_unique<QMovie>(location);
    QLabel* label = new QLabel;

    label->setMovie(m_movie.get());
    m_movie->start();

    m_callback(label, location);
}
