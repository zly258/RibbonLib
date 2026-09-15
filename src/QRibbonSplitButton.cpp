#include "QRibbonSplitButton.h"
#include "QRibbonMetrics.h"
#include "QRibbonStyle.h"
#include "QRibbonTextLayout.h"

#include <QAction>
#include <QEvent>
#include <QFontMetrics>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>

void QRibbonSplitButton::updateIconSize()
{
    m_iconSize = (m_size == QRibbonButtonSize::Large)
        ? QRibbonMetrics::largeIconSize()
        : QRibbonMetrics::smallIconSize();
}

QIcon QRibbonSplitButton::effectiveIcon() const
{
    return m_icon;
}

QRibbonSplitButton::QRibbonSplitButton(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("RibbonSplitButton");
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover, true);
    setAttribute(Qt::WA_StyledBackground, true);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    updateIconSize();
    updateStateProperties();
}

QRibbonSplitButton::QRibbonSplitButton(const QIcon &icon,
                                       const QString &text,
                                       QRibbonButtonSize size,
                                       QWidget *parent)
    : QWidget(parent)
    , m_size(size)
    , m_icon(icon)
    , m_text(text)
{
    setObjectName("RibbonSplitButton");
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover, true);
    setAttribute(Qt::WA_StyledBackground, true);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setAccessibleName(m_text);
    updateIconSize();
    updateStateProperties();
}

QRibbonSplitButton::~QRibbonSplitButton()
{
    if (m_defaultAction) {
        disconnect(m_defaultAction, nullptr, this, nullptr);
    }
    if (m_menu && m_menu->parent() == this) {
        delete m_menu;
    }
}

void QRibbonSplitButton::setButtonSize(QRibbonButtonSize size)
{
    if (m_size == size) return;

    m_size = size;
    updateIconSize();
    updateStateProperties();
    updateGeometry();
    update();
}

void QRibbonSplitButton::setIcon(const QIcon &icon)
{
    if (m_icon.cacheKey() == icon.cacheKey()) return;

    m_icon = icon;
    updateGeometry();
    update();
}

void QRibbonSplitButton::setText(const QString &text)
{
    if (m_text == text) return;

    m_text = text;
    setAccessibleName(m_text);
    updateGeometry();
    update();
}

void QRibbonSplitButton::setCheckable(bool checkable)
{
    if (m_checkable == checkable) return;

    m_checkable = checkable;
    if (!m_checkable) {
        m_checked = false;
    }
    updateStateProperties();
    update();
}

void QRibbonSplitButton::setChecked(bool checked)
{
    if (!m_checkable || m_checked == checked) return;

    m_checked = checked;
    updateStateProperties();
    update();
    emit toggled(checked);
}

void QRibbonSplitButton::setMenu(QMenu *menu)
{
    if (m_menu == menu) return;

    if (m_defaultAction) {
        disconnect(m_defaultAction, nullptr, this, nullptr);
        m_defaultAction = nullptr;
    }
    if (m_menu) {
        disconnect(m_menu, nullptr, this, nullptr);
    }

    m_menu = menu;
    if (!m_menu) {
        updateGeometry();
        update();
        return;
    }

    m_menu->setObjectName(QStringLiteral("RibbonSplitMenu"));
    QRibbonStyle::applyMenuStyle(m_menu);

    connect(m_menu, &QMenu::aboutToShow, this, [this]() {
        m_arrowPressed = false;
        updateStateProperties();
        update();
    });
    connect(m_menu, &QMenu::triggered, this, &QRibbonSplitButton::onMenuActionTriggered);

    if (m_menu->defaultAction()) {
        setDefaultAction(m_menu->defaultAction());
    }

    updateGeometry();
}

void QRibbonSplitButton::setDefaultAction(QAction *action)
{
    if (!action || !m_menu || action == m_defaultAction) {
        if (action && action == m_defaultAction) {
            syncFromDefaultAction();
        }
        return;
    }

    if (m_defaultAction) {
        disconnect(m_defaultAction, nullptr, this, nullptr);
    }

    m_defaultAction = action;
    m_menu->setDefaultAction(action);
    connect(m_defaultAction, &QAction::changed, this, [this]() {
        syncFromDefaultAction();
    });

    syncFromDefaultAction();
}

void QRibbonSplitButton::syncFromDefaultAction()
{
    if (!m_defaultAction) return;

    m_icon = m_defaultAction->icon();
    m_text = m_defaultAction->text();
    setAccessibleName(m_text);
    setToolTip(m_defaultAction->toolTip());
    QWidget::setEnabled(m_defaultAction->isEnabled());
    setVisible(m_defaultAction->isVisible());
    m_checkable = m_defaultAction->isCheckable();
    m_checked = m_checkable && m_defaultAction->isChecked();

    updateStateProperties();
    updateGeometry();
    update();
}

