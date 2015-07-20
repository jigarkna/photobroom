/*
 * Dialog for photos adding.
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
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



#include "photos_add_dialog.hpp"
#include "ui_photos_add_dialog.h"


PhotosAddDialog::PhotosAddDialog(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::PhotosAddDialog)
{
    ui->setupUi(this);
}

PhotosAddDialog::~PhotosAddDialog()
{
    delete ui;
}
