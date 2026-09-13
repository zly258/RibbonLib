#include "QRibbonMenu.h"
#include "QRibbonStyle.h"

QRibbonMenu::QRibbonMenu(QWidget *parent)
    : QMenu(parent)
{
    setObjectName(QStringLiteral("RibbonMenu"));
    QRibbonStyle::applyMenuStyle(this);
}

QRibbonMenu::QRibbonMenu(const QString &title, QWidget *parent)
    : QMenu(title, parent)
{
    setObjectName(QStringLiteral("RibbonMenu"));
    QRibbonStyle::applyMenuStyle(this);
}
