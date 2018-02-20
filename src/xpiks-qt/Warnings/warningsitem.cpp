/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2018 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "warningsitem.h"
#include <QStringList>
#include <QString>
#include <QSet>
#include <QFileInfo>
#include <QSize>
#include "../Helpers/stringhelper.h"
#include "../Common/flags.h"
#include "../Models/artworkmetadata.h"
#include "../Common/defines.h"
#include "../Models/imageartwork.h"
#include "../Models/videoartwork.h"
#include "iwarningssettings.h"

namespace Warnings {
    QSet<QString> toLowerSet(const QStringList &from) {
        QSet<QString> result;
        result.reserve(from.length());
        foreach(const QString &str, from) {
            result.insert(str.toLower());
        }
        return result;
    }

    WarningsItem::WarningsItem(Models::ArtworkMetadata *checkableItem, Common::WarningsCheckFlags checkingFlags):
        m_CheckableItem(checkableItem),
        m_CheckingFlags(checkingFlags),
        m_FlagsToSet((Common::WarningFlags)0),
        m_FlagsToDrop((Common::WarningFlags)0)
    {
        checkableItem->acquire();
    }

    WarningsItem::~WarningsItem() {
        if (m_CheckableItem->release()) {
            LOG_WARNING << "Item #" << m_CheckableItem->getItemID() << "could have been removed";
        }
    }

    void WarningsItem::checkWarnings(IWarningsSettings *warningsSettings) {
        Q_ASSERT(warningsSettings != nullptr);

        if (needCheckAll()) {
            checkImageProperties(warningsSettings);
            checkVideoProperties(warningsSettings);
            checkFilename(warningsSettings);
        }

        checkDescription(warningsSettings);
        checkTitle(warningsSettings);
        checkKeywords(warningsSettings);
        checkSpelling(warningsSettings);
        checkDuplicates(warningsSettings);

        submitWarnings();
    }

    void WarningsItem::submitWarnings() {
        if (m_CheckingFlags == Common::WarningsCheckFlags::All) {
            m_CheckableItem->setWarningsFlags((Common::flag_t)m_FlagsToSet);
        } else {
            m_CheckableItem->dropWarningsFlags((Common::flag_t)m_FlagsToDrop);
            m_CheckableItem->addWarningsFlags((Common::flag_t)m_FlagsToSet);
        }
    }

    void WarningsItem::checkImageProperties(IWarningsSettings *warningsSettings) {
        Models::ArtworkMetadata *item = m_CheckableItem;
        Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork *>(item);
        if (image == nullptr) {
            dropFlag(Common::WarningFlags::SizeLessThanMinimum);
            dropFlag(Common::WarningFlags::ImageFileIsTooBig);
            return;
        }

        const double minimumMegapixels = warningsSettings->getMinMegapixels();

        QSize size = image->getImageSize();
        const double currentProd = size.width() * size.height() / 1000000.0;
        const bool sizeNotEnoughValue = (0.1 < currentProd) && (currentProd < minimumMegapixels);
        accountFlag(Common::WarningFlags::SizeLessThanMinimum, sizeNotEnoughValue);

        qint64 filesize = item->getFileSize();
        double filesizeMB = (double)filesize;
        filesizeMB /= (1024.0*1024.0);

        const double maxImageFileSizeMB = warningsSettings->getMaxImageFilesizeMB();
        const bool imageTooBigValue = (filesizeMB >= maxImageFileSizeMB);
        accountFlag(Common::WarningFlags::ImageFileIsTooBig, imageTooBigValue);
    }

