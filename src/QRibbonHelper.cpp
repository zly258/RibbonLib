#include "QRibbonHelper.h"
#include "QApplicationButton.h"
#include "QRibbonButton.h"
#include "QRibbonGroup.h"
#include "QRibbonMenu.h"
#include "QRibbonSplitButton.h"
#include "QRibbonTab.h"
#include "QRibbonWidget.h"
#include "RibbonAction.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeySequence>
#include <QMenu>
#include <QSet>
#include <QStyle>
#include <QVariant>

static QRibbonButtonSize toButtonSize(const QString &value)
{
    return value.trimmed().compare(QStringLiteral("small"), Qt::CaseInsensitive) == 0
        ? QRibbonButtonSize::Small
        : QRibbonButtonSize::Large;
}

static QIcon qtStandardIcon(const QString &name)
{
    const QString key = name.trimmed().toLower();
    if (key.isEmpty()) return QIcon();

    static const QHash<QString, QStyle::StandardPixmap> icons {
        {QStringLiteral("file"), QStyle::SP_FileIcon},
        {QStringLiteral("sp_fileicon"), QStyle::SP_FileIcon},
        {QStringLiteral("folder"), QStyle::SP_DirIcon},
        {QStringLiteral("sp_diricon"), QStyle::SP_DirIcon},
        {QStringLiteral("home"), QStyle::SP_DirHomeIcon},
        {QStringLiteral("sp_dirhomeicon"), QStyle::SP_DirHomeIcon},
        {QStringLiteral("open"), QStyle::SP_DialogOpenButton},
        {QStringLiteral("sp_dialogopenbutton"), QStyle::SP_DialogOpenButton},
        {QStringLiteral("save"), QStyle::SP_DialogSaveButton},
        {QStringLiteral("sp_dialogsavebutton"), QStyle::SP_DialogSaveButton},
        {QStringLiteral("close"), QStyle::SP_DialogCloseButton},
        {QStringLiteral("sp_dialogclosebutton"), QStyle::SP_DialogCloseButton},
        {QStringLiteral("apply"), QStyle::SP_DialogApplyButton},
        {QStringLiteral("sp_dialogapplybutton"), QStyle::SP_DialogApplyButton},
        {QStringLiteral("cancel"), QStyle::SP_DialogCancelButton},
        {QStringLiteral("sp_dialogcancelbutton"), QStyle::SP_DialogCancelButton},
        {QStringLiteral("reset"), QStyle::SP_DialogResetButton},
        {QStringLiteral("sp_dialogresetbutton"), QStyle::SP_DialogResetButton},
        {QStringLiteral("help"), QStyle::SP_DialogHelpButton},
        {QStringLiteral("sp_dialoghelpbutton"), QStyle::SP_DialogHelpButton},
        {QStringLiteral("info"), QStyle::SP_MessageBoxInformation},
        {QStringLiteral("sp_messageboxinformation"), QStyle::SP_MessageBoxInformation},
        {QStringLiteral("warning"), QStyle::SP_MessageBoxWarning},
        {QStringLiteral("sp_messageboxwarning"), QStyle::SP_MessageBoxWarning},
        {QStringLiteral("error"), QStyle::SP_MessageBoxCritical},
        {QStringLiteral("sp_messageboxcritical"), QStyle::SP_MessageBoxCritical},
        {QStringLiteral("question"), QStyle::SP_MessageBoxQuestion},
        {QStringLiteral("sp_messageboxquestion"), QStyle::SP_MessageBoxQuestion},
        {QStringLiteral("back"), QStyle::SP_ArrowBack},
        {QStringLiteral("sp_arrowback"), QStyle::SP_ArrowBack},
        {QStringLiteral("forward"), QStyle::SP_ArrowForward},
        {QStringLiteral("sp_arrowforward"), QStyle::SP_ArrowForward},
        {QStringLiteral("up"), QStyle::SP_ArrowUp},
        {QStringLiteral("sp_arrowup"), QStyle::SP_ArrowUp},
        {QStringLiteral("down"), QStyle::SP_ArrowDown},
        {QStringLiteral("sp_arrowdown"), QStyle::SP_ArrowDown},
        {QStringLiteral("reload"), QStyle::SP_BrowserReload},
        {QStringLiteral("sp_browserreload"), QStyle::SP_BrowserReload},
        {QStringLiteral("stop"), QStyle::SP_BrowserStop},
        {QStringLiteral("sp_browserstop"), QStyle::SP_BrowserStop},
        {QStringLiteral("play"), QStyle::SP_MediaPlay},
        {QStringLiteral("sp_mediaplay"), QStyle::SP_MediaPlay},
        {QStringLiteral("pause"), QStyle::SP_MediaPause},
        {QStringLiteral("sp_mediapause"), QStyle::SP_MediaPause},
        {QStringLiteral("trash"), QStyle::SP_TrashIcon},
        {QStringLiteral("sp_trashicon"), QStyle::SP_TrashIcon},
        {QStringLiteral("settings"), QStyle::SP_FileDialogDetailedView},
        {QStringLiteral("sp_filedialogdetailedview"), QStyle::SP_FileDialogDetailedView},
        {QStringLiteral("list"), QStyle::SP_FileDialogListView},
        {QStringLiteral("sp_filedialoglistview"), QStyle::SP_FileDialogListView},
        {QStringLiteral("maximize"), QStyle::SP_TitleBarMaxButton},
        {QStringLiteral("sp_titlebarmaxbutton"), QStyle::SP_TitleBarMaxButton}
    };

    const auto it = icons.constFind(key);
    return it == icons.constEnd() ? QIcon() : QApplication::style()->standardIcon(it.value());
}

