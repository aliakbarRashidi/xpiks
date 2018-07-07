/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2018 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ARTWORKSHELPERS_H
#define ARTWORKSHELPERS_H

#include <QVector>
#include <vector>
#include <memory>

namespace Artworks {
    class ArtworkMetadata;
    class ArtworkMetadataLocker;
    class ImageArtwork;
    class VideoArtwork;
    class ArtworksSnapshot;
}

namespace Helpers {
    void splitImagesVideo(const QVector<Artworks::ArtworkMetadata *> &artworks, QVector<Artworks::ArtworkMetadata *> &imageArtworks, QVector<Artworks::ArtworkMetadata *> &videoArtworks);
    void splitImagesVideo(const std::vector<std::shared_ptr<Artworks::ArtworkMetadataLocker> > &rawSnapshot,
                          std::vector<std::shared_ptr<Artworks::ArtworkMetadataLocker> > &imagesRawSnapshot,
                          std::vector<std::shared_ptr<Artworks::ArtworkMetadataLocker> > &videoRawSnapshot);
    int retrieveImagesCount(const std::vector<std::shared_ptr<Artworks::ArtworkMetadataLocker> > &rawSnapshot);
    int retrieveVideosCount(const std::vector<std::shared_ptr<Artworks::ArtworkMetadataLocker> > &rawSnapshot);
    int findAndAttachVectors(const Artworks::ArtworksSnapshot &snapshot, QVector<int> &modifiedIndices);
}

#endif // ARTWORKSHELPERS_H
