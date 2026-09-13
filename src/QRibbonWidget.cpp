#include "QRibbonWidget.h"
#include "QApplicationButton.h"
#include "QRibbonMetrics.h"
#include "QRibbonStyle.h"
#include "QRibbonTab.h"

#include <QAction>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QTabBar>
#include <QToolButton>
#include <QVBoxLayout>

QRibbonWidget::QRibbonWidget(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("RibbonWidget");
    setAttribute(Qt::WA_StyledBackground, true);
    setupUI();
    QRibbonStyle::applyDefaultStyle(this);
}

void QRibbonWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    m_topBarWidget = new QWidget(this);
    m_topBarWidget->setObjectName("RibbonTopBar");
    m_topBarWidget->setAttribute(Qt::WA_StyledBackground, true);
    m_topBarWidget->setFixedHeight(QRibbonMetrics::TopBarHeight);

    m_topLayout = new QHBoxLayout(m_topBarWidget);
    m_topLayout->setContentsMargins(0, 0, 4, 0);
    m_topLayout->setSpacing(0);

    m_applicationButton = new QApplicationButton(m_topBarWidget);
    m_topLayout->addWidget(m_applicationButton, 0, Qt::AlignLeft | Qt::AlignVCenter);

    m_tabBar = new QTabBar(m_topBarWidget);
    m_tabBar->setObjectName("RibbonTabBar");
    m_tabBar->setExpanding(false);
    m_tabBar->setUsesScrollButtons(true);
    m_tabBar->setElideMode(Qt::ElideRight);
    m_tabBar->setDocumentMode(true);
    m_tabBar->setDrawBase(false);
    m_tabBar->setMovable(false);
    m_tabBar->setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
    m_tabBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_tabBar->setFixedHeight(QRibbonMetrics::TopBarHeight);
    connect(m_tabBar, &QTabBar::currentChanged, this, &QRibbonWidget::onTabChanged);
    m_topLayout->addWidget(m_tabBar);
    m_topLayout->addStretch();

    m_accessBarWidget = new QWidget(m_topBarWidget);
    m_accessBarWidget->setObjectName("RibbonAccessBar");
    m_accessBarWidget->setFixedHeight(QRibbonMetrics::TopBarHeight);
    m_accessBarWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    m_accessBarLayout = new QHBoxLayout(m_accessBarWidget);
    m_accessBarLayout->setContentsMargins(4, 0, 2, 0);
    m_accessBarLayout->setSpacing(1);
    m_topLayout->addWidget(m_accessBarWidget, 0, Qt::AlignRight | Qt::AlignVCenter);

    m_mainLayout->addWidget(m_topBarWidget);

    m_stackedWidget = new QStackedWidget(this);
    m_stackedWidget->setObjectName("RibbonContent");
    m_stackedWidget->setFixedHeight(QRibbonMetrics::ContentHeight);
    m_stackedWidget->setAttribute(Qt::WA_StyledBackground, true);
    m_stackedWidget->setAutoFillBackground(false);
    m_mainLayout->addWidget(m_stackedWidget);

    setFixedHeight(QRibbonMetrics::RibbonHeight);
}

QToolButton *QRibbonWidget::addAccessBarAction(QAction *action)
{
    if (!m_accessBarLayout || !action) return nullptr;

    auto *button = new QToolButton(m_accessBarWidget);
    button->setObjectName(QStringLiteral("RibbonAccessButton"));
    button->setAutoRaise(true);
    button->setDefaultAction(action);
    button->setFixedHeight(QRibbonMetrics::AccessButtonHeight);
    button->setIconSize(QRibbonMetrics::accessIconSize());
    button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    auto syncPresentation = [button, action]() {
        const bool hasIcon = !action->icon().isNull();
        button->setToolButtonStyle(hasIcon
            ? Qt::ToolButtonIconOnly
            : Qt::ToolButtonTextOnly);

        if (hasIcon) {
            button->setFixedWidth(QRibbonMetrics::AccessButtonHeight);
        } else {
            button->setMinimumWidth(0);
            button->setMaximumWidth(QWIDGETSIZE_MAX);
        }

        button->setToolTip(action->toolTip().isEmpty() ? action->text() : action->toolTip());
    };
    syncPresentation();
    connect(action, &QAction::changed, button, syncPresentation);

    m_accessBarLayout->addWidget(button);
    return button;
}