    void WarningsItem::checkVideoProperties(IWarningsSettings *warningsSettings) {
        Models::ArtworkMetadata *item = m_CheckableItem;
        Models::VideoArtwork *video = dynamic_cast<Models::VideoArtwork *>(item);

        if (video == nullptr) {
            dropFlag(Common::WarningFlags::VideoFileIsTooBig);
            dropFlag(Common::WarningFlags::VideoIsTooLong);
            dropFlag(Common::WarningFlags::VideoIsTooShort);
            return;
        }

        qint64 filesize = item->getFileSize();
        double filesizeMB = (double)filesize;
        filesizeMB /= (1024.0*1024.0);

        double maxVideoFileSizeMB = warningsSettings->getMaxVideoFilesizeMB();
        const bool videoTooBigValue = (filesizeMB >= maxVideoFileSizeMB);
        accountFlag(Common::WarningFlags::VideoFileIsTooBig, videoTooBigValue);

        const double duration = video->getDuration();
        const double maxDuration = warningsSettings->getMaxVideoDurationSeconds();
        const double minDuration = warningsSettings->getMinVideoDurationSeconds();

        const bool durationTooLongValue = (duration > maxDuration);
        accountFlag(Common::WarningFlags::VideoIsTooLong, durationTooLongValue);

        const bool durationTooShortValue = (duration < minDuration);
        accountFlag(Common::WarningFlags::VideoIsTooShort, durationTooShortValue);
    }

    void WarningsItem::checkFilename(IWarningsSettings *warningsSettings) {
        const QString &allowedFilenameCharacters = warningsSettings->getAllowedFilenameCharacters();

        QFileInfo fi(m_CheckableItem->getFilepath());
        QString filename = fi.fileName();

        const int length = filename.length();
        bool anyBadSymbol = false;
        for (int i = 0; i < length; ++i) {
            const QChar c = filename[i];
            const bool isOk = c.isLetter() || c.isDigit() ||
                        allowedFilenameCharacters.contains(c);

            if (!isOk) {
                anyBadSymbol = true;
                break;
            }
        }

        accountFlag(Common::WarningFlags::FilenameSymbols, anyBadSymbol);
    }

    void WarningsItem::checkKeywords(IWarningsSettings *warningsSettings) {
        LOG_INTEGRATION_TESTS << "#";
        Q_UNUSED(warningsSettings);
        Common::BasicKeywordsModel *keywordsModel = m_CheckableItem->getBasicModel();

        const int keywordsCount = keywordsModel->getKeywordsCount();

        {
            const bool keywordsEmptyValue = (keywordsCount == 0);
            accountFlag(Common::WarningFlags::NoKeywords, keywordsEmptyValue);
        }

        {
            const int minimumKeywordsCount = warningsSettings->getMinKeywordsCount();
            const bool tooFewKeywordsValue = (0 < keywordsCount) && (keywordsCount < minimumKeywordsCount);
            accountFlag(Common::WarningFlags::TooFewKeywords, tooFewKeywordsValue);
        }

        {
            const int maximumKeywordsCount = warningsSettings->getMaxKeywordsCount();
            const bool tooManyKeywordsValue = (keywordsCount > maximumKeywordsCount);
            accountFlag(Common::WarningFlags::TooManyKeywords, tooManyKeywordsValue);
        }
    }

    void WarningsItem::checkDescription(IWarningsSettings *warningsSettings) {
        LOG_INTEGRATION_TESTS << "#";

        const int descriptionLength = getDescription().length();

        {
            const bool descriptionEmptyValue = (descriptionLength == 0);
            accountFlag(Common::WarningFlags::DescriptionIsEmpty, descriptionEmptyValue);
        }

        {
            const int maximumDescriptionLength = warningsSettings->getMaxDescriptionLength();
            const bool descriptionTooBigValue = (descriptionLength > maximumDescriptionLength);
            accountFlag(Common::WarningFlags::DescriptionTooBig, descriptionTooBigValue);
        }

        QStringList descriptionWords = getDescriptionWords();
        const int wordsLength = descriptionWords.length();

        {
            const int minWordsCount = warningsSettings->getMinWordsCount();
            const bool notEnoughKeywordsValue = (0 < wordsLength) && (wordsLength < minWordsCount);
            accountFlag(Common::WarningFlags::DescriptionNotEnoughWords, notEnoughKeywordsValue);
        }
    }

