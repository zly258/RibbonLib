#include "QApplicationButton.h"

#include <QPaintEvent>
#include <QFontMetrics>

QApplicationButton::QApplicationButton(QWidget *parent)
    : QToolButton(parent)
{
    setObjectName("ApplicationButton");
    // Width adapts naturally to text length; height aligns with the TabBar.
    setFixedHeight(29);
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
    setMenu(menu);
}

QSize QApplicationButton::sizeHint() const
{
    QFontMetrics fm(font());
    const int textWidth = fm.horizontalAdvance(text());
    const int horizontalPadding = 24;
    const int menuReserve = menu() ? 0 : 0;
    const int width = qMax(44, textWidth + horizontalPadding + menuReserve);
    return QSize(width, 29);
}

QSize QApplicationButton::minimumSizeHint() const
{
    QFontMetrics fm(font());
    const int textWidth = fm.horizontalAdvance(text());
    return QSize(qMax(44, textWidth + 18), 29);
}

void QApplicationButton::paintEvent(QPaintEvent *event)
{
    QToolButton::paintEvent(event);
}
