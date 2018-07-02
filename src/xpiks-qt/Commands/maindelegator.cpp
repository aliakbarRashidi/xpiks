#include "maindelegator.h"
#include "commandmanager.h"
#include "../Models/artworksrepository.h"
#include "../Models/artitemsmodel.h"
#include "../Models/imageartwork.h"
#include "../Models/combinedartworksmodel.h"
#include "../Models/artworkuploader.h"
#include "../Models/uploadinforepository.h"
#include "../Models/uploadinfo.h"
#include "../Models/artworkmetadata.h"
#include "../Encryption/secretsmanager.h"
#include "../UndoRedo/undoredomanager.h"
#include "../Models/ziparchiver.h"
#include "../Suggestion/keywordssuggestor.h"
#include "../Commands/addartworkscommand.h"
#include "../Models/filteredartworkslistmodel.h"
#include "../Models/recentdirectoriesmodel.h"
#include "../Models/recentfilesmodel.h"
#include "../Models/artworkelement.h"
#include "../SpellCheck/duplicatesreviewmodel.h"
#include "../SpellCheck/spellcheckerservice.h"
#include "../Models/settingsmodel.h"
#include "../SpellCheck/spellchecksuggestionmodel.h"
#include "../MetadataIO/metadataioservice.h"
#include "../Connectivity/telemetryservice.h"
#include "../Connectivity/updateservice.h"
#include "../Models/logsmodel.h"
#include "../Encryption/aes-qt.h"
#include "../MetadataIO/metadataiocoordinator.h"
#include "../Plugins/pluginmanager.h"
#include "../Warnings/warningsservice.h"
#include "../Models/languagesmodel.h"
#include "../AutoComplete/autocompleteservice.h"
#include "../QMLExtensions/imagecachingservice.h"
#include "../Models/findandreplacemodel.h"
#include "../Models/deletekeywordsviewmodel.h"
#include "../Helpers/helpersqmlwrapper.h"
#include "../Helpers/updatehelpers.h"
#include "../Common/imetadataoperator.h"
#include "../Translation/translationmanager.h"
#include "../Translation/translationservice.h"
#include "../Models/uimanager.h"
#include "../Models/artworkproxymodel.h"
#include "../Models/sessionmanager.h"
#include "../Warnings/warningsmodel.h"
#include "../QuickBuffer/quickbuffer.h"
#include "../QuickBuffer/currenteditableartwork.h"
#include "../QuickBuffer/currenteditableproxyartwork.h"
#include "../Maintenance/maintenanceservice.h"
#include "../QMLExtensions/videocachingservice.h"
#include "../QMLExtensions/artworksupdatehub.h"
#include "../Helpers/asynccoordinator.h"
#include "../Storage/database.h"
#include "../Models/switchermodel.h"
#include "../Connectivity/requestsservice.h"
#include "../AutoComplete/keywordsautocompletemodel.h"
#include "../MetadataIO/csvexportmodel.h"

namespace Commands {
    void MainDelegator::addWarningsService(Common::IServiceBase<Common::IBasicArtwork, Common::WarningsCheckFlags> *service) {
        if (service != NULL) {
            // TODO: check if we don't have such checker
            m_WarningsCheckers.append(service);
        }
    }

    void MainDelegator::addAvailabilityListener(Helpers::IFileNotAvailableModel *listener) {
        m_AvailabilityListeners.append(listener);
    }

    void MainDelegator::recordHistoryItem(std::unique_ptr<UndoRedo::IHistoryItem> &historyItem) const {
        auto *undoRedoManager = m_CommandManager->getUndoRedoManager();
        if (undoRedoManager) {
            undoRedoManager->recordHistoryItem(historyItem);
        }
    }

    void MainDelegator::recodePasswords(const QString &oldMasterPassword,
                                        const QString &newMasterPassword,
                                        const std::vector<std::shared_ptr<Models::UploadInfo> > &uploadInfos) const {
        auto *secretsManager = m_CommandManager->getSecretsManager();
        if (secretsManager) {
            LOG_INFO << uploadInfos.size() << "item(s)";

            for (auto &info: uploadInfos) {
                if (info->hasPassword()) {
                    QString newPassword = secretsManager->recodePassword(
                        info->getPassword(), oldMasterPassword, newMasterPassword);
                    info->setPassword(newPassword);
                }
            }
        }
    }

