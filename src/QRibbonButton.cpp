#include "QRibbonButton.h"
#include "QRibbonMetrics.h"
#include "QRibbonMenu.h"

#include <QAction>
#include <QApplication>
#include <QEvent>
#include <QFontMetrics>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>

/*
 * QRibbonButton Implementation Notes
 * ------------------------------------------------------------
 * 1. Inherits QWidget (not QToolButton), so setIconSize() is not available.
 * 2. Icon size is stored in m_iconSize: Large is 32x32, Small is 16x16.
 * 3. Button width is calculated naturally from icon, text, and padding without a fixed width.
 */

void QRibbonButton::updateIconSize()
{
    m_iconSize = (m_size == QRibbonButtonSize::Large)
        ? QRibbonMetrics::largeIconSize()
        : QRibbonMetrics::smallIconSize();
}

QIcon QRibbonButton::effectiveIcon() const
{
    if (!m_icon.isNull()) {
        return m_icon;
    }

    QStyle *styleObject = this->style();
    if (!styleObject && qApp) {
        styleObject = qApp->style();
    }

    return styleObject
        ? styleObject->standardIcon(QStyle::SP_MessageBoxQuestion)
        : QIcon();
}

QRibbonButton::QRibbonButton(QWidget *parent)
    : QWidget(parent)
{
    setupButton();
}

QRibbonButton::QRibbonButton(const QIcon &icon, const QString &text, QRibbonButtonSize size, QWidget *parent)
    : QWidget(parent)
    , m_size(size)
    , m_icon(icon)
    , m_text(text)
{
    setupButton();
}

void QRibbonButton::setupButton()
{
    setObjectName("RibbonButton");
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_Hover, true);
    setAttribute(Qt::WA_StyledBackground, true);

    updateIconSize();
    updateStateProperties();

    if (m_size == QRibbonButtonSize::Large) {
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        setFixedHeight(72);
    } else {
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        setFixedHeight(22);
    }

    updateGeometry();
}

void QRibbonButton::setButtonSize(QRibbonButtonSize size)
{
    if (m_size == size) {
        return;
    }

    m_size = size;
    setupButton();
    update();
}

QRibbonButtonSize QRibbonButton::buttonSize() const
{
    return m_size;
}

void QRibbonButton::setIcon(const QIcon &icon)
{
    if (m_icon.cacheKey() == icon.cacheKey()) {
        return;
    }

    m_icon = icon;
    updateGeometry();
    update();
}

void QRibbonButton::setText(const QString &text)
{
    if (m_text == text) {
        return;
    }

    m_text = text;
    updateGeometry();
    update();
}

void QRibbonButton::setIconName(const QString &iconName)
{
    if (m_iconName == iconName) {
        return;
    }

    m_iconName = iconName;
    updateIconForTheme();
}

void QRibbonButton::setEnabled(bool enabled)
{
    QWidget::setEnabled(enabled);
    if (!enabled) {
        m_pressed = false;
        m_hovered = false;
    }

    updateStateProperties();
    update();
}

void QRibbonButton::setCheckable(bool checkable)
{
    if (m_checkable == checkable) {
        return;
    }

    m_checkable = checkable;
    if (!m_checkable && m_checked) {
        m_checked = false;
    }

    updateStateProperties();
    update();
}

void QRibbonButton::setChecked(bool checked)
{
    if (!m_checkable) {
        return;
    }

    if (m_checked == checked) {
        return;
    }

    m_checked = checked;
    updateStateProperties();
    update();
    emit toggled(checked);
}

