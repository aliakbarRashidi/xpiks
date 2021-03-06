/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2019 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "updateservice.h"

#include <QString>
#include <QThread>

#include "Common/version.h"
#include "Connectivity/updatescheckerworker.h"
#include "Helpers/updatehelpers.h"
#include "Models/settingsmodel.h"
#include "Models/switchermodel.h"

namespace Common {
    class ISystemEnvironment;
}

namespace Connectivity {
    UpdateService::UpdateService(Common::ISystemEnvironment &environment,
                                 Models::SettingsModel &settingsModel,
                                 Models::SwitcherModel &switcherModel,
                                 Maintenance::MaintenanceService &maintenanceService):
        m_Environment(environment),
        m_UpdatesCheckerWorker(nullptr),
        m_SettingsModel(settingsModel),
        m_SwitcherModel(switcherModel),
        m_MaintenanceService(maintenanceService),
        m_State("updater", environment)
    {
    }

    void UpdateService::initialize() {
        LOG_DEBUG << "#";
        m_State.init();
    }

    void UpdateService::startChecking() {
#ifdef WITH_UPDATES
        LOG_VERBOSE_OR_DEBUG << "settings check update:" << m_SettingsModel.getCheckForUpdates();
        LOG_VERBOSE_OR_DEBUG << "switcher updates:" << m_SwitcherModel.getUpdateEnabled();
        const bool startWorker = m_SettingsModel.getCheckForUpdates() &&
                m_SwitcherModel.getUpdateEnabled();
#else
        const bool startWorker = false;
#endif

        if (startWorker) {
            updateSettings();
            auto pathToUpdate = getPathToUpdate();
            doStartChecking(pathToUpdate);
        } else {
            LOG_INFO << "Update checking disabled";
        }
    }

    void UpdateService::stopChecking() {
        LOG_DEBUG << "#";
        emit cancelRequested();
    }

    void UpdateService::tryToUpgradeXpiks() {
        const int availableVersion = getAvailableUpdateVersion();
        if (m_HaveUpgradeConsent && (availableVersion > XPIKS_VERSION_INT)) {
            const QString pathToUpdate = getPathToUpdate();
            LOG_INFO << "About to install update from" << pathToUpdate;
            Helpers::installUpdate(m_Environment, pathToUpdate);
        }
    }

    void UpdateService::doStartChecking(const QString &pathToUpdate) {
        LOG_DEBUG << "path to update:" << pathToUpdate;

        m_UpdatesCheckerWorker = new UpdatesCheckerWorker(m_Environment, m_SettingsModel, m_MaintenanceService, pathToUpdate);
        QThread *thread = new QThread();
        m_UpdatesCheckerWorker->moveToThread(thread);

        QObject::connect(thread, &QThread::started, m_UpdatesCheckerWorker, &UpdatesCheckerWorker::process);
        QObject::connect(m_UpdatesCheckerWorker, &UpdatesCheckerWorker::stopped, thread, &QThread::quit);

        QObject::connect(m_UpdatesCheckerWorker, &UpdatesCheckerWorker::stopped, m_UpdatesCheckerWorker, &UpdatesCheckerWorker::deleteLater);
        QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        QObject::connect(this, &UpdateService::cancelRequested,
                         m_UpdatesCheckerWorker, &UpdatesCheckerWorker::cancelRequested);

        QObject::connect(m_UpdatesCheckerWorker, &UpdatesCheckerWorker::updateAvailable,
                         this, &UpdateService::updateAvailable);
        QObject::connect(m_UpdatesCheckerWorker, &UpdatesCheckerWorker::updateDownloaded,
                         this, &UpdateService::updateDownloadedHandler);

        QObject::connect(m_UpdatesCheckerWorker, &UpdatesCheckerWorker::stopped,
                         this, &UpdateService::workerFinished);

        thread->start(QThread::LowPriority);
    }

    void UpdateService::updateSettings() {
        LOG_DEBUG << "#";

        const int availableValue = getAvailableUpdateVersion();
        LOG_DEBUG << "Available:" << availableValue << "current:" << XPIKS_VERSION_INT;

        if ((0 < availableValue) && (availableValue <= XPIKS_VERSION_INT)) {
            LOG_DEBUG << "Flushing available update settings values";
            setPathToUpdate("");
            setAvailableUpdateVersion(0);
        }
    }

    void UpdateService::workerFinished() {
        LOG_DEBUG << "#";
    }

    void UpdateService::updateDownloadedHandler(const QString &updatePath, int version) {
        LOG_DEBUG << "#";

        setAvailableUpdateVersion(version);
        setPathToUpdate(updatePath);

        emit updateDownloaded();
    }
}
