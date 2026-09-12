#ifndef QRIBBONHELPER_H
#define QRIBBONHELPER_H

/*
 * QRibbonHelper
 * ------------------------------------------------------------
 * Ribbon JSON builder helper class.
 *
 * Constructs QActions, Ribbon tabs, groups, and buttons from actions.json
 * and ribbon.json configuration files.
 * Supports binding custom RibbonAction objects without coupling to
 * any specific application document or viewport context.
 */

#include "RibbonLibGlobal.h"

#include <QObject>
#include <QMap>
#include <QIcon>
#include <QAction>
#include <QPointer>
#include <QHash>
#include <QVariantMap>

class QRibbonWidget;
class RibbonAction;
class QRibbonButton;
class QRibbonSplitButton;

// Helper class: builds Ribbon UI from JSON resource/config files
class RIBBONLIB_EXPORT QRibbonHelper : public QObject {
    Q_OBJECT
public:
    explicit QRibbonHelper(QObject *parent = nullptr);

    // Loads actions.json and ribbon.json from specified resource/file paths
    bool loadFromResources(const QString &ribbonRes = ":/resources/ribbon.json",
                           const QString &actionsRes = ":/resources/actions.json");

    // Builds the specified QRibbonWidget based on loaded data
    bool buildRibbon(QRibbonWidget *ribbonWidget);

    // Returns parsed QAction map (id -> QAction*)
    const QMap<QString, QAction*> &actions() const { return m_actions; }

    // Set / get ribbonWidget reference
    void setRibbonWidget(QRibbonWidget *w) { m_ribbonWidget = w; }
    QRibbonWidget* ribbonWidget() const { return m_ribbonWidget; }

    // Retrieves QAction by id; returns nullptr if not found
    QAction *action(const QString &id) const { return m_actions.value(id, nullptr); }

    // Returns the last error message, or empty if successful
    QString errorString() const { return m_errorString; }

    // RibbonAction binding
    // Applications can register custom RibbonAction instances. Existing QActions with matching IDs
    // will be reused and have their properties synchronized.
    void registerRibbonAction(RibbonAction *action);
    void unregisterRibbonAction(const QString &id);
    RibbonAction *ribbonAction(const QString &id) const;
    void clearRibbonActions();
    void clearActions();

    // State and menu updates
    // Updates enabled state of all actions except the application button menu
    void updateActionEnabled(bool enabled);
    // Updates recent files menu items (typically inserted after 'open')
    void updateRecentFilesMenu();

signals:
    // Emitted when any JSON-defined action is triggered, forwarding the action id
    void actionTriggered(const QString &id);

private:
    // JSON parsing helpers
    bool parseActionsJson(const QByteArray &bytes);
    bool parseRibbonJson(const QByteArray &bytes);

    // Internal building steps
    bool buildApplicationButton(QRibbonWidget *ribbonWidget);
    bool buildTabs(QRibbonWidget *ribbonWidget);
    bool buildAccessBar(QRibbonWidget *ribbonWidget);

    QAction *ensureQAction(const QString &id);
    void bindRibbonAction(RibbonAction *ribbonAction, QAction *qAction);
    void syncQActionFromRibbonAction(RibbonAction *ribbonAction, QAction *qAction);
    QString actionParams(const QString &id) const;
    void setActionParams(const QString &id, const QString &params);
    bool warn(const QString &message);
    void bindQActionToButton(QAction *action, QRibbonButton *button);
    void bindQActionToSplitButton(QAction *action, QRibbonSplitButton *button);
    void rebuildRibbonActionBindings();
    QString resolveResourcePath(const QString &path) const;

private:
    // actions.json parsed results (id -> QAction*)
    QMap<QString, QAction*> m_actions;
    QHash<QString, QPointer<RibbonAction>> m_ribbonActions;
    QHash<QString, QString> m_actionParams;

    // ribbon.json root structure (stored as QVariantMap for lightweight access)
    QVariantMap m_ribbonRoot;
    QString m_errorString;
    QString m_resourceBaseDir;

    // Constructed Ribbon widget (for state and menu updates)
    QPointer<QRibbonWidget> m_ribbonWidget;
};

#endif // QRIBBONHELPER_H
