#ifndef QRIBBONMENU_H
#define QRIBBONMENU_H

#include "RibbonLibGlobal.h"

#include <QMenu>
#include <QHash>
#include <QPointer>

/*
 * QRibbonMenu
 * Encapsulates menu item icon scaling for Ribbon menus:
 * - Recursively scales QAction icons to the target icon size on showEvent;
 * - Restores original QAction icons on hideEvent to avoid modifying shared actions;
 * - Configures target icon size via setIconTargetSize (default 16x16 for compact items).
 */
class RIBBONLIB_EXPORT QRibbonMenu : public QMenu
{
    Q_OBJECT
public:
    explicit QRibbonMenu(QWidget *parent = nullptr);
    explicit QRibbonMenu(const QString &title, QWidget *parent = nullptr);

    void setIconTargetSize(const QSize &sz);
    QSize iconTargetSize() const { return m_iconTargetSize; }

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private:
    void applyIconScaleRecursive(QMenu *menu);
    void restoreIconsRecursive(QMenu *menu);

private:
    QSize m_iconTargetSize {16, 16};
    // Stores original icons before scaling for restoration on hide
    QHash<QAction*, QIcon> m_originalIcons;
};

#endif // QRIBBONMENU_H
