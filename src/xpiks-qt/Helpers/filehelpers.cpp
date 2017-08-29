/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "filehelpers.h"
#include <QRegExp>
#include <QFileInfo>
#include <QDir>
#include <QVector>
#include "../Common/defines.h"
#include "../Models/artworkmetadata.h"
#include "../Models/imageartwork.h"

void Helpers::extractFilePathes(const QVector<Models::ArtworkMetadata *> &artworkList,
                       QStringList &filePathes,
                       QStringList &zipsPathes) {

    int size = artworkList.length();
    filePathes.reserve(size);
    zipsPathes.reserve(size);
    LOG_DEBUG << "Generating filepathes for" << size << "item(s)";

    for (int i = 0; i < size; ++i) {
        Models::ArtworkMetadata *metadata = artworkList.at(i);
        QString filepath = metadata->getFilepath();
        filePathes.append(filepath);

        Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork*>(metadata);

        if (image != NULL && image->hasVectorAttached()) {
            filePathes.append(image->getAttachedVectorPath());

            QString zipPath = Helpers::getArchivePath(filepath);
            zipsPathes.append(zipPath);
        } else {
            zipsPathes.append(filepath);
        }
    }
}

QStringList Helpers::convertToVectorFilenames(const QStringList &item) {
    QStringList converted;
    converted.reserve(item.length()*2);

    foreach (const QString &item, item) {
        converted.append(convertToVectorFilenames(item));
    }

    return converted;
}

QStringList Helpers::convertToVectorFilenames(const QString &path) {
    QStringList result;

    QString base;
    if (path.endsWith(".jpg", Qt::CaseInsensitive) ||
            path.endsWith(".tif", Qt::CaseInsensitive)) {
        base = path.mid(0, path.size() - 4);
        result << (base + ".eps") << (base + ".ai");
    } else if (path.endsWith(".jpeg", Qt::CaseInsensitive) ||
               path.endsWith(".tiff", Qt::CaseInsensitive)) {
        base = path.mid(0, path.size() - 5);
        result << (base + ".eps") << (base + ".ai");
    }

    return result;
}

QString Helpers::getArchivePath(const QString &artworkPath) {
    QFileInfo fi(artworkPath);
    QString archiveName = fi.baseName() + ".zip";

    QString basePath = fi.absolutePath();
    QDir dir(basePath);
    QString archivePath = dir.filePath(archiveName);
    return archivePath;
}

QString Helpers::getImagePath(const QString &path) {
    QString result = path;

    QRegExp regExp("(.*)[.](ai|eps|zip)", Qt::CaseInsensitive);
    result.replace(regExp, "\\1.jpg");

    return result;
}

bool Helpers::couldBeVideo(const QString &artworkPath) {
    static QVector<QString> videoExtensions({"avi", "mpeg", "mpg", "mpe", "vob", "qt", "mov", "asf", "asx", "wm", "wmv", "mp4", "webm", "flv"});
    bool found = false;

    for (auto &ext: videoExtensions) {
        if (artworkPath.endsWith(ext, Qt::CaseInsensitive)) {
            found = true;
            break;
        }
    }

    return found;
}

QString Helpers::describeFileSize(qint64 filesizeBytes) {
    double size = filesizeBytes;
    size /= 1024.0*1024.0;

    QString sizeDescription;
    if (size >= 1) {
        sizeDescription = QString::number(size, 'f', 2) + QLatin1String(" MB");
    } else {
        size *= 1024;
        sizeDescription = QString::number(size, 'f', 2) + QLatin1String(" KB");
    }

    return sizeDescription;
}


bool Helpers::ensureDirectoryExists(const QString &path) {
    bool anyError = false;

    if (!QDir(path).exists()) {
        LOG_INFO << "Creating" << path;
        if (!QDir().mkpath(path)) {
            anyError = true;
        }
    }

    return !anyError;
}