#include "filteredmodel_tests.h"
#include "Mocks/artitemsmodelmock.h"
#include "Mocks/commandmanagermock.h"
#include "Mocks/artworksrepositorymock.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/Models/artworksrepository.h"
#include "../../xpiks-qt/Models/ziparchiver.h"
#include "../../xpiks-qt/Models/settingsmodel.h"
#include "Mocks/coretestsenvironment.h"

#define DECLARE_MODELS_AND_GENERATE(count) \
    Mocks::CoreTestsEnvironment environment; \
    Mocks::CommandManagerMock commandManagerMock;\
    Mocks::ArtItemsModelMock artItemsModelMock;\
    Mocks::ArtworksRepositoryMock artworksRepository(environment);\
    Models::FilteredArtItemsProxyModel filteredItemsModel;\
    commandManagerMock.InjectDependency(&artworksRepository);\
    commandManagerMock.InjectDependency(&artItemsModelMock);\
    filteredItemsModel.setSourceModel(&artItemsModelMock);\
    commandManagerMock.InjectDependency(&filteredItemsModel);\
    commandManagerMock.generateAndAddArtworks(count);\
    Models::SettingsModel settingsModel(environment); \
    settingsModel.initializeConfigs(); \
    settingsModel.setSearchUsingAnd(false); \
    commandManagerMock.InjectDependency(&settingsModel);

void FilteredModelTests::invertSelectionForEmptyTest(){
    DECLARE_MODELS_AND_GENERATE(10);

    int allItemsCount = filteredItemsModel.getItemsCount();

    filteredItemsModel.invertSelectionArtworks();

    int selected = filteredItemsModel.retrieveNumberOfSelectedItems();
    QCOMPARE(selected, allItemsCount);
}

void FilteredModelTests::invertSelectionForAllSelectedTest() {
    DECLARE_MODELS_AND_GENERATE(10);

    int allItemsCount = filteredItemsModel.getItemsCount();
    for (int i = 0; i < allItemsCount; i++) {
        artItemsModelMock.getArtwork(i)->setIsSelected(true);
    }

    filteredItemsModel.invertSelectionArtworks();

    int selected = filteredItemsModel.retrieveNumberOfSelectedItems();
    QCOMPARE(selected, 0);
}

void FilteredModelTests::invertSelectionForSingleTest(){
    DECLARE_MODELS_AND_GENERATE(10);

    int allItemsCount = filteredItemsModel.getItemsCount();

    artItemsModelMock.getArtwork(0)->setIsSelected(true);

    filteredItemsModel.invertSelectionArtworks();

    int selected = filteredItemsModel.retrieveNumberOfSelectedItems();
    QCOMPARE(selected, (allItemsCount-1));
}

void FilteredModelTests::invertSelectionForHalfSelectedTest(){
    DECLARE_MODELS_AND_GENERATE(10);

    int allItemsCount = filteredItemsModel.getItemsCount();
    for (int i =0; i < allItemsCount; i++) {
        artItemsModelMock.getArtwork(i)->setIsSelected(i < allItemsCount/2);
    }

    filteredItemsModel.invertSelectionArtworks();

    int selected = filteredItemsModel.retrieveNumberOfSelectedItems();
    QCOMPARE(selected, (allItemsCount - allItemsCount/2));
}

void FilteredModelTests::invertSelectionForEvenCountTest() {
    DECLARE_MODELS_AND_GENERATE(10);

    int allItemsCount = filteredItemsModel.getItemsCount();
    for (int i = 0; i < allItemsCount; i++) {
        artItemsModelMock.getArtwork(i)->setIsSelected(i < allItemsCount/3);
    }

    filteredItemsModel.invertSelectionArtworks();

    int selected = filteredItemsModel.retrieveNumberOfSelectedItems();
    QCOMPARE(selected, (allItemsCount - allItemsCount/3));
}

void FilteredModelTests::invertSelectionForOddCountTest(){
    DECLARE_MODELS_AND_GENERATE(11);

    int allItemsCount = filteredItemsModel.getItemsCount();
    for (int i =0; i<allItemsCount; i++) {
        artItemsModelMock.getArtwork(i)->setIsSelected(i < allItemsCount/3);
    }

    filteredItemsModel.invertSelectionArtworks();

    int selected = filteredItemsModel.retrieveNumberOfSelectedItems();
    QCOMPARE(selected, (allItemsCount - allItemsCount/3));
}

