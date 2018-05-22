#ifndef CORETESTSENVIRONMENT_H
#define CORETESTSENVIRONMENT_H

#include "../../xpiks-qt/Common/isystemenvironment.h"
#include <QDir>

namespace Mocks {
    class CoreTestsEnvironment: public Common::ISystemEnvironment {
        virtual QString root() const override { return QString(); }
        virtual QString path(const QStringList &path) override { return path.join(QDir::separator()); }
        virtual bool ensureDirExists(const QString &name) override { return true; }
        virtual bool getIsInMemoryOnly() const override { return true; }
    };
}

#endif // CORETESTSENVIRONMENT_H