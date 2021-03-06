/*
 * Photo Broom - photos management tool.
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

#ifndef FEATURESMANAGER_HPP
#define FEATURESMANAGER_HPP

#include <memory>
#include <set>
#include <vector>

#include "ifeatures_manager.hpp"

#include "core_export.h"

struct ILogger;
struct ILoggerFactory;


class CORE_EXPORT FeaturesManager final: public IFeaturesManager
{
    public:
        FeaturesManager(ILoggerFactory *);
        ~FeaturesManager();

        void detect() override;
        void add(IFeatureDetector *) override;

        bool has(const QString &) const override;

    private:
        std::vector<IFeatureDetector *> m_detectors;
        std::set<QString> m_features;
        std::unique_ptr<ILogger> m_logger;
};

#endif // FEATURESMANAGER_HPP