QRibbonHelper::QRibbonHelper(QObject *parent)
    : QObject(parent)
{
}

bool QRibbonHelper::warn(const QString &message)
{
    m_errorString = message;
    qWarning() << "[RibbonLib]" << message;
    return false;
}

QString QRibbonHelper::resolveResourcePath(const QString &path) const
{
    if (path.isEmpty()) return QString();

    if (path.startsWith(QStringLiteral(":/")) || QFileInfo(path).isAbsolute()) {
        return path;
    }

    return m_resourceBaseDir.isEmpty()
        ? path
        : QDir(m_resourceBaseDir).filePath(path);
}

void QRibbonHelper::syncAction(QAction *source, QAction *target)
{
    if (!source || !target || source == target) return;

    target->setObjectName(source->objectName());
    target->setText(source->text());
    target->setIcon(source->icon());
    target->setToolTip(source->toolTip());
    target->setStatusTip(source->statusTip());
    target->setShortcut(source->shortcut());
    target->setEnabled(source->isEnabled());
    target->setVisible(source->isVisible());
    target->setCheckable(source->isCheckable());
    target->setChecked(source->isCheckable() && source->isChecked());
}

void QRibbonHelper::triggerAction(const QString &id, const QString &params, bool checked)
{
    RibbonAction *ribbon = ribbonAction(id);
    if (ribbon) {
        if (ribbon->isCheckable() && ribbon->isChecked() != checked) {
            ribbon->setChecked(checked);
        }
        ribbon->trigger(params);
    }

    emit actionTriggered(id);
}

QAction *QRibbonHelper::ensureQAction(const QString &id)
{
    if (id.isEmpty()) return nullptr;

    if (QAction *existing = m_actions.value(id, nullptr)) {
        return existing;
    }

    auto *action = new QAction(this);
    action->setObjectName(id);
    action->setText(id);

    connect(action, &QAction::triggered, this, [this, id](bool checked) {
        triggerAction(id, QString(), checked);
    });

    m_actions.insert(id, action);
    return action;
}

QAction *QRibbonHelper::createBoundAction(const QString &id, const QString &params)
{
    QAction *source = m_actions.value(id, nullptr);
    if (!source) return nullptr;

    if (params.isEmpty()) {
        return source;
    }

    auto *bound = new QAction(this);
    m_boundActions.append(bound);
    syncAction(source, bound);

    connect(source, &QAction::changed, bound, [this, source, bound]() {
        syncAction(source, bound);
    });

    connect(bound, &QAction::toggled, source, [source](bool checked) {
        if (source->isCheckable() && source->isChecked() != checked) {
            source->setChecked(checked);
        }
    });

    connect(bound, &QAction::triggered, this, [this, id, params](bool checked) {
        triggerAction(id, params, checked);
    });

    return bound;
}

