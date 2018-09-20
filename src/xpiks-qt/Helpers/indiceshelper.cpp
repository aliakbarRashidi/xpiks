/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2018 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "indiceshelper.h"
#include <cmath>
#include <algorithm>

namespace Artworks {
    class ArtworkMetadata;
}

namespace Helpers {
    template<class T>
    int splitIntoChunks(const QVector<T> &items, int chunksCount, QVector<QVector<T> > &chunks) {
        int size = items.size();

        if (items.isEmpty() || (chunksCount == 0) || (chunksCount > size)) { return 0; }

        if (chunksCount == 1) {
            chunks << items;
            return 1;
        }

        int chunkSize = ceil((size + 0.0) / chunksCount);

        int left = chunkSize;
        while (left < size) {
            chunks.push_back(items.mid(left - chunkSize, chunkSize));
            left += chunkSize;
        }

        if (left - chunkSize < size) {
            chunks.push_back(items.mid(left - chunkSize, chunkSize));
        }

        return chunks.length();
    }

    template
    int splitIntoChunks<Artworks::ArtworkMetadata*>(const QVector<Artworks::ArtworkMetadata*> &items,
    int chunksCount, QVector<QVector<Artworks::ArtworkMetadata*> > &chunks);

#ifdef CORE_TESTS
    template
    int splitIntoChunks<int>(const QVector<int> &items, int chunksCount, QVector<QVector<int> > &chunks);
#endif

    bool segmentsOverlap(const std::pair<int, int> &a, const std::pair<int, int> &b) {
        if (a.first <= b.first) {
            return b.first <= a.second;
        } else {
            return a.first <= b.second;
        }
    }

    std::pair<int, int> unionOverlappingSegments(const std::pair<int, int> &a, const std::pair<int, int> &b) {
        Q_ASSERT(segmentsOverlap(a, b));
        return std::make_pair(std::min(a.first, b.first), std::max(a.second, b.second));
    }

    RangesVector unionRanges(RangesVector &ranges) {
        if (ranges.size() <= 1) { return ranges; }

        std::sort(std::begin(ranges), std::end(ranges),
                  [](const std::pair<int, int> &a, const std::pair<int, int> &b) {
            return (a.first == b.first) ? (a.second < b.second) : (a.first < b.first);
        });

        auto prev = ranges.at(0);
        size_t size = ranges.size();

        RangesVector results;
        results.reserve(size);

        for (size_t i = 1; i < size; ++i) {
            auto &curr = ranges.at(i);

            if (segmentsOverlap(prev, curr)) {
                prev = unionOverlappingSegments(prev, curr);
            } else {
                results.emplace_back(prev);
                prev = curr;
            }
        }

        results.emplace_back(prev);

        return results;
    }
}