    int MainDelegator::reimportMetadata(const Artworks::ArtworksSnapshot &snapshot) const {
        LOG_DEBUG << "#";
        int importID = 0;

    #ifndef CORE_TESTS
        auto *metadataIOCoordinator = m_CommandManager->getMetadataIOCoordinator();
        if (metadataIOCoordinator != nullptr) {
            importID = metadataIOCoordinator->readMetadataExifTool(snapshot, INVALID_BATCH_ID);
        }
    #else
        Q_UNUSED(snapshot);
    #endif

        return importID;
    }

    void MainDelegator::writeMetadata(const Artworks::WeakArtworksSnapshot &artworks, bool useBackups) const {
        LOG_DEBUG << "#";

    #ifndef CORE_TESTS
        auto *metadataIOService = m_CommandManager->getMetadataIOService();
        auto *metadataIOCoordinator = m_CommandManager->getMetadataIOCoordinator();

        if (metadataIOService != nullptr) {
            metadataIOService->writeArtworks(artworks);
        }

        if (metadataIOCoordinator != nullptr) {
            metadataIOCoordinator->writeMetadataExifTool(artworks, useBackups);
        }

    #else
        Q_UNUSED(artworks);
        Q_UNUSED(useBackups);
    #endif
    }

    void MainDelegator::wipeAllMetadata(const Artworks::ArtworksSnapshot &artworks, bool useBackups) const {
        LOG_DEBUG << "#";

    #ifndef CORE_TESTS
        auto *metadataIOCoordinator = m_CommandManager->getMetadataIOCoordinator();

        if (metadataIOCoordinator != nullptr) {
            metadataIOCoordinator->wipeAllMetadataExifTool(artworks, useBackups);
        }

    #else
        Q_UNUSED(artworks);
        Q_UNUSED(useBackups);
    #endif
    }

    void MainDelegator::updateArtworksAtIndices(const QVector<int> &indices) const {
        auto *artItemsModel = m_CommandManager->getArtItemsModel();
        if (artItemsModel) {
            artItemsModel->updateItemsAtIndices(indices);
        }
    }

    void MainDelegator::updateArtworks(const Artworks::WeakArtworksSnapshot &artworks) const {
        auto *artItemsModel = m_CommandManager->getArtItemsModel();
        if (artItemsModel != nullptr) {
            QVector<int> indices;
            indices.reserve((int)artworks.size());
            for (auto *artwork: artworks) {
                indices.push_back((int)artwork->getLastKnownIndex());
            }

            artItemsModel->updateItemsAtIndices(indices);
        }
    }

    void MainDelegator::updateArtworks(const Artworks::ArtworksSnapshot::Container &artworks) {
        auto *artItemsModel = m_CommandManager->getArtItemsModel();
        if (artItemsModel != nullptr) {
            QVector<int> indices;
            indices.reserve((int)artworks.size());
            for (auto &locker: artworks) {
                Artworks::ArtworkMetadata *artwork = locker->getArtworkMetadata();
                indices.push_back((int)artwork->getLastKnownIndex());
            }

            artItemsModel->updateItemsAtIndices(indices);
        }
    }

    void MainDelegator::autoDiscoverExiftool() const {
    #ifndef CORE_TESTS
        auto *metadataIOCoordinator = m_CommandManager->getMetadataIOCoordinator();
        if (metadataIOCoordinator) {
            metadataIOCoordinator->autoDiscoverExiftool();
        }
    #endif
    }

    void MainDelegator::cleanupOldXpksBackups(const QString &directory) const {
    #ifndef CORE_TESTS
        auto *maintenanceService = m_CommandManager->getMaintenanceService();
        if (maintenanceService != nullptr) {
            maintenanceService->cleanupOldXpksBackups(directory);
        }
    #endif
    }

    void MainDelegator::submitKeywordForSpellCheck(Artworks::BasicKeywordsModel *item, int keywordIndex) const {
        Q_ASSERT(item != NULL);
        auto *spellCheckerService = m_CommandManager->getSpellCheckerService();
        const Common::WordAnalysisFlags wordAnalysisFlags = getWordAnalysisFlags();
        if ((wordAnalysisFlags != Common::WordAnalysisFlags::None) && (spellCheckerService != NULL)) {
            spellCheckerService->submitKeyword(item, keywordIndex);
        }
    }

