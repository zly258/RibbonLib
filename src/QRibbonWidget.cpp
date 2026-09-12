#include "QRibbonWidget.h"
#include "QRibbonMenu.h"
#include "QApplicationButton.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QAction>
#include <QColor>
#include <QEvent>

QRibbonWidget::QRibbonWidget(QWidget *parent)
    : QWidget(parent)
    , m_defaultButtonSize(QRibbonButtonSize::Large)
    , m_applicationButton(nullptr)
{
    setObjectName("RibbonWidget");
    // Enable styled background so QSS can render the Ribbon background
    setAttribute(Qt::WA_StyledBackground, true);
    setupUI();
}

void QRibbonWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    
    // Integrated top bar: ApplicationButton + TabBar + AccessBar
    m_topBarWidget = new QWidget(this);
    m_topBarWidget->setObjectName("RibbonTopBar");
    m_topBarWidget->setAttribute(Qt::WA_StyledBackground, true);
    m_topLayout = new QHBoxLayout(m_topBarWidget);
    m_topLayout->setContentsMargins(0, 0, 4, 0);
    m_topLayout->setSpacing(0);

    // Left application button
    m_applicationButton = new QApplicationButton(m_topBarWidget);
    m_applicationButton->setFixedHeight(29);
    m_topLayout->addWidget(m_applicationButton, 0, Qt::AlignLeft | Qt::AlignVCenter);

    // Tab bar
    m_tabBar = new QTabBar(m_topBarWidget);
    m_tabBar->setObjectName("RibbonTabBar");
    m_tabBar->setExpanding(false);
    m_tabBar->setUsesScrollButtons(true);
    m_tabBar->setElideMode(Qt::ElideRight);
    m_tabBar->setDocumentMode(true);
    m_tabBar->setFixedHeight(29);
    connect(m_tabBar, &QTabBar::currentChanged, this, &QRibbonWidget::onTabChanged);
    m_topLayout->addWidget(m_tabBar);
    m_topLayout->addStretch();

    // Right access bar
    m_accessBarWidget = new QWidget(m_topBarWidget);
    m_accessBarWidget->setObjectName("RibbonAccessBar");
    m_accessBarWidget->setFixedHeight(29);
    m_accessBarLayout = new QHBoxLayout(m_accessBarWidget);
    m_accessBarLayout->setContentsMargins(2, 0, 0, 0);
    m_accessBarLayout->setSpacing(1);
    m_topLayout->addWidget(m_accessBarWidget, /*stretch*/0, Qt::AlignRight | Qt::AlignVCenter);
    
    // Main layout: top bar + content area
    m_topBarWidget->setFixedHeight(29);
    m_mainLayout->addWidget(m_topBarWidget);
    
    // Fixed height for content area; width is handled by current tab page internally
    m_stackedWidget = new QStackedWidget(this);
    m_stackedWidget->setObjectName("RibbonContent");
    m_stackedWidget->setFixedHeight(96);
    m_stackedWidget->setAttribute(Qt::WA_StyledBackground, true);
    m_stackedWidget->setAutoFillBackground(false);
    m_mainLayout->addWidget(m_stackedWidget);
    
    setFixedHeight(29 + 96);
}

// Add AccessBar action. AccessBar buttons are icon-only with a compact 22x22 footprint.
QToolButton *QRibbonWidget::addAccessBarAction(QAction *action)
{
    if (!m_accessBarLayout || !action) return nullptr;
    QToolButton *btn = new QToolButton(m_accessBarWidget);
    btn->setAutoRaise(true);
    btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    btn->setIconSize(QSize(16, 16));
    btn->setDefaultAction(action);
    btn->setFixedSize(22, 22);
    m_accessBarLayout->addWidget(btn);
    return btn;
}

// Tab management
QRibbonTab *QRibbonWidget::addTab(const QString &title)
{
    QRibbonTab *tab = new QRibbonTab(title, this);
    m_tabs.append(tab);
    m_stackedWidget->addWidget(tab);
    m_tabBar->addTab(title);
    return tab;
}

void QRibbonWidget::removeTab(int index)
{
    if (index >= 0 && index < m_tabs.size()) {
        QRibbonTab *tab = m_tabs.takeAt(index);
        m_stackedWidget->removeWidget(tab);
        m_tabBar->removeTab(index);
        tab->deleteLater();
    }
}

void QRibbonWidget::removeTab(QRibbonTab *tab)
{
    int index = m_tabs.indexOf(tab);
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

QRibbonTab *QRibbonWidget::currentTab() const
{
    int index = m_tabBar->currentIndex();
    if (index >= 0 && index < m_tabs.size()) {
        return m_tabs[index];
    }
    return nullptr;
}

QRibbonTab *QRibbonWidget::tab(int index) const
{
    if (index >= 0 && index < m_tabs.size()) {
        return m_tabs[index];
    }
    return nullptr;
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
    m_stackedWidget->setCurrentIndex(index);
    if (index >= 0 && index < m_tabs.size() && m_tabs[index]) {
        m_tabs[index]->refreshLayout();
    }
    emit tabChanged(index);
    updateContextTab(index);
}

void QRibbonWidget::setDefaultButtonSize(QRibbonButtonSize size)
{
    m_defaultButtonSize = size;
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
    if (m_applicationButton) {
        if (m_applicationButton->parent() != m_topBarWidget) {
            m_applicationButton->setParent(m_topBarWidget);
        }
        m_applicationButton->setFixedHeight(29);
        m_applicationButton->updateGeometry();
        m_topLayout->insertWidget(0, m_applicationButton, 0, Qt::AlignLeft | Qt::AlignVCenter);
    }
}


void QRibbonWidget::onButtonClicked()
{
    QRibbonButton *button = qobject_cast<QRibbonButton*>(sender());
    if (button) {
        emit buttonClicked(button);
    }
}

bool QRibbonWidget::eventFilter(QObject *obj, QEvent *event)
{
    return QWidget::eventFilter(obj, event);
}


void QRibbonWidget::setTabContext(int index, const QString &contextTitle, const QColor &color)
{
    if (index < 0 || index >= m_tabs.size()) return;
    m_tabContexts[index] = {contextTitle, color};
    if (m_tabBar->currentIndex() == index) {
        updateContextTab(index);
    }
}

void QRibbonWidget::clearTabContext(int index)
{
    m_tabContexts.remove(index);
    if (m_tabBar->currentIndex() == index) {
        updateContextTab(index);
    }
}

void QRibbonWidget::updateContextTab(int index)
{
    for (int i = 0; i < m_tabBar->count(); ++i) {
        m_tabBar->setTabTextColor(i, QColor());
        m_tabBar->setTabToolTip(i, QString());
    }

    auto it = m_tabContexts.find(index);
    if (it == m_tabContexts.end()) {
        setFixedHeight(29 + 96);
        return;
    }

    const ContextInfo &ci = it.value();
    m_tabBar->setTabTextColor(index, ci.color.darker(180));
    m_tabBar->setTabToolTip(index, ci.title);
    setFixedHeight(29 + 96);
}
