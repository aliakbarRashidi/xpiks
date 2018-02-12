#include "warningscheck_tests.h"
#include "../../xpiks-qt/Warnings/warningsitem.h"
#include "Mocks/artworkmetadatamock.h"
#include "Mocks/warningssettingsmock.h"
#include "../../xpiks-qt/Common/flags.h"
#include "../../xpiks-qt/Models/artworkmetadata.h"
#include "../../xpiks-qt/Common/basicmetadatamodel.h"

void WarningsCheckTests::emptyKeywordsTest() {
    Mocks::ArtworkMetadataMock artwork;
    artwork.initialize();
    Mocks::WarningsSettingsMock warningsSettings;

    QVERIFY(artwork.getWarningsFlags() == 0);

    Warnings::WarningsItem(&artwork).checkWarnings(&warningsSettings);

    QVERIFY(Common::HasFlag(artwork.getWarningsFlags(), Common::WarningFlags::NoKeywords));
    QVERIFY(!Common::HasFlag(artwork.getWarningsFlags(), Common::WarningFlags::TooFewKeywords));
}

void WarningsCheckTests::tooFewKeywordsTest() {
    Mocks::ArtworkMetadataMock artwork;
    artwork.initialize();
    Mocks::WarningsSettingsMock warningsSettings;

    artwork.appendKeyword("random");
    QVERIFY(artwork.getWarningsFlags() == 0);

    Warnings::WarningsItem(&artwork).checkWarnings(&warningsSettings);

    QVERIFY(!Common::HasFlag(artwork.getWarningsFlags(), Common::WarningFlags::NoKeywords));
    QVERIFY(Common::HasFlag(artwork.getWarningsFlags(), Common::WarningFlags::TooFewKeywords));
}

void WarningsCheckTests::appendingKeywordChangesWarningsTest() {
    Mocks::ArtworkMetadataMock artwork;
    artwork.initialize();
    Mocks::WarningsSettingsMock warningsSettings;

    Warnings::WarningsItem(&artwork).checkWarnings(&warningsSettings);

    QVERIFY(Common::HasFlag(artwork.getWarningsFlags(), Common::WarningFlags::NoKeywords));
    QVERIFY(!Common::HasFlag(artwork.getWarningsFlags(), Common::WarningFlags::TooFewKeywords));

    artwork.appendKeyword("random");

    Warnings::WarningsItem(&artwork).checkWarnings(&warningsSettings);

    QVERIFY(!Common::HasFlag(artwork.getWarningsFlags(), Common::WarningFlags::NoKeywords));
    QVERIFY(Common::HasFlag(artwork.getWarningsFlags(), Common::WarningFlags::TooFewKeywords));
}

void WarningsCheckTests::spellingKeywordsTest() {
    Mocks::ArtworkMetadataMock artwork;
    artwork.initialize();
    Mocks::WarningsSettingsMock warningsSettings;

    artwork.appendKeyword("qwerty");
    auto &rawKeywords = artwork.getBasicModel()->getRawKeywords();
    rawKeywords[0].m_IsCorrect = false;

    QVERIFY(artwork.getWarningsFlags() == 0);

    Warnings::WarningsItem(&artwork).checkWarnings(&warningsSettings);

    QVERIFY(Common::HasFlag(artwork.getWarningsFlags(), Common::WarningFlags::SpellErrorsInKeywords));
}

void WarningsCheckTests::spellingKeywordsChangesWhenRemovedTest() {
    Mocks::ArtworkMetadataMock artwork;
    artwork.initialize();
    Mocks::WarningsSettingsMock warningsSettings;

    artwork.appendKeyword("qwerty");
    auto &rawKeywords = artwork.getBasicModel()->getRawKeywords();
    rawKeywords[0].m_IsCorrect = false;

    QVERIFY(artwork.getWarningsFlags() == 0);

    Warnings::WarningsItem(&artwork).checkWarnings(&warningsSettings);

    QVERIFY(Common::HasFlag(artwork.getWarningsFlags(), Common::WarningFlags::SpellErrorsInKeywords));
    QVERIFY(!Common::HasFlag(artwork.getWarningsFlags(), Common::WarningFlags::NoKeywords));

    QString dummy;
    artwork.removeLastKeyword(dummy);

    Warnings::WarningsItem(&artwork).checkWarnings(&warningsSettings);

    QVERIFY(!Common::HasFlag(artwork.getWarningsFlags(), Common::WarningFlags::SpellErrorsInKeywords));
    QVERIFY(Common::HasFlag(artwork.getWarningsFlags(), Common::WarningFlags::NoKeywords));
}

