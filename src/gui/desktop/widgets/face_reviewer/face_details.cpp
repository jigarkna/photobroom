/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
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

#include "face_details.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include <core/cross_thread_call.hpp>
#include <core/itask_executor.hpp>
#include <core/task_executor_utils.hpp>

using namespace std::placeholders;


FaceDetails::FaceDetails(const QString& name,
                         IModelFaceFinder* finder,
                         ITaskExecutor* executor,
                         const std::vector<PersonInfo>& pi,
                         QWidget* p):
    QGroupBox(name, p),
    m_pi(pi),
    m_executor(executor),
    m_optButton(nullptr),
    m_photo(nullptr),
    m_occurences(nullptr),
    m_modelFaceFinder(finder)
{
    QHBoxLayout* l = new QHBoxLayout(this);
    QVBoxLayout* dl = new QVBoxLayout;
    m_optButton = new QPushButton(tr("Find better"), this);
    m_photo = new QLabel(this);
    m_occurences = new QLabel(this);

    dl->addWidget(m_occurences);
    dl->addWidget(m_optButton);

    l->addWidget(m_photo);
    l->addLayout(dl);
    l->addStretch();

    connect(m_optButton, &QPushButton::clicked, this, &FaceDetails::optimize);

    if (pi.empty() == false)
        apply(m_modelFaceFinder->current(pi.front().p_id));

    m_occurences->setText(tr("On %n photo(s)", "", pi.size()));
}


void FaceDetails::setModelPhoto(const QPixmap& p)
{
    m_photo->setPixmap(p);
}


void FaceDetails::setModelPhoto(const QImage& img)
{
    const QPixmap pixmap = QPixmap::fromImage(img);
    setModelPhoto(pixmap);
}


void FaceDetails::optimize()
{
    m_optButton->clearFocus();
    m_optButton->setDisabled(true);

    std::function<void(const QString &)> callback = std::bind(&FaceDetails::apply, this, _1);
    auto safe_callback = make_cross_thread_function<const QString &>(this, callback);

    m_modelFaceFinder->findBest(m_pi, safe_callback);
}


void FaceDetails::apply(const QString& path)
{
    m_optButton->setEnabled(true);

    if (path.isEmpty() == false)
    {
        runOn(m_executor, [this, path]
        {
            const QImage faceImg(path);
            const QImage scaled = faceImg.scaled(QSize(100, 100), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

            // do not call slot directly - make sure it will be called from main thread
            QMetaObject::invokeMethod(this, "setModelPhoto", Q_ARG(QImage, scaled));
        });
    }
}
