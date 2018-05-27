/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2018 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "statefulentity.h"
#include <QDir>
#include "../Helpers/constants.h"
#include "../Helpers/filehelpers.h"

namespace Common {
    StatefulEntity::StatefulEntity(const QString &stateName, ISystemEnvironment &environment):
        m_StateName(stateName),
        m_FilePath(environment.path({Constants::STATES_DIR,
                                     QString("%1.json").arg(stateName)})),
        m_StateMap(new Helpers::JsonObjectMap()),
        m_InitCounter(0),
        m_MemoryOnly(environment.getIsInMemoryOnly())
    {
        Q_ASSERT(!stateName.endsWith(".json", Qt::CaseInsensitive));
    }

    void StatefulEntity::init() {
        LOG_DEBUG << m_StateName;

        if (m_InitCounter.fetchAndAddOrdered(1) == 0) {
            m_StateMap = Helpers::LocalConfig(m_FilePath, m_MemoryOnly).readMap();
        } else {
            LOG_WARNING << "Attempt to initialize state" << m_StateName << "twice";
            Q_ASSERT(false);
        }
    }

    void StatefulEntity::sync() {
        LOG_DEBUG << m_StateName << "in memory:" << m_MemoryOnly;

        if (m_InitCounter.loadAcquire() > 0) {
            Helpers::LocalConfig(m_FilePath, m_MemoryOnly).writeMap(m_StateMap);
        } else {
            LOG_WARNING << "State" << m_StateName << "is not initialized!";
            Q_ASSERT(false);
        }
    }
}
