#include "QRibbonHelper.h"
#include "QApplicationButton.h"
#include "QRibbonGroup.h"
#include "QRibbonMenu.h"
#include "QRibbonTab.h"
#include "QRibbonWidget.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeySequence>
#include <QStyle>
#include <QVariant>

namespace
{
QRibbonButtonSize toButtonSize(const QString &value)
{
    return value.trimmed().compare(QStringLiteral("small"), Qt::CaseInsensitive) == 0
        ? QRibbonButtonSize::Small
        : QRibbonButtonSize::Large;
}

QIcon qtStandardIcon(const QString &name)
{
    const QString key = name.trimmed().toLower();
    if (key.isEmpty()) return QIcon();

    static const QHash<QString, QStyle::StandardPixmap> icons {
        {QStringLiteral("file"), QStyle::SP_FileIcon},
        {QStringLiteral("folder"), QStyle::SP_DirIcon},
        {QStringLiteral("home"), QStyle::SP_DirHomeIcon},
        {QStringLiteral("open"), QStyle::SP_DialogOpenButton},
        {QStringLiteral("save"), QStyle::SP_DialogSaveButton},
        {QStringLiteral("close"), QStyle::SP_DialogCloseButton},
        {QStringLiteral("apply"), QStyle::SP_DialogApplyButton},
        {QStringLiteral("cancel"), QStyle::SP_DialogCancelButton},
        {QStringLiteral("reset"), QStyle::SP_DialogResetButton},
        {QStringLiteral("help"), QStyle::SP_DialogHelpButton},
        {QStringLiteral("info"), QStyle::SP_MessageBoxInformation},
        {QStringLiteral("warning"), QStyle::SP_MessageBoxWarning},
        {QStringLiteral("error"), QStyle::SP_MessageBoxCritical},
        {QStringLiteral("question"), QStyle::SP_MessageBoxQuestion},
        {QStringLiteral("back"), QStyle::SP_ArrowBack},
        {QStringLiteral("forward"), QStyle::SP_ArrowForward},
        {QStringLiteral("up"), QStyle::SP_ArrowUp},
        {QStringLiteral("down"), QStyle::SP_ArrowDown},
        {QStringLiteral("reload"), QStyle::SP_BrowserReload},
        {QStringLiteral("stop"), QStyle::SP_BrowserStop},
        {QStringLiteral("play"), QStyle::SP_MediaPlay},
        {QStringLiteral("pause"), QStyle::SP_MediaPause},
        {QStringLiteral("trash"), QStyle::SP_TrashIcon},
        {QStringLiteral("settings"), QStyle::SP_FileDialogDetailedView},
        {QStringLiteral("list"), QStyle::SP_FileDialogListView},
        {QStringLiteral("maximize"), QStyle::SP_TitleBarMaxButton},
        {QStringLiteral("sp_fileicon"), QStyle::SP_FileIcon},
        {QStringLiteral("sp_diricon"), QStyle::SP_DirIcon},
        {QStringLiteral("sp_dirhomeicon"), QStyle::SP_DirHomeIcon},
        {QStringLiteral("sp_dialogopenbutton"), QStyle::SP_DialogOpenButton},
        {QStringLiteral("sp_dialogsavebutton"), QStyle::SP_DialogSaveButton},
        {QStringLiteral("sp_dialogclosebutton"), QStyle::SP_DialogCloseButton},
        {QStringLiteral("sp_dialogapplybutton"), QStyle::SP_DialogApplyButton},
        {QStringLiteral("sp_dialogcancelbutton"), QStyle::SP_DialogCancelButton},
        {QStringLiteral("sp_dialogresetbutton"), QStyle::SP_DialogResetButton},
        {QStringLiteral("sp_dialoghelpbutton"), QStyle::SP_DialogHelpButton},
        {QStringLiteral("sp_messageboxinformation"), QStyle::SP_MessageBoxInformation},
        {QStringLiteral("sp_messageboxwarning"), QStyle::SP_MessageBoxWarning},
        {QStringLiteral("sp_messageboxcritical"), QStyle::SP_MessageBoxCritical},
        {QStringLiteral("sp_messageboxquestion"), QStyle::SP_MessageBoxQuestion},
        {QStringLiteral("sp_arrowback"), QStyle::SP_ArrowBack},
        {QStringLiteral("sp_arrowforward"), QStyle::SP_ArrowForward},
        {QStringLiteral("sp_arrowup"), QStyle::SP_ArrowUp},
        {QStringLiteral("sp_arrowdown"), QStyle::SP_ArrowDown},
        {QStringLiteral("sp_browserreload"), QStyle::SP_BrowserReload},
        {QStringLiteral("sp_browserstop"), QStyle::SP_BrowserStop},
        {QStringLiteral("sp_mediaplay"), QStyle::SP_MediaPlay},
        {QStringLiteral("sp_mediapause"), QStyle::SP_MediaPause},
        {QStringLiteral("sp_trashicon"), QStyle::SP_TrashIcon},
        {QStringLiteral("sp_filedialogdetailedview"), QStyle::SP_FileDialogDetailedView},
        {QStringLiteral("sp_filedialoglistview"), QStyle::SP_FileDialogListView},
        {QStringLiteral("sp_titlebarmaxbutton"), QStyle::SP_TitleBarMaxButton}
    };

    const auto it = icons.constFind(key);
    return it == icons.constEnd() ? QIcon() : QApplication::style()->standardIcon(it.value());
}
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
    if (path.startsWith(QStringLiteral(":/")) || QFileInfo(path).isAbsolute()) return path;
    return m_resourceBaseDir.isEmpty() ? path : QDir(m_resourceBaseDir).filePath(path);
}

