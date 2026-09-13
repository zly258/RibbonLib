#ifndef QRIBBONHELPER_H
#define QRIBBONHELPER_H

#include "RibbonLibGlobal.h"

#include <QAction>
#include <QHash>
#include <QList>
#include <QMap>
#include <QObject>
#include <QPointer>
#include <QVariantMap>

class QRibbonWidget;
class RibbonAction;
class QRibbonButton;
class QRibbonSplitButton;

class RIBBONLIB_EXPORT QRibbonHelper : public QObject
{
    Q_OBJECT

public:
    explicit QRibbonHelper(QObject *parent = nullptr);

    bool loadFromResources(const QString &ribbonRes = ":/resources/ribbon.json",
                           const QString &actionsRes = ":/resources/actions.json");
    bool buildRibbon(QRibbonWidget *ribbonWidget);

    const QMap<QString, QAction*> &actions() const { return m_actions; }
    QAction *action(const QString &id) const { return m_actions.value(id, nullptr); }

    void setRibbonWidget(QRibbonWidget *widget) { m_ribbonWidget = widget; }
    QRibbonWidget *ribbonWidget() const { return m_ribbonWidget; }

    QString errorString() const { return m_errorString; }

    void registerRibbonAction(RibbonAction *action);
    void unregisterRibbonAction(const QString &id);
    RibbonAction *ribbonAction(const QString &id) const;
    void clearRibbonActions();
    void clearActions();

    void updateActionEnabled(bool enabled);

signals:
    void actionTriggered(const QString &id);

private:
    bool parseActionsJson(const QByteArray &bytes);
    bool parseRibbonJson(const QByteArray &bytes);

    bool buildApplicationButton(QRibbonWidget *ribbonWidget);
    bool buildTabs(QRibbonWidget *ribbonWidget);
    bool buildAccessBar(QRibbonWidget *ribbonWidget);

    QAction *ensureQAction(const QString &id);
    QAction *createBoundAction(const QString &id, const QString &params);
    void syncAction(QAction *source, QAction *target);
    void triggerAction(const QString &id, const QString &params, bool checked);

    void bindRibbonAction(RibbonAction *ribbonAction, QAction *qAction);
    void syncQActionFromRibbonAction(RibbonAction *ribbonAction, QAction *qAction);
    void bindQActionToButton(QAction *action, QRibbonButton *button);
    void bindQActionToSplitButton(QAction *action, QRibbonSplitButton *button);
    void rebuildRibbonActionBindings();

    QString resolveResourcePath(const QString &path) const;
    bool warn(const QString &message);

    QMap<QString, QAction*> m_actions;
    QList<QAction*> m_boundActions;
    QHash<QString, QPointer<RibbonAction>> m_ribbonActions;

    QVariantMap m_ribbonRoot;
    QString m_errorString;
    QString m_resourceBaseDir;
    QPointer<QRibbonWidget> m_ribbonWidget;
};

#endif // QRIBBONHELPER_H
