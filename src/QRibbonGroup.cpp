#include "QRibbonGroup.h"
#include "QRibbonButton.h"
#include "QRibbonMetrics.h"

#include <QFontMetrics>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QVBoxLayout>

QRibbonGroup::QRibbonGroup(const QString &title, QWidget *parent)
    : QFrame(parent)
    , m_title(title)
{
    setObjectName("RibbonGroup");
    setFrameStyle(QFrame::NoFrame);
    setupLayout();
    setVisible(false);
}

void QRibbonGroup::setupLayout()
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(QRibbonMetrics::GroupHorizontalPadding,
                                     2,
                                     QRibbonMetrics::GroupHorizontalPadding,
                                     0);
    m_mainLayout->setSpacing(0);

    m_contentWidget = new QWidget(this);
    m_contentWidget->setObjectName("RibbonGroupContent");
    m_contentWidget->setAttribute(Qt::WA_StyledBackground, true);
    m_contentWidget->setAutoFillBackground(false);
    m_contentWidget->setFixedHeight(QRibbonMetrics::GroupContentHeight);

    m_contentLayout = new QHBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(0, 0, 0, 0);
    m_contentLayout->setSpacing(QRibbonMetrics::GroupContentSpacing);
    m_contentLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_mainLayout->addWidget(m_contentWidget);

    m_titleLabel = new QLabel(m_title, this);
    m_titleLabel->setObjectName("RibbonGroupTitle");
    m_titleLabel->setAlignment(Qt::AlignCenter | Qt::AlignBottom);
    m_titleLabel->setFixedHeight(QRibbonMetrics::GroupTitleHeight);
    m_titleLabel->setWordWrap(false);
    m_titleLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_mainLayout->addWidget(m_titleLabel);

    setFixedHeight(QRibbonMetrics::ContentHeight);
    updateContentWidth();
}

void QRibbonGroup::markContentAdded()
{
    ++m_contentItemCount;
    if (!isVisible()) {
        setVisible(true);
    }

    updateContentWidth();
    emit layoutChanged();
}

void QRibbonGroup::updateContentWidth()
{
    if (isEmpty()) {
        m_contentWidth = 0;
        updateGeometry();
        return;
    }

    int contentWidth = 0;

    if (m_contentLayout) {
        const QMargins margins = m_contentLayout->contentsMargins();
        contentWidth += margins.left() + margins.right();

        int visibleItemCount = 0;
        for (int i = 0; i < m_contentLayout->count(); ++i) {
            QLayoutItem *item = m_contentLayout->itemAt(i);
            if (!item) continue;

            int itemWidth = 0;
            if (QWidget *widget = item->widget()) {
                itemWidth = qMax(widget->sizeHint().width(), widget->minimumSizeHint().width());
            } else if (QLayout *layout = item->layout()) {
                for (int j = 0; j < layout->count(); ++j) {
                    QLayoutItem *child = layout->itemAt(j);
                    if (!child || !child->widget()) continue;
                    QWidget *childWidget = child->widget();
                    itemWidth = qMax(itemWidth,
                                     qMax(childWidget->sizeHint().width(),
                                          childWidget->minimumSizeHint().width()));
                }
            }

            if (itemWidth > 0) {
                contentWidth += itemWidth;
                ++visibleItemCount;
            }
        }

        if (visibleItemCount > 1) {
            contentWidth += (visibleItemCount - 1) * m_contentLayout->spacing();
        }
    }

    const int groupPadding = QRibbonMetrics::GroupHorizontalPadding * 2;
    const int titleWidth = m_titleLabel
        ? QFontMetrics(m_titleLabel->font()).horizontalAdvance(m_title) + groupPadding + 4
        : QRibbonMetrics::LargeButtonMinWidth;

    const int newWidth = qMax(QRibbonMetrics::LargeButtonMinWidth,
                              qMax(contentWidth + groupPadding, titleWidth));
    if (m_contentWidth == newWidth) {
        updateGeometry();
        return;
    }

    m_contentWidth = newWidth;
    updateGeometry();
    emit layoutChanged();
}

QSize QRibbonGroup::sizeHint() const
{
    return QSize(m_contentWidth, QRibbonMetrics::ContentHeight);
}

