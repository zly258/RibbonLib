#ifndef RIBBONWIDGET_H
#define RIBBONWIDGET_H

/*
 * QRibbonWidget
 * ------------------------------------------------------------
 * Top-level Ribbon control widget.
 *
 * Combines ApplicationButton, TabBar, Ribbon content area, and AccessBar.
 * Manages overall Ribbon layout and tab switching without coupling to
 * specific business actions or commands.
 */

#include "RibbonLibGlobal.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QToolButton>
#include <QLabel>
#include <QFrame>
#include <QScrollArea>
#include <QButtonGroup>
#include <QIcon>
#include <QPushButton>
#include <QScrollBar>
#include <QSize>
#include <QTabWidget>
#include <QTabBar>
#include <QStackedWidget>
#include <QMenu>
#include <QMap>
#include <QColor>

class QFrame;
class QToolButton;
class QLabel;

#include "QRibbonButton.h"
#include "QRibbonGroup.h"
#include "QRibbonTab.h"

class QRibbonTab;
class QApplicationButton;

// Main Ribbon control widget (TabBar + StackedWidget)
class RIBBONLIB_EXPORT QRibbonWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QRibbonWidget(QWidget *parent = nullptr);

    // Tab management
    QRibbonTab *addTab(const QString &title);
    void removeTab(int index);
    void removeTab(QRibbonTab *tab);
    void setCurrentTab(int index);
    QRibbonTab *currentTab() const;
    QRibbonTab *tab(int index) const;
    int tabCount() const;
    int currentIndex() const;
    
    // Context Category: sets colored stripe and title for a specific tab
    void setTabContext(int index, const QString &contextTitle, const QColor &color);
    void clearTabContext(int index);
    
    // Global button size setting
    void setDefaultButtonSize(QRibbonButtonSize size);
    QRibbonButtonSize defaultButtonSize() const { return m_defaultButtonSize; }
    
    // ApplicationButton support
    void setApplicationButton(QApplicationButton *button);
    QApplicationButton* applicationButton() const { return m_applicationButton; }

    // AccessBar (top-right quick access action area)
    QWidget *accessBarWidget() const { return m_accessBarWidget; }
    // Adds a QAction to AccessBar, returning the created QToolButton
    QToolButton *addAccessBarAction(QAction *action);

signals:
    void buttonClicked(QRibbonButton *button);
    void tabChanged(int index);

private slots:
    void onButtonClicked();
    void onTabChanged(int index);

private:
    void setupUI();
    bool eventFilter(QObject *obj, QEvent *event) override;
    void updateContextTab(int index);
    
    QVBoxLayout *m_mainLayout;
    QWidget *m_topBarWidget {nullptr};
    QHBoxLayout *m_topLayout;
    QTabBar *m_tabBar;
    QStackedWidget *m_stackedWidget;
    
    QRibbonButtonSize m_defaultButtonSize;
    QList<QRibbonTab*> m_tabs;
    struct ContextInfo { QString title; QColor color; };
    QMap<int, ContextInfo> m_tabContexts;
    
    QApplicationButton *m_applicationButton;

    // AccessBar (top-right button area)
    QWidget *m_accessBarWidget {nullptr};
    QHBoxLayout *m_accessBarLayout {nullptr};
};

#endif // RIBBONWIDGET_H
