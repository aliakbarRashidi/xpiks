/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2018 Taras Kushnir "kushnirTV@gmail.com"
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef XPIKSAPP_H
#define XPIKSAPP_H

#include <Commands/commandmanager.h>
#include <Commands/appmessages.h>

#include <Services/artworksupdatehub.h>

#include <QMLExtensions/colorsmodel.h>
#include <QMLExtensions/imagecachingservice.h>
#include <QMLExtensions/videocachingservice.h>

#include <Models/Artworks/artworkslistmodel.h>
#include <Models/Editing/artworkproxymodel.h>
#include <Models/Artworks/artworksrepository.h>
#include <Models/Connectivity/artworkuploader.h>
#include <Models/Editing/combinedartworksmodel.h>
#include <Models/Editing/deletekeywordsviewmodel.h>
#include <Models/Artworks/filteredartworkslistmodel.h>
#include <Models/Editing/findandreplacemodel.h>
#include <Models/languagesmodel.h>
#include <Models/logsmodel.h>
#include <Models/Session/recentdirectoriesmodel.h>
#include <Models/Session/recentfilesmodel.h>
#include <Models/Session/sessionmanager.h>
#include <Models/settingsmodel.h>
#include <Models/switchermodel.h>
#include <Models/uimanager.h>
#include <Models/Connectivity/ziparchiver.h>
#include <Models/Editing/currenteditablemodel.h>

#include <Connectivity/requestsservice.h>
#include <Connectivity/telemetryservice.h>
#include <Connectivity/updateservice.h>

#include <Services/Warnings/warningsmodel.h>
#include <Services/Warnings/warningsservice.h>

#include <UndoRedo/undoredomanager.h>

#include <Storage/databasemanager.h>

#include <Encryption/isecretsstorage.h>

#include <Microstocks/microstockapiclients.h>

#include <Suggestion/keywordssuggestor.h>

#include <KeywordsPresets/presetkeywordsmodel.h>

#include <ftpcoordinator.h>

#include <Services/SpellCheck/duplicatesreviewmodel.h>
#include <Services/SpellCheck/spellcheckerservice.h>
#include <Services/SpellCheck/spellchecksuggestionmodel.h>
#include <Services/SpellCheck/userdicteditmodel.h>

#include <MetadataIO/csvexportmodel.h>
#include <MetadataIO/metadataiocoordinator.h>
#include <MetadataIO/metadataioservice.h>

#include <Services/AutoComplete/autocompleteservice.h>
#include <Services/AutoComplete/keywordsautocompletemodel.h>

#include <Services/Translation/translationmanager.h>
#include <Services/Translation/translationservice.h>

#include <Models/Editing/quickbuffer.h>

#include <Services/Maintenance/maintenanceservice.h>

#include <Plugins/pluginmanager.h>
#include <Plugins/uiprovider.h>

#include <Helpers/helpersqmlwrapper.h>
#include <Helpers/ifilenotavailablemodel.h>

#include <Common/isystemenvironment.h>

class QQmlContext;
class QQuickWindow;

class XpiksApp: public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool pluginsAvailable READ getPluginsAvailable CONSTANT)
    Q_PROPERTY(bool isUpdateDownloaded READ getIsUpdateDownloaded NOTIFY isUpdateDownloadedChanged)
public:
    XpiksApp(Common::ISystemEnvironment &environment);
    virtual ~XpiksApp();

public:
    bool getIsUpdateDownloaded();
    bool getPluginsAvailable() const;

public:
    Plugins::UIProvider *getUIProvider();
    QMLExtensions::ImageCachingService &getImageCachingService() { return m_ImageCachingService; }

public:
    void startLogging();
    virtual void initialize();
    void setupUI(QQmlContext *context);
    void start();
    void stop();
    void setupWindow(QQuickWindow *window);

public:
    Q_INVOKABLE void shutdown();
    Q_INVOKABLE void upgradeNow();
    Q_INVOKABLE void debugCrash();

public:
    Q_INVOKABLE void addFiles(const QList<QUrl> &urls);
    Q_INVOKABLE void addDirectories(const QList<QUrl> &urls);
    Q_INVOKABLE void dropItems(const QList<QUrl> &urls);

public:
    Q_INVOKABLE void removeSelectedArtworks();
    Q_INVOKABLE void removeDirectory(int index);
    Q_INVOKABLE void removeUnavailableFiles();

