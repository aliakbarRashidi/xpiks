/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2018 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef UICOMMAND_H
#define UICOMMAND_H

#include <QObject>
#include <QQmlEngine>

namespace QMLExtensions {
    class UICommandID : public QObject
    {
        Q_OBJECT
    public:
        explicit UICommandID(QObject *parent = 0): QObject(parent) {}

        // enum values must start with capital letter
        enum CommandID {
            SaveSession,
            SetMasterPassword,
            // -------------------
            EditSelectedArtworks,
            FixSpellingInSelected,
            ReviewDuplicatesInSelected,
            SaveSelected,
            WipeMetadataInSelected,
            RemoveSelected,
            ReimportFromSelected,
            ExportSelectedToCSV,
            FindAndReplaceInSelected,
            UploadSelected,
            // ------------------
            FixSpellingCombined,
            FixSpellingSingle,
            FixSpellingArtwork,
            ShowDuplicatesSingle,
            ShowDuplicatesCombined,
            AcceptPresetCompletionCombined
        };
        Q_ENUMS(CommandID)
    };
}

#endif // UICOMMAND_H
