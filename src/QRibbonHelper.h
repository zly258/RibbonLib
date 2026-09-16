#ifndef QRIBBONHELPER_H
#define QRIBBONHELPER_H

#include "RibbonLibGlobal.h"

#include <QAction>
#include <QByteArray>
#include <QMap>
#include <QObject>
#include <QString>
#include <QVariantMap>

class QRibbonWidget;

class RIBBONLIB_EXPORT QRibbonHelper : public QObject
{
    Q_OBJECT

public:
    explicit QRibbonHelper(QObject *parent = nullptr);

    bool loadActions(const QString &actionsResource);
    bool loadLayout(const QString &layoutResource);
    bool buildRibbon(QRibbonWidget *ribbonWidget);

    const QMap<QString, QAction*> &actions() const { return m_actions; }
    QAction *action(const QString &id) const { return m_actions.value(id, nullptr); }
    void clearActions();

    QString errorString() const { return m_errorString; }

signals:
    void actionTriggered(const QString &id);

private:
    bool parseActionsJson(const QByteArray &bytes);
    bool parseLayoutJson(const QByteArray &bytes);
    bool buildApplicationButton(QRibbonWidget *ribbonWidget);
    bool buildTabs(QRibbonWidget *ribbonWidget);
    bool buildAccessBar(QRibbonWidget *ribbonWidget);

    QAction *ensureAction(const QString &id);
    QString resolveResourcePath(const QString &path) const;
    bool warn(const QString &message);

    QMap<QString, QAction*> m_actions;
    QVariantMap m_layoutRoot;
    QString m_errorString;
    QString m_resourceBaseDir;
};

#endif // QRIBBONHELPER_H
