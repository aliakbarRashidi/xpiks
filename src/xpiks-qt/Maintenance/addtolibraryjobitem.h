/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
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

#ifndef ADDTOLIBRARYJOBITEM_H
#define ADDTOLIBRARYJOBITEM_H

#include <QVector>
#include "imaintenanceitem.h"

namespace Models {
    class ArtworkMetadata;
}

namespace Suggestion {
    class LocalLibrary;
}

namespace Maintenance {
    class AddToLibraryJobItem : public IMaintenanceItem
    {
    public:
        AddToLibraryJobItem(const QVector<Models::ArtworkMetadata *> artworksList, Suggestion::LocalLibrary *localLibrary);

    public:
        void doAddToLibrary();
        virtual void processJob() override;

    private:
        QVector<Models::ArtworkMetadata *> m_ArtworksList;
        Suggestion::LocalLibrary *m_LocalLibrary;
    };
}

#endif // ADDTOLIBRARYJOBITEM_H
