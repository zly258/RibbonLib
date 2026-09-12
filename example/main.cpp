#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>
#include <QLibraryInfo>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application metadata
    app.setApplicationName("RibbonLib Example");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("RibbonLib");
    
    try {
        // Use Fusion style as baseline
        app.setStyle(QStyleFactory::create("Fusion"));
        
        // Configure typography based on OS platform
        QFont font = QApplication::font();
        QString defaultFamily;
    #ifdef Q_OS_WINDOWS
        defaultFamily = "Segoe UI";
    #elif defined(Q_OS_MACOS)
        defaultFamily = "PingFang SC";
    #elif defined(Q_OS_LINUX)
        defaultFamily = "WenQuanYi Micro Hei";
    #else
        defaultFamily = "Arial";
    #endif
        font.setPointSize(9);
        font.setFamily(defaultFamily);

        app.setFont(font);

        // Load Ribbon Light QSS stylesheet
        QString ribbonQssContent;
        QFile qssFile(QCoreApplication::applicationDirPath() + "/example/resources/styles/ribbon.qss");
        if (qssFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            ribbonQssContent = QString::fromUtf8(qssFile.readAll());
        }

        // Apply stylesheet
        app.setStyleSheet(ribbonQssContent);
        QApplication::processEvents();

    } catch (...) {
        qDebug() << "Exception occurred while applying Fusion theme";
    }

    // Create and show main window
    MainWindow window;
    window.show();
    
    return app.exec();
}
