#include "QRibbonSplitButton.h"
#include "QRibbonMetrics.h"
#include "QRibbonMenu.h"

#include <QPainter>
#include <QMouseEvent>
#include <QStyle>
#include <QStyleOption>
#include <QCursor>
#include <QApplication>
#include <QEvent>

// Constructor / updates
void QRibbonSplitButton::updateIconSize()
{
    m_iconSize = (m_size == QRibbonButtonSize::Large)
        ? QRibbonMetrics::largeIconSize()
        : QRibbonMetrics::smallIconSize();
}

QIcon QRibbonSplitButton::effectiveIcon() const
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

QRibbonSplitButton::QRibbonSplitButton(QWidget *parent)
    : QWidget(parent)
    , m_size(QRibbonButtonSize::Large)
    , m_checkable(false)
    , m_checked(false)
    , m_hovered(false)
    , m_pressed(false)
    , m_arrowPressed(false)
    , m_arrowHovered(false)
    , m_menu(nullptr)
{
    setObjectName("RibbonSplitButton");
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_StyledBackground, true);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    updateIconSize();
    updateStateProperties();
}

QRibbonSplitButton::QRibbonSplitButton(const QIcon &icon, const QString &text, QRibbonButtonSize size, QWidget *parent)
    : QWidget(parent)
    , m_size(size)
    , m_icon(icon)
    , m_text(text)
    , m_checkable(false)
    , m_checked(false)
    , m_hovered(false)
    , m_pressed(false)
    , m_arrowPressed(false)
    , m_arrowHovered(false)
    , m_menu(nullptr)
{
    setObjectName("RibbonSplitButton");
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_StyledBackground, true);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    updateIconSize();
    updateStateProperties();
}

QRibbonSplitButton::~QRibbonSplitButton()
{
    if (m_menu && m_menu->parent() == this) {
        delete m_menu;
    }
}

void QRibbonSplitButton::setButtonSize(QRibbonButtonSize size)
{
    if (m_size != size) {
        m_size = size;
        updateIconSize();
        updateGeometry();
        update();
    }
}

void QRibbonSplitButton::setIcon(const QIcon &icon)
{
    if (m_icon.cacheKey() != icon.cacheKey()) {
        m_icon = icon;
        updateGeometry();
        update();
    }
}

void QRibbonSplitButton::setText(const QString &text)
{
    if (m_text != text) {
        m_text = text;
        updateGeometry();
        update();
    }
}

void QRibbonSplitButton::setCheckable(bool checkable)
{
    if (m_checkable != checkable) {
        m_checkable = checkable;
        update();
    }
}

void QRibbonSplitButton::setChecked(bool checked)
{
    if (m_checkable && m_checked != checked) {
        m_checked = checked;
        update();
        emit toggled(checked);
    }
    updateStateProperties();
}

void QRibbonSplitButton::setMenu(QMenu *menu)
{
    if (m_menu == menu) return;
    
    if (m_menu) {
        m_menu->removeEventFilter(this);
        m_menu->disconnect(this);
    }
    
    m_menu = menu;
    if (m_menu) {
        // Uniform styles are set in resources/styles/ribbon.qss; avoid overriding here
        // Set objectName for stylesheet selector differentiation if needed
        if (m_menu->objectName().isEmpty()) {
            m_menu->setObjectName(QStringLiteral("RibbonSplitMenu"));
        }
        m_menu->installEventFilter(this);
        connect(m_menu, &QMenu::aboutToShow, this, [this]() {
            m_arrowPressed = false;
            updateStateProperties();
            update();
        });
        connect(m_menu, &QMenu::triggered, this, &QRibbonSplitButton::onMenuActionTriggered);
        // Monitor changed signal for all QActions in menu to sync button appearance when default action changes
        for (QAction *a : m_menu->actions()) {
            if (!a) continue;
            connect(a, &QAction::changed, this, [this, a]() {
                if (!m_menu) return;
                QAction *def = m_menu->defaultAction();
                if (def == a) {
                    // Sync icon, text, tooltip, and enabled state of the default action
                    setIcon(a->icon());
                    setText(a->text());
                    setToolTip(a->toolTip());
                    QWidget::setEnabled(a->isEnabled());
                    update();
                }
            });
        }
        updateMenuIconTargetSize();
    }
    
    updateGeometry();
}