QSize QRibbonGroup::minimumSizeHint() const
{
    return QSize(m_contentWidth, QRibbonMetrics::ContentHeight);
}

void QRibbonGroup::addButton(QRibbonButton *button)
{
    if (!button) return;

    if (button->buttonSize() == QRibbonButtonSize::Large) {
        addLargeWidget(button);
    } else {
        addSmallWidget(button);
    }
}

void QRibbonGroup::addButton(const QIcon &icon, const QString &text, QRibbonButtonSize size)
{
    auto *button = new QRibbonButton(icon, text, size, this);
    addButton(button);
}

void QRibbonGroup::addSeparator()
{
    auto *separator = new QFrame(this);
    separator->setObjectName("RibbonGroupSeparator");
    separator->setFrameShape(QFrame::VLine);
    separator->setFrameShadow(QFrame::Plain);
    separator->setFixedSize(1, QRibbonMetrics::GroupSeparatorHeight);

    if (m_contentLayout) {
        m_contentLayout->addWidget(separator);
    }

    markContentAdded();
}

void QRibbonGroup::addWidget(QWidget *widget)
{
    if (!widget || !m_contentLayout) return;

    widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_contentLayout->addWidget(widget);
    markContentAdded();
}

void QRibbonGroup::addLargeWidget(QWidget *widget)
{
    if (!widget || !m_contentLayout) return;

    m_currentSmallColumnWidget = nullptr;
    m_currentSmallColumnLayout = nullptr;
    m_currentSmallRow = 0;

    widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    widget->setFixedHeight(QRibbonMetrics::LargeButtonHeight);
    m_contentLayout->addWidget(widget);
    markContentAdded();
}

void QRibbonGroup::addSmallWidget(QWidget *widget)
{
    if (!widget || !m_contentLayout) return;

    if (!m_currentSmallColumnWidget || !m_currentSmallColumnLayout
        || m_currentSmallRow >= QRibbonMetrics::SmallRowCount) {
        m_currentSmallColumnWidget = new QWidget(m_contentWidget);
        m_currentSmallColumnWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        m_currentSmallColumnWidget->setFixedHeight(QRibbonMetrics::GroupContentHeight);

        m_currentSmallColumnLayout = new QGridLayout(m_currentSmallColumnWidget);
        m_currentSmallColumnLayout->setContentsMargins(0, 0, 0, 0);
        m_currentSmallColumnLayout->setHorizontalSpacing(0);
        m_currentSmallColumnLayout->setVerticalSpacing(QRibbonMetrics::SmallRowSpacing);

        for (int row = 0; row < QRibbonMetrics::SmallRowCount; ++row) {
            m_currentSmallColumnLayout->setRowMinimumHeight(row, QRibbonMetrics::SmallRowHeight);
            m_currentSmallColumnLayout->setRowStretch(row, 0);
        }

        m_contentLayout->addWidget(m_currentSmallColumnWidget);
        m_currentSmallRow = 0;
    }

    widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    widget->setFixedHeight(QRibbonMetrics::SmallButtonHeight);
    m_currentSmallColumnLayout->addWidget(widget, m_currentSmallRow, 0);

    int columnWidth = 0;
    for (int i = 0; i < m_currentSmallColumnLayout->count(); ++i) {
        QLayoutItem *item = m_currentSmallColumnLayout->itemAt(i);
        if (!item || !item->widget()) continue;

        QWidget *childWidget = item->widget();
        columnWidth = qMax(columnWidth,
                           qMax(childWidget->sizeHint().width(),
                                childWidget->minimumSizeHint().width()));
    }

    m_currentSmallColumnWidget->setMinimumWidth(columnWidth);

    ++m_currentSmallRow;
    if (m_currentSmallRow >= QRibbonMetrics::SmallRowCount) {
        m_currentSmallColumnWidget = nullptr;
        m_currentSmallColumnLayout = nullptr;
        m_currentSmallRow = 0;
    }

    markContentAdded();
}

void QRibbonGroup::setTitle(const QString &title)
{
    if (m_title == title) return;

    m_title = title;
    if (m_titleLabel) {
        m_titleLabel->setText(m_title);
    }

    updateContentWidth();
    update();
}
