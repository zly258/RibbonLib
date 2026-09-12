#include "QRibbonMenu.h"
#include <QAction>
#include <QShowEvent>
#include <QHideEvent>

QRibbonMenu::QRibbonMenu(QWidget *parent)
    : QMenu(parent)
{
}

QRibbonMenu::QRibbonMenu(const QString &title, QWidget *parent)
    : QMenu(title, parent)
{
}

void QRibbonMenu::setIconTargetSize(const QSize &sz)
{
    if (sz.isValid() && sz.width() > 0 && sz.height() > 0) {
        m_iconTargetSize = sz;
    }
}

void QRibbonMenu::showEvent(QShowEvent *event)
{
    // Recursively scale icons before display and record originals
    m_originalIcons.clear();
    applyIconScaleRecursive(this);
    QMenu::showEvent(event);
}

void QRibbonMenu::hideEvent(QHideEvent *event)
{
    // Restore original icons upon hide and clear cache
    restoreIconsRecursive(this);
    m_originalIcons.clear();
    QMenu::hideEvent(event);
}

void QRibbonMenu::applyIconScaleRecursive(QMenu *menu)
{
    if (!menu) return;
    const auto acts = menu->actions();
    for (QAction *a : acts) {
        if (!a) continue;
        if (QMenu *sub = a->menu()) {
            applyIconScaleRecursive(sub);
        }
        const QIcon ic = a->icon();
        if (!ic.isNull()) {
            // Record original icon once
            if (!m_originalIcons.contains(a)) {
                m_originalIcons.insert(a, ic);
            }
            const QPixmap pm = ic.pixmap(m_iconTargetSize);
            a->setIcon(QIcon(pm));
        }
    }
}

void QRibbonMenu::restoreIconsRecursive(QMenu *menu)
{
    if (!menu) return;
    const auto acts = menu->actions();
    for (QAction *a : acts) {
        if (!a) continue;
        if (QMenu *sub = a->menu()) {
            restoreIconsRecursive(sub);
        }
        auto it = m_originalIcons.find(a);
        if (it != m_originalIcons.end()) {
            a->setIcon(it.value());
        }
    }
}