QRibbonTab *QRibbonWidget::addTab(const QString &title, const QString &id)
{
    auto *tabPage = new QRibbonTab(title, this);
    tabPage->setId(id.isEmpty() ? title : id);
    m_tabs.append(tabPage);
    m_stackedWidget->addWidget(tabPage);
    m_tabBar->addTab(title);
    return tabPage;
}

void QRibbonWidget::removeTab(int index)
{
    if (index < 0 || index >= m_tabs.size()) {
        return;
    }

    QRibbonTab *tabPage = m_tabs.takeAt(index);
    m_tabContexts.remove(tabPage);
    m_stackedWidget->removeWidget(tabPage);
    m_tabBar->removeTab(index);
    tabPage->deleteLater();
}

void QRibbonWidget::removeTab(QRibbonTab *tabPage)
{
    const int index = m_tabs.indexOf(tabPage);
    if (index >= 0) {
        removeTab(index);
    }
}

void QRibbonWidget::setCurrentTab(int index)
{
    if (index >= 0 && index < m_tabs.size()) {
        m_tabBar->setCurrentIndex(index);
        m_stackedWidget->setCurrentIndex(index);
    }
}

bool QRibbonWidget::setCurrentTab(const QString &id)
{
    const int index = indexOfTabId(id);
    if (index < 0) {
        return false;
    }

    setCurrentTab(index);
    return true;
}

QRibbonTab *QRibbonWidget::currentTab() const
{
    return tab(currentIndex());
}

QRibbonTab *QRibbonWidget::tab(int index) const
{
    return (index >= 0 && index < m_tabs.size()) ? m_tabs[index] : nullptr;
}

int QRibbonWidget::indexOfTabId(const QString &id) const
{
    if (id.isEmpty()) {
        return -1;
    }

    for (int i = 0; i < m_tabs.size(); ++i) {
        if (m_tabs[i] && m_tabs[i]->id() == id) {
            return i;
        }
    }
    return -1;
}

int QRibbonWidget::tabCount() const
{
    return m_tabs.size();
}

int QRibbonWidget::currentIndex() const
{
    return m_tabBar ? m_tabBar->currentIndex() : -1;
}

void QRibbonWidget::onTabChanged(int index)
{
    if (m_stackedWidget) {
        m_stackedWidget->setCurrentIndex(index);
    }
    if (QRibbonTab *tabPage = tab(index)) {
        tabPage->refreshLayout();
    }
    emit tabChanged(index);
    updateContextTab(index);
}

void QRibbonWidget::setApplicationButton(QApplicationButton *button)
{
    if (button == m_applicationButton) {
        return;
    }

    if (m_applicationButton) {
        m_topLayout->removeWidget(m_applicationButton);
        if (m_applicationButton->parent() == this || m_applicationButton->parent() == m_topBarWidget) {
            m_applicationButton->deleteLater();
        }
    }

    m_applicationButton = button;
    if (!m_applicationButton) {
        return;
    }

    if (m_applicationButton->parent() != m_topBarWidget) {
        m_applicationButton->setParent(m_topBarWidget);
    }
    m_applicationButton->setFixedHeight(QRibbonMetrics::TopBarHeight);
    m_applicationButton->updateGeometry();
    m_topLayout->insertWidget(0, m_applicationButton, 0, Qt::AlignLeft | Qt::AlignVCenter);
}

void QRibbonWidget::setTabContext(int index, const QString &contextTitle, const QColor &color)
{
    QRibbonTab *tabPage = tab(index);
    if (!tabPage) return;

    m_tabContexts.insert(tabPage, {contextTitle, color});
    if (currentIndex() == index) {
        updateContextTab(index);
    }
}

void QRibbonWidget::clearTabContext(int index)
{
    QRibbonTab *tabPage = tab(index);
    if (!tabPage) return;

    m_tabContexts.remove(tabPage);
    if (currentIndex() == index) {
        updateContextTab(index);
    }
}

void QRibbonWidget::updateContextTab(int index)
{
    for (int i = 0; i < m_tabBar->count(); ++i) {
        m_tabBar->setTabTextColor(i, QColor());
        m_tabBar->setTabToolTip(i, QString());
    }

    QRibbonTab *tabPage = tab(index);
    if (!tabPage) {
        return;
    }

    const auto it = m_tabContexts.constFind(tabPage);
    if (it == m_tabContexts.constEnd()) {
        return;
    }

    m_tabBar->setTabTextColor(index, it->color.darker(180));
    m_tabBar->setTabToolTip(index, it->title);
}
