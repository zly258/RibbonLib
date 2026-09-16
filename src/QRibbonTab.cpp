#include "QRibbonTab.h"
#include "QRibbonGroup.h"
#include "QRibbonMetrics.h"

#include <QEvent>
#include <QFrame>
#include <QResizeEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QShowEvent>
#include <QTimer>
#include <QVBoxLayout>
#include <QWheelEvent>

QRibbonTab::QRibbonTab(const QString &title, QWidget *parent)
    : QWidget(parent)
    , m_id(title)
    , m_title(title)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setObjectName("RibbonTabPage");
    setupUI();
}

void QRibbonTab::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setObjectName("RibbonTabScrollArea");
    m_scrollArea->setWidgetResizable(false);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setFrameStyle(QFrame::NoFrame);
    m_scrollArea->setContentsMargins(0, 0, 0, 0);
    m_scrollArea->setAttribute(Qt::WA_StyledBackground, true);
    m_scrollArea->setAutoFillBackground(false);

    if (m_scrollArea->viewport()) {
        m_scrollArea->viewport()->setAttribute(Qt::WA_StyledBackground, true);
        m_scrollArea->viewport()->setAutoFillBackground(false);
        m_scrollArea->viewport()->installEventFilter(this);
    }

    m_content = new QWidget();
    m_content->setObjectName("RibbonTabContent");
    m_content->setAttribute(Qt::WA_StyledBackground, true);
    m_content->setAutoFillBackground(false);
    m_content->setFixedHeight(QRibbonMetrics::ContentHeight);
    m_content->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    m_layout = new QHBoxLayout(m_content);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(QRibbonMetrics::TabGroupSpacing);
    m_layout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_layout->addStretch(1);

    m_scrollArea->setWidget(m_content);
    layout->addWidget(m_scrollArea);

    scheduleContentWidthUpdate();
}

void QRibbonTab::refreshLayout()
{
    updateContentWidth();
}

void QRibbonTab::scheduleContentWidthUpdate()
{
    if (m_widthUpdatePending) {
        return;
    }

    m_widthUpdatePending = true;
    QTimer::singleShot(0, this, [this]() {
        m_widthUpdatePending = false;
        updateContentWidth();
    });
}

void QRibbonTab::updateContentWidth()
{
    if (!m_content || !m_layout) {
        return;
    }

    m_layout->invalidate();
    m_layout->activate();

    int totalWidth = 0;
    const QMargins margins = m_layout->contentsMargins();
    totalWidth += margins.left() + margins.right();

    int groupCount = 0;
    for (QRibbonGroup *group : m_groups) {
        if (!group || !group->isVisible()) {
            continue;
        }

        totalWidth += qMax(group->sizeHint().width(), group->minimumSizeHint().width());
        ++groupCount;
    }

    if (groupCount > 1) {
        totalWidth += (groupCount - 1) * m_layout->spacing();
    }

    const int viewportWidth = (m_scrollArea && m_scrollArea->viewport())
        ? m_scrollArea->viewport()->width()
        : 0;

    const int finalWidth = qMax(totalWidth, viewportWidth);
    m_content->setMinimumWidth(finalWidth);
    m_content->resize(finalWidth, QRibbonMetrics::ContentHeight);
    m_content->updateGeometry();

    if (m_scrollArea && m_scrollArea->horizontalScrollBar()) {
        m_scrollArea->horizontalScrollBar()->setPageStep(qMax(1, viewportWidth));
    }
}

bool QRibbonTab::eventFilter(QObject *obj, QEvent *event)
{
    if (m_scrollArea && obj == m_scrollArea->viewport() && event->type() == QEvent::Wheel) {
        QScrollBar *hbar = m_scrollArea->horizontalScrollBar();
        if (!hbar || hbar->maximum() <= 0) {
            return QWidget::eventFilter(obj, event);
        }

        QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
        int delta = wheelEvent->pixelDelta().x();
        if (delta == 0) {
            delta = wheelEvent->pixelDelta().y();
        }
        if (delta == 0) {
            delta = wheelEvent->angleDelta().y() / 2;
        }
        if (delta == 0) {
            delta = wheelEvent->angleDelta().x() / 2;
        }

        if (delta != 0) {
            hbar->setValue(hbar->value() - delta);
            return true;
        }
    }

    return QWidget::eventFilter(obj, event);
}

void QRibbonTab::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    scheduleContentWidthUpdate();
}

void QRibbonTab::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    scheduleContentWidthUpdate();
}

QRibbonGroup *QRibbonTab::addGroup(const QString &title)
{
    QRibbonGroup *group = new QRibbonGroup(title, this);
    m_groups.append(group);

    connect(group, &QRibbonGroup::layoutChanged, this, &QRibbonTab::scheduleContentWidthUpdate);

    if (m_layout->count() > 0) {
        QLayoutItem *last = m_layout->itemAt(m_layout->count() - 1);
        if (last && last->spacerItem()) {
            last = m_layout->takeAt(m_layout->count() - 1);
            delete last;
        }
    }

    m_layout->addWidget(group);
    m_layout->addStretch(1);
    scheduleContentWidthUpdate();
    return group;
}

QRibbonGroup *QRibbonTab::addGroup(const QString &title,
                                   const QList<QAction*> &actions,
                                   QRibbonButtonSize size)
{
    QRibbonGroup *group = addGroup(title);
    group->addActions(actions, size);
    return group;
}

void QRibbonTab::removeGroup(QRibbonGroup *group)
{
    if (!group || !m_groups.contains(group)) {
        return;
    }

    m_groups.removeOne(group);
    m_layout->removeWidget(group);
    group->deleteLater();
    scheduleContentWidthUpdate();
}

void QRibbonTab::clear()
{
    for (QRibbonGroup *group : m_groups) {
        m_layout->removeWidget(group);
        group->deleteLater();
    }

    m_groups.clear();
    scheduleContentWidthUpdate();
}