    void WarningsItem::checkTitle(IWarningsSettings *warningsSettings) {
        LOG_INTEGRATION_TESTS << "#";

        const int titleLength = getTitle().length();

        {
            const bool titleEmptyValue = (titleLength == 0);
            accountFlag(Common::WarningFlags::TitleIsEmpty, titleEmptyValue);
        }

        QStringList titleWords = getTitleWords();
        const int partsLength = titleWords.length();

        {
            const int minWordsCount = warningsSettings->getMinWordsCount();
            const bool titleNotEnoughWords = (0 < partsLength) && (partsLength < minWordsCount);
            accountFlag(Common::WarningFlags::TitleNotEnoughWords, titleNotEnoughWords);
        }

        {
            const int maxWordsCount = warningsSettings->getMaxTitleWords();
            const bool titleTooManyWords = (partsLength > maxWordsCount);
            accountFlag(Common::WarningFlags::TitleTooManyWords, titleTooManyWords);
        }
    }

    void WarningsItem::checkSpelling(IWarningsSettings *warningsSettings) {
        LOG_INTEGRATION_TESTS << "#";
        Q_UNUSED(warningsSettings);

        auto *keywordsModel = m_CheckableItem->getBasicModel();

        {
            const bool keywordsSpellingErrors = (keywordsModel->hasKeywordsSpellError());
            LOG_INTEGRATION_TESTS << "Detected keywords spell error:" << keywordsSpellingErrors;
            accountFlag(Common::WarningFlags::SpellErrorsInKeywords, keywordsSpellingErrors);
        }

        {
            const bool descriptionSpellingErrors = (keywordsModel->hasDescriptionSpellError());
            LOG_INTEGRATION_TESTS << "Detected description spell error:" << descriptionSpellingErrors;
            accountFlag(Common::WarningFlags::SpellErrorsInDescription, descriptionSpellingErrors);
        }

        {
            const bool titleSpellingErrors = (keywordsModel->hasTitleSpellError());
            LOG_INTEGRATION_TESTS << "Detected title spell error:" << titleSpellingErrors;
            accountFlag(Common::WarningFlags::SpellErrorsInTitle, titleSpellingErrors);
        }
    }

    void WarningsItem::checkDuplicates(IWarningsSettings *warningsSettings) {
        Q_UNUSED(warningsSettings);
        const QSet<QString> &keywordsSet = getKeywordsSet();

        bool keywordsInTitleValue = false;
        QStringList titleWords = getTitleWords();
        if (!titleWords.isEmpty()) {
            QSet<QString> titleWordsSet = toLowerSet(titleWords);
            QSet<QString> intersection = titleWordsSet.intersect(keywordsSet);

            keywordsInTitleValue = (!intersection.isEmpty());
        }

        accountFlag(Common::WarningFlags::KeywordsInTitle, keywordsInTitleValue);

        bool keywordsInDescriptionValue = false;
        QStringList descriptionWords = getDescriptionWords();
        if (!descriptionWords.isEmpty()) {
            QSet<QString> descriptionWordsSet = toLowerSet(descriptionWords);
            QSet<QString> intersection = descriptionWordsSet.intersect(keywordsSet);

            keywordsInDescriptionValue = (!intersection.isEmpty());
        }

        accountFlag(Common::WarningFlags::KeywordsInDescription, keywordsInDescriptionValue);
    }

    QString WarningsItem::getDescription() const {
        return m_CheckableItem->getDescription();
    }

    QString WarningsItem::getTitle() const {
        return m_CheckableItem->getTitle();
    }

    QSet<QString> WarningsItem::getKeywordsSet() const {
        return m_CheckableItem->getKeywordsSet();
    }

    void WarningsItem::accountFlag(Common::WarningFlags flag, bool value) {
        if (value) {
            m_FlagsToSet |= flag;
        }

        m_FlagsToDrop |= flag;
    }

    void WarningsItem::dropFlag(Common::WarningFlags flag) {
        m_FlagsToDrop |= flag;
    }

    QStringList WarningsItem::getDescriptionWords() const {
        QStringList words;
        Helpers::splitText(getDescription(), words);
        return words;
    }

    QStringList WarningsItem::getTitleWords() const {
        QStringList words;
        Helpers::splitText(getTitle(), words);
        return words;
    }
}