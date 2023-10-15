#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "transcription.h"



int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<Transcription>("Transcription", 1, 0, "Transcription");


    QQmlApplicationEngine engine;


    const QUrl url(u"qrc:/Flickable/Main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
