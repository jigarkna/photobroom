/*
 * Default (system) icons loader.
 * Copyright (C) 2015  <copyright holder> <email>
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

#include "icons_loader.hpp"

#include <cassert>

#include <QApplication>


IconsLoader::IconsLoader()
{

}


IconsLoader::~IconsLoader()
{

}


QIcon IconsLoader::getIcon(Icon icon) const
{

    static std::map<Icon, std::pair<const char *, QStyle::StandardPixmap>> icons =
    {
        { Icon::New,      {"document-new",        QStyle::SP_FileIcon}                 },
        { Icon::Open,     {"document-open",       QStyle::SP_DirOpenIcon}              },
        { Icon::Quit,     {"application-exit",    QStyle::SP_DialogCloseButton}        },
        { Icon::Close,    {"window-close",        QStyle::SP_DialogCloseButton}        },
        { Icon::Settings, {"applications-system", QStyle::SP_CustomBase}               },
        { Icon::Help,     {"help-contents",       QStyle::QStyle::SP_DialogHelpButton} },
        { Icon::About,    {"help-about",          QStyle::QStyle::SP_DialogHelpButton} },
        { Icon::AboutQt,  {"help-about",          QStyle::QStyle::SP_DialogHelpButton} },
    };

    QIcon result;
    QStyle* style = QApplication::style();

    auto it = icons.find(icon);
    assert(it != icons.end());

    const std::pair<const char *, QStyle::StandardPixmap>& info = it->second;

    if (QIcon::hasThemeIcon(info.first))
        result = QIcon::fromTheme(info.first);
    else if (info.second != QStyle::SP_CustomBase)
        result = style->standardIcon(info.second);

    return result;
}