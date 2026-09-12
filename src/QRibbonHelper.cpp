#include "QRibbonHelper.h"
#include "QRibbonWidget.h"
#include "QRibbonTab.h"
#include "QRibbonGroup.h"
#include "QRibbonButton.h"
#include "QRibbonSplitButton.h"
#include "QApplicationButton.h"
#include "QRibbonMenu.h"
#include "RibbonAction.h"

#include <QFile>
#include <QStatusBar>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QAction>
#include <QApplication>
#include <QIcon>
#include <QMenu>
#include <QKeySequence>
#include <QFileInfo>
#include <QDir>
#include <QVariant>
#include <QCoreApplication>
#include <QDebug>

// Helper: map string to QRibbonButtonSize
static QRibbonButtonSize toButtonSize(const QString &s)
{
    const QString v = s.trimmed().toLower();
    if (v == "small") return QRibbonButtonSize::Small;
    return QRibbonButtonSize::Large;
}

// Helper: elide string middle (forward declaration)
[[maybe_unused]] static QString elideMiddle(const QString &text, int maxLength);

// Update recent files menu items (placeholder)
void QRibbonHelper::updateRecentFilesMenu()
{
}

QRibbonHelper::QRibbonHelper(QObject *parent) : QObject(parent)
{
}

bool QRibbonHelper::warn(const QString &message)
{
    m_errorString = message;
    qWarning() << "[RibbonLib]" << message;
    return false;
}

QString QRibbonHelper::actionParams(const QString &id) const
{
    const auto it = m_actionParams.find(id);
    if (it != m_actionParams.end()) {
        return it.value();
    }

    RibbonAction *ra = ribbonAction(id);
    return ra ? ra->defaultParams() : QString();
}

void QRibbonHelper::setActionParams(const QString &id, const QString &params)
{
    if (id.isEmpty()) return;
    if (params.isEmpty()) {
        m_actionParams.remove(id);
    } else {
        m_actionParams.insert(id, params);
    }
}

QString QRibbonHelper::resolveResourcePath(const QString &path) const
{
    if (path.isEmpty()) return QString();

    if (path.startsWith(QStringLiteral(":/")) || QFileInfo(path).isAbsolute()) {
        return path;
    }

    if (!m_resourceBaseDir.isEmpty()) {
        return QDir(m_resourceBaseDir).filePath(path);
    }

    return path;
}


QAction *QRibbonHelper::ensureQAction(const QString &id)
{
    if (id.isEmpty()) return nullptr;

    QAction *act = m_actions.value(id, nullptr);
    if (act) return act;

    act = new QAction(this);
    act->setObjectName(id);
    act->setText(id);

    QObject::connect(act, &QAction::triggered, this, [this, id, act]() {
        RibbonAction *ra = ribbonAction(id);
        const QString params = actionParams(id);

        if (ra) {
            if (ra->isCheckable() && ra->isChecked() != act->isChecked()) {
                ra->setChecked(act->isChecked());
            }
            ra->trigger(params);
        }

        // Retain unified action id signal for applications not using custom RibbonActions.
        emit actionTriggered(id);
    });

    m_actions.insert(id, act);
    return act;
}

void QRibbonHelper::syncQActionFromRibbonAction(RibbonAction *ribbonAction, QAction *qAction)
{
    if (!ribbonAction || !qAction) return;

    const QString title = ribbonAction->name().isEmpty() ? ribbonAction->id() : ribbonAction->name();
    qAction->setText(title);
    qAction->setToolTip(ribbonAction->description());

    if (!ribbonAction->icon().isNull()) {
        qAction->setIcon(ribbonAction->icon());
    }

    if (!ribbonAction->shortcut().isEmpty()) {
        qAction->setShortcut(ribbonAction->shortcut());
    }

    qAction->setEnabled(ribbonAction->isEnabled());
    qAction->setVisible(ribbonAction->isVisible());
    qAction->setCheckable(ribbonAction->isCheckable());
    if (ribbonAction->isCheckable()) {
        const bool oldBlocked = qAction->blockSignals(true);
        qAction->setChecked(ribbonAction->isChecked());
        qAction->blockSignals(oldBlocked);
    }
}