    void MainDelegator::submitForSpellCheck(const Artworks::WeakArtworksSnapshot &items) const {
        const Common::WordAnalysisFlags wordAnalysisFlags = getWordAnalysisFlags();
        auto *spellCheckerService = m_CommandManager->getSpellCheckerService();
        if ((wordAnalysisFlags != Common::WordAnalysisFlags::None) &&
                (spellCheckerService != NULL) &&
                !items.empty()) {
            std::vector<Artworks::BasicKeywordsModel *> itemsToSubmit;
            size_t count = items.size();
            itemsToSubmit.reserve(count);

            for (auto *artwork: items) {
                itemsToSubmit.push_back(artwork->getBasicModel());
            }

            this->submitForSpellCheck(itemsToSubmit);
        }
    }

    void MainDelegator::submitForSpellCheck(const std::vector<Artworks::BasicKeywordsModel *> &items) const {
        auto *spellCheckerService = m_CommandManager->getSpellCheckerService();
        const Common::WordAnalysisFlags wordAnalysisFlags = getWordAnalysisFlags();
        if ((wordAnalysisFlags != Common::WordAnalysisFlags::None) && (spellCheckerService != NULL)) {
            spellCheckerService->submitItems(items);
        }
    }

    void MainDelegator::submitItemForSpellCheck(Artworks::BasicKeywordsModel *item, Common::SpellCheckFlags flags) const {
        Q_ASSERT(item != NULL);
        const Common::WordAnalysisFlags wordAnalysisFlags = getWordAnalysisFlags();
        auto *spellCheckerService = m_CommandManager->getSpellCheckerService();
        if ((wordAnalysisFlags != Common::WordAnalysisFlags::None) && (spellCheckerService != NULL)) {
            spellCheckerService->submitItem(item, flags);
        }
    }

    void MainDelegator::checkSemanticDuplicates(Artworks::BasicKeywordsModel *item) const {
        Q_ASSERT(item != NULL);
        const Common::WordAnalysisFlags wordAnalysisFlags = getWordAnalysisFlags();
        auto *spellCheckerService = m_CommandManager->getSpellCheckerService();
        if (Common::HasFlag(wordAnalysisFlags, Common::WordAnalysisFlags::Stemming) && (spellCheckerService != NULL)) {
            spellCheckerService->submitItem(item, Common::SpellCheckFlags::All);
        }
    }

    void MainDelegator::setupSpellCheckSuggestions(Artworks::IMetadataOperator *item, int index, Common::SuggestionFlags flags) const {
        Q_ASSERT(item != NULL);
        auto *spellCheckSuggestionModel = m_CommandManager->getSpellSuggestionsModel();
        if (spellCheckSuggestionModel) {
            spellCheckSuggestionModel->setupModel(item, index, flags);
            reportUserAction(Connectivity::UserAction::SpellSuggestions);
        }
    }

    void MainDelegator::setupSpellCheckSuggestions(std::vector<std::pair<Artworks::IMetadataOperator *, int> > &itemPairs, Common::SuggestionFlags flags) const {
        auto *spellCheckSuggestionModel = m_CommandManager->getSpellSuggestionsModel();
        if (spellCheckSuggestionModel) {
            spellCheckSuggestionModel->setupModel(itemPairs, flags);
            reportUserAction(Connectivity::UserAction::SpellSuggestions);
        }
    }

    void MainDelegator::submitForSpellCheck(const std::vector<Artworks::BasicKeywordsModel *> &items,
                                                       const QStringList &wordsToCheck) const {
        auto *spellCheckerService = m_CommandManager->getSpellCheckerService();
        Common::WordAnalysisFlags wordAnalysisFlags = getWordAnalysisFlags();
        if ((wordAnalysisFlags != Common::WordAnalysisFlags::None) && (spellCheckerService != NULL)) {
            spellCheckerService->submitItems(items, wordsToCheck);
        }
    }

    void MainDelegator::submitKeywordsForWarningsCheck(Artworks::ArtworkMetadata *item) const {
        Q_ASSERT(item != NULL);
        this->submitForWarningsCheck(item, Common::WarningsCheckFlags::Keywords);
    }

    void MainDelegator::setupDuplicatesModel(Artworks::BasicMetadataModel *item) {
        auto *duplicatesModel = m_CommandManager->getDuplicatesReviewModel();
        if (duplicatesModel != nullptr) {
            duplicatesModel->setupModel(item);
        }
    }