void FilteredModelTests::removeMetadataMarksAsModifiedTest() {
    DECLARE_MODELS_AND_GENERATE(1);

    auto *artwork = artItemsModelMock.getMockArtwork(0);

    QVERIFY(!artwork->isModified());

    artwork->set("title", "description", QStringList() << "keyword1" << "keyword2");
    artwork->setIsSelected(true);
    filteredItemsModel.removeMetadataInSelected();

    QVERIFY(artwork->isModified());
}

void FilteredModelTests::removeMetadataDeletesMetadataTest() {
    DECLARE_MODELS_AND_GENERATE(10);

    for (int i = 0; i < 10; ++i) {
        auto *artwork = artItemsModelMock.getMockArtwork(i);
        artwork->set("title", "description", QStringList() << "keyword1" << "keyword2");
        artwork->setIsSelected(true);
    }

    filteredItemsModel.removeMetadataInSelected();

    for (int i = 0; i < 10; ++i) {
        Mocks::ArtworkMetadataMock *artwork = artItemsModelMock.getMockArtwork(i);
        QVERIFY(artwork->isDescriptionEmpty());
        QVERIFY(artwork->isTitleEmpty());
        QVERIFY(artwork->areKeywordsEmpty());
        QVERIFY(artwork->isModified());
    }
}

void FilteredModelTests::selectedCountAddTest() {
    DECLARE_MODELS_AND_GENERATE(10);
    QObject::connect(&artItemsModelMock, &Models::ArtItemsModel::artworkSelectedChanged,
                     &filteredItemsModel, &Models::FilteredArtItemsProxyModel::itemSelectedChanged);

    QCOMPARE(filteredItemsModel.getSelectedArtworksCount(), 0);

    for (int i = 0; i < 10; i += 2) {
        Mocks::ArtworkMetadataMock *artwork = artItemsModelMock.getMockArtwork(i);
        artwork->setIsSelected(true);
    }

    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    QCOMPARE(filteredItemsModel.getSelectedArtworksCount(), 5);
}

void FilteredModelTests::selectedCountSubtractTest() {
    DECLARE_MODELS_AND_GENERATE(10);
    QObject::connect(&artItemsModelMock, &Models::ArtItemsModel::artworkSelectedChanged,
                     &filteredItemsModel, &Models::FilteredArtItemsProxyModel::itemSelectedChanged);

    QCOMPARE(filteredItemsModel.getSelectedArtworksCount(), 0);

    filteredItemsModel.selectFilteredArtworks();

    for (int i = 0; i < 10; i += 2) {
        Mocks::ArtworkMetadataMock *artwork = artItemsModelMock.getMockArtwork(i);
        artwork->setIsSelected(false);
    }

    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    QCOMPARE(filteredItemsModel.getSelectedArtworksCount(), 5);
}

void FilteredModelTests::findSelectedIndexTest() {
    DECLARE_MODELS_AND_GENERATE(10);
    artItemsModelMock.getArtwork(4)->setIsSelected(true);
    int index = filteredItemsModel.findSelectedItemIndex();
    QCOMPARE(index, 4);

    artItemsModelMock.getArtwork(9)->setIsSelected(true);
    index = filteredItemsModel.findSelectedItemIndex();
    QCOMPARE(index, -1);
}

void FilteredModelTests::clearKeywordsTest() {
    DECLARE_MODELS_AND_GENERATE(1);

    Mocks::ArtworkMetadataMock *artwork = artItemsModelMock.getMockArtwork(0);
    artwork->set("title", "description", QStringList() << "keyword1" << "keyword2");

    filteredItemsModel.clearKeywords(0);

    QVERIFY(!artwork->isDescriptionEmpty());
    QVERIFY(!artwork->isTitleEmpty());
    QVERIFY(artwork->areKeywordsEmpty());
    QVERIFY(artwork->isModified());
}

void FilteredModelTests::detachVectorFromSelectedTest() {
    DECLARE_MODELS_AND_GENERATE(10);

    for (int i = 0; i < 10; ++i) {
        Mocks::ArtworkMetadataMock *artwork = artItemsModelMock.getMockArtwork(i);
        artwork->set("title", "description", QStringList() << "keyword1" << "keyword2");
        artwork->attachVector(QString(artwork->getFilepath()).replace(".jpg", ".eps"));
        artwork->setIsSelected(true);
    }

    filteredItemsModel.detachVectorFromSelected();

    for (int i = 0; i < 10; ++i) {
        Mocks::ArtworkMetadataMock *artwork = artItemsModelMock.getMockArtwork(i);
        QVERIFY(!artwork->isModified());
        QVERIFY(!artwork->hasVectorAttached());
    }
}

