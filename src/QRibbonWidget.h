#ifndef RIBBONWIDGET_H
#define RIBBONWIDGET_H

#include "RibbonLibGlobal.h"

#include <QColor>
#include <QHash>
#include <QList>
#include <QString>
#include <QWidget>

class QAction;
class QApplicationButton;
class QHBoxLayout;
class QStackedWidget;
class QTabBar;
class QToolButton;
class QVBoxLayout;
class QRibbonTab;

class RIBBONLIB_EXPORT QRibbonWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QRibbonWidget(QWidget *parent = nullptr);

    QRibbonTab *addTab(const QString &title, const QString &id = QString());
    void removeTab(int index);
    void removeTab(QRibbonTab *tab);
    void setCurrentTab(int index);
    bool setCurrentTab(const QString &id);
    QRibbonTab *currentTab() const;
    QRibbonTab *tab(int index) const;
    int indexOfTabId(const QString &id) const;
    int tabCount() const;
    int currentIndex() const;

    void setTabContext(int index, const QString &contextTitle, const QColor &color);
    void clearTabContext(int index);

    void setApplicationButton(QApplicationButton *button);
    QApplicationButton *applicationButton() const { return m_applicationButton; }

    QWidget *accessBarWidget() const { return m_accessBarWidget; }
    QToolButton *addAccessBarAction(QAction *action);

signals:
    void tabChanged(int index);

private slots:
    void onTabChanged(int index);

private:
    void setupUI();
    void updateContextTab(int index);

    QVBoxLayout *m_mainLayout {nullptr};
    QWidget *m_topBarWidget {nullptr};
    QHBoxLayout *m_topLayout {nullptr};
    QTabBar *m_tabBar {nullptr};
    QStackedWidget *m_stackedWidget {nullptr};

    QList<QRibbonTab*> m_tabs;
    struct ContextInfo { QString title; QColor color; };
    QHash<QRibbonTab*, ContextInfo> m_tabContexts;

    QApplicationButton *m_applicationButton {nullptr};
    QWidget *m_accessBarWidget {nullptr};
    QHBoxLayout *m_accessBarLayout {nullptr};
};

#endif // RIBBONWIDGET_H
