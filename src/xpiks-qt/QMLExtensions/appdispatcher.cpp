/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2018 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "appdispatcher.h"
#include <Commands/Base/icommandmanager.h>
#include <Commands/Base/iuicommandtemplate.h>
#include <Commands/Base/templateduicommand.h>

namespace QMLExtensions {
    AppDispatcher::AppDispatcher(Commands::ICommandManager &commandManager, QObject *parent) :
        QObject(parent),
        m_CommandManager(commandManager)
    {
    }

    void AppDispatcher::dispatch(int commandID, QJSValue value) {
        using namespace Commands;
        auto range = m_ComandsMap.equal_range(commandID);
        for (auto it = range.first; it != range.second; it++) {
            m_CommandManager.processCommand(
                        std::make_shared<TemplatedUICommand>(value, it->second));
        }
    }

    void AppDispatcher::registerCommand(int commandID, const std::shared_ptr<Commands::IUICommandTemplate> &command) {
        m_ComandsMap.insert({commandID, command});
    }
}