void FilteredModelTests::setSelectedForZippingTest() {
    DECLARE_MODELS_AND_GENERATE(10);
    Models::ZipArchiver zipArchiver;
    commandManagerMock.InjectDependency(&zipArchiver);

    for (int i = 0; i < 10; ++i) {
        auto *artwork = artItemsModelMock.getMockArtwork(i);
        artwork->set("title", "description", QStringList() << "keyword1" << "keyword2");
        artwork->attachVector("/path/to/random/vector.eps");

        if (i % 2) {
            artwork->setIsSelected(true);
        }
    }

    filteredItemsModel.setSelectedForZipping();

    auto &snapshot = zipArchiver.getArtworksSnapshot();
    QCOMPARE((int)snapshot.size(), 5);
}

void FilteredModelTests::filterModifiedItemsTest() {
    DECLARE_MODELS_AND_GENERATE(10);

    for (int i = 0; i < 10; ++i) {
        auto *artwork = artItemsModelMock.getMockArtwork(i);
        artwork->set("title", "description", QStringList() << "keyword1" << "keyword2");

        if (i % 2) {
            artwork->setModified();
        }
    }

    filteredItemsModel.setSearchTerm("x:modified");
    QCOMPARE(filteredItemsModel.getItemsCount(), 5);
}

void FilteredModelTests::filterEmptyItemsTest() {
    DECLARE_MODELS_AND_GENERATE(10);

    for (int i = 0; i < 10; ++i) {
        auto *artwork = artItemsModelMock.getMockArtwork(i);

        if (i % 2) {
            artwork->set("title", "description", QStringList() << "keyword1" << "keyword2");
        } else {
            artwork->set("", "", QStringList());
        }
    }

    filteredItemsModel.setSearchTerm("x:empty");
    QCOMPARE(filteredItemsModel.getItemsCount(), 5);
}

void FilteredModelTests::filterKeywordsUsingAndTest() {
    DECLARE_MODELS_AND_GENERATE(10);
    settingsModel.setSearchUsingAnd(true);

    for (int i = 0; i < 10; ++i) {
        auto *artwork = artItemsModelMock.getMockArtwork(i);

        if (i % 2 == 0) {
            artwork->set("title", "description", QStringList() << "keyword1" << "keyword2" << "mess2");
            if (i % 4 == 0) {
                artwork->setModified();
            }
        } else {
            artwork->set("", "", QStringList());
        }
    }

    filteredItemsModel.setSearchTerm("keyword1 keyword2");
    QCOMPARE(filteredItemsModel.getItemsCount(), 5);

    filteredItemsModel.setSearchTerm("keyword1 keyword2 x:modified");
    QCOMPARE(filteredItemsModel.getItemsCount(), 3);
}

void FilteredModelTests::filterKeywordsUsingOrTest() {
    DECLARE_MODELS_AND_GENERATE(10);

    for (int i = 0; i < 10; ++i) {
        auto *artwork = artItemsModelMock.getMockArtwork(i);

        if (i % 2 == 0) {
            artwork->set("title", "description", QStringList() << "keyword1" << "mess1");
        } else {
            artwork->set("title", "description", QStringList() << "keyword2" << "mess2");
        }
    }

    filteredItemsModel.setSearchTerm("keyword1");
    QCOMPARE(filteredItemsModel.getItemsCount(), 5);

    filteredItemsModel.setSearchTerm("keyword1 keyword2");
    QCOMPARE(filteredItemsModel.getItemsCount(), 10);
}

void FilteredModelTests::filterStrictKeywordTest() {
    DECLARE_MODELS_AND_GENERATE(10);

    for (int i = 0; i < 10; ++i) {
        auto *artwork = artItemsModelMock.getMockArtwork(i);

        if (i % 2 == 0) {
            artwork->set("title", "description", QStringList() << "keyword1" << "mess1");
        } else {
            artwork->set("title", "description", QStringList() << "keyword2" << "mess2");
        }
    }

    filteredItemsModel.setSearchTerm("keyword");
    QCOMPARE(filteredItemsModel.getItemsCount(), 10);

    filteredItemsModel.setSearchTerm("!keyword");
    QCOMPARE(filteredItemsModel.getItemsCount(), 0);


    filteredItemsModel.setSearchTerm("!keyword1");
    QCOMPARE(filteredItemsModel.getItemsCount(), 5);
}

