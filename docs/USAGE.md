# RibbonLib Detailed Usage Guide

[README](../README.md) | [中文文档](./USAGE.zh-CN.md)

RibbonLib is a focused Qt Widgets Ribbon library. The recommended model is **QAction first**: application code owns commands and business state; RibbonLib owns Ribbon presentation, layout, geometry and local styling.

## 1. Integration

```cmake
add_subdirectory(thirdparty/RibbonLib)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

or:

```cmake
find_package(RibbonLib CONFIG REQUIRED)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

Use the umbrella header:

```cpp
#include <RibbonLib.h>
```

Consumers do not copy RibbonLib QSS and do not apply RibbonLib styles to the whole `QApplication`.

## 2. QAction-first construction

Define application commands once:

```cpp
auto *openAction = new QAction(openIcon, tr("Open"), this);
openAction->setShortcut(QKeySequence::Open);
connect(openAction, &QAction::triggered,
        this, &MainWindow::openDocument);
```

Describe Ribbon structure separately:

```cpp
auto *ribbon = new QRibbonWidget(this);
setMenuWidget(ribbon);

auto *home = ribbon->addTab(tr("Home"), "home");
auto *document = home->addGroup(tr("Document"));
document->addAction(openAction, QRibbonButtonSize::Large);
document->addActions({saveAction, closeAction}, QRibbonButtonSize::Small);
```

Or build a compact group directly:

```cpp
home->addGroup(tr("Edit"),
               {undoAction, redoAction, findAction},
               QRibbonButtonSize::Small);
```

The bound Ribbon button synchronizes `text`, `icon`, `enabled`, `visible`, `checkable`, `checked`, `shortcut`, `toolTip` and `statusTip` from the QAction. Triggering the Ribbon button triggers the same QAction.

## 3. Ribbon-only display text

`QAction::text()` should remain a clean command name because the same QAction may appear in menus, Quick Access, context menus and other Qt UI.

Ribbon-specific text can be overridden without changing the QAction:

```cpp
auto *button = group->addAction(settingsAction,
                                QRibbonButtonSize::Large);
button->setDisplayText(tr("Model\nSettings"));
```

Equivalent convenience form:

```cpp
group->addAction(settingsAction,
                 QRibbonButtonSize::Large,
                 tr("Model\nSettings"));
```

Use `clearDisplayText()` to return to `QAction::text()`.

## 4. Large text rule: explicit breaks only

Large buttons no longer perform automatic wrapping.

Rules:

- no `\n` -> one visual line;
- one explicit `\n` -> two visual lines;
- additional line breaks are normalized into spaces;
- Small buttons remain single-line;
- long one-line Large text increases button width instead of being wrapped automatically;
- use Ribbon-only display text when a manually controlled two-line label is desired.

This makes layout deterministic across languages, fonts and DPI settings.

## 5. Width calculation

Ribbon buttons have no fixed minimum width.

Normal button width is calculated from:

- icon width when present;
- actual display-text width;
- icon/text spacing when both are present;
- horizontal padding.

Split buttons additionally reserve the dropdown-arrow region.

This means short commands stay compact and long commands receive the width they actually need. A `QRibbonGroup` may still become wider than its content when required to display the group title.

## 6. Split Button QAction-first

Create a menu from existing actions:

```cpp
auto *menu = new QRibbonMenu(group);
menu->addAction(createPlateAction);
menu->addAction(createBeamAction);
menu->addAction(createColumnAction);
```

Then create the split button directly from the default QAction:

```cpp
auto *split = group->addSplitAction(createPlateAction,
                                    menu,
                                    QRibbonButtonSize::Large,
                                    tr("Create\nMember"));
```

Or construct the widget directly:

```cpp
auto *split = new QRibbonSplitButton(createPlateAction,
                                     menu,
                                     QRibbonButtonSize::Large,
                                     group);
```

The primary area triggers `defaultAction()`. The dropdown opens the supplied menu. When the default action changes, icon/state are synchronized. Ribbon-only display text remains independent from `QAction::text()`.

`setDefaultAction()` no longer requires the menu to be set first, so command binding and menu assignment are independent.

## 7. Large and Small layout

Small controls are placed in deterministic three-row columns. Large controls occupy the full command height. Height, icon size and vertical metrics remain library-controlled; only width follows actual content.

Use Large for visually important commands and Small for compact command sets. Do not use Large merely to force a wider control.

## 8. Checkable state

Prefer QAction as the single state source:

```cpp
auto *wireframe = new QAction(wireframeIcon, tr("Wireframe"), this);
wireframe->setCheckable(true);
wireframe->setChecked(false);

group->addAction(wireframe, QRibbonButtonSize::Small);
```

Changing QAction state from a menu, shortcut or business layer updates the Ribbon presentation automatically.

## 9. Quick Access

```cpp
ribbon->addAccessBarAction(saveAction);
ribbon->addAccessBarAction(undoAction);
ribbon->addAccessBarAction(redoAction);
```

Actions with icons are shown icon-only. Text-only actions remain text-only.

## 10. Application Menu

```cpp
auto *menu = new QRibbonMenu(ribbon);
menu->addAction(openAction);
menu->addAction(saveAction);
ribbon->applicationButton()->setApplicationMenu(menu);
```

The same QAction instances can be used in Ribbon groups, Application Menu and Quick Access.

## 11. Custom widgets

`addWidget`, `addLargeWidget` and `addSmallWidget` remain available for non-command controls such as combo boxes and spin boxes. For normal commands, prefer `addAction()` / `addActions()`.

## 12. JSON

JSON-driven construction remains supported through `QRibbonHelper`, but it is optional. Direct C++ + QAction construction is simpler when the host already owns a command layer.

```cpp
QRibbonHelper helper;
if (!helper.loadFromResources(ribbonPath, actionsPath)) {
    qWarning() << helper.errorString();
    return;
}
helper.buildRibbon(ribbon);
```

## 13. RibbonAction

`RibbonAction` remains optional. Use it only when its command abstraction matches the host architecture. Applications that already use QAction or another command system do not need to adopt it.

## 14. Styling and DPI

RibbonLib owns its Ribbon-scoped QSS and deterministic vertical metrics. Consumers should not copy the QSS, manually tune button heights, or compensate for 125% / 150% scaling in host code.

## 15. Migration

Old manual code:

```cpp
auto *button = new QRibbonButton(icon, tr("Open"), QRibbonButtonSize::Large, group);
connect(button, &QRibbonButton::clicked, this, &MainWindow::openDocument);
group->addButton(button);
```

Preferred code:

```cpp
auto *openAction = new QAction(icon, tr("Open"), this);
connect(openAction, &QAction::triggered, this, &MainWindow::openDocument);
group->addAction(openAction, QRibbonButtonSize::Large);
```

If Ribbon needs an intentional two-line label, use `setDisplayText()` rather than inserting a newline into `QAction::text()`.

## 16. Scope

RibbonLib stays deliberately small: no Dock framework, application command bus, plugin framework, MVVM framework or global theme engine.