QSize QRibbonSplitButton::sizeHint() const
{
    const QFontMetrics fm(font());
    const int iconW = effectiveIcon().isNull() ? 0 : m_iconSize.width();
    const int iconH = effectiveIcon().isNull() ? 0 : m_iconSize.height();
    const int textW = m_text.isEmpty() ? 0 : fm.horizontalAdvance(m_text);
    const int textH = m_text.isEmpty() ? 0 : fm.height();

    if (m_size == QRibbonButtonSize::Large) {
        const int wrappedTextW = m_text.isEmpty()
            ? 0
            : QRibbonTextLayout::preferredTwoLineWidth(font(), m_text);
        const int width = qMax(QRibbonMetrics::SplitButtonMinWidth,
                               qMax(iconW, wrappedTextW + QRibbonMetrics::SplitArrowWidth)
                                   + QRibbonMetrics::LargeButtonHPadding * 2);
        return QSize(width, QRibbonMetrics::LargeButtonHeight);
    }

    const int spacing = (!effectiveIcon().isNull() && !m_text.isEmpty())
        ? QRibbonMetrics::ButtonTextSpacing
        : 0;
    const int width = qMax(QRibbonMetrics::SplitButtonMinWidth,
                           iconW + spacing + textW + QRibbonMetrics::SplitArrowWidth
                               + QRibbonMetrics::SmallButtonHPadding * 2);
    const int height = qMax(QRibbonMetrics::SmallButtonMinHeight,
                            qMax(iconH, textH) + 8);
    return QSize(width, height);
}

QSize QRibbonSplitButton::minimumSizeHint() const
{
    return sizeHint();
}

void QRibbonSplitButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QStyleOption option;
    option.initFrom(this);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);

    const QIcon iconToPaint = effectiveIcon();
    const QIcon::Mode iconMode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
    const QIcon::State iconState = m_checked ? QIcon::On : QIcon::Off;

    const QRect iconRect = getIconRect();
    if (!iconToPaint.isNull() && iconRect.isValid()) {
        iconToPaint.paint(&painter, iconRect, Qt::AlignCenter, iconMode, iconState);
    }

    const QRect textRect = getTextRect();
    if (!m_text.isEmpty() && textRect.isValid()) {
        const QPalette::ColorGroup group = isEnabled() ? QPalette::Active : QPalette::Disabled;
        painter.setPen(option.palette.color(group, QPalette::WindowText));

        if (m_size == QRibbonButtonSize::Large) {
            QRibbonTextLayout::drawTwoLineText(painter, textRect, m_text);
        } else {
            QString drawText = m_text;
            const int newlineIndex = drawText.indexOf('\n');
            if (newlineIndex >= 0) {
                drawText = drawText.left(newlineIndex);
            }
            const QString elidedText = QFontMetrics(font()).elidedText(drawText,
                                                                       Qt::ElideRight,
                                                                       textRect.width());
            painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, elidedText);
        }
    }

    if (m_menu) {
        const QRect arrowRect = getDropArrowRect();
        if (arrowRect.isValid()) {
            if (m_arrowHovered || m_arrowPressed) {
                QColor highlight = option.palette.color(QPalette::Highlight);
                highlight.setAlpha(m_arrowPressed ? 48 : 26);
                painter.fillRect(arrowRect, highlight);
                painter.setPen(option.palette.color(QPalette::Midlight));
                painter.drawLine(arrowRect.topLeft(), arrowRect.bottomLeft());
            }

            QStyleOption arrowOption;
            arrowOption.initFrom(this);
            arrowOption.rect = QRect(arrowRect.center().x() - 4,
                                     arrowRect.center().y() - 3,
                                     8,
                                     6);
            arrowOption.state = QStyle::State_None;
            if (isEnabled()) {
                arrowOption.state |= QStyle::State_Enabled | QStyle::State_Active;
            }
            if (m_arrowHovered) arrowOption.state |= QStyle::State_MouseOver;
            if (m_arrowPressed) arrowOption.state |= QStyle::State_Sunken;
            style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &arrowOption, &painter, this);
        }
    }

    if (hasFocus()) {
        QStyleOptionFocusRect focusOption;
        focusOption.initFrom(this);
        focusOption.rect = rect().adjusted(1, 1, -1, -1);
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &focusOption, &painter, this);
    }
}

void QRibbonSplitButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || !isEnabled()) {
        QWidget::mousePressEvent(event);
        return;
    }

    if (isInArrowArea(event->pos())) {
        m_arrowPressed = true;
    } else {
        m_pressed = true;
    }

    updateStateProperties();
    update();
    event->accept();
}

void QRibbonSplitButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || !isEnabled()) {
        QWidget::mouseReleaseEvent(event);
        return;
    }

    const bool inside = rect().contains(event->pos());
    const bool inArrow = isInArrowArea(event->pos());

    if (m_arrowPressed) {
        m_arrowPressed = false;
        updateStateProperties();
        update();

        if (inside && inArrow && m_menu) {
            m_menu->popup(mapToGlobal(QPoint(0, height())));
            event->accept();
            return;
        }
    } else if (m_pressed) {
        m_pressed = false;
        updateStateProperties();
        update();

        if (inside && !inArrow) {
            if (m_defaultAction) {
                m_defaultAction->trigger();
            } else {
                if (m_checkable) {
                    setChecked(!m_checked);
                }
                emit clicked();
            }
            event->accept();
            return;
        }
    }

    m_pressed = false;
    m_arrowPressed = false;
    updateStateProperties();
    update();
    event->accept();
}

void QRibbonSplitButton::mouseMoveEvent(QMouseEvent *event)
{
    if (isEnabled()) {
        updateHoverState(event->pos());
    }
    QWidget::mouseMoveEvent(event);
}

void QRibbonSplitButton::leaveEvent(QEvent *event)
{
    m_arrowHovered = false;
    m_pressed = false;
    m_arrowPressed = false;
    updateStateProperties();
    update();
    QWidget::leaveEvent(event);
}

void QRibbonSplitButton::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange && !isEnabled()) {
        m_pressed = false;
        m_arrowPressed = false;
        m_arrowHovered = false;
        updateStateProperties();
        update();
    }
    QWidget::changeEvent(event);
}

QRect QRibbonSplitButton::getIconRect() const
{
    if (effectiveIcon().isNull()) return QRect();

    if (m_size == QRibbonButtonSize::Large) {
        const int x = qMax(0, (width() - m_iconSize.width()) / 2);
        return QRect(x,
                     QRibbonMetrics::LargeIconTop,
                     m_iconSize.width(),
                     m_iconSize.height());
    }

    const int y = (height() - m_iconSize.height()) / 2;
    return QRect(QRibbonMetrics::ButtonSidePadding,
                 y,
                 m_iconSize.width(),
                 m_iconSize.height());
}

QRect QRibbonSplitButton::getTextRect() const
{
    if (m_text.isEmpty()) return QRect();

    if (m_size == QRibbonButtonSize::Large) {
        const int right = m_menu ? QRibbonMetrics::SplitArrowWidth : 0;
        return QRect(QRibbonMetrics::ButtonSidePadding,
                     QRibbonMetrics::LargeTextTop,
                     qMax(0, width() - QRibbonMetrics::ButtonSidePadding * 2 - right),
                     QRibbonMetrics::LargeTextHeight);
    }

    const int left = effectiveIcon().isNull()
        ? QRibbonMetrics::ButtonSidePadding
        : QRibbonMetrics::ButtonSidePadding + m_iconSize.width() + QRibbonMetrics::ButtonTextSpacing;
    const int right = m_menu ? QRibbonMetrics::SplitArrowWidth : QRibbonMetrics::ButtonSidePadding;
    return QRect(left, 0, qMax(0, width() - left - right), height());
}

QRect QRibbonSplitButton::getDropArrowRect() const
{
    if (!m_menu) return QRect();

    if (m_size == QRibbonButtonSize::Large) {
        return QRect(width() - QRibbonMetrics::SplitArrowWidth,
                     QRibbonMetrics::LargeTextTop - 1,
                     QRibbonMetrics::SplitArrowWidth,
                     qMax(0, height() - QRibbonMetrics::LargeTextTop + 1));
    }

    return QRect(width() - QRibbonMetrics::SplitArrowWidth,
                 0,
                 QRibbonMetrics::SplitArrowWidth,
                 height());
}

bool QRibbonSplitButton::isInArrowArea(const QPoint &pos) const
{
    return m_menu && getDropArrowRect().contains(pos);
}

void QRibbonSplitButton::updateHoverState(const QPoint &pos)
{
    const bool arrowHovered = isInArrowArea(pos);
    if (m_arrowHovered == arrowHovered) return;

    m_arrowHovered = arrowHovered;
    update();
}

void QRibbonSplitButton::onMenuActionTriggered(QAction *action)
{
    if (action) {
        setDefaultAction(action);
    }
}

void QRibbonSplitButton::updateStateProperties()
{
    bool changed = false;

    const QString sizeName = m_size == QRibbonButtonSize::Large
        ? QStringLiteral("large")
        : QStringLiteral("small");
    if (property("buttonSize").toString() != sizeName) {
        setProperty("buttonSize", sizeName);
        changed = true;
    }

    const bool pressed = m_pressed || m_arrowPressed;
    if (property("pressed").toBool() != pressed) {
        setProperty("pressed", pressed);
        changed = true;
    }
    if (property("checked").toBool() != m_checked) {
        setProperty("checked", m_checked);
        changed = true;
    }

    if (changed) {
        style()->unpolish(this);
        style()->polish(this);
    }
}