void FilteredModelTests::filterDescriptionTest() {
    DECLARE_MODELS_AND_GENERATE(10);

    for (int i = 0; i < 10; ++i) {
        auto *artwork = artItemsModelMock.getMockArtwork(i);

        if (i % 2) {
            artwork->set("title", "description", QStringList() << "keyword1" << "keyword2");
        } else {
            artwork->set("", "", QStringList());
        }
    }

    filteredItemsModel.setSearchTerm("desc");
    QCOMPARE(filteredItemsModel.getItemsCount(), 5);

    filteredItemsModel.setSearchTerm("!desc");
    QCOMPARE(filteredItemsModel.getItemsCount(), 0);

    filteredItemsModel.setSearchTerm("description");
    QCOMPARE(filteredItemsModel.getItemsCount(), 5);

    // strict match does not work for description/title
    filteredItemsModel.setSearchTerm("!description");
    QCOMPARE(filteredItemsModel.getItemsCount(), 0);
}

void FilteredModelTests::filterTitleTest() {
    DECLARE_MODELS_AND_GENERATE(10);

    for (int i = 0; i < 10; ++i) {
        auto *artwork = artItemsModelMock.getMockArtwork(i);

        if (i % 2) {
            artwork->set("title", "description", QStringList() << "keyword1" << "keyword2");
        } else {
            artwork->set("", "", QStringList());
        }
    }

    filteredItemsModel.setSearchTerm("tit");
    QCOMPARE(filteredItemsModel.getItemsCount(), 5);

    filteredItemsModel.setSearchTerm("!tit");
    QCOMPARE(filteredItemsModel.getItemsCount(), 0);

    filteredItemsModel.setSearchTerm("title");
    QCOMPARE(filteredItemsModel.getItemsCount(), 5);

    // strict match does not work for description/title
    filteredItemsModel.setSearchTerm("!title");
    QCOMPARE(filteredItemsModel.getItemsCount(), 0);
}

void FilteredModelTests::filterDescriptionAndKeywordsTest() {
    DECLARE_MODELS_AND_GENERATE(10);

    for (int i = 0; i < 10; ++i) {
        auto *artwork = artItemsModelMock.getMockArtwork(i);

        if (i % 2 == 0) {
            artwork->set("title", "description", QStringList() << "keyword1" << "mess1");
        } else {
            artwork->set("title", "description", QStringList() << "keyword2" << "mess2");
        }
    }

    filteredItemsModel.setSearchTerm("keyword1 desc");
    QCOMPARE(filteredItemsModel.getItemsCount(), 10);

    filteredItemsModel.setSearchTerm("!keyword desc");
    QCOMPARE(filteredItemsModel.getItemsCount(), 10);

    settingsModel.setSearchUsingAnd(true);

    filteredItemsModel.setSearchTerm("!keyword desc");
    QCOMPARE(filteredItemsModel.getItemsCount(), 0);

    filteredItemsModel.setSearchTerm("!keyword2 description");
    QCOMPARE(filteredItemsModel.getItemsCount(), 5);
}

void FilteredModelTests::filterTitleAndKeywordsTest() {
    DECLARE_MODELS_AND_GENERATE(10);

    for (int i = 0; i < 10; ++i) {
        auto *artwork = artItemsModelMock.getMockArtwork(i);

        if (i % 2 == 0) {
            artwork->set("title", "description", QStringList() << "keyword1" << "mess1");
        } else {
            artwork->set("title", "description", QStringList() << "keyword2" << "mess2");
        }
    }

    filteredItemsModel.setSearchTerm("keyword1 tit");
    QCOMPARE(filteredItemsModel.getItemsCount(), 10);

    filteredItemsModel.setSearchTerm("!keyword tit");
    QCOMPARE(filteredItemsModel.getItemsCount(), 10);

    settingsModel.setSearchUsingAnd(true);

    filteredItemsModel.setSearchTerm("!keyword tit");
    QCOMPARE(filteredItemsModel.getItemsCount(), 0);

    filteredItemsModel.setSearchTerm("!keyword2 title");
    QCOMPARE(filteredItemsModel.getItemsCount(), 5);
}

void FilteredModelTests::clearEmptyKeywordsTest() {
    DECLARE_MODELS_AND_GENERATE(1);
    Models::ArtworkMetadata *artwork = artItemsModelMock.getArtwork(0);
    artwork->clearKeywords();

    commandManagerMock.resetAnyCommandProcessed();
    filteredItemsModel.clearKeywords(0);
    QVERIFY(!commandManagerMock.anyCommandProcessed());
}
