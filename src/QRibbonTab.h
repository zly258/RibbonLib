#ifndef QRIBBONTAB_H
#define QRIBBONTAB_H

/*
 * QRibbonTab
 * ------------------------------------------------------------
 * Individual Ribbon tab page.
 *
 * Horizontally arranges multiple QRibbonGroup panels.
 * When content exceeds the visible width, scrollbars remain hidden
 * but mouse wheel events are translated into horizontal scrolling.
 */

#include "RibbonLibGlobal.h"

#include <QWidget>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QList>
#include <QString>

class QRibbonGroup;

// Ribbon tab page widget
class RIBBONLIB_EXPORT QRibbonTab : public QWidget
{
    Q_OBJECT
public:
    explicit QRibbonTab(const QString &title, QWidget *parent = nullptr);

    // Group management
    QRibbonGroup *addGroup(const QString &title);
    void removeGroup(QRibbonGroup *group);
    void clear();
    void refreshLayout();

    QString title() const { return m_title; }
    void setTitle(const QString &title) { m_title = title; }

private:
    void setupUI();
    void updateContentWidth();
    void scheduleContentWidthUpdate();
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

    QString m_title;
    QScrollArea *m_scrollArea {nullptr};
    QWidget *m_content {nullptr};
    QHBoxLayout *m_layout {nullptr};
    QList<QRibbonGroup*> m_groups;
    bool m_widthUpdatePending { false };
};

#endif // QRIBBONTAB_H
