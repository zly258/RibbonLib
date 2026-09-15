#include "QRibbonButton.h"
#include "QRibbonMetrics.h"
#include "QRibbonTextLayout.h"

#include <QEvent>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>

void QRibbonButton::updateIconSize()
{
    m_iconSize = (m_size == QRibbonButtonSize::Large)
        ? QRibbonMetrics::largeIconSize()
        : QRibbonMetrics::smallIconSize();
}

QIcon QRibbonButton::effectiveIcon() const
{
    return m_icon;
}

QRibbonButton::QRibbonButton(QWidget *parent)
    : QWidget(parent)
{
    setupButton();
}

QRibbonButton::QRibbonButton(const QIcon &icon,
                             const QString &text,
                             QRibbonButtonSize size,
                             QWidget *parent)
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
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setAccessibleName(m_text);

    updateIconSize();
    setFixedHeight(m_size == QRibbonButtonSize::Large
        ? QRibbonMetrics::LargeButtonHeight
        : QRibbonMetrics::SmallButtonHeight);
    updateStateProperties();
    updateGeometry();
}

void QRibbonButton::setButtonSize(QRibbonButtonSize size)
{
    if (m_size == size) return;

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
    if (m_icon.cacheKey() == icon.cacheKey()) return;

    m_icon = icon;
    updateGeometry();
    update();
}

void QRibbonButton::setText(const QString &text)
{
    if (m_text == text) return;

    m_text = text;
    setAccessibleName(m_text);
    updateGeometry();
    update();
}

void QRibbonButton::setEnabled(bool enabled)
{
    QWidget::setEnabled(enabled);
    if (!enabled) {
        m_pressed = false;
    }
    updateStateProperties();
    update();
}

void QRibbonButton::setCheckable(bool checkable)
{
    if (m_checkable == checkable) return;

    m_checkable = checkable;
    if (!m_checkable) {
        m_checked = false;
    }
    updateStateProperties();
    update();
}

void QRibbonButton::setChecked(bool checked)
{
    if (!m_checkable || m_checked == checked) return;

    m_checked = checked;
    updateStateProperties();
    update();
    emit toggled(checked);
}

void QRibbonButton::setShortcut(const QKeySequence &shortcut)
{
    m_shortcutText = shortcut.toString(QKeySequence::NativeText);
}

QKeySequence QRibbonButton::shortcut() const
{
    return QKeySequence::fromString(m_shortcutText, QKeySequence::NativeText);
}

QSize QRibbonButton::sizeHint() const
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
        const int width = qMax(QRibbonMetrics::LargeButtonMinWidth,
                               qMax(iconW, wrappedTextW)
                                   + QRibbonMetrics::LargeButtonHPadding * 2);
        return QSize(width, QRibbonMetrics::LargeButtonHeight);
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
        const QPalette::ColorGroup group = isEnabled() ? QPalette::Active : QPalette::Disabled;
        painter.setPen(option.palette.color(group, QPalette::WindowText));
        if (m_size == QRibbonButtonSize::Large) {
            QRibbonTextLayout::drawTwoLineText(painter, textRect, m_text);
        } else {
            painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, m_text);
        }
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
    event->accept();
}

void QRibbonButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || !isEnabled()) {
        QWidget::mouseReleaseEvent(event);
        return;
    }

    const bool trigger = m_pressed && rect().contains(event->pos());
    m_pressed = false;

    if (trigger) {
        if (m_checkable) {
            m_checked = !m_checked;
            emit toggled(m_checked);
        }
        emit clicked();
    }

    updateStateProperties();
    update();
    event->accept();
}

void QRibbonButton::leaveEvent(QEvent *event)
{
    m_pressed = false;
    updateStateProperties();
    update();
    QWidget::leaveEvent(event);
}

void QRibbonButton::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange && !isEnabled()) {
        m_pressed = false;
        updateStateProperties();
        update();
    }
    QWidget::changeEvent(event);
}

QRect QRibbonButton::getIconRect() const
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

QRect QRibbonButton::getTextRect() const
{
    if (m_text.isEmpty()) return QRect();

    if (m_size == QRibbonButtonSize::Large) {
        return QRect(QRibbonMetrics::ButtonSidePadding,
                     QRibbonMetrics::LargeTextTop,
                     qMax(0, width() - QRibbonMetrics::ButtonSidePadding * 2),
                     QRibbonMetrics::LargeTextHeight);
    }

    const int left = effectiveIcon().isNull()
        ? QRibbonMetrics::ButtonSidePadding
        : QRibbonMetrics::ButtonSidePadding + m_iconSize.width() + QRibbonMetrics::ButtonTextSpacing;
    return QRect(left,
                 0,
                 qMax(0, width() - left - QRibbonMetrics::ButtonSidePadding),
                 height());
}

void QRibbonButton::updateStateProperties()
{
    bool changed = false;

    const QString sizeName = m_size == QRibbonButtonSize::Large
        ? QStringLiteral("large")
        : QStringLiteral("small");
    if (property("buttonSize").toString() != sizeName) {
        setProperty("buttonSize", sizeName);
        changed = true;
    }
    if (property("pressed").toBool() != m_pressed) {
        setProperty("pressed", m_pressed);
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