void QRibbonHelper::syncQActionFromRibbonAction(RibbonAction *ribbonAction, QAction *qAction)
{
    if (!ribbonAction || !qAction) return;

    qAction->setText(ribbonAction->name().isEmpty() ? ribbonAction->id() : ribbonAction->name());
    qAction->setToolTip(ribbonAction->description());
    qAction->setIcon(ribbonAction->icon());
    qAction->setShortcut(ribbonAction->shortcut());
    qAction->setEnabled(ribbonAction->isEnabled());
    qAction->setVisible(ribbonAction->isVisible());
    qAction->setCheckable(ribbonAction->isCheckable());
    qAction->setChecked(ribbonAction->isCheckable() && ribbonAction->isChecked());
}

void QRibbonHelper::bindRibbonAction(RibbonAction *ribbonAction, QAction *qAction)
{
    if (!ribbonAction || !qAction) return;

    syncQActionFromRibbonAction(ribbonAction, qAction);

    disconnect(ribbonAction, nullptr, qAction, nullptr);
    connect(ribbonAction, &RibbonAction::changed, qAction, [this, ribbonAction, qAction]() {
        syncQActionFromRibbonAction(ribbonAction, qAction);
    });

    disconnect(qAction, nullptr, ribbonAction, nullptr);
    connect(qAction, &QAction::toggled, ribbonAction, [ribbonAction](bool checked) {
        if (ribbonAction->isCheckable() && ribbonAction->isChecked() != checked) {
            ribbonAction->setChecked(checked);
        }
    });
}

void QRibbonHelper::registerRibbonAction(RibbonAction *action)
{
    if (!action || action->id().isEmpty()) return;

    QAction *qAction = ensureQAction(action->id());
    m_ribbonActions.insert(action->id(), action);
    bindRibbonAction(action, qAction);
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
    qDeleteAll(m_boundActions);
    m_boundActions.clear();

    qDeleteAll(m_actions);
    m_actions.clear();
}

void QRibbonHelper::bindQActionToButton(QAction *action, QRibbonButton *button)
{
    if (!action || !button) return;

    auto sync = [action, button]() {
        button->setEnabled(action->isEnabled());
        button->setVisible(action->isVisible());
        button->setText(action->text());
        button->setIcon(action->icon());
        button->setToolTip(action->toolTip());
        button->setShortcut(action->shortcut());
        button->setCheckable(action->isCheckable());
        if (action->isCheckable()) {
            button->setChecked(action->isChecked());
        }
    };

    sync();
    connect(button, &QRibbonButton::clicked, action, &QAction::trigger);
    connect(action, &QAction::changed, button, sync);
}

void QRibbonHelper::bindQActionToSplitButton(QAction *action, QRibbonSplitButton *button)
{
    if (!action || !button) return;

    auto sync = [action, button]() {
        button->setEnabled(action->isEnabled());
        button->setVisible(action->isVisible());
        button->setText(action->text());
        button->setIcon(action->icon());
        button->setToolTip(action->toolTip());
        button->setCheckable(action->isCheckable());
        if (action->isCheckable()) {
            button->setChecked(action->isChecked());
        }
    };

    sync();
    connect(button, &QRibbonSplitButton::clicked, action, &QAction::trigger);
    connect(action, &QAction::changed, button, sync);
}

RibbonAction *QRibbonHelper::ribbonAction(const QString &id) const
{
    const auto it = m_ribbonActions.constFind(id);
    return it == m_ribbonActions.constEnd() ? nullptr : it.value().data();
}

void QRibbonHelper::rebuildRibbonActionBindings()
{
    for (auto it = m_ribbonActions.begin(); it != m_ribbonActions.end(); ++it) {
        RibbonAction *ribbon = it.value().data();
        if (!ribbon) continue;
        bindRibbonAction(ribbon, ensureQAction(ribbon->id()));
    }
}

