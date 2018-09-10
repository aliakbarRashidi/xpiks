#include "testshost.h"
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickImageProvider>
#include "xpiksuitestsapp.h"
#include <Common/logging.h>

class FakeImageProvider: public QQuickImageProvider
{
public:
    FakeImageProvider()
        : QQuickImageProvider(QQuickImageProvider::Image)
    {
    }

public:
    virtual QImage requestImage(const QString &id, QSize *size, const QSize& requestedSize) override {
        Q_UNUSED(id);
        Q_UNUSED(requestedSize);

        const int width = 300;
        const int height = 300;

        if (size) {
            *size = QSize(width, height);
        }

        QImage img(width, height, QImage::Format_RGB888);
        img.fill(Qt::black);

        return img;
    }
};

TestsHost::TestsHost(QObject *parent) :
    QObject(parent)
{
}

int TestsHost::getSmallSleepTime() const {
#if defined(TRAVIS_CI) || defined(APPVEYOR)
    return 500;
#else
    return 300;
#endif
}

int TestsHost::getNormalSleepTime() const {
#if defined(TRAVIS_CI) || defined(APPVEYOR)
    return 800;
#else
    return 600;
#endif
}

void TestsHost::qmlEngineCallback(QQmlEngine *engine) {
    m_XpiksApp->setupUI(engine->rootContext());    

    engine->addImageProvider("cached", new FakeImageProvider());
    engine->addImageProvider("global", new FakeImageProvider());

    for (auto &path: m_XpiksApp->getQmlImportPaths()) {
        engine->addImportPath(path);
    }
}

void TestsHost::setup() {
    LOG_DEBUG << "#";
    m_XpiksApp->setupUITests();
    processPendingEvents();
}

void TestsHost::cleanupTest() {
    LOG_DEBUG << "#";
    m_XpiksApp->cleanupTest();
    processPendingEvents();
}

void TestsHost::cleanup() {
    LOG_DEBUG << "#";
    m_XpiksApp->cleanup();
    processPendingEvents();
}

void TestsHost::setApp(XpiksUITestsApp *app) {
    Q_ASSERT(app != nullptr);
    m_XpiksApp = app;
}

void TestsHost::processPendingEvents() {
    QEventLoop loop;
    loop.processEvents(QEventLoop::AllEvents);
}