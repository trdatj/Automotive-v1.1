#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "SerialManager.h"
#include "NetworkManager.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    SerialManager serialManager;
    engine.rootContext()->setContextProperty("serialManager", &serialManager);
    serialManager.start();

    NetworkManager networkManager;
    engine.rootContext()->setContextProperty("networkManager", &networkManager);
    qmlRegisterType<NetworkManager>("com.mycompany.network", 1, 0, "NetworkManager");

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
