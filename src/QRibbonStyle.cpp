#include "QRibbonStyle.h"

#include <QFile>
#include <QMenu>
#include <QWidget>

void QRibbonStyle::ensureResources()
{
    static const bool initialized = []() {
        Q_INIT_RESOURCE(ribbonlib);
        return true;
    }();
    Q_UNUSED(initialized);
}

QString QRibbonStyle::defaultStyleSheet()
{
    ensureResources();

    static const QString styleSheet = []() {
        QFile file(QStringLiteral(":/RibbonLib/styles/ribbon.qss"));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return QString();
        }
        return QString::fromUtf8(file.readAll());
    }();

    return styleSheet;
}

void QRibbonStyle::applyDefaultStyle(QWidget *ribbonRoot)
{
    if (!ribbonRoot || !ribbonRoot->styleSheet().isEmpty()) {
        return;
    }

    const QString qss = defaultStyleSheet();
    if (!qss.isEmpty()) {
        ribbonRoot->setStyleSheet(qss);
    }
}

void QRibbonStyle::applyMenuStyle(QMenu *menu)
{
    if (!menu || !menu->styleSheet().isEmpty()) {
        return;
    }

    const QString qss = defaultStyleSheet();
    if (!qss.isEmpty()) {
        menu->setStyleSheet(qss);
    }
}
