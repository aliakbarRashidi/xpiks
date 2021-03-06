/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2019 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SWITCHERMODEL_H
#define SWITCHERMODEL_H

#include <QObject>
#include <QString>
#include <QTimer>

#include "Common/statefulentity.h"
#include "Connectivity/switcherconfig.h"

namespace Common {
    class ISystemEnvironment;
}

namespace Connectivity {
    class IRequestsService;
}

namespace Models {
    class SwitcherModel: public QObject
    {
        Q_OBJECT
        Q_PROPERTY(bool isDonationCampaign1Active READ getIsDonationCampaign1On NOTIFY switchesUpdated)
        Q_PROPERTY(QString donateCampaign1Link READ getDonateCampaign1Link CONSTANT)
        Q_PROPERTY(bool isDonateCampaign1LinkClicked READ getDonateCampaign1LinkClicked NOTIFY donateCampaign1LinkClicked)
        Q_PROPERTY(bool isDonateCampaign1Stage2On READ getIsDonateCampaign1Stage2On NOTIFY switchesUpdated)
        Q_PROPERTY(bool useAutoImport READ getUseAutoImport NOTIFY switchesUpdated)
        Q_PROPERTY(bool keywordsDragDropEnabled READ getKeywordsDragDropEnabled NOTIFY switchesUpdated)
    public:
        SwitcherModel(Common::ISystemEnvironment &environment,
                      QObject *parent=nullptr);

    public:
        void initialize();
        void updateConfigs(Connectivity::IRequestsService &requestsService);
        void afterInitializedCallback();

    private:
        void ensureSessionTokenValid();

    public:
        bool getIsDonationCampaign1On() { return m_Config.isSwitchOn(Connectivity::SwitcherConfig::DonateCampaign1, m_Threshold); }
        bool getIsDonateCampaign1Stage2On() { return m_Config.isSwitchOn(Connectivity::SwitcherConfig::DonateCampaign1Stage2, m_Threshold); }
        bool getProgressiveSuggestionPreviewsOn() { return m_Config.isSwitchOn(Connectivity::SwitcherConfig::ProgressiveSuggestionPreviews, m_Threshold); }
        bool getUseDirectMetadataExport() { return m_Config.isSwitchOn(Connectivity::SwitcherConfig::DirectMetadataExport, m_Threshold); }
        bool getGoodQualityVideoPreviews() { return m_Config.isSwitchOn(Connectivity::SwitcherConfig::GoodQualityVideoPreviews, m_Threshold); }
        bool getUseAutoImport() { return m_Config.isSwitchOn(Connectivity::SwitcherConfig::MetadataAutoImport, m_Threshold); }
        bool getGettySuggestionEnabled() { return m_Config.isSwitchOn(Connectivity::SwitcherConfig::GettySuggestionEnabled, m_Threshold); }
        bool getUpdateEnabled() { return m_Config.isSwitchOn(Connectivity::SwitcherConfig::UpdateEnabled, m_Threshold); }
        bool getKeywordsDragDropEnabled() { return m_Config.isSwitchOn(Connectivity::SwitcherConfig::KeywordsDragDropEnabled, m_Threshold); }
        bool getIsTelemetryEnabled() { return m_Config.isSwitchOn(Connectivity::SwitcherConfig::TelemetryEnabled, m_Threshold); }

    public:
        bool getDonateCampaign1LinkClicked() const;
        QString getDonateCampaign1Link() const { return QString("https://xpiksapp.com/donatecampaign/"); }

#if defined(INTEGRATION_TESTS) || defined(UI_TESTS)
    public:
        void setRemoteConfigOverride(const QByteArray &data) { m_Config.setRemoteOverride(data); }
#endif

    public:
        Q_INVOKABLE void setDonateCampaign1LinkClicked();

    private slots:
        void configUpdated();
        void onDelayTimer();

    signals:
        void switchesUpdated();
        void donateCampaign1LinkClicked();
        void donateDialogRequested();

    private:
        void readEngagementConfig();

    private:
        Common::StatefulEntity m_State;
        QTimer m_DelayTimer;
        Connectivity::SwitcherConfig m_Config;
        int m_Threshold;
    };
}

#endif // SWITCHERMODEL_H