    void MainDelegator::setupDuplicatesModel(const std::vector<Artworks::ArtworkMetadata*> &items) const {
        auto *duplicatesModel = m_CommandManager->getDuplicatesReviewModel();
        if (duplicatesModel != nullptr) {
            duplicatesModel->setupModel(items);
        }
    }

    void MainDelegator::submitForWarningsCheck(Artworks::ArtworkMetadata *item, Common::WarningsCheckFlags flags) const {
        Q_ASSERT(item != NULL);

        auto *warningsService = m_CommandManager->getWarningsService();

        if (warningsService != NULL) {
            warningsService->submitItem(item, flags);
        }

        const int count = m_WarningsCheckers.length();

        LOG_INTEGRATION_TESTS << count << "checkers available";

        for (int i = 0; i < count; ++i) {
            Common::IServiceBase<Common::IBasicArtwork, Common::WarningsCheckFlags> *checker = m_WarningsCheckers.at(i);
            if (checker->isAvailable()) {
                checker->submitItem(item, flags);
            }
        }
    }

    void MainDelegator::submitForWarningsCheck(const Artworks::WeakArtworksSnapshot &items) const {
        auto *warningsService = m_CommandManager->getWarningsService();

        if (warningsService != NULL) {
            warningsService->submitItems(items);
        }

        if (!m_WarningsCheckers.isEmpty()) {
            std::vector<Common::IBasicArtwork *> itemsToSubmit;
            size_t size = items.size();
            itemsToSubmit.reserve(size);

            for (auto &item: items) {
                itemsToSubmit.push_back(item);
            }

            this->submitForWarningsCheck(itemsToSubmit);
        }
    }

    void MainDelegator::submitForWarningsCheck(const std::vector<Common::IBasicArtwork *> &items) const {
        int count = m_WarningsCheckers.length();

        for (int i = 0; i < count; ++i) {
            Common::IServiceBase<Common::IBasicArtwork, Common::WarningsCheckFlags> *checker = m_WarningsCheckers.at(i);
            if (checker->isAvailable()) {
                checker->submitItems(items);
            }
        }
    }

    void MainDelegator::saveArtworkBackup(Artworks::ArtworkMetadata *metadata) const {
        Q_ASSERT(metadata != NULL);
    #ifndef CORE_TESTS
        auto *metadataIOService = m_CommandManager->getMetadataIOService();
        if (metadataIOService != nullptr) {
            metadataIOService->writeArtwork(metadata);
        }
    #endif
    }

    void MainDelegator::saveArtworksBackups(const Artworks::WeakArtworksSnapshot &artworks) const {
    #ifndef CORE_TESTS
        auto *metadataIOService = m_CommandManager->getMetadataIOService();
        if (metadataIOService != NULL) {
            metadataIOService->writeArtworks(artworks);
        }
    #else
        Q_UNUSED(artworks);
    #endif
    }

    void MainDelegator::reportUserAction(Connectivity::UserAction userAction) const {
    #ifndef CORE_TESTS
        auto *telemetryService = m_CommandManager->getTelemetryService();
        if (telemetryService) {
            telemetryService->reportAction(userAction);
        }
    #else
        Q_UNUSED(userAction);
    #endif
    }

    int MainDelegator::restoreFiles(const QStringList &filenames, const QStringList &vectors) {
        LOG_INFO << filenames.size() << "file(s)";

        if (filenames.empty()) { return 0; }

        // we should have saved all vectors from that session
        // directory is considered to be not full and restored later
        Common::flag_t flags = 0;
        Common::SetFlag(flags, Commands::AddArtworksCommand::FlagIsSessionRestore);

    #if !defined(CORE_TESTS) && !defined(INTEGRATION_TESTS)
        auto *settingsModel = m_CommandManager->getSettingsModel();
        auto *switcherModel = m_CommandManager->getSwitcherModel();

        if ((settingsModel != nullptr) && (switcherModel != nullptr)) {
            if (settingsModel->getUseAutoImport() && switcherModel->getUseAutoImport()) {
                Common::SetFlag(flags, Commands::AddArtworksCommand::FlagAutoImport);
            }
        }
    #endif

        std::shared_ptr<Commands::AddArtworksCommand> addArtworksCommand(new Commands::AddArtworksCommand(filenames, vectors, flags));
        std::shared_ptr<Commands::CommandResult> result = m_CommandManager->processCommand(addArtworksCommand);
        std::shared_ptr<Commands::AddArtworksCommandResult> addArtworksResult =
            std::dynamic_pointer_cast<Commands::AddArtworksCommandResult>(result);

        int addedCount = addArtworksResult->m_NewFilesAdded;
        return addedCount;
    }

