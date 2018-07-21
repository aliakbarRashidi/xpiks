/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2018 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "artworksinspectionhub.h"
#include <Services/Warnings/warningsservice.h>
#include <Services/SpellCheck/spellcheckservice.h>
#include <Models/settingsmodel.h>

namespace Services {
    ArtworksInspectionHub::ArtworksInspectionHub(SpellCheck::SpellCheckService &spellCheckService,
                                                 Warnings::WarningsService &warningsService,
                                                 Models::SettingsModel &settingsModel):
        m_SpellCheckService(spellCheckService),
        m_WarningsService(warningsService),
        m_SettingsModel(settingsModel)
    {
    }

    void ArtworksInspectionHub::handleMessage(const Artworks::ArtworkMetadata *&change) {
        inspectArtwork(change);
    }

    void ArtworksInspectionHub::handleMessage(const Artworks::BasicKeywordsModel *&change) {
        inspectBasicModel(change);
    }

    void ArtworksInspectionHub::inspectArtwork(const Artworks::ArtworkMetadata *artwork) {
        if (isSpellCheckAvailable()) {
            m_SpellCheckService.submitArtwork(artwork);
        } else {
            m_WarningsService.submitItem(artwork);
        }
    }

    void ArtworksInspectionHub::inspectArtworks(const Artworks::ArtworksSnapshot &snapshot) {
        if (isSpellCheckAvailable()) {
            m_SpellCheckService.submitArtworks(snapshot);
        } else {
            m_WarningsService.submitItems(snapshot);
        }
    }

    void ArtworksInspectionHub::inspectBasicModel(const Artworks::BasicKeywordsModel *basicModel) {
        if (isSpellCheckAvailable()) {
            m_SpellCheckService.submitItem(basicModel, Common::SpellCheckFlags::All);
        }
    }

    bool ArtworksInspectionHub::isSpellCheckAvailable() const {
        return m_SettingsModel.getUseSpellCheck();
    }
}