QAction *QRibbonHelper::ensureAction(const QString &id)
{
    if (id.isEmpty()) return nullptr;
    if (QAction *existing = m_actions.value(id, nullptr)) return existing;

    auto *action = new QAction(this);
    action->setObjectName(id);
    action->setText(id);
    connect(action, &QAction::triggered, this, [this, id]() { emit actionTriggered(id); });
    m_actions.insert(id, action);
    return action;
}

void QRibbonHelper::clearActions()
{
    qDeleteAll(m_actions);
    m_actions.clear();
}

bool QRibbonHelper::loadActions(const QString &actionsResource)
{
    m_errorString.clear();
    clearActions();

    const QFileInfo info(actionsResource);
    m_resourceBaseDir = actionsResource.startsWith(QStringLiteral(":/"))
        ? info.path()
        : info.absolutePath();

    QFile file(actionsResource);
    if (!file.open(QIODevice::ReadOnly)) {
        return warn(QStringLiteral("Failed to open actions JSON: %1").arg(actionsResource));
    }
    return parseActionsJson(file.readAll());
}

bool QRibbonHelper::loadLayout(const QString &layoutResource)
{
    m_errorString.clear();
    m_layoutRoot.clear();

    QFile file(layoutResource);
    if (!file.open(QIODevice::ReadOnly)) {
        return warn(QStringLiteral("Failed to open Ribbon layout JSON: %1").arg(layoutResource));
    }
    return parseLayoutJson(file.readAll());
}

bool QRibbonHelper::parseActionsJson(const QByteArray &bytes)
{
    QJsonParseError error {};
    const QJsonDocument document = QJsonDocument::fromJson(bytes, &error);
    if (error.error != QJsonParseError::NoError || !document.isObject()) {
        return warn(QStringLiteral("Failed to parse actions JSON: %1").arg(error.errorString()));
    }

    const QJsonArray definitions = document.object().value(QStringLiteral("actions")).toArray();
    for (const QJsonValue &value : definitions) {
        const QJsonObject definition = value.toObject();
        const QString id = definition.value(QStringLiteral("id")).toString().trimmed();
        if (id.isEmpty()) continue;

        QAction *action = ensureAction(id);
        action->setText(definition.value(QStringLiteral("name")).toString(id));
        action->setToolTip(definition.value(QStringLiteral("description")).toString());

        QString iconPath = definition.value(QStringLiteral("icon")).toString().trimmed();
        QString standardIcon = definition.value(QStringLiteral("standardIcon")).toString().trimmed();
        if (iconPath.startsWith(QStringLiteral("qt:"), Qt::CaseInsensitive)) {
            standardIcon = iconPath.mid(3);
            iconPath.clear();
        }

        QIcon icon;
        if (!iconPath.isEmpty()) {
            const QString resolved = resolveResourcePath(iconPath);
            if (QFile::exists(resolved)) icon = QIcon(resolved);
        }
        if (icon.isNull() && !standardIcon.isEmpty()) icon = qtStandardIcon(standardIcon);
        if (!icon.isNull()) action->setIcon(icon);

        const QString shortcut = definition.value(QStringLiteral("shortcut")).toString();
        if (!shortcut.isEmpty()) action->setShortcut(QKeySequence(shortcut));
        if (definition.contains(QStringLiteral("enabled"))) action->setEnabled(definition.value(QStringLiteral("enabled")).toBool(true));
        if (definition.contains(QStringLiteral("visible"))) action->setVisible(definition.value(QStringLiteral("visible")).toBool(true));
        if (definition.contains(QStringLiteral("checkable"))) action->setCheckable(definition.value(QStringLiteral("checkable")).toBool(false));
        if (definition.contains(QStringLiteral("checked"))) {
            action->setChecked(action->isCheckable() && definition.value(QStringLiteral("checked")).toBool(false));
        }
    }
    return true;
}

