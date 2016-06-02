﻿/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2016 Taras Kushnir <kushnirTV@gmail.com>
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

#ifndef SETTINGSMODEL_H
#define SETTINGSMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QString>
#include "../Common/baseentity.h"

#define SETTINGS_EPSILON 1e-9

namespace Models {

    int ensureInBounds(int value, int boundA, int boundB);
    double ensureInBounds(double value, double boundA, double boundB);

    class SettingsModel : public QObject, public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(QString exifToolPath READ getExifToolPath WRITE setExifToolPath NOTIFY exifToolPathChanged)
        Q_PROPERTY(double minMegapixelCount READ getMinMegapixelCount WRITE setMinMegapixelCount NOTIFY minMegapixelCountChanged)
        Q_PROPERTY(int maxDescriptionLength READ getMaxDescriptionLength WRITE setMaxDescriptionLength NOTIFY maxDescriptionLengthChanged)
        Q_PROPERTY(int maxKeywordsCount READ getMaxKeywordsCount WRITE setMaxKeywordsCount NOTIFY maxKeywordsCountChanged)
        Q_PROPERTY(int uploadTimeout READ getUploadTimeout WRITE setUploadTimeout NOTIFY uploadTimeoutChanged)
        Q_PROPERTY(bool mustUseMasterPassword READ getMustUseMasterPassword WRITE setMustUseMasterPassword NOTIFY mustUseMasterPasswordChanged)
        Q_PROPERTY(bool mustUseConfirmations READ getMustUseConfirmations WRITE setMustUseConfirmations NOTIFY mustUseConfirmationsChanged)
        Q_PROPERTY(bool saveBackups READ getSaveBackups WRITE setSaveBackups NOTIFY saveBackupsChanged)
        Q_PROPERTY(double keywordSizeScale READ getKeywordSizeScale WRITE setKeywordSizeScale NOTIFY keywordSizeScaleChanged)
        Q_PROPERTY(int dismissDuration READ getDismissDuration WRITE setDismissDuration NOTIFY dismissDurationChanged)
        Q_PROPERTY(int maxParallelUploads READ getMaxParallelUploads WRITE setMaxParallelUploads NOTIFY maxParallelUploadsChanged)
        Q_PROPERTY(bool fitSmallPreview READ getFitSmallPreview WRITE setFitSmallPreview NOTIFY fitSmallPreviewChanged)
        Q_PROPERTY(bool searchUsingAnd READ getSearchUsingAnd WRITE setSearchUsingAnd NOTIFY searchUsingAndChanged)
        Q_PROPERTY(double scrollSpeedScale READ getScrollSpeedScale WRITE setScrollSpeedScale NOTIFY scrollSpeedScaleChanged)
        Q_PROPERTY(bool useSpellCheck READ getUseSpellCheck WRITE setUseSpellCheck NOTIFY useSpellCheckChanged)
        Q_PROPERTY(bool userStatistics READ getUserStatistics WRITE setUserStatistics NOTIFY userStatisticsChanged)
        Q_PROPERTY(bool updateService READ getUpdateService WRITE setUpdateService NOTIFY updateServiceChanged)
        Q_PROPERTY(QString dictionaryPath READ getDictionaryPath WRITE setDictionaryPath NOTIFY dictionaryPathChanged)
        Q_PROPERTY(bool autoFindVectors READ getAutoFindVectors WRITE setAutoFindVectors NOTIFY autoFindVectorsChanged)
        Q_PROPERTY(QString selectedLocale READ getSelectedLocale WRITE setSelectedLocale NOTIFY selectedLocaleChanged)
        Q_PROPERTY(int selectedThemeIndex READ getSelectedThemeIndex WRITE setSelectedThemeIndex NOTIFY selectedThemeIndexChanged)
        Q_PROPERTY(bool useAutoComplete READ getUseAutoComplete WRITE setUseAutoComplete NOTIFY useAutoCompleteChanged)
        Q_PROPERTY(bool useExifTool READ getUseExifTool WRITE setUseExifTool NOTIFY useExifToolChanged)
        Q_PROPERTY(bool autoCacheImages READ getAutoCacheImages WRITE setAutoCacheImages NOTIFY autoCacheImagesChanged)

    public:
        explicit SettingsModel(QObject *parent = 0);
        virtual ~SettingsModel() {}

    public:
        void saveExiftool();
        void saveLocale();

    public:
        Q_INVOKABLE void resetAllValues();
        Q_INVOKABLE void saveAllValues();
        Q_INVOKABLE void clearMasterPasswordSettings();
        Q_INVOKABLE void resetExifTool();
        Q_INVOKABLE void resetDictPath();
        Q_INVOKABLE void readAllValues();
        Q_INVOKABLE void raiseMasterPasswordSignal() { emit mustUseMasterPasswordChanged(m_MustUseMasterPassword); }

