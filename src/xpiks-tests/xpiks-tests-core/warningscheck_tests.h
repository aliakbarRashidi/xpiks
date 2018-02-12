#ifndef WARNINGSCHECKTESTS_H
#define WARNINGSCHECKTESTS_H

#include <QObject>
#include <QtTest/QtTest>

class WarningsCheckTests : public QObject
{
    Q_OBJECT
private slots:
    void emptyKeywordsTest();
    void tooFewKeywordsTest();
    void appendingKeywordChangesWarningsTest();
    void spellingKeywordsTest();
    void spellingKeywordsChangesWhenRemovedTest();
    void spellingDescriptionTest();
    void spellingDescriptionChangesTest();
    void emptyDescriptionTest();
    void descriptionLengthChangesTest();
};

#endif // WARNINGSCHECKTESTS_H
