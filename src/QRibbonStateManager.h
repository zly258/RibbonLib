#ifndef QRIBBONSTATEMANAGER_H
#define QRIBBONSTATEMANAGER_H

#include "RibbonLibGlobal.h"

#include <QObject>
#include <QPointer>
#include <QSettings>
#include <QString>

class QRibbonWidget;

class RIBBONLIB_EXPORT QRibbonStateManager : public QObject
{
    Q_OBJECT

public:
    QRibbonStateManager(QRibbonWidget *ribbon, QObject *parent = nullptr);

    void saveState();
    void restoreState();

private:
    QString key(const QString &name) const;

    QPointer<QRibbonWidget> m_ribbon;
    QString m_settingsPrefix {QStringLiteral("Ribbon")};
    QSettings m_settings;
};

#endif // QRIBBONSTATEMANAGER_H
