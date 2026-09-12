#ifndef QRIBBONSTATEMANAGER_H
#define QRIBBONSTATEMANAGER_H

#include "RibbonLibGlobal.h"

#include <QObject>
#include <QString>
#include <QSettings>

class QRibbonWidget;

// Ribbon state manager: automatically persists and restores Ribbon state
class RIBBONLIB_EXPORT QRibbonStateManager : public QObject
{
    Q_OBJECT

public:
    QRibbonStateManager(QRibbonWidget *ribbon, QObject *parent = nullptr);

    void saveState();
    void restoreState();

private:
    QString key(const QString &name) const;

    QRibbonWidget *m_ribbon;
    QString m_settingsPrefix { QStringLiteral("Ribbon") };
    QSettings m_settings;
};

#endif // QRIBBONSTATEMANAGER_H