bool QRibbonHelper::loadFromResources(const QString &ribbonRes, const QString &actionsRes)
{
    m_errorString.clear();
    m_ribbonRoot.clear();
    clearActions();

    const QFileInfo actionsInfo(actionsRes);
    if (actionsInfo.exists()) {
        m_resourceBaseDir = actionsRes.startsWith(QStringLiteral(":/"))
            ? actionsInfo.path()
            : actionsInfo.absolutePath();
    } else {
        m_resourceBaseDir.clear();
    }

    QFile actionsFile(actionsRes);
    if (!actionsFile.open(QIODevice::ReadOnly)) {
        return warn(QStringLiteral("Failed to open actions.json: %1").arg(actionsRes));
    }
    if (!parseActionsJson(actionsFile.readAll())) {
        return false;
    }
    rebuildRibbonActionBindings();

    QFile ribbonFile(ribbonRes);
    if (!ribbonFile.open(QIODevice::ReadOnly)) {
        return warn(QStringLiteral("Failed to open ribbon.json: %1").arg(ribbonRes));
    }
    return parseRibbonJson(ribbonFile.readAll());
}

bool QRibbonHelper::parseActionsJson(const QByteArray &bytes)
{
    QJsonParseError error{};
    const QJsonDocument document = QJsonDocument::fromJson(bytes, &error);
    if (error.error != QJsonParseError::NoError || !document.isObject()) {
        return warn(QStringLiteral("Failed to parse actions.json: %1").arg(error.errorString()));
    }

    const QJsonArray actions = document.object().value(QStringLiteral("actions")).toArray();
    for (const QJsonValue &value : actions) {
        const QJsonObject object = value.toObject();
        const QString id = object.value(QStringLiteral("id")).toString();
        if (id.isEmpty()) continue;

        QAction *action = ensureQAction(id);
        if (action->property("fromJson").toBool()) {
            qWarning() << "[RibbonLib] actions.json contains duplicate id:" << id;
        }
        action->setProperty("fromJson", true);
        action->setText(object.value(QStringLiteral("name")).toString(id));
        action->setToolTip(object.value(QStringLiteral("description")).toString());

        QString iconSpec = object.value(QStringLiteral("icon")).toString().trimmed();
        QString standardIconName = object.value(QStringLiteral("standardIcon")).toString().trimmed();
        if (iconSpec.startsWith(QStringLiteral("qt:"), Qt::CaseInsensitive)) {
            standardIconName = iconSpec.mid(3);
            iconSpec.clear();
        }

        QIcon icon;
        bool customIconMissing = false;
        if (!iconSpec.isEmpty()) {
            const QString iconPath = resolveResourcePath(iconSpec);
            if (QFile::exists(iconPath)) {
                icon = QIcon(iconPath);
            } else {
                customIconMissing = true;
            }
        }

        if (icon.isNull() && !standardIconName.isEmpty()) {
            icon = qtStandardIcon(standardIconName);
        }

        if (!icon.isNull()) {
            action->setIcon(icon);
        } else if (customIconMissing) {
            qWarning() << "[RibbonLib] icon file does not exist and no valid fallback is available:"
                       << iconSpec;
        } else if (!standardIconName.isEmpty()) {
            qWarning() << "[RibbonLib] unknown Qt standard icon:" << standardIconName;
        }

        const QString shortcut = object.value(QStringLiteral("shortcut")).toString();
        if (!shortcut.isEmpty()) {
            action->setShortcut(QKeySequence(shortcut));
        }

        if (object.contains(QStringLiteral("enabled"))) {
            action->setEnabled(object.value(QStringLiteral("enabled")).toBool(true));
        }
        if (object.contains(QStringLiteral("visible"))) {
            action->setVisible(object.value(QStringLiteral("visible")).toBool(true));
        }
        if (object.contains(QStringLiteral("checkable"))) {
            action->setCheckable(object.value(QStringLiteral("checkable")).toBool(false));
        }
        if (object.contains(QStringLiteral("checked"))) {
            action->setChecked(object.value(QStringLiteral("checked")).toBool(false));
        }
    }

    return true;
}

