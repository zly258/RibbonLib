#include "QRibbonStateManager.h"
#include "QRibbonWidget.h"
#include "QRibbonTab.h"
#include <QApplication>
#include <QSettings>

QRibbonStateManager::QRibbonStateManager(QRibbonWidget *ribbon, QObject *parent)
    : QObject(parent)
    , m_ribbon(ribbon)
    , m_settings(QSettings::IniFormat, QSettings::UserScope, 
                QApplication::organizationName().isEmpty() ? "RibbonLib" : QApplication::organizationName(),
                QApplication::applicationName().isEmpty() ? "RibbonState" : QApplication::applicationName())
{
}

void QRibbonStateManager::saveState()
{
    if (!m_ribbon) return;

    m_settings.setValue(key(QStringLiteral("currentTabIndex")), m_ribbon->currentIndex());
    m_settings.sync();
}

void QRibbonStateManager::restoreState()
{
    if (!m_ribbon) return;

    const int currentIndex = m_settings.value(key(QStringLiteral("currentTabIndex")), -1).toInt();
    if (currentIndex >= 0 && currentIndex < m_ribbon->tabCount()) {
        m_ribbon->setCurrentTab(currentIndex);
    }
}

QString QRibbonStateManager::key(const QString &name) const
{
    return m_settingsPrefix + "/" + name;
}