#include "QApplicationButton.h"
#include "QRibbonMetrics.h"
#include "QRibbonStyle.h"

#include <QFontMetrics>
#include <QMenu>

QApplicationButton::QApplicationButton(QWidget *parent)
    : QToolButton(parent)
{
    setObjectName("ApplicationButton");
    setFixedHeight(QRibbonMetrics::TopBarHeight);
    setPopupMode(QToolButton::InstantPopup);
    setToolButtonStyle(Qt::ToolButtonTextOnly);
    setText("File");
    setAutoRaise(false);
}

void QApplicationButton::setApplicationMenu(QMenu *menu)
{
    if (m_menu && m_menu->parent() == this) {
        m_menu->deleteLater();
    }

    m_menu = menu;
    if (m_menu) {
        m_menu->setObjectName(QStringLiteral("RibbonApplicationMenu"));
        QRibbonStyle::applyMenuStyle(m_menu);
    }
    setMenu(menu);
}

QSize QApplicationButton::sizeHint() const
{
    const QFontMetrics fm(font());
    const int width = qMax(QRibbonMetrics::ApplicationButtonMinWidth,
                           fm.horizontalAdvance(text()) + 24);
    return QSize(width, QRibbonMetrics::TopBarHeight);
}

QSize QApplicationButton::minimumSizeHint() const
{
    const QFontMetrics fm(font());
    const int width = qMax(QRibbonMetrics::ApplicationButtonMinWidth,
                           fm.horizontalAdvance(text()) + 18);
    return QSize(width, QRibbonMetrics::TopBarHeight);
}