bool QRibbonHelper::parseRibbonJson(const QByteArray &bytes)
{
    QJsonParseError error{};
    const QJsonDocument document = QJsonDocument::fromJson(bytes, &error);
    if (error.error != QJsonParseError::NoError || !document.isObject()) {
        return warn(QStringLiteral("Failed to parse ribbon.json: %1").arg(error.errorString()));
    }

    const QJsonObject ribbon = document.object().value(QStringLiteral("ribbon")).toObject();
    if (ribbon.isEmpty()) {
        return warn(QStringLiteral("ribbon.json is missing ribbon node"));
    }

    m_ribbonRoot = ribbon.toVariantMap();
    return true;
}

bool QRibbonHelper::buildApplicationButton(QRibbonWidget *ribbonWidget)
{
    const QVariantMap definition = m_ribbonRoot.value(QStringLiteral("applicationButton")).toMap();
    if (definition.isEmpty()) return true;

    auto *button = new QApplicationButton(ribbonWidget);
    const QString title = definition.value(QStringLiteral("title")).toString();
    if (!title.isEmpty()) {
        button->setText(title);
    }

    auto *menu = new QMenu(button);
    menu->setObjectName(QStringLiteral("RibbonApplicationMenu"));

    const QVariantList items = definition.value(QStringLiteral("menu")).toList();
    for (const QVariant &item : items) {
        if (item.toString() == QStringLiteral("-")) {
            menu->addSeparator();
            continue;
        }

        const QVariantMap entry = item.toMap();
        const QString id = entry.value(QStringLiteral("id")).toString();
        if (id.isEmpty()) continue;

        QAction *action = createBoundAction(id, entry.value(QStringLiteral("params")).toString());
        if (!action) {
            qWarning() << "[RibbonLib] applicationButton menu references non-existent action:" << id;
            continue;
        }
        menu->addAction(action);
    }

    button->setApplicationMenu(menu);
    ribbonWidget->setApplicationButton(button);
    return true;
}

bool QRibbonHelper::buildTabs(QRibbonWidget *ribbonWidget)
{
    const QVariantList tabs = m_ribbonRoot.value(QStringLiteral("tabs")).toList();
    for (const QVariant &tabValue : tabs) {
        const QVariantMap tabDefinition = tabValue.toMap();
        const QString title = tabDefinition.value(QStringLiteral("title")).toString();
        if (title.isEmpty()) continue;

        const QString tabId = tabDefinition.value(QStringLiteral("id")).toString();
        QRibbonTab *tab = ribbonWidget->addTab(title, tabId);

        const QVariantList panels = tabDefinition.value(QStringLiteral("panels")).toList();
        for (const QVariant &panelValue : panels) {
            const QVariantMap panelDefinition = panelValue.toMap();
            const QString panelTitle = panelDefinition.value(QStringLiteral("title")).toString();
            if (panelTitle.isEmpty()) continue;

            QRibbonGroup *group = tab->addGroup(panelTitle);
            const QVariantList items = panelDefinition.value(QStringLiteral("items")).toList();

            for (const QVariant &itemValue : items) {
                const QVariantMap item = itemValue.toMap();
                const QString id = item.value(QStringLiteral("id")).toString();
                if (id.isEmpty()) continue;

                QAction *baseAction = m_actions.value(id, nullptr);
                if (!baseAction) {
                    qWarning() << "[RibbonLib] ribbon.json references non-existent action:" << id;
                    continue;
                }

                const QRibbonButtonSize size = toButtonSize(item.value(QStringLiteral("style")).toString());
                const QVariantList menuItems = item.value(QStringLiteral("menu")).toList();

                if (!menuItems.isEmpty()) {
                    auto *splitButton = new QRibbonSplitButton(baseAction->icon(), baseAction->text(), size, group);
                    auto *menu = new QRibbonMenu(group);
                    menu->setObjectName(QStringLiteral("RibbonSplitMenu"));

                    QAction *defaultAction = nullptr;
                    const QString defaultId = item.value(QStringLiteral("defaultId")).toString();

                    for (const QVariant &menuValue : menuItems) {
                        if (menuValue.toString() == QStringLiteral("-")) {
                            menu->addSeparator();
                            continue;
                        }

                        const QVariantMap menuEntry = menuValue.toMap();
                        const QString menuId = menuEntry.value(QStringLiteral("id")).toString();
                        if (menuId.isEmpty()) continue;

                        QAction *menuAction = createBoundAction(
                            menuId,
                            menuEntry.value(QStringLiteral("params")).toString());
                        if (!menuAction) {
                            qWarning() << "[RibbonLib] menu references non-existent action:" << menuId;
                            continue;
                        }

                        menu->addAction(menuAction);
                        if (!defaultId.isEmpty() && menuId == defaultId) {
                            defaultAction = menuAction;
                        }
                    }

                    if (!defaultAction) {
                        for (QAction *action : menu->actions()) {
                            if (action && !action->isSeparator() && action->isEnabled()) {
                                defaultAction = action;
                                break;
                            }
                        }
                    }

                    splitButton->setMenu(menu);
                    if (defaultAction) {
                        splitButton->setDefaultAction(defaultAction);
                    } else {
                        bindQActionToSplitButton(baseAction, splitButton);
                    }

                    if (size == QRibbonButtonSize::Large) {
                        group->addLargeWidget(splitButton);
                    } else {
                        group->addSmallWidget(splitButton);
                    }
                    continue;
                }

                QAction *boundAction = createBoundAction(
                    id,
                    item.value(QStringLiteral("params")).toString());
                if (!boundAction) continue;

                auto *button = new QRibbonButton(boundAction->icon(), boundAction->text(), size, group);
                bindQActionToButton(boundAction, button);
                group->addButton(button);
            }
        }
    }

    return true;
}