private:
    void doAddFiles(const std::shared_ptr<Filesystem::IFilesCollection> &files, Common::AddFilesFlags flags);
    void afterServicesStarted();
    void executeMaintenanceJobs();
    void connectEntitiesSignalsSlots();
    void injectActionsTemplates();

signals:
    void globalCloseRequested();
    void globalBeforeDestruction();
    void upgradeInitiated();
    void isUpdateDownloadedChanged();
    void artworksAdded(int importID, int imagesCount, int vectorsCount);

private slots:
    void servicesInitialized(int status);

protected:
    Commands::AppMessages m_Messages;
    QMLExtensions::ColorsModel m_ColorsModel;
    Models::LogsModel m_LogsModel;
    Models::SettingsModel m_SettingsModel;
    Models::RecentDirectoriesModel m_RecentDirectorieModel;
    Models::ArtworksRepository m_ArtworksRepository;
    Models::FilteredArtworksRepository m_FilteredArtworksRepository;
    Models::ArtworksListModel m_ArtworksListModel;
    Models::CombinedArtworksModel m_CombinedArtworksModel;
    Models::UploadInfoRepository m_UploadInfoRepository;
    Connectivity::RequestsService m_RequestsService;
    KeywordsPresets::PresetKeywordsModel m_PresetsModel;
    KeywordsPresets::FilteredPresetKeywordsModel m_FilteredPresetsModel;
    Warnings::WarningsService m_WarningsService;
    Encryption::SecretsManager m_SecretsManager;
    UndoRedo::UndoRedoManager m_UndoRedoManager;
    Models::ZipArchiver m_ZipArchiver;
    Storage::DatabaseManager m_DatabaseManager;
    std::shared_ptr<Encryption::ISecretsStorage> m_SecretsStorage;
    Microstocks::MicrostockAPIClients m_ApiClients;
    Models::FilteredArtworksListModel m_FilteredArtworksListModel;
    Models::RecentFilesModel m_RecentFileModel;
    std::shared_ptr<libxpks::net::FtpCoordinator> m_FtpCoordinator;
    Models::ArtworkUploader m_ArtworkUploader;
    SpellCheck::SpellCheckerService m_SpellCheckerService;
    SpellCheck::SpellCheckSuggestionModel m_SpellCheckSuggestionModel;
    SpellCheck::UserDictEditModel m_UserDictEditModel;
    MetadataIO::MetadataIOService m_MetadataIOService;
    Warnings::WarningsModel m_WarningsModel;
    Models::LanguagesModel m_LanguagesModel;
    AutoComplete::KeywordsAutoCompleteModel m_AutoCompleteModel;
    AutoComplete::AutoCompleteService m_AutoCompleteService;
    QMLExtensions::ImageCachingService m_ImageCachingService;
    Models::FindAndReplaceModel m_ReplaceModel;
    Models::DeleteKeywordsViewModel m_DeleteKeywordsModel;
    Models::ArtworkProxyModel m_ArtworkProxyModel;
    Translation::TranslationManager m_TranslationManager;
    Translation::TranslationService m_TranslationService;
    Models::UIManager m_UIManager;
    Models::SessionManager m_SessionManager;
    Models::QuickBuffer m_QuickBuffer;
    Maintenance::MaintenanceService m_MaintenanceService;
    QMLExtensions::VideoCachingService m_VideoCachingService;
    QMLExtensions::ArtworksUpdateHub m_ArtworksUpdateHub;
    Models::SwitcherModel m_SwitcherModel;
    SpellCheck::DuplicatesReviewModel m_DuplicatesModel;
    MetadataIO::CsvExportModel m_CsvExportModel;
    Connectivity::UpdateService m_UpdateService;
    Suggestion::KeywordsSuggestor m_KeywordsSuggestor;
    MetadataIO::MetadataIOCoordinator m_MetadataIOCoordinator;
    Connectivity::TelemetryService m_TelemetryService;
    Plugins::PluginManager m_PluginManager;
    Plugins::PluginsWithActionsModel m_PluginsWithActions;
    Helpers::HelpersQmlWrapper m_HelpersQmlWrapper;
    Commands::CommandManager m_CommandManager;
    Models::CurrentEditableModel m_CurrentEditable;

    Helpers::AsyncCoordinator m_InitCoordinator;
    QVector<Helpers::IFileNotAvailableModel*> m_AvailabilityListeners;
    volatile bool m_ServicesInitialized = false;
    volatile bool m_AfterInitCalled = false;
};

#endif // XPIKSAPP_H
