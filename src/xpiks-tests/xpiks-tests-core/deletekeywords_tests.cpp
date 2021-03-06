#include "deletekeywords_tests.h"

#include <memory>

#include <QStringList>

#include "Artworks/basickeywordsmodel.h"
#include "Commands/Base/icommand.h"
#include "KeywordsPresets/presetkeywordsmodel.h"
#include "Models/Editing/deletekeywordsviewmodel.h"
#include "Models/Session/recentdirectoriesmodel.h"

#include "Mocks/artworkmetadatamock.h"
#include "Mocks/artworkslistmodelmock.h"
#include "Mocks/artworksrepositorymock.h"
#include "Mocks/artworksupdatermock.h"
#include "Mocks/coretestsenvironment.h"

#define DECLARE_MODELS_AND_GENERATE(count) \
    Mocks::CoreTestsEnvironment environment; \
    Models::RecentDirectoriesModel recentDirectories(environment);\
    recentDirectories.initialize();\
    Mocks::ArtworksRepositoryMock artworksRepository(recentDirectories); \
    Mocks::ArtworksListModelMock artworksListModel(artworksRepository); \
    KeywordsPresets::PresetKeywordsModel keywordsPresets(environment);\
    Mocks::ArtworksUpdaterMock updater;\
    Models::DeleteKeywordsViewModel deleteKeywordsModel(updater, keywordsPresets); \
    artworksListModel.generateAndAddArtworks(count);

void DeleteKeywordsTests::smokeTest() {
    DECLARE_MODELS_AND_GENERATE(2);

    const QString keywordToDelete = "keyword1";
    QStringList keywords;
    keywords << keywordToDelete << "keyword2" << "keyword3";

    artworksListModel.foreachArtwork([&keywords](int, std::shared_ptr<Mocks::ArtworkMetadataMock> const &artwork) {
        artwork->setKeywords(keywords);
    });

    deleteKeywordsModel.setArtworks(artworksListModel.createArtworksSnapshot());
    deleteKeywordsModel.appendKeywordToDelete(keywordToDelete);
    deleteKeywordsModel.getActionCommand(true)->execute();

    artworksListModel.foreachArtwork([&](int, std::shared_ptr<Mocks::ArtworkMetadataMock> const &artwork) {
        QCOMPARE(artwork->rowCount(), keywords.length() - 1);
        auto &keywordsModel = artwork->getBasicModel();
        QVERIFY(!keywordsModel.containsKeyword(keywordToDelete));
    });
}

void DeleteKeywordsTests::keywordsCombinedTest() {
    DECLARE_MODELS_AND_GENERATE(3);

    const QString keywordToDelete = "keywordToDelete";

    artworksListModel.foreachArtwork([&](int index, std::shared_ptr<Mocks::ArtworkMetadataMock> const &artwork) {
        artwork->clearKeywords();
        artwork->appendKeyword("keyword" + QString::number(index));
        artwork->appendKeyword(keywordToDelete);
    });

    deleteKeywordsModel.setArtworks(artworksListModel.createArtworksSnapshot());

    QVERIFY(deleteKeywordsModel.containsCommonKeyword(keywordToDelete));

    deleteKeywordsModel.appendKeywordToDelete(keywordToDelete);
    deleteKeywordsModel.getActionCommand(true)->execute();

    artworksListModel.foreachArtwork([&](int, std::shared_ptr<Mocks::ArtworkMetadataMock> const &artwork) {
        auto &keywordsModel = artwork->getBasicModel();
        QVERIFY(!keywordsModel.containsKeyword(keywordToDelete));
    });
}

void DeleteKeywordsTests::doesNotDeleteOtherCaseTest() {
    DECLARE_MODELS_AND_GENERATE(3);

    const QString keywordToDelete = "keywordToDelete";

    artworksListModel.foreachArtwork([&](int index, std::shared_ptr<Mocks::ArtworkMetadataMock> const &artwork) {
        artwork->clearKeywords();
        artwork->appendKeyword("keyword" + QString::number(index));
        artwork->appendKeyword(keywordToDelete);
    });

    deleteKeywordsModel.setArtworks(artworksListModel.createArtworksSnapshot());
    deleteKeywordsModel.appendKeywordToDelete(keywordToDelete.toUpper());
    deleteKeywordsModel.setCaseSensitive(true);
    deleteKeywordsModel.getActionCommand(true)->execute();

    artworksListModel.foreachArtwork([&](int, std::shared_ptr<Mocks::ArtworkMetadataMock> const &artwork) {
        auto &keywordsModel = artwork->getBasicModel();
        QCOMPARE(keywordsModel.getKeywordsCount(), 2);
        QVERIFY(keywordsModel.containsKeyword(keywordToDelete));
    });
}

void DeleteKeywordsTests::doesNotDeleteNoKeywordsTest() {
    DECLARE_MODELS_AND_GENERATE(3);

    const QString keywordToDelete = "keywordToDelete";

    artworksListModel.foreachArtwork([&](int index, std::shared_ptr<Mocks::ArtworkMetadataMock> const &artwork) {
        artwork->clearKeywords();
        artwork->appendKeyword("keyword" + QString::number(index));
        artwork->appendKeyword(keywordToDelete);
    });

    deleteKeywordsModel.setArtworks(artworksListModel.createArtworksSnapshot());
    deleteKeywordsModel.getActionCommand(true)->execute();

    artworksListModel.foreachArtwork([&](int, std::shared_ptr<Mocks::ArtworkMetadataMock> const &artwork) {
        auto &keywordsModel = artwork->getBasicModel();
        QCOMPARE(keywordsModel.getKeywordsCount(), 2);
        QVERIFY(keywordsModel.containsKeyword(keywordToDelete));
    });
}

void DeleteKeywordsTests::deleteCaseInsensitiveTest() {
    DECLARE_MODELS_AND_GENERATE(5);

    const QString keywordToDelete = "keywordToDelete";

    artworksListModel.foreachArtwork([&](int index, std::shared_ptr<Mocks::ArtworkMetadataMock> const &artwork) {
        artwork->clearKeywords();
        artwork->appendKeyword("keyword" + QString::number(index));
        artwork->appendKeyword(index % 2 == 0 ? keywordToDelete.toLower() : keywordToDelete.toUpper());
    });

    deleteKeywordsModel.setArtworks(artworksListModel.createArtworksSnapshot());
    deleteKeywordsModel.appendKeywordToDelete(keywordToDelete);
    deleteKeywordsModel.setCaseSensitive(false);
    deleteKeywordsModel.getActionCommand(true)->execute();

    artworksListModel.foreachArtwork([&](int, std::shared_ptr<Mocks::ArtworkMetadataMock> const &artwork) {
        auto &keywordsModel = artwork->getBasicModel();
        QCOMPARE(keywordsModel.getKeywordsCount(), 1);
        QVERIFY(!keywordsModel.containsKeyword(keywordToDelete));
    });
}
