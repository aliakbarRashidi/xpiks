/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2018 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <QByteArray>
#include <QDataStream>
#include <QAtomicInt>
#include <QMutex>
#include <QHash>
#include <QString>
#include <QVector>
#include <QPair>
#include <vector>
#include <memory>
#include <functional>
#include "../Common/defines.h"
#include "../Common/isystemenvironment.h"
#include "idatabasemanager.h"
#include "idatabase.h"

struct sqlite3;
struct sqlite3_stmt;

namespace Helpers {
    class AsyncCoordinator;
}

namespace Storage {
    // super simple wrapper over sqlite
    // to make it look like a key-value storage
    class Database: public IDatabase {
    public:
        Database(int id, Helpers::AsyncCoordinator *finalizeCoordinator);
        virtual ~Database();

    private:
        class Transaction {
        public:
            Transaction(sqlite3 *database);
            virtual ~Transaction();

        private:
            sqlite3 *m_Database;
            bool m_Started;
        };

    public:
        class Table: public IDbTable {
        public:
            Table(sqlite3 *database, const QString &tableName);

        public:
            virtual bool initialize() override;
            virtual void finalize() override;

        public:
            virtual bool tryGetValue(const QByteArray &key, QByteArray &value) override;
            virtual bool trySetValue(const QByteArray &key, const QByteArray &value) override;
            virtual bool tryAddValue(const QByteArray &key, const QByteArray &value) override;
            virtual bool trySetMany(const QVector<QPair<QByteArray, QByteArray> > &keyValueList, QVector<int> &failedIndices) override;
            virtual int tryAddMany(const QVector<QPair<QByteArray, QByteArray> > &keyValueList) override;
            virtual bool tryDeleteRecord(const QByteArray &key) override;
            virtual bool tryDeleteMany(const QVector<QByteArray> &keysList) override;
            virtual void foreachRow(const std::function<bool (QByteArray &, QByteArray &)> &action) override;

        private:
            QString m_TableName;
            sqlite3 *m_Database;
            sqlite3_stmt *m_GetStatement;
            sqlite3_stmt *m_SetStatement;
            sqlite3_stmt *m_AddStatement;
            sqlite3_stmt *m_DelStatement;
            sqlite3_stmt *m_AllStatement;
        };

    public:
        bool open(const char *fullDbPath);
        void close();
        bool initialize();
        void finalize();
        void sync();
        std::shared_ptr<IDbTable> getTable(const QString &name);
        QStringList retrieveTableNames();

    private:
        void doClose();
        bool executeStatement(const char *stmt);

    private:
        int m_ID;
        Helpers::AsyncCoordinator *m_FinalizeCoordinator = nullptr;
        sqlite3 *m_Database = nullptr;
        sqlite3_stmt *m_GetTablesStatement = nullptr;
        std::vector<std::shared_ptr<IDbTable> > m_Tables;
        volatile bool m_IsOpened = false;
    };
}

#endif // DATABASE_H