void QRibbonSplitButton::setDefaultAction(QAction *action)
{
    if (!action || !m_menu) return;
    
    // Set default action for the menu
    m_menu->setDefaultAction(action);
    
    // Sync button icon and text
    setIcon(action->icon());
    setText(action->text());
    setToolTip(action->toolTip());
    setEnabled(action->isEnabled());
    
    // Connect action change signal
    connect(action, &QAction::changed, this, [this, action]() {
        setIcon(action->icon());
        setText(action->text());
        setToolTip(action->toolTip());
        setEnabled(action->isEnabled());
    });
}

QSize QRibbonSplitButton::sizeHint() const
{
    QFontMetrics fm(font());

    const int iconW = effectiveIcon().isNull() ? 0 : m_iconSize.width();
    const int iconH = effectiveIcon().isNull() ? 0 : m_iconSize.height();
    const int textW = m_text.isEmpty() ? 0 : fm.horizontalAdvance(m_text);
    const int textH = m_text.isEmpty() ? 0 : fm.height();

    if (m_size == QRibbonButtonSize::Large) {
        const int width = qMax(QRibbonMetrics::SplitButtonMinWidth,
                               qMax(iconW, textW) + QRibbonMetrics::SplitArrowWidth
                               + QRibbonMetrics::LargeButtonHPadding * 2);
        const int height = qMax(QRibbonMetrics::LargeButtonMinHeight,
                                iconH + textH + 14);
        return QSize(width, height);
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
        const QPalette::ColorGroup colorGroup = isEnabled() ? QPalette::Active : QPalette::Disabled;
        painter.setPen(option.palette.color(colorGroup, QPalette::ButtonText));

        QFontMetrics fm(font());
        QString drawText = m_text;
        const int newlineIndex = drawText.indexOf('\n');
        if (newlineIndex >= 0) {
            drawText = drawText.left(newlineIndex);
        }

        const QString elidedText = fm.elidedText(drawText, Qt::ElideRight, textRect.width());
        const int textFlags = (m_size == QRibbonButtonSize::Large)
            ? (Qt::AlignLeft | Qt::AlignTop)
            : (Qt::AlignLeft | Qt::AlignVCenter);
        painter.drawText(textRect, textFlags, elidedText);
    }

    if (m_menu) {
        const QRect arrowRect = getDropArrowRect();
        if (arrowRect.isValid()) {
            QStyleOption arrowOption;
            arrowOption.initFrom(this);
            arrowOption.rect = QRect(arrowRect.center().x() - 4,
                                     arrowRect.center().y() - 3,
                                     8,
                                     6);
            arrowOption.state = QStyle::State_Enabled | QStyle::State_Active;
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
    if (event->button() == Qt::LeftButton && isEnabled()) {
        const bool inArrow = isInArrowArea(event->pos());
        if (inArrow) {
            m_arrowPressed = true;
        } else {
            m_pressed = true;
        }
        updateStateProperties();
        update();
    }
    
    event->accept();
}

void QRibbonSplitButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isEnabled()) {
        const bool inBtn = rect().contains(event->pos());
        const bool inArrow = isInArrowArea(event->pos());
        
        if (m_arrowPressed) {
            m_arrowPressed = false;
            updateStateProperties();
            update();
            
            if (inArrow && m_menu) {
                // Open drop-down menu
                updateMenuIconTargetSize();
                
                // Large button: menu below; Small button: menu below offset right
                QPoint menuPos;
                if (m_size == QRibbonButtonSize::Large) {
                    menuPos = mapToGlobal(rect().bottomLeft());
                } else {
                    menuPos = mapToGlobal(rect().bottomRight() - QPoint(20, 0));
                }
                
                // Retain style and palette before popup
                m_menu->popup(menuPos);
                return;
            }
        } else if (m_pressed && inBtn && !inArrow) {
            // Body click: execute default action or emit clicked
            m_pressed = false;
            updateStateProperties();
            update();
            
            if (m_menu && m_menu->defaultAction()) {
                m_menu->defaultAction()->trigger();
            } else {
                if (m_checkable) {
                    setChecked(!m_checked);
                }
                emit clicked();
            }
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
    m_hovered = false;
    m_arrowHovered = false;
    updateStateProperties();
    update();
    QWidget::leaveEvent(event);
}

void QRibbonSplitButton::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange) {
        if (!isEnabled()) {
            m_pressed = false;
            m_arrowPressed = false;
            m_hovered = false;
            m_arrowHovered = false;
        }
        updateStateProperties();
        update();
    }
    QWidget::changeEvent(event);
}