bool QRibbonHelper::buildAccessBar(QRibbonWidget *ribbonWidget)
{
    QVariantList items;
    const QVariantMap quickAccess = m_ribbonRoot.value(QStringLiteral("quickAccessBar")).toMap();
    if (!quickAccess.isEmpty()) {
        items = quickAccess.value(QStringLiteral("items")).toList();
    } else {
        items = m_ribbonRoot.value(QStringLiteral("accessBar")).toList();
    }

    for (const QVariant &value : items) {
        QString id;
        QString params;

        if (value.canConvert<QVariantMap>()) {
            const QVariantMap entry = value.toMap();
            id = entry.value(QStringLiteral("id")).toString();
            params = entry.value(QStringLiteral("params")).toString();
        } else {
            id = value.toString();
        }

        if (id.isEmpty()) continue;

        QAction *action = createBoundAction(id, params);
        if (!action) {
            qWarning() << "[RibbonLib] accessBar references non-existent action:" << id;
            continue;
        }
        ribbonWidget->addAccessBarAction(action);
    }

    return true;
}

bool QRibbonHelper::buildRibbon(QRibbonWidget *ribbonWidget)
{
    m_errorString.clear();
    if (!ribbonWidget) {
        return warn(QStringLiteral("buildRibbon failed: ribbonWidget is null"));
    }
    if (m_ribbonRoot.isEmpty()) {
        return warn(QStringLiteral("buildRibbon failed: ribbon.json is not loaded"));
    }

    m_ribbonWidget = ribbonWidget;

    return buildApplicationButton(ribbonWidget)
        && buildTabs(ribbonWidget)
        && buildAccessBar(ribbonWidget);
}

void QRibbonHelper::updateActionEnabled(bool enabled)
{
    QSet<QString> applicationActionIds;
    if (m_ribbonWidget && m_ribbonWidget->applicationButton()
        && m_ribbonWidget->applicationButton()->applicationMenu()) {
        for (QAction *action : m_ribbonWidget->applicationButton()->applicationMenu()->actions()) {
            if (action && !action->objectName().isEmpty()) {
                applicationActionIds.insert(action->objectName());
            }
        }
    }

    for (auto it = m_actions.begin(); it != m_actions.end(); ++it) {
        if (!applicationActionIds.contains(it.key()) && it.value()) {
            it.value()->setEnabled(enabled);
        }
    }
}