void QRibbonHelper::bindRibbonAction(RibbonAction *ribbonAction, QAction *qAction)
{
    if (!ribbonAction || !qAction) return;

    syncQActionFromRibbonAction(ribbonAction, qAction);

    QObject::disconnect(ribbonAction, nullptr, qAction, nullptr);
    QObject::connect(ribbonAction, &RibbonAction::changed, qAction, [this, ribbonAction, qAction]() {
        syncQActionFromRibbonAction(ribbonAction, qAction);
    });

    QObject::disconnect(qAction, nullptr, ribbonAction, nullptr);
    QObject::connect(qAction, &QAction::toggled, ribbonAction, [ribbonAction](bool checked) {
        if (ribbonAction->isCheckable() && ribbonAction->isChecked() != checked) {
            ribbonAction->setChecked(checked);
        }
    });
}

void QRibbonHelper::registerRibbonAction(RibbonAction *action)
{
    if (!action) return;

    const QString id = action->id();
    if (id.isEmpty()) return;

    QAction *act = ensureQAction(id);
    m_ribbonActions.insert(id, action);
    bindRibbonAction(action, act);
}

void QRibbonHelper::unregisterRibbonAction(const QString &id)
{
    m_ribbonActions.remove(id);
}

void QRibbonHelper::clearRibbonActions()
{
    m_ribbonActions.clear();
}

void QRibbonHelper::clearActions()
{
    qDeleteAll(m_actions);
    m_actions.clear();
    m_actionParams.clear();
}

void QRibbonHelper::bindQActionToButton(QAction *action, QRibbonButton *button)
{
    if (!action || !button) return;

    button->setEnabled(action->isEnabled());
    button->setVisible(action->isVisible());
    button->setText(action->text());
    button->setIcon(action->icon());
    button->setToolTip(action->toolTip());

    QObject::connect(button, &QRibbonButton::clicked, action, &QAction::trigger);

    QObject::connect(action, &QAction::changed, button, [button, action]() {
        button->setEnabled(action->isEnabled());
        button->setVisible(action->isVisible());
        button->setText(action->text());
        button->setIcon(action->icon());
        button->setToolTip(action->toolTip());
    });
}

void QRibbonHelper::bindQActionToSplitButton(QAction *action, QRibbonSplitButton *button)
{
    if (!action || !button) return;

    button->setEnabled(action->isEnabled());
    button->setVisible(action->isVisible());
    button->setText(action->text());
    button->setIcon(action->icon());
    button->setToolTip(action->toolTip());

    QObject::connect(button, &QRibbonSplitButton::clicked, action, &QAction::trigger);

    QObject::connect(action, &QAction::changed, button, [button, action]() {
        button->setEnabled(action->isEnabled());
        button->setVisible(action->isVisible());
        button->setText(action->text());
        button->setIcon(action->icon());
        button->setToolTip(action->toolTip());
    });
}

RibbonAction *QRibbonHelper::ribbonAction(const QString &id) const
{
    const auto it = m_ribbonActions.find(id);
    if (it == m_ribbonActions.end()) return nullptr;
    return it.value().data();
}

void QRibbonHelper::rebuildRibbonActionBindings()
{
    for (auto it = m_ribbonActions.begin(); it != m_ribbonActions.end(); ++it) {
        RibbonAction *ra = it.value().data();
        if (!ra) continue;

        QAction *act = ensureQAction(ra->id());
        bindRibbonAction(ra, act);
    }
}


