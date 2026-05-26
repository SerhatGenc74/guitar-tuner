#include "mainwindow.h"

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "tunerbackend.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // MainWindow w;
    // w.show();
    QQmlApplicationEngine engine;
    TunerBackend backend;


    engine.rootContext()->setContextProperty("backend", &backend);

    const QUrl url(QStringLiteral("qrc:/Gauger.qml"));
    engine.load(url);

    return a.exec();
}