    public:
        QString getExifToolPath() const { return m_ExifToolPath; }
        double getMinMegapixelCount() const { return m_MinMegapixelCount; }
        int getMaxDescriptionLength() const { return m_MaxDescriptionLength; }
        int getMaxKeywordsCount() const { return m_MaxKeywordsCount; }
        int getUploadTimeout() const { return m_UploadTimeout; }
        bool getMustUseMasterPassword() const { return m_MustUseMasterPassword; }
        bool getMustUseConfirmations() const { return m_MustUseConfirmations; }
        bool getSaveBackups() const { return m_SaveBackups; }
        double getKeywordSizeScale() const { return m_KeywordSizeScale; }
        int getDismissDuration() const { return m_DismissDuration; }
        int getMaxParallelUploads() const { return m_MaxParallelUploads; }
        bool getFitSmallPreview() const { return m_FitSmallPreview; }
        bool getSearchUsingAnd() const { return m_SearchUsingAnd; }
        double getScrollSpeedScale() const { return m_ScrollSpeedScale; }
        bool getUseSpellCheck() const { return m_UseSpellCheck; }
        bool getUserStatistics() const { return m_UserStatistics; }
        bool getUpdateService() const { return m_CheckForUpdates; }
        QString getDictionaryPath() const { return m_DictPath; }
        bool getAutoFindVectors() const { return m_AutoFindVectors; }
        QString getSelectedLocale() const { return m_SelectedLocale; }
        int getSelectedThemeIndex() const { return m_SelectedThemeIndex; }
        bool getUseAutoComplete() const { return m_UseAutoComplete; }
        bool getUseExifTool() const { return m_UseExifTool; }
        bool getAutoCacheImages() const { return m_AutoCacheImages; }

    signals:
        void settingsReset();
        void exifToolPathChanged(QString exifToolPath);
        void minMegapixelCountChanged(double minMegapixelCount);
        void maxDescriptionLengthChanged(int maxDescriptionLength);
        void maxKeywordsCountChanged(int maxKeywordsCount);
        void uploadTimeoutChanged(int uploadTimeout);
        void mustUseMasterPasswordChanged(bool mustUseMasterPassword);
        void mustUseConfirmationsChanged(bool mustUseConfirmations);
        void saveBackupsChanged(bool saveBackups);
        void keywordSizeScaleChanged(double value);
        void dismissDurationChanged(int value);
        void maxParallelUploadsChanged(int value);
        void fitSmallPreviewChanged(bool value);
        void searchUsingAndChanged(bool value);
        void scrollSpeedScaleChanged(double value);
        void useSpellCheckChanged(bool value);
        void userStatisticsChanged(bool value);
        void updateServiceChanged(bool value);
        void dictionaryPathChanged(QString path);
        void autoFindVectorsChanged(bool value);
        void selectedLocaleChanged(QString value);
        void selectedThemeIndexChanged(int value);
        void useAutoCompleteChanged(bool value);
        void useExifToolChanged(bool value);
        void autoCacheImagesChanged(bool value);

    public:
        void setExifToolPath(QString exifToolPath) {
            if (m_ExifToolPath == exifToolPath)
                return;

            m_ExifToolPath = exifToolPath;
            emit exifToolPathChanged(exifToolPath);
        }

        void setMinMegapixelCount(double minMegapixelCount) {
            if (qAbs(m_MinMegapixelCount - minMegapixelCount) <= SETTINGS_EPSILON)
                return;

            m_MinMegapixelCount = ensureInBounds(minMegapixelCount, 0.0, 100.0);
            emit minMegapixelCountChanged(m_MinMegapixelCount);
        }

        void setMaxDescriptionLength(int maxDescriptionLength) {
            if (m_MaxDescriptionLength == maxDescriptionLength)
                return;

            m_MaxDescriptionLength = ensureInBounds(maxDescriptionLength, 0, 500);
            emit maxDescriptionLengthChanged(m_MaxDescriptionLength);
        }

        void setMaxKeywordsCount(int maxKeywordsCount) {
            if (m_MaxKeywordsCount == maxKeywordsCount)
                return;

            m_MaxKeywordsCount = ensureInBounds(maxKeywordsCount, 0, 1000);
            emit maxKeywordsCountChanged(m_MaxKeywordsCount);
        }

        void setUploadTimeout(int uploadTimeout) {
            if (m_UploadTimeout == uploadTimeout)
                return;

            m_UploadTimeout = ensureInBounds(uploadTimeout, 5, 30);
            emit uploadTimeoutChanged(m_UploadTimeout);
        }

        void setMustUseMasterPassword(bool mustUseMasterPassword) {
            if (m_MustUseMasterPassword == mustUseMasterPassword)
                return;

            m_MustUseMasterPassword = mustUseMasterPassword;
            emit mustUseMasterPasswordChanged(mustUseMasterPassword);
        }