QRect QRibbonSplitButton::getIconRect() const
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

QRect QRibbonSplitButton::getTextRect() const
{
    if (m_text.isEmpty()) {
        return QRect();
    }

    if (m_size == QRibbonButtonSize::Large) {
        const int right = m_menu ? QRibbonMetrics::SplitArrowWidth : 0;
        return QRect(8, 44, qMax(0, width() - 16 - right), height() - 46);
    }

    const int left = effectiveIcon().isNull()
        ? 6
        : 6 + m_iconSize.width() + QRibbonMetrics::ButtonTextSpacing;
    const int right = m_menu ? QRibbonMetrics::SplitArrowWidth : 6;
    return QRect(left, 0, qMax(0, width() - left - right), height());
}

QRect QRibbonSplitButton::getDropArrowRect() const
{
    if (!m_menu) {
        return QRect();
    }

    if (m_size == QRibbonButtonSize::Large) {
        return QRect(width() - QRibbonMetrics::SplitArrowWidth, 0,
                     QRibbonMetrics::SplitArrowWidth, height());
    }

    return QRect(width() - QRibbonMetrics::SplitArrowWidth, 0,
                 QRibbonMetrics::SplitArrowWidth, height());
}

bool QRibbonSplitButton::isInArrowArea(const QPoint &pos) const
{
    if (!m_menu) return false;
    return getDropArrowRect().contains(pos);
}

void QRibbonSplitButton::updateHoverState(const QPoint &pos)
{
    const bool oldHovered = m_hovered;
    const bool oldArrowHovered = m_arrowHovered;
    
    m_hovered = rect().contains(pos);
    m_arrowHovered = m_hovered && isInArrowArea(pos);
    
    if (m_hovered != oldHovered || m_arrowHovered != oldArrowHovered) {
        updateStateProperties();
        update();
    }
}

void QRibbonSplitButton::onMenuActionTriggered(QAction *action)
{
    if (!m_menu) return;
    
    // Update default action
    m_menu->setDefaultAction(action);
    
    // Sync button appearance with action
    setIcon(action->icon());
    setText(action->text());
    setToolTip(action->toolTip());
    setEnabled(action->isEnabled());
}

void QRibbonSplitButton::updateMenuIconTargetSize()
{
    if (!m_menu) return;
    
    if (auto rbMenu = qobject_cast<QRibbonMenu*>(m_menu)) {
        const QSize sz = (m_size == QRibbonButtonSize::Large) ? m_iconSize : m_iconSize;
        rbMenu->setIconTargetSize(sz);
    }
}

void QRibbonSplitButton::updateStateProperties()
{
    const bool effectivePressed = m_pressed || m_arrowPressed;
    const bool effectiveHovered = m_hovered || m_arrowHovered;
    if (property("hovered").toBool() != effectiveHovered) setProperty("hovered", effectiveHovered);
    if (property("pressed").toBool() != effectivePressed) setProperty("pressed", effectivePressed);
    if (property("checked").toBool() != m_checked) setProperty("checked", m_checked);
    style()->unpolish(this);
    style()->polish(this);
}
