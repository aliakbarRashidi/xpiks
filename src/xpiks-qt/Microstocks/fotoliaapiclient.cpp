/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2018 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "fotoliaapiclient.h"
#include <QUrlQuery>
#include "../Connectivity/simpleapirequest.h"
#include "../Encryption/aes-qt.h"
#include "apisecrets.h"

Microstocks::FotoliaAPIClient::FotoliaAPIClient(Encryption::ISecretsStorage *secretsStorage):
    m_SecretsStorage(secretsStorage)
{
    Q_ASSERT(secretsStorage != nullptr);
}

std::shared_ptr<Connectivity::IConnectivityRequest> Microstocks::FotoliaAPIClient::search(const Microstocks::SearchQuery &query, const std::shared_ptr<Connectivity::IConnectivityResponse> &response) {
    Encryption::SecretPair apiSecret;
    if (!m_SecretsStorage->tryFindPair(FotoliaAPIKey, apiSecret)) { Q_ASSERT(false); }
    QString decodedAPIKey = Encryption::decodeText(apiSecret.m_Value, apiSecret.m_Key);

    QUrl url = buildSearchQuery(decodedAPIKey, query);
    QString resourceUrl = QString::fromLocal8Bit(url.toEncoded());

    std::shared_ptr<Connectivity::IConnectivityRequest> request(new Connectivity::SimpleAPIRequest(resourceUrl, QStringList(), response));
    return request;
}

QUrl Microstocks::FotoliaAPIClient::buildSearchQuery(const QString &apiKey, const Microstocks::SearchQuery &query) const {
    QUrlQuery urlQuery;

    urlQuery.addQueryItem("search_parameters[language_id]", "2");
    urlQuery.addQueryItem("search_parameters[thumbnail_size]", "160");
    urlQuery.addQueryItem("search_parameters[limit]", QString::number(query.m_PageSize));
    urlQuery.addQueryItem("search_parameters[order]", "nb_downloads");
    urlQuery.addQueryItem("search_parameters[words]", query.m_SearchTerms.join(' '));
    urlQuery.addQueryItem("result_columns[0]", "nb_results");
    urlQuery.addQueryItem("result_columns[1]", "title");
    urlQuery.addQueryItem("result_columns[2]", "keywords");
    urlQuery.addQueryItem("result_columns[3]", "thumbnail_url");
    urlQuery.addQueryItem("result_columns[4]", "id");
    urlQuery.addQueryItem(resultsTypeToString(query.m_Flags), "1");

    QUrl url;
    url.setUrl(QLatin1String("http://api.fotolia.com/Rest/1/search/getSearchResults"));
    url.setUserName(apiKey);
    //url.setPassword("");
    url.setQuery(urlQuery);
    return url;
}

QString Microstocks::FotoliaAPIClient::resultsTypeToString(Common::flag_t queryFlags) const {
    if (Common::HasFlag(queryFlags, Microstocks::AllImages)) { return QLatin1String("search_parameters[filters][content_type:all]"); }
    else if (Common::HasFlag(queryFlags, Microstocks::Photos)) { return QLatin1String("search_parameters[filters][content_type:photo]"); }
    else if (Common::HasFlag(queryFlags, Microstocks::Vectors)) { return QLatin1String("search_parameters[filters][content_type:vector]"); }
    else if (Common::HasFlag(queryFlags, Microstocks::Illustrations)) { return QLatin1String("search_parameters[filters][content_type:illustration]"); }
    else { return QString(); }
}
