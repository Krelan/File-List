#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include "filemodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    FileModel fileModel;
    fileModel.loadDirectory(QDir::currentPath());  // Загружаем файлы из текущей директории

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("fileModel", &fileModel);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("FileBrowser", "Main");

    return app.exec();
}