bool QRibbonHelper::loadFromResources(const QString &ribbonRes, const QString &actionsRes)
{
    m_errorString.clear();
    m_ribbonRoot.clear();
    clearActions();

    const QFileInfo actionsInfo(actionsRes);
    if (!actionsRes.startsWith(QStringLiteral(":/")) && actionsInfo.exists()) {
        m_resourceBaseDir = actionsInfo.absolutePath();
    } else {
        m_resourceBaseDir.clear();
    }

    // Load actions.json
    {
        QFile f(actionsRes);
        if (!f.open(QIODevice::ReadOnly)) {
            return warn(QString("Failed to open actions.json: %1").arg(actionsRes));
        }
        const QByteArray bytes = f.readAll();
        if (!parseActionsJson(bytes)) return false;
        rebuildRibbonActionBindings();
    }

    // Load ribbon.json
    {
        QFile f(ribbonRes);
        if (!f.open(QIODevice::ReadOnly)) {
            return warn(QString("Failed to open ribbon.json: %1").arg(ribbonRes));
        }
        const QByteArray bytes = f.readAll();
        if (!parseRibbonJson(bytes)) return false;
    }
    return true;
}

bool QRibbonHelper::parseActionsJson(const QByteArray &bytes)
{
    QJsonParseError err{};
    QJsonDocument doc = QJsonDocument::fromJson(bytes, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        return warn(QString("Failed to parse actions.json: %1").arg(err.errorString()));
    }
    const QJsonObject root = doc.object();
    const QJsonArray arr = root.value("actions").toArray();

    for (const QJsonValue &v : arr) {
        const QJsonObject o = v.toObject();
        const QString id = o.value("id").toString();
        if (id.isEmpty()) continue;
        QAction *act = ensureQAction(id);
        if (act->property("fromJson").toBool()) {
            qWarning() << "[RibbonLib] actions.json contains duplicate id:" << id;
        }
        act->setProperty("fromJson", true);
        act->setText(o.value("name").toString(id));
        act->setToolTip(o.value("description").toString());
        const QString iconPath = resolveResourcePath(o.value("icon").toString());
        if (!iconPath.isEmpty()) {
            if (!iconPath.startsWith(QStringLiteral(":/")) && !QFileInfo::exists(iconPath)) {
                qWarning() << "[RibbonLib] icon file does not exist:" << iconPath;
            }
            act->setIcon(QIcon(iconPath));
        }
        const QString sc = o.value("shortcut").toString();
        if (!sc.isEmpty()) act->setShortcut(QKeySequence(sc));
        if (o.contains("enabled")) act->setEnabled(o.value("enabled").toBool(true));
        if (o.contains("visible")) act->setVisible(o.value("visible").toBool(true));
        if (o.contains("checkable")) act->setCheckable(o.value("checkable").toBool(false));
        if (o.contains("checked")) act->setChecked(o.value("checked").toBool(false));
    }
    return true;
}

bool QRibbonHelper::parseRibbonJson(const QByteArray &bytes)
{
    QJsonParseError err{};
    QJsonDocument doc = QJsonDocument::fromJson(bytes, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        return warn(QString("Failed to parse ribbon.json: %1").arg(err.errorString()));
    }
    const QJsonObject root = doc.object();
    const QJsonObject rib = root.value("ribbon").toObject();
    if (rib.isEmpty()) {
        return warn(QString("ribbon.json is missing ribbon node"));
    }
    m_ribbonRoot = rib.toVariantMap();
    return true;
}

bool QRibbonHelper::buildApplicationButton(QRibbonWidget *ribbonWidget)
{
    const QVariantMap appBtn = m_ribbonRoot.value("applicationButton").toMap();
    if (appBtn.isEmpty()) return true; // Optional

    QApplicationButton *btn = new QApplicationButton(ribbonWidget);
    const QString title = appBtn.value("title").toString();
    if (!title.isEmpty()) btn->setText(title);

    QMenu *menu = new QMenu(btn);
    menu->setObjectName("RibbonApplicationMenu");
    menu->setMinimumWidth(0);
    const QVariantList items = appBtn.value("menu").toList();
    for (const QVariant &it : items) {
        if (it.toString() == "-") {
            menu->addSeparator();
            continue;
        }
        const QVariantMap m = it.toMap();
        const QString id = m.value("id").toString();
        if (id.isEmpty()) continue;
        QAction *act = m_actions.value(id, nullptr);
        if (!act) {
            qWarning() << "[RibbonLib] applicationButton menu references non-existent action:" << id;
            continue;
        }
        const QString params = m.value("params").toString();
        if (!params.isEmpty()) {
            setActionParams(id, params);
        }
        menu->addAction(act);
    }
    btn->setApplicationMenu(menu);
    ribbonWidget->setApplicationButton(btn);
    return true;
}

