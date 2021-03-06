/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2019 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef WORDANALYSISRESULT_H
#define WORDANALYSISRESULT_H

#include <atomic>

#include <QString>

namespace Common {
    struct WordAnalysisResult {
        WordAnalysisResult():
            m_IsCorrect(true),
            m_HasDuplicates(false)
        { }

        WordAnalysisResult(const QString &stem, bool isCorrect):
            m_Stem(stem),
            m_IsCorrect(isCorrect),
            m_HasDuplicates(false)
        { }

        WordAnalysisResult(const QString &stem, bool isCorrect, bool hasDuplicates):
            m_Stem(stem),
            m_IsCorrect(isCorrect),
            m_HasDuplicates(hasDuplicates)
        { }

        WordAnalysisResult(WordAnalysisResult const &other):
            m_Stem(other.m_Stem)
        {
            m_IsCorrect.store(other.m_IsCorrect);
            m_HasDuplicates.store(other.m_HasDuplicates);
        }

        void reset() {
            m_Stem.clear();
            m_IsCorrect = true;
            m_HasDuplicates = false;
        }

        WordAnalysisResult &operator=(WordAnalysisResult const &other) {
            m_Stem = other.m_Stem;
            m_IsCorrect.store(other.m_IsCorrect);
            m_HasDuplicates.store(other.m_HasDuplicates);
            return *this;
        }

        QString m_Stem;
        std::atomic_bool m_IsCorrect;
        std::atomic_bool m_HasDuplicates;
    };
}

#endif // WORDANALYSISRESULT_H