    void MainDelegator::restartSpellChecking() {
        auto *spellCheckerService = m_CommandManager->getSpellCheckerService();
        if (spellCheckerService) {
            spellCheckerService->restartWorker();
        }
    }

    void MainDelegator::disableSpellChecking() {
        LOG_DEBUG << "#";
        auto *artItemsModel = m_CommandManager->getArtItemsModel();
        if (artItemsModel) {
            artItemsModel->resetSpellCheckResults();
        }
    }

    void MainDelegator::disableDuplicatesCheck() {
        LOG_DEBUG << "#";
        auto *artItemsModel = m_CommandManager->getArtItemsModel();
        if (artItemsModel) {
            artItemsModel->resetDuplicatesInfo();
        }
    }

    void MainDelegator::generateCompletions(const QString &prefix, Artworks::BasicKeywordsModel *source) const {
    #ifndef CORE_TESTS
        auto *autoCompleteService = m_CommandManager->getAutoCompleteService();
        if (autoCompleteService != NULL) {
            autoCompleteService->generateCompletions(prefix, source);
        }
    #else
        Q_UNUSED(prefix);
        Q_UNUSED(source);
    #endif
    }

    void MainDelegator::removeUnavailableFiles() {
        LOG_DEBUG << "#";
        auto *combinedArtworksModel = m_CommandManager->getCombinedArtworksModel();
        auto *artItemsModel = m_CommandManager->getArtItemsModel();

        combinedArtworksModel->generateAboutToBeRemoved();
        artItemsModel->generateAboutToBeRemoved();
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

        int size = m_AvailabilityListeners.size();
        for (int i = 0; i < size; ++i) {
            m_AvailabilityListeners[i]->removeUnavailableItems();
        }

        artItemsModel->removeUnavailableItems();

    #ifndef CORE_TESTS
        auto *undoRedoManager = m_CommandManager->getUndoRedoManager();
        undoRedoManager->discardLastAction();
    #endif

        auto *artworksRepository = m_CommandManager->getArtworksRepository();

        if (artworksRepository->canPurgeUnavailableFiles()) {
            artworksRepository->purgeUnavailableFiles();
        } else {
            LOG_INFO << "Unavailable files purging postponed";
        }
    }

    void MainDelegator::registerCurrentItem(Artworks::ArtworkMetadata *artwork) const {
        auto *uiManager = m_CommandManager->getUIManager();
        if (uiManager != nullptr) {
            std::shared_ptr<QuickBuffer::ICurrentEditable> currentItem(new QuickBuffer::CurrentEditableArtwork(
                                                                           artwork,
                                                                           artwork->getLastKnownIndex(),
                                                                           m_CommandManager));
            uiManager->registerCurrentItem(currentItem);
        }
    }

    void MainDelegator::registerCurrentItem(Models::ArtworkProxyBase *artworkProxy) const {
        auto *uiManager = m_CommandManager->getUIManager();
        if (uiManager != nullptr) {
            std::shared_ptr<QuickBuffer::ICurrentEditable> currentItem(new QuickBuffer::CurrentEditableProxyArtwork(artworkProxy));
            uiManager->registerCurrentItem(currentItem);
        }
    }

    void MainDelegator::clearCurrentItem() const {
        LOG_DEBUG << "#";
        auto *uiManager = m_CommandManager->getUIManager();
        if (uiManager != nullptr) {
            uiManager->clearCurrentItem();
        }
    }

    Common::WordAnalysisFlags MainDelegator::getWordAnalysisFlags() const {
        auto *settingsModel = m_CommandManager->getSettingsModel();

        Common::WordAnalysisFlags result = Common::WordAnalysisFlags::None;
        if (settingsModel != NULL) {
            if (settingsModel->getUseSpellCheck()) {
                Common::SetFlag(result, Common::WordAnalysisFlags::Spelling);
            }

            if (settingsModel->getDetectDuplicates()) {
                Common::SetFlag(result, Common::WordAnalysisFlags::Stemming);
            }
        }
        return result;
    }
}