bool QRibbonHelper::buildTabs(QRibbonWidget *ribbonWidget)
{
    const QVariantList tabs = m_ribbonRoot.value("tabs").toList();
    for (const QVariant &tv : tabs) {
        const QVariantMap t = tv.toMap();
        const QString tabTitle = t.value("title").toString();
        if (tabTitle.isEmpty()) continue;
        QRibbonTab *tab = ribbonWidget->addTab(tabTitle);

        const QVariantList panels = t.value("panels").toList();
        for (const QVariant &pv : panels) {
            const QVariantMap p = pv.toMap();
            const QString panelTitle = p.value("title").toString();
            if (panelTitle.isEmpty()) continue;
            QRibbonGroup *group = tab->addGroup(panelTitle);

            const QVariantList items = p.value("items").toList();
            for (const QVariant &iv : items) {
                const QVariantMap m = iv.toMap();
                const QString id = m.value("id").toString();
                if (id.isEmpty()) continue;
                QAction *act = m_actions.value(id, nullptr);
                if (!act) {
                    qWarning() << "[RibbonLib] ribbon.json references non-existent action:" << id;
                    continue;
                }

                const QString params = m.value("params").toString();
                if (!params.isEmpty()) {
                    setActionParams(id, params);
                }

                const QRibbonButtonSize size = toButtonSize(m.value("style").toString());
                // Create button: decide whether to use split dropdown button
                QIcon icon = act->icon();
                QString text = act->text();
                QRibbonButton *btn = nullptr;

                // Dropdown menu support: if item defines a menu, use QRibbonSplitButton
                const QVariantList menuItems = m.value("menu").toList();
                if (!menuItems.isEmpty()) {
                    // Split button widget
                    auto sb = new QRibbonSplitButton(icon, text, size, group);
                    // Build menu, using QRibbonMenu for icon sizing support
                    QMenu *menu = new QRibbonMenu(group);
                    for (const QVariant &mv : menuItems) {
                        if (mv.toString() == "-") {
                            menu->addSeparator();
                            continue;
                        }
                        const QVariantMap mm = mv.toMap();
                        const QString mid = mm.value("id").toString();
                        if (mid.isEmpty()) continue;
                        QAction *mact = m_actions.value(mid, nullptr);
                        if (!mact) {
                            qWarning() << "[RibbonLib] menu references non-existent action:" << mid;
                            continue;
                        }
                        const QString mparams = mm.value("params").toString();
                        if (!mparams.isEmpty()) {
                            setActionParams(mid, mparams);
                        }
                        menu->addAction(mact);
                    }
                    sb->setMenu(menu);
                    // If default action is set, sync appearance; otherwise choose first valid item
                    QAction *def = menu->defaultAction();
                    if (!def) {
                        const auto acts = menu->actions();
                        for (QAction *a : acts) { if (a && a->isEnabled() && !a->isSeparator()) { def = a; break; } }
                        if (def) menu->setDefaultAction(def);
                    }
                    if (def) sb->setDefaultAction(def);

                    // Sync QAction state and changes
                    bindQActionToSplitButton(act, sb);

                    // Add to group according to button size to keep layout order
                    if (size == QRibbonButtonSize::Large) {
                        group->addLargeWidget(sb);
                    } else {
                        group->addSmallWidget(sb);
                    }
                } else {
                    // Standard action button
                    btn = new QRibbonButton(icon, text, size, group);
                    bindQActionToButton(act, btn);
                    group->addButton(btn);
                }
            }
        }
    }
    return true;
}

