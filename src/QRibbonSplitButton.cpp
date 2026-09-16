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

QString QRibbonSplitButton::effectiveText() const
{
    return m_hasDisplayText ? m_displayText : m_text;
}

QString QRibbonSplitButton::displayText() const
{
    return effectiveText();
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
    : QRibbonSplitButton(parent)
{
    m_size = size;
    m_icon = icon;
    m_text = text;
    setAccessibleName(effectiveText());
    updateIconSize();
    updateStateProperties();
}

QRibbonSplitButton::QRibbonSplitButton(QAction *defaultAction,
                                       QMenu *menu,
                                       QRibbonButtonSize size,
                                       QWidget *parent)
    : QRibbonSplitButton(parent)
{
    m_size = size;
    updateIconSize();
    setMenu(menu);
    setDefaultAction(defaultAction);
    updateStateProperties();
}

QRibbonSplitButton::~QRibbonSplitButton()
{
    if (m_defaultAction) disconnect(m_defaultAction, nullptr, this, nullptr);
    if (m_menu && m_menu->parent() == this) delete m_menu.data();
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
    setAccessibleName(effectiveText());
    updateGeometry();
    update();
}

void QRibbonSplitButton::setDisplayText(const QString &text)
{
    if (m_hasDisplayText && m_displayText == text) return;
    m_displayText = text;
    m_hasDisplayText = true;
    setAccessibleName(effectiveText());
    updateGeometry();
    update();
}

void QRibbonSplitButton::clearDisplayText()
{
    if (!m_hasDisplayText) return;
    m_displayText.clear();
    m_hasDisplayText = false;
    setAccessibleName(effectiveText());
    updateGeometry();
    update();
}

void QRibbonSplitButton::setCheckable(bool checkable)
{
    if (m_checkable == checkable) return;
    m_checkable = checkable;
    if (!m_checkable) m_checked = false;
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
    if (m_menu) disconnect(m_menu, nullptr, this, nullptr);
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

    if (m_defaultAction) {
        m_menu->setDefaultAction(m_defaultAction.data());
    } else if (m_menu->defaultAction()) {
        setDefaultAction(m_menu->defaultAction());
    }
    updateGeometry();
    update();
}

void QRibbonSplitButton::setDefaultAction(QAction *action)
{
    if (m_defaultAction == action) {
        syncFromDefaultAction();
        return;
    }

    if (m_defaultAction) disconnect(m_defaultAction, nullptr, this, nullptr);
    m_defaultAction = action;

    if (!m_defaultAction) {
        updateGeometry();
        update();
        return;
    }

    if (m_menu) m_menu->setDefaultAction(m_defaultAction.data());
    connect(m_defaultAction, &QAction::changed, this, &QRibbonSplitButton::syncFromDefaultAction);
    connect(m_defaultAction, &QObject::destroyed, this, [this]() { m_defaultAction = nullptr; });
    syncFromDefaultAction();
}

void QRibbonSplitButton::syncFromDefaultAction()
{
    if (!m_defaultAction) return;
    m_icon = m_defaultAction->icon();
    m_text = m_defaultAction->text();
    setAccessibleName(effectiveText());
    setToolTip(m_defaultAction->toolTip().isEmpty() ? m_defaultAction->text() : m_defaultAction->toolTip());
    setStatusTip(m_defaultAction->statusTip());
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
    const QIcon icon = effectiveIcon();
    const QString label = effectiveText();
    const int iconW = icon.isNull() ? 0 : m_iconSize.width();
    const int iconH = icon.isNull() ? 0 : m_iconSize.height();
    const int textH = label.isEmpty() ? 0 : fm.height();
    const int arrowW = m_menu ? QRibbonMetrics::SplitArrowWidth : 0;

    if (m_size == QRibbonButtonSize::Large) {
        const int textW = label.isEmpty() ? 0 : QRibbonTextLayout::preferredTwoLineWidth(font(), label);
        const int width = qMax(iconW, textW + arrowW) + QRibbonMetrics::LargeButtonHPadding * 2;
        return QSize(width, QRibbonMetrics::LargeButtonHeight);
    }

    const int textW = label.isEmpty() ? 0 : fm.horizontalAdvance(label.section(QChar('\n'), 0, 0));
    const int spacing = (!icon.isNull() && !label.isEmpty()) ? QRibbonMetrics::ButtonTextSpacing : 0;
    const int width = iconW + spacing + textW + arrowW + QRibbonMetrics::SmallButtonHPadding * 2;
    const int height = qMax(QRibbonMetrics::SmallButtonMinHeight, qMax(iconH, textH) + 8);
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

    const QString label = effectiveText();
    const QRect textRect = getTextRect();
    if (!label.isEmpty() && textRect.isValid()) {
        const QPalette::ColorGroup group = isEnabled() ? QPalette::Active : QPalette::Disabled;
        painter.setPen(option.palette.color(group, QPalette::WindowText));
        if (m_size == QRibbonButtonSize::Large) {
            QRibbonTextLayout::drawTwoLineText(painter, textRect, label);
        } else {
            const QString drawText = label.section(QChar('\n'), 0, 0);
            const QString elidedText = QFontMetrics(font()).elidedText(drawText, Qt::ElideRight, textRect.width());
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
            arrowOption.rect = QRect(arrowRect.center().x() - 4, arrowRect.center().y() - 3, 8, 6);
            arrowOption.state = QStyle::State_None;
            if (isEnabled()) arrowOption.state |= QStyle::State_Enabled | QStyle::State_Active;
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
    if (isInArrowArea(event->pos())) m_arrowPressed = true;
    else m_pressed = true;
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
            if (m_defaultAction) m_defaultAction->trigger();
            else {
                if (m_checkable) setChecked(!m_checked);
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
    if (isEnabled()) updateHoverState(event->pos());
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
        return QRect(x, QRibbonMetrics::LargeIconTop, m_iconSize.width(), m_iconSize.height());
    }
    const int y = (height() - m_iconSize.height()) / 2;
    return QRect(QRibbonMetrics::ButtonSidePadding, y, m_iconSize.width(), m_iconSize.height());
}

QRect QRibbonSplitButton::getTextRect() const
{
    if (effectiveText().isEmpty()) return QRect();
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
    return QRect(width() - QRibbonMetrics::SplitArrowWidth, 0, QRibbonMetrics::SplitArrowWidth, height());
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
    if (action) setDefaultAction(action);
}

void QRibbonSplitButton::updateStateProperties()
{
    bool changed = false;
    const QString sizeName = m_size == QRibbonButtonSize::Large ? QStringLiteral("large") : QStringLiteral("small");
    if (property("buttonSize").toString() != sizeName) { setProperty("buttonSize", sizeName); changed = true; }
    const bool pressed = m_pressed || m_arrowPressed;
    if (property("pressed").toBool() != pressed) { setProperty("pressed", pressed); changed = true; }
    if (property("checked").toBool() != m_checked) { setProperty("checked", m_checked); changed = true; }
    if (changed) {
        style()->unpolish(this);
        style()->polish(this);
    }
}
