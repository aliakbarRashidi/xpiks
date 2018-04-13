/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2018 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "microstockservice.h"

namespace Microstocks {
    MicrostockService::MicrostockService(IMicrostockAPIClient *apiClient, Connectivity::RequestsService &requestsService):
        m_ApiClient(apiClient),
        m_RequestsService(requestsService)
    {
        Q_ASSERT(apiClient != nullptr);
    }

    void MicrostockService::search(const SearchQuery &query, const std::shared_ptr<Connectivity::IConnectivityResponse> &response) {
        std::shared_ptr<Connectivity::IConnectivityRequest> request = m_ApiClient->search(query, response);
        m_RequestsService.sendRequest(request);
    }
}