// Load AccessBar buttons from ribbon.json
// Supports two formats:
// 1) Legacy: "accessBar": ["new", "open", ...]
// 2) Object: "quickAccessBar": { "items": [ {"id":"new"}, {"id":"open"} ] }
bool QRibbonHelper::buildAccessBar(QRibbonWidget *ribbonWidget)
{
    // Prefer new object format
    const QVariantMap quick = m_ribbonRoot.value("quickAccessBar").toMap();
    QVariantList ids;
    if (!quick.isEmpty()) {
        const QVariantList items = quick.value("items").toList();
        for (const QVariant &it : items) {
            const QVariantMap m = it.toMap();
            const QString id = m.value("id").toString();
            if (!id.isEmpty()) ids.push_back(id);
        }
    } else {
        // Fallback to legacy format
        const QVariantList access = m_ribbonRoot.value("accessBar").toList();
        for (const QVariant &v : access) {
            const QString id = v.toString();
            if (!id.isEmpty()) ids.push_back(id);
        }
    }

    if (ids.isEmpty()) return true; // Optional

    for (const QVariant &v : ids) {
        const QString id = v.toString();
        QAction *act = m_actions.value(id, nullptr);
        if (!act) {
            qWarning() << "[RibbonLib] accessBar references non-existent action:" << id;
            continue;
        }
        ribbonWidget->addAccessBarAction(act);
    }
    return true;
}

bool QRibbonHelper::buildRibbon(QRibbonWidget *ribbonWidget)
{
    m_errorString.clear();
    if (!ribbonWidget) return warn(QString("buildRibbon failed: ribbonWidget is null"));
    if (m_ribbonRoot.isEmpty()) return warn(QString("buildRibbon failed: ribbon.json is not loaded"));

    // Save pointer for menu and state updates
    m_ribbonWidget = ribbonWidget;

    if (!buildApplicationButton(ribbonWidget)) return false;
    if (!buildTabs(ribbonWidget)) return false;
    if (!buildAccessBar(ribbonWidget)) return false;
    return true;
}

// Update enabled state for all actions except application menu
void QRibbonHelper::updateActionEnabled(bool enabled)
{
    // Collect application menu actions to prevent them from being disabled
    QList<QAction*> appActions;
    if (m_ribbonWidget && m_ribbonWidget->applicationButton() && m_ribbonWidget->applicationButton()->applicationMenu()) {
        appActions = m_ribbonWidget->applicationButton()->applicationMenu()->actions();
    }

    for (auto it = m_actions.begin(); it != m_actions.end(); ++it) {
        QAction *act = it.value();
        if (!appActions.contains(act)) {
            act->setEnabled(enabled);
        }
    }

    // Traverse Ribbon widget tree to update button enabled states
    if (m_ribbonWidget) {
        const auto splitButtons = m_ribbonWidget->findChildren<QRibbonSplitButton*>();
        for (QRibbonSplitButton *sb : splitButtons) {
            if (!sb) continue;
            sb->setEnabled(enabled);
            // Also update menu actions
            if (sb->menu()) {
                for (QAction *ma : sb->menu()->actions()) {
                    if (ma && !appActions.contains(ma)) {
                        ma->setEnabled(enabled);
                    }
                }
            }
        }

        const auto buttons = m_ribbonWidget->findChildren<QRibbonButton*>();
        for (QRibbonButton *btn : buttons) {
            if (!btn) continue;
            btn->setEnabled(enabled);
        }
    }
}

// Helper: elide string middle
[[maybe_unused]] static QString elideMiddle(const QString &text, int maxLength)
{
    if (text.length() <= maxLength) return text;
    const QString ellipsis = "...";
    const int keep = maxLength - ellipsis.length();
    if (keep <= 0) return ellipsis;
    const int left = keep / 2;
    const int right = keep - left;
    return text.left(left) + ellipsis + text.right(right);
}