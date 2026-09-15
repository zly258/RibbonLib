#include <QApplication>
#include <QFile>
#include <QFontMetrics>
#include <QMenu>
#include <QMetaObject>
#include <QPushButton>
#include <QTemporaryDir>
#include <QToolButton>
#include <QStringList>

#include "RibbonLib.h"
#include "QRibbonMetrics.h"
#include "QRibbonTextLayout.h"

class TestAction final : public RibbonAction
{
public:
    explicit TestAction(QObject *parent = nullptr)
        : RibbonAction(parent)
    {
        setName(QStringLiteral("Test"));
    }

    QString id() const override { return QStringLiteral("test.action"); }

    void execute(const QString &params) override
    {
        calls.append(params);
    }

    void cleanup() override {}

    QStringList calls;
};

static bool writeFile(const QString &path, const QByteArray &content)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) return false;
    return file.write(content) == content.size();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setStyleSheet(QStringLiteral(
        "QPushButton#HostButton { background: rgb(1, 2, 3); }"));
    const QString hostStyle = app.styleSheet();

    QPushButton hostButton;
    hostButton.setObjectName(QStringLiteral("HostButton"));

    QMenu hostMenu;
    QRibbonMenu ribbonMenu;
    QRibbonWidget ribbon;

    if (app.styleSheet() != hostStyle) return 1;
    if (ribbon.styleSheet().isEmpty()) return 2;
    if (!hostMenu.styleSheet().isEmpty()) return 3;
    if (ribbonMenu.styleSheet().isEmpty()) return 4;

    ribbon.addTab(QStringLiteral("Home"), QStringLiteral("home"));
    ribbon.addTab(QStringLiteral("View"), QStringLiteral("view"));

    if (!ribbon.setCurrentTab(QStringLiteral("view"))) return 5;
    if (!ribbon.currentTab() || ribbon.currentTab()->id() != QStringLiteral("view")) return 6;
    if (ribbon.indexOfTabId(QStringLiteral("home")) != 0) return 7;
    if (ribbon.indexOfTabId(QStringLiteral("view")) != 1) return 8;

    QTemporaryDir directory;
    if (!directory.isValid()) return 9;

    const QString actionsPath = directory.filePath(QStringLiteral("actions.json"));
    const QString ribbonPath = directory.filePath(QStringLiteral("ribbon.json"));

    const QByteArray actionsJson = R"json(
{
  "actions": [
    { "id": "test.action", "name": "Test" },
    { "id": "test.icon", "name": "Icon", "icon": "missing.svg", "standardIcon": "open" },
    { "id": "test.check.top", "name": "Top", "icon": "qt:apply", "checkable": true },
    { "id": "test.check.middle", "name": "Middle", "icon": "qt:info", "checkable": true },
    { "id": "test.check.bottom", "name": "Bottom", "icon": "qt:warning", "checkable": true, "checked": true }
  ]
}
)json";

    const QByteArray ribbonJson = R"json(
{
  "ribbon": {
    "quickAccessBar": {
      "items": [
        { "id": "test.icon" },
        { "id": "test.action" }
      ]
    },
    "tabs": [
      {
        "id": "home",
        "title": "Home",
        "panels": [
          {
            "title": "Test",
            "items": [
              { "id": "test.action", "style": "small", "params": "first" },
              { "id": "test.action", "style": "small", "params": "second" }
            ]
          },
          {
            "title": "Checkable",
            "items": [
              { "id": "test.check.top", "style": "small" },
              { "id": "test.check.middle", "style": "small" },
              { "id": "test.check.bottom", "style": "small" }
            ]
          }
        ]
      }
    ]
  }
}
)json";

    if (!writeFile(actionsPath, actionsJson) || !writeFile(ribbonPath, ribbonJson)) return 10;

    QRibbonHelper helper;
    TestAction action;
    helper.registerRibbonAction(&action);

    if (!helper.loadFromResources(ribbonPath, actionsPath)) return 11;
    if (!helper.action(QStringLiteral("test.icon"))
        || helper.action(QStringLiteral("test.icon"))->icon().isNull()) return 17;

    QRibbonWidget jsonRibbon;
    if (!helper.buildRibbon(&jsonRibbon)) return 12;

    const QList<QRibbonButton*> buttons = jsonRibbon.findChildren<QRibbonButton*>();
    if (buttons.size() != 5) return 13;

    QList<QRibbonButton*> actionButtons;
    QRibbonButton *topCheckable = nullptr;
    QRibbonButton *bottomCheckable = nullptr;
    for (QRibbonButton *button : buttons) {
        if (!button) continue;
        if (button->text() == QStringLiteral("Test")) {
            actionButtons.append(button);
        } else if (button->text() == QStringLiteral("Top")) {
            topCheckable = button;
        } else if (button->text() == QStringLiteral("Bottom")) {
            bottomCheckable = button;
        }
    }

    if (actionButtons.size() != 2 || !topCheckable || !bottomCheckable) return 20;
    if (!bottomCheckable->isCheckable() || !bottomCheckable->isChecked()) return 21;

    jsonRibbon.resize(800, jsonRibbon.sizeHint().height());
    jsonRibbon.show();
    app.processEvents();

    QWidget *topColumn = topCheckable->parentWidget();
    QWidget *bottomColumn = bottomCheckable->parentWidget();
    if (!topColumn || !bottomColumn || topColumn != bottomColumn) return 22;

    const int topClearance = topCheckable->geometry().top() - topColumn->rect().top();
    const int bottomClearance = bottomColumn->rect().bottom() - bottomCheckable->geometry().bottom();
    if (topClearance < QRibbonMetrics::SmallColumnVerticalPadding) return 23;
    if (bottomClearance < QRibbonMetrics::SmallColumnVerticalPadding) return 24;

    const QList<QToolButton*> accessButtons =
        jsonRibbon.accessBarWidget()->findChildren<QToolButton*>(QString(), Qt::FindDirectChildrenOnly);
    if (accessButtons.size() != 2) return 18;

    bool foundIconAccess = false;
    bool foundTextAccess = false;
    for (QToolButton *button : accessButtons) {
        if (!button || !button->defaultAction()) continue;
        const QString id = button->defaultAction()->objectName();
        if (id == QStringLiteral("test.icon")) {
            foundIconAccess = button->toolButtonStyle() == Qt::ToolButtonIconOnly;
        } else if (id == QStringLiteral("test.action")) {
            foundTextAccess = button->toolButtonStyle() == Qt::ToolButtonTextOnly;
        }
    }
    if (!foundIconAccess || !foundTextAccess) return 19;

    QMetaObject::invokeMethod(actionButtons[0], "clicked", Qt::DirectConnection);
    QMetaObject::invokeMethod(actionButtons[1], "clicked", Qt::DirectConnection);

    if (action.calls.size() != 2) return 14;
    if (action.calls[0] != QStringLiteral("first")) return 15;
    if (action.calls[1] != QStringLiteral("second")) return 16;

    QMetaObject::invokeMethod(bottomCheckable, "clicked", Qt::DirectConnection);
    QAction *bottomAction = helper.action(QStringLiteral("test.check.bottom"));
    if (!bottomAction || bottomAction->isChecked() || bottomCheckable->isChecked()) return 25;

    const QString longLabel = QStringLiteral("Long Large Button Label");
    QRibbonButton largeButton(QIcon(), longLabel, QRibbonButtonSize::Large);
    const QSize largeHint = largeButton.sizeHint();
    const int availableTextWidth = largeHint.width() - QRibbonMetrics::ButtonSidePadding * 2;
    if (QRibbonTextLayout::wrappedLineCount(largeButton.font(), longLabel, availableTextWidth)
        > QRibbonTextLayout::LargeTextLineCount) return 26;
    if (largeHint.height() != QRibbonMetrics::LargeButtonHeight) return 27;

    const int singleLineWidth = QFontMetrics(largeButton.font()).horizontalAdvance(longLabel)
        + QRibbonMetrics::LargeButtonHPadding * 2;
    if (largeHint.width() >= singleLineWidth) return 28;

    return 0;
}