bool QRibbonHelper::parseLayoutJson(const QByteArray &bytes)
{
    QJsonParseError error {};
    const QJsonDocument document = QJsonDocument::fromJson(bytes, &error);
    if (error.error != QJsonParseError::NoError || !document.isObject()) {
        return warn(QStringLiteral("Failed to parse Ribbon layout JSON: %1").arg(error.errorString()));
    }

    const QJsonObject ribbon = document.object().value(QStringLiteral("ribbon")).toObject();
    if (ribbon.isEmpty()) return warn(QStringLiteral("Ribbon layout JSON is missing the ribbon node"));
    m_layoutRoot = ribbon.toVariantMap();
    return true;
}

bool QRibbonHelper::buildApplicationButton(QRibbonWidget *ribbonWidget)
{
    const QVariantMap definition = m_layoutRoot.value(QStringLiteral("applicationButton")).toMap();
    if (definition.isEmpty()) return true;

    QApplicationButton *button = ribbonWidget->applicationButton();
    if (!button) {
        button = new QApplicationButton(ribbonWidget);
        ribbonWidget->setApplicationButton(button);
    }

    const QString title = definition.value(QStringLiteral("title")).toString();
    if (!title.isEmpty()) button->setText(title);

    auto *menu = new QRibbonMenu(button);
    const QVariantList items = definition.value(QStringLiteral("menu")).toList();
    for (const QVariant &item : items) {
        if (item.toString() == QStringLiteral("-")) {
            menu->addSeparator();
            continue;
        }
        const QString id = item.toMap().value(QStringLiteral("id")).toString();
        if (QAction *action = m_actions.value(id, nullptr)) menu->addAction(action);
    }
    button->setApplicationMenu(menu);
    return true;
}

bool QRibbonHelper::buildTabs(QRibbonWidget *ribbonWidget)
{
    const QVariantList tabs = m_layoutRoot.value(QStringLiteral("tabs")).toList();
    for (const QVariant &tabValue : tabs) {
        const QVariantMap tabDefinition = tabValue.toMap();
        const QString title = tabDefinition.value(QStringLiteral("title")).toString();
        if (title.isEmpty()) continue;

        QRibbonTab *tab = ribbonWidget->addTab(title, tabDefinition.value(QStringLiteral("id")).toString());
        const QVariantList groups = tabDefinition.value(QStringLiteral("panels")).toList();
        for (const QVariant &groupValue : groups) {
            const QVariantMap groupDefinition = groupValue.toMap();
            const QString groupTitle = groupDefinition.value(QStringLiteral("title")).toString();
            if (groupTitle.isEmpty()) continue;

            QRibbonGroup *group = tab->addGroup(groupTitle);
            const QVariantList items = groupDefinition.value(QStringLiteral("items")).toList();
            for (const QVariant &itemValue : items) {
                const QVariantMap item = itemValue.toMap();
                QAction *action = m_actions.value(item.value(QStringLiteral("id")).toString(), nullptr);
                if (!action) continue;

                const QRibbonButtonSize size = toButtonSize(item.value(QStringLiteral("style")).toString());
                const QString displayText = item.value(QStringLiteral("displayText")).toString();
                const QVariantList menuItems = item.value(QStringLiteral("menu")).toList();

                if (!menuItems.isEmpty()) {
                    auto *menu = new QRibbonMenu(group);
                    for (const QVariant &menuValue : menuItems) {
                        if (menuValue.toString() == QStringLiteral("-")) {
                            menu->addSeparator();
                            continue;
                        }
                        const QString menuId = menuValue.toMap().value(QStringLiteral("id")).toString();
                        if (QAction *menuAction = m_actions.value(menuId, nullptr)) menu->addAction(menuAction);
                    }
                    if (displayText.isEmpty()) group->addSplitAction(action, menu, size);
                    else group->addSplitAction(action, menu, size, displayText);
                } else if (displayText.isEmpty()) {
                    group->addAction(action, size);
                } else {
                    group->addAction(action, size, displayText);
                }
            }
        }
    }
    return true;
}

bool QRibbonHelper::buildAccessBar(QRibbonWidget *ribbonWidget)
{
    const QVariantMap definition = m_layoutRoot.value(QStringLiteral("quickAccessBar")).toMap();
    const QVariantList items = definition.value(QStringLiteral("items")).toList();
    for (const QVariant &value : items) {
        const QString id = value.canConvert<QVariantMap>()
            ? value.toMap().value(QStringLiteral("id")).toString()
            : value.toString();
        if (QAction *action = m_actions.value(id, nullptr)) ribbonWidget->addAccessBarAction(action);
    }
    return true;
}

bool QRibbonHelper::buildRibbon(QRibbonWidget *ribbonWidget)
{
    if (!ribbonWidget) return warn(QStringLiteral("Ribbon widget is null"));
    if (m_layoutRoot.isEmpty()) return warn(QStringLiteral("No Ribbon layout has been loaded"));
    if (!buildApplicationButton(ribbonWidget)) return false;
    if (!buildAccessBar(ribbonWidget)) return false;
    return buildTabs(ribbonWidget);
}