        void setMustUseConfirmations(bool mustUseConfirmations) {
            if (m_MustUseConfirmations == mustUseConfirmations)
                return;

            m_MustUseConfirmations = mustUseConfirmations;
            emit mustUseConfirmationsChanged(mustUseConfirmations);
        }

        void setSaveBackups(bool saveBackups) {
            if (m_SaveBackups == saveBackups)
                return;

            m_SaveBackups = saveBackups;
            emit saveBackupsChanged(saveBackups);
        }

        void setKeywordSizeScale(double value) {
            if (qAbs(m_KeywordSizeScale - value) <= SETTINGS_EPSILON)
                return;

            m_KeywordSizeScale = ensureInBounds(value, 1.0, 1.2);
            emit keywordSizeScaleChanged(m_KeywordSizeScale);
        }

        void setDismissDuration(int value) {
            if (m_DismissDuration == value)
                return;

            m_DismissDuration = ensureInBounds(value, 1, 100);
            emit dismissDurationChanged(m_DismissDuration);
        }

        void setMaxParallelUploads(int value) {
            if (m_MaxParallelUploads == value)
                return;

            m_MaxParallelUploads = ensureInBounds(value, 1, 4);
            emit maxParallelUploadsChanged(m_MaxParallelUploads);
        }

        void setFitSmallPreview(bool value) {
            if (m_FitSmallPreview == value)
                return;

            m_FitSmallPreview = value;
            emit fitSmallPreviewChanged(value);
        }

        void setSearchUsingAnd(bool value) {
            if (m_SearchUsingAnd == value)
                return;

            m_SearchUsingAnd = value;
            emit searchUsingAndChanged(value);
        }

        void setScrollSpeedScale(double value) {
            if (qAbs(m_ScrollSpeedScale - value) <= SETTINGS_EPSILON)
                return;

            m_ScrollSpeedScale = ensureInBounds(value, 0.1, 2.0);
            emit scrollSpeedScaleChanged(m_ScrollSpeedScale);
        }

        void setUseSpellCheck(bool value) {
            if (m_UseSpellCheck == value)
                return;

            m_UseSpellCheck = value;
            emit useSpellCheckChanged(value);
        }

        void setUserStatistics(bool value) {
            if (m_UserStatistics == value)
                return;

            m_UserStatistics = value;
            emit userStatisticsChanged(value);
        }

        void setUpdateService(bool value) {
            if (m_CheckForUpdates == value)
                return;

            m_CheckForUpdates = value;
            emit updateServiceChanged(value);
        }

        void setDictionaryPath(QString path) {
            if (m_DictPath == path)
                return;

            m_DictPath = path;
            emit dictionaryPathChanged(path);
            m_DictsPathChanged = true;
        }

        void setAutoFindVectors(bool value) {
            if (value != m_AutoFindVectors) {
                m_AutoFindVectors = value;
                emit autoFindVectorsChanged(value);
            }
        }

        void setSelectedLocale(QString value) {
            if (value != m_SelectedLocale) {
                m_SelectedLocale = value;
                emit selectedLocaleChanged(value);
            }
        }

        void setSelectedThemeIndex(int value) {
            if (value != m_SelectedThemeIndex) {
                m_SelectedThemeIndex = value;
                emit selectedThemeIndexChanged(value);
            }
        }

        void setUseAutoComplete(bool value) {
            if (value != m_UseAutoComplete) {
                m_UseAutoComplete = value;
                emit useAutoCompleteChanged(value);
            }
        }

        void setUseExifTool(bool value) {
            if (value != m_UseExifTool) {
                m_UseExifTool = value;
                emit useExifToolChanged(value);
            }
        }

        void setAutoCacheImages(bool value) {
            if (value != m_AutoCacheImages) {
                m_AutoCacheImages = value;
                emit autoCacheImagesChanged(value);
            }
        }

#ifndef INTEGRATION_TESTS
    private:
#else
    public:
#endif
        void resetToDefault();

    private:
        QString m_ExifToolPath;
        QString m_DictPath;
        QString m_SelectedLocale;
        double m_MinMegapixelCount;
        double m_KeywordSizeScale;
        double m_ScrollSpeedScale;
        int m_MaxDescriptionLength;
        int m_MaxKeywordsCount;
        int m_UploadTimeout; // in minutes
        int m_DismissDuration;
        int m_MaxParallelUploads;
        int m_SelectedThemeIndex;
        bool m_MustUseMasterPassword;
        bool m_MustUseConfirmations;
        bool m_SaveBackups;
        bool m_FitSmallPreview;
        bool m_SearchUsingAnd;
        bool m_UseSpellCheck;
        bool m_UserStatistics;
        bool m_CheckForUpdates;
        bool m_DictsPathChanged;
        bool m_AutoFindVectors;
        bool m_UseAutoComplete;
        bool m_UseExifTool;
        bool m_AutoCacheImages;
    };
}

#endif // SETTINGSMODEL_H
