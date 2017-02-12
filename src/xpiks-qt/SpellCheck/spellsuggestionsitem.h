/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
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

#ifndef KEYWORDSPELLSUGGESTIONS_H
#define KEYWORDSPELLSUGGESTIONS_H

#include <QString>
#include <QStringList>
#include <QAbstractListModel>
#include <QVector>
#include <vector>
#include <memory>
#include "../Common/flags.h"

namespace Common {
    class IMetadataOperator;
    class BasicKeywordsModel;
    class BasicMetadataModel;
}

namespace SpellCheck {

    class SpellSuggestionsItem: public QAbstractListModel {
        Q_OBJECT
    public:
        SpellSuggestionsItem(const QString &word, const QString &origin);
        SpellSuggestionsItem(const QString &word);
        virtual ~SpellSuggestionsItem() {}

    public:
        enum KeywordSpellSuggestions_Roles {
            SuggestionRole = Qt::UserRole + 1,
            // not same as inner m_IsSelected
            // used for selected replacement
            IsSelectedRole,
            EditReplacementIndexRole
        };

    public:
#if defined(CORE_TESTS) || defined(INTEGRATION_TESTS)
        virtual QString toDebugString() const { return QString("%1 -> (%2)").arg(m_Word).arg(m_Suggestions.join(", ")); }
#endif
        const QString &getWord() const { return m_Word; }
        int getReplacementIndex() const { return m_ReplacementIndex; }

        bool setReplacementIndex(int value);

        const QString &getReplacement() const { return m_Suggestions.at(m_ReplacementIndex); }
        void setSuggestions(const QStringList &suggestions);

        const QString &getReplacementOrigin() const { return m_ReplacementOrigin; }
        bool anyReplacementSelected() const { return m_ReplacementIndex != -1; }

        bool getReplacementSucceeded() const { return m_ReplacementSucceeded; }

    public:
        virtual void replaceToSuggested() = 0;

        // doesn't work like that because of f&cking c++ standard
        // about accessing base protected members in derived class
        // (you cannot access protected members of freestanding objects of base type)
        //protected:
        virtual void replaceToSuggested(const QString &word, const QString &replacement) = 0;

    signals:
        void replacementIndexChanged();

    public:
        virtual int rowCount(const QModelIndex & parent = QModelIndex()) const override;
        virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    protected:
        virtual QHash<int, QByteArray> roleNames() const override;
        void setReplacementSucceeded(bool succeeded) { m_ReplacementSucceeded = succeeded; }
        const QStringList &getSuggestions() const { return m_Suggestions; }

    private:
        QStringList m_Suggestions;
        QString m_Word;
        QString m_ReplacementOrigin;
        int m_ReplacementIndex;
        bool m_ReplacementSucceeded;
    };

    class KeywordSpellSuggestions: public SpellSuggestionsItem
    {
        Q_OBJECT
    public:
        KeywordSpellSuggestions(Common::BasicKeywordsModel *basicModel, const QString &keyword, int originalIndex, const QString &origin);
        KeywordSpellSuggestions(Common::BasicKeywordsModel *basicModel, const QString &keyword, int originalIndex);
        virtual ~KeywordSpellSuggestions();

    public:
#if defined(CORE_TESTS) || defined(INTEGRATION_TESTS)
        virtual QString toDebugString() const override { return QString("KeywordReplace: %1 -> (%2)").arg(getWord()).arg(getSuggestions().join(", ")); }
#endif
        Common::BasicKeywordsModel *getKeywordsModel() const { return m_BasicModel; }
        int getOriginalIndex() const { return m_OriginalIndex; }
        bool isPotentialDuplicate() const { return m_ReplaceResult == Common::KeywordReplaceResult::FailedDuplicate; }
        virtual void replaceToSuggested() override;

        // TODO: fix this back in future when c++ will be normal language (see comments in base class)
    //protected:
        virtual void replaceToSuggested(const QString &word, const QString &replacement) override;

    private:
        int m_OriginalIndex;
        Common::KeywordReplaceResult m_ReplaceResult;
        Common::BasicKeywordsModel *m_BasicModel;
    };

    class DescriptionSpellSuggestions: public SpellSuggestionsItem
    {
        Q_OBJECT
    public:
        DescriptionSpellSuggestions(Common::BasicMetadataModel *metadataModel, const QString &word);
        virtual ~DescriptionSpellSuggestions();

    public:
#if defined(CORE_TESTS) || defined(INTEGRATION_TESTS)
        virtual QString toDebugString() const override { return "DescReplace: " + SpellSuggestionsItem::toDebugString(); }
#endif
        virtual void replaceToSuggested() override;

    //protected:
        virtual void replaceToSuggested(const QString &word, const QString &replacement) override;

    private:
        Common::BasicMetadataModel *m_MetadataModel;
    };

    class TitleSpellSuggestions: public SpellSuggestionsItem
    {
        Q_OBJECT
    public:
        TitleSpellSuggestions(Common::BasicMetadataModel *metadataModel, const QString &word);
        virtual ~TitleSpellSuggestions();

    public:
#if defined(CORE_TESTS) || defined(INTEGRATION_TESTS)
        virtual QString toDebugString() const override { return "TitleReplace: " + SpellSuggestionsItem::toDebugString(); }
#endif
        virtual void replaceToSuggested() override;

    //protected:
        virtual void replaceToSuggested(const QString &word, const QString &replacement) override;

    private:
        Common::BasicMetadataModel *m_MetadataModel;
    };

    class CombinedSpellSuggestions: public SpellSuggestionsItem {
        Q_OBJECT
    public:
        CombinedSpellSuggestions(const QString &word, std::vector<std::shared_ptr<SpellSuggestionsItem> > &suggestions);
        virtual ~CombinedSpellSuggestions();

    public:
#if defined(CORE_TESTS) || defined(INTEGRATION_TESTS)
        virtual QString toDebugString() const override { return "Multireplace: " + SpellSuggestionsItem::toDebugString(); }
#endif
        std::vector<std::shared_ptr<KeywordSpellSuggestions> > getKeywordsDuplicateSuggestions() const;
        virtual void replaceToSuggested() override;

    //protected:
        virtual void replaceToSuggested(const QString &word, const QString &replacement) override;

    private:
        std::vector<std::shared_ptr<SpellSuggestionsItem> > m_SpellSuggestions;
    };
}

Q_DECLARE_METATYPE(SpellCheck::KeywordSpellSuggestions*)

#endif // KEYWORDSPELLSUGGESTIONS_H