void WarningsCheckTests::spellingDescriptionTest() {
    Mocks::ArtworkMetadataMock artwork;
    artwork.initialize();
    Mocks::WarningsSettingsMock warningsSettings;

    artwork.setDescription("valid and crp test");
    artwork.getBasicModel()->getSpellCheckInfo()->setDescriptionErrors(QSet<QString>() << "crp");

    QVERIFY(artwork.getWarningsFlags() == 0);

    Warnings::WarningsItem(&artwork).checkWarnings(&warningsSettings);

    QVERIFY(Common::HasFlag(artwork.getWarningsFlags(), Common::WarningFlags::SpellErrorsInDescription));
}

void WarningsCheckTests::spellingDescriptionChangesTest() {
    Mocks::ArtworkMetadataMock artwork;
    artwork.initialize();
    Mocks::WarningsSettingsMock warningsSettings;

    artwork.setDescription("valid and crp test");
    artwork.getBasicModel()->getSpellCheckInfo()->setDescriptionErrors(QSet<QString>() << "crp");

    QVERIFY(artwork.getWarningsFlags() == 0);

    Warnings::WarningsItem(&artwork).checkWarnings(&warningsSettings);

    QVERIFY(Common::HasFlag(artwork.getWarningsFlags(), Common::WarningFlags::SpellErrorsInDescription));

    artwork.getBasicModel()->getSpellCheckInfo()->clear();

    Warnings::WarningsItem(&artwork).checkWarnings(&warningsSettings);

    QVERIFY(!Common::HasFlag(artwork.getWarningsFlags(), Common::WarningFlags::SpellErrorsInDescription));
}

void WarningsCheckTests::emptyDescriptionTest() {
    Mocks::ArtworkMetadataMock artwork;
    artwork.initialize();
    Mocks::WarningsSettingsMock warningsSettings;

    QVERIFY(artwork.getWarningsFlags() == 0);

    Warnings::WarningsItem(&artwork).checkWarnings(&warningsSettings);

    QVERIFY(Common::HasFlag(artwork.getWarningsFlags(), Common::WarningFlags::DescriptionIsEmpty));
    QVERIFY(!Common::HasFlag(artwork.getWarningsFlags(), Common::WarningFlags::DescriptionNotEnoughWords));
}

void WarningsCheckTests::descriptionLengthChangesTest() {
    Mocks::ArtworkMetadataMock artwork;
    artwork.initialize();
    Mocks::WarningsSettingsMock warningsSettings;

    QVERIFY(artwork.getWarningsFlags() == 0);

    Warnings::WarningsItem(&artwork).checkWarnings(&warningsSettings);

    QVERIFY(Common::HasFlag(artwork.getWarningsFlags(), Common::WarningFlags::DescriptionIsEmpty));
    QVERIFY(!Common::HasFlag(artwork.getWarningsFlags(), Common::WarningFlags::DescriptionNotEnoughWords));

    artwork.setDescription("two words");
    Warnings::WarningsItem(&artwork).checkWarnings(&warningsSettings);

    QVERIFY(!Common::HasFlag(artwork.getWarningsFlags(), Common::WarningFlags::DescriptionIsEmpty));
    QVERIFY(Common::HasFlag(artwork.getWarningsFlags(), Common::WarningFlags::DescriptionNotEnoughWords));

    artwork.setDescription("three words now");
    Warnings::WarningsItem(&artwork).checkWarnings(&warningsSettings);

    QVERIFY(!Common::HasFlag(artwork.getWarningsFlags(), Common::WarningFlags::DescriptionIsEmpty));
    QVERIFY(!Common::HasFlag(artwork.getWarningsFlags(), Common::WarningFlags::DescriptionNotEnoughWords));
}