QSize QRibbonButton::sizeHint() const
{
    QFontMetrics fm(font());

    const int iconW = effectiveIcon().isNull() ? 0 : m_iconSize.width();
    const int iconH = effectiveIcon().isNull() ? 0 : m_iconSize.height();
    const int textW = m_text.isEmpty() ? 0 : fm.horizontalAdvance(m_text);
    const int textH = m_text.isEmpty() ? 0 : fm.height();

    if (m_size == QRibbonButtonSize::Large) {
        const int width = qMax(QRibbonMetrics::LargeButtonMinWidth,
                               qMax(iconW, textW) + QRibbonMetrics::LargeButtonHPadding * 2);
        const int height = qMax(QRibbonMetrics::LargeButtonMinHeight,
                                iconH + textH + 14);
        return QSize(width, height);
    }

    const int spacing = (!effectiveIcon().isNull() && !m_text.isEmpty())
        ? QRibbonMetrics::ButtonTextSpacing
        : 0;
    const int width = qMax(QRibbonMetrics::SmallButtonMinWidth,
                           iconW + spacing + textW + QRibbonMetrics::SmallButtonHPadding * 2);
    const int height = qMax(QRibbonMetrics::SmallButtonMinHeight,
                            qMax(iconH, textH) + 8);
    return QSize(width, height);
}

QSize QRibbonButton::minimumSizeHint() const
{
    return sizeHint();
}

void QRibbonButton::paintEvent(QPaintEvent *event)
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
        const QPalette::ColorGroup colorGroup = isEnabled() ? QPalette::Active : QPalette::Disabled;
        painter.setPen(option.palette.color(colorGroup, QPalette::ButtonText));
        const int textFlags = (m_size == QRibbonButtonSize::Large)
            ? (Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap)
            : (Qt::AlignLeft | Qt::AlignVCenter);
        painter.drawText(textRect, textFlags, m_text);
    }
}

void QRibbonButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || !isEnabled()) {
        QWidget::mousePressEvent(event);
        return;
    }

    m_pressed = true;
    updateStateProperties();
    update();
}

void QRibbonButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || !isEnabled()) {
        QWidget::mouseReleaseEvent(event);
        return;
    }

    const bool wasPressed = m_pressed;
    m_pressed = false;

    if (wasPressed && rect().contains(event->pos())) {
        if (m_checkable) {
            m_checked = !m_checked;
            emit toggled(m_checked);
        }
        emit clicked();
    }

    updateStateProperties();
    update();
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void QRibbonButton::enterEvent(QEnterEvent *event)
#else
void QRibbonButton::enterEvent(QEvent *event)
#endif
{
    Q_UNUSED(event);
    m_hovered = true;
    updateStateProperties();
    update();
}

void QRibbonButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_hovered = false;
    m_pressed = false;
    updateStateProperties();
    update();
}

void QRibbonButton::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::EnabledChange) {
        updateStateProperties();
        update();
    }
}

QRect QRibbonButton::getIconRect() const
{
    if (effectiveIcon().isNull()) {
        return QRect();
    }

    if (m_size == QRibbonButtonSize::Large) {
        return QRect(8, 8, m_iconSize.width(), m_iconSize.height());
    }

    const int y = (height() - m_iconSize.height()) / 2;
    return QRect(6, y, m_iconSize.width(), m_iconSize.height());
}

QRect QRibbonButton::getTextRect() const
{
    if (m_text.isEmpty()) {
        return QRect();
    }

    if (m_size == QRibbonButtonSize::Large) {
        return QRect(8, 44, qMax(0, width() - 16), height() - 46);
    }

    const int left = effectiveIcon().isNull()
        ? 6
        : 6 + m_iconSize.width() + QRibbonMetrics::ButtonTextSpacing;
    return QRect(left, 0, width() - left - 6, height());
}

void QRibbonButton::onThemeChanged(bool isDark)
{
    Q_UNUSED(isDark);
    updateIconForTheme();
}

void QRibbonButton::updateIconForTheme()
{
    update();
}

void QRibbonButton::updateStateProperties()
{
    setProperty("buttonSize", m_size == QRibbonButtonSize::Large ? "large" : "small");
    setProperty("hovered", m_hovered);
    setProperty("pressed", m_pressed);
    setProperty("checked", m_checked);
    style()->unpolish(this);
    style()->polish(this);
}
