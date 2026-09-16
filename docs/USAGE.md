# RibbonLib Detailed Usage Guide

[README](../README.md) | [中文文档](./USAGE.zh-CN.md)

This document is the stable integration reference for RibbonLib. The recommended model is **QAction first**: application code owns commands and command state; RibbonLib owns Ribbon presentation, layout, local styling and DPI behavior.

## 1. Design model

RibbonLib intentionally stays small.

Responsibilities are divided as follows:

- the host owns business logic, documents, viewports and command execution;
- `QAction` is the preferred public command model;
- RibbonLib owns Ribbon tabs, groups, buttons, state presentation and layout;
- the same `QAction` may be reused in Ribbon, menus, toolbars and Quick Access;
- command state must not be duplicated across UI controls;
- Ribbon styling remains local and must not require a global application stylesheet;
- only `Large` and `Small` button sizes are part of the public size model;
- direct C++ construction is recommended; JSON is optional.

The intended result is that normal host code describes only commands and grouping.

## 2. Requirements

- CMake 3.16+
- C++17
- Qt 5.14.2+ or Qt 6
- Qt Widgets
- Windows or Linux

Static linking is the default; shared builds are supported.

## 3. Integration

### 3.1 Source dependency

```cmake
add_subdirectory(thirdparty/RibbonLib)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

Use the umbrella header:

```cpp
#include <RibbonLib.h>
```

### 3.2 Installed package

```bash
./build.sh Release --install
```

```cmake
find_package(RibbonLib CONFIG REQUIRED)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

No external Ribbon stylesheet is required in either mode.

## 4. Recommended QAction-first construction

### 4.1 Define commands once

```cpp
auto *openAction = new QAction(openIcon, tr("Open"), this);
openAction->setShortcut(QKeySequence::Open);
openAction->setToolTip(tr("Open a document"));
connect(openAction, &QAction::triggered,
        this, &MainWindow::openDocument);

auto *wireframeAction = new QAction(wireframeIcon, tr("Wireframe"), this);
wireframeAction->setCheckable(true);
wireframeAction->setChecked(false);
connect(wireframeAction, &QAction::toggled,
        this, &MainWindow::setWireframe);
```

### 4.2 Describe Ribbon structure

```cpp
auto *ribbon = new QRibbonWidget(this);
setMenuWidget(ribbon);

auto *home = ribbon->addTab(tr("Home"), "home");

home->addGroup(tr("Document"), {
    openAction,
    saveAction,
    closeAction
});

home->addGroup(tr("View"), {
    wireframeAction,
    axesAction,
    perspectiveAction
});
```

`QRibbonTab::addGroup(title, actions)` creates the group and adds the supplied actions as Small buttons by default.

### 4.3 Choose button sizes explicitly when needed

```cpp
auto *create = home->addGroup(tr("Create"));

create->addAction(newAction, QRibbonButtonSize::Large);
create->addActions({lineAction, circleAction, arcAction},
                   QRibbonButtonSize::Small);
```

This should be the normal construction style for production applications.

## 5. QAction binding behavior

A `QRibbonButton` bound to a `QAction` automatically synchronizes:

- `text`;
- `icon`;
- `enabled`;
- `visible`;
- `checkable`;
- `checked`;
- `shortcut`;
- `toolTip`;
- `statusTip`.

Example:

```cpp
auto *button = group->addAction(wireframeAction,
                                QRibbonButtonSize::Small);
```

Changing the QAction later updates the Ribbon button:

```cpp
wireframeAction->setEnabled(false);
wireframeAction->setChecked(true);
wireframeAction->setText(tr("Wireframe View"));
```

Clicking the Ribbon button calls `QAction::trigger()`.

This allows one action to be reused safely:

```cpp
group->addAction(saveAction, QRibbonButtonSize::Large);
ribbon->addAccessBarAction(saveAction);
fileMenu->addAction(saveAction);
```

Do not manually mirror `enabled`, `checked` or `visible` state into each control when the controls share the same QAction.

## 6. Public controls

### 6.1 `QRibbonWidget`

Main Ribbon container:

```cpp
QRibbonTab *addTab(const QString &title,
                   const QString &id = QString());
void removeTab(int index);
void removeTab(QRibbonTab *tab);
void setCurrentTab(int index);
bool setCurrentTab(const QString &id);
QRibbonTab *currentTab() const;
int indexOfTabId(const QString &id) const;
int tabCount() const;
int currentIndex() const;
```

It also owns the Application button, Quick Access bar and contextual tab presentation.

### 6.2 `QRibbonTab`

Basic form:

```cpp
auto *home = ribbon->addTab(tr("Home"), "home");
auto *document = home->addGroup(tr("Document"));
```

Shortcut form:

```cpp
home->addGroup(tr("Document"), {
    openAction,
    saveAction,
    closeAction
});
```

With explicit size:

```cpp
home->addGroup(tr("Create"),
               {boxAction, cylinderAction, sphereAction},
               QRibbonButtonSize::Large);
```

Use stable tab IDs whenever tabs are selected or restored programmatically.

### 6.3 `QRibbonGroup`

Recommended command APIs:

```cpp
QRibbonButton *addAction(QAction *action,
                         QRibbonButtonSize size = QRibbonButtonSize::Large);
void addActions(const QList<QAction*> &actions,
                QRibbonButtonSize size = QRibbonButtonSize::Small);
```

Lower-level content APIs remain available:

```cpp
group->addButton(button);
group->addSeparator();
group->addWidget(widget);
group->addLargeWidget(widget);
group->addSmallWidget(widget);
```

Use the low-level APIs for custom controls, not for ordinary application commands that already have a QAction.

### 6.4 `QRibbonButton`

Manual construction:

```cpp
auto *button = new QRibbonButton(icon,
                                 tr("Custom"),
                                 QRibbonButtonSize::Large,
                                 group);
```

QAction-bound construction:

```cpp
auto *button = new QRibbonButton(saveAction,
                                 QRibbonButtonSize::Large,
                                 group);
```

or bind later:

```cpp
button->setDefaultAction(saveAction);
```

The current bound action is available through:

```cpp
QAction *action = button->defaultAction();
```

When no QAction is bound, the existing manual state API remains valid:

```cpp
button->setEnabled(true);
button->setCheckable(true);
button->setChecked(true);
button->setShortcut(QKeySequence("Ctrl+S"));
```

For normal application commands, prefer QAction binding.

## 7. Large and Small layout

### 7.1 Small buttons

Small controls are placed in deterministic three-row columns. RibbonLib owns row height, spacing and border clearance.

Use Small for compact, frequently used commands with one-line labels.

### 7.2 Large buttons

Large buttons reserve space for:

1. icon area;
2. up to two lines of text;
3. checked/hover/pressed border clearance.

The host should not manually set button height, icon size or margins.

### 7.3 Large text wrapping

Rules:

- maximum visual lines: two;
- the first explicit `\n` is honored;
- later explicit line breaks are converted to spaces;
- without explicit line breaks, automatic wrapping may be used;
- wrapping prefers word boundaries and may fall back to character boundaries;
- labels are not shortened with `...`;
- the same rules are used by Large split buttons.

## 8. Checkable commands

Preferred form:

```cpp
auto *axesAction = new QAction(axesIcon, tr("Axes"), this);
axesAction->setCheckable(true);
axesAction->setChecked(true);

group->addAction(axesAction, QRibbonButtonSize::Small);
```

Later state changes are automatic:

```cpp
axesAction->setChecked(false);
axesAction->setEnabled(document != nullptr);
```

Do not manually call both `action->setChecked()` and `button->setChecked()` for the same bound command.

## 9. Quick Access bar

```cpp
ribbon->addAccessBarAction(saveAction);
ribbon->addAccessBarAction(undoAction);
ribbon->addAccessBarAction(redoAction);
```

Presentation policy:

- icon available -> icon-only Quick Access button;
- no icon -> text-only button.

The action still owns shortcut and state.

## 10. Split buttons and menus

```cpp
auto *menu = new QRibbonMenu(this);
auto *optionA = menu->addAction(tr("Option A"));
auto *optionB = menu->addAction(tr("Option B"));

menu->setDefaultAction(optionA);

auto *split = new QRibbonSplitButton(createIcon,
                                     tr("Create"),
                                     QRibbonButtonSize::Large,
                                     group);
split->setMenu(menu);
split->setDefaultAction(optionA);
group->addLargeWidget(split);
```

The main area triggers the default action; the arrow opens the menu. Default-action icon/text/state are synchronized from QAction.

Use `QRibbonMenu` for Ribbon-owned menus so style remains isolated.

## 11. Application button

```cpp
auto *menu = new QRibbonMenu(this);
menu->addAction(openAction);
menu->addAction(saveAction);
menu->addSeparator();
menu->addAction(exitAction);

ribbon->applicationButton()->setText(tr("File"));
ribbon->applicationButton()->setApplicationMenu(menu);
```

Reuse existing application QActions instead of creating duplicate menu-only commands.

## 12. Context tabs

```cpp
const int index = ribbon->indexOfTabId("tools");
ribbon->setTabContext(index, tr("Tools"), QColor("#0078d4"));
```

Clear with:

```cpp
ribbon->clearTabContext(index);
```

RibbonLib controls presentation only; the host decides when a contextual tab is shown or selected.

## 13. Custom widgets

```cpp
auto *combo = new QComboBox(group);
combo->addItems({"A", "B", "C"});
group->addSmallWidget(combo);
```

Guidelines:

- use custom widgets only when a QAction-backed command is not sufficient;
- use Qt size policies rather than fixed host geometry;
- keep custom styling local;
- let RibbonLib determine the vertical slot geometry.

## 14. JSON construction

JSON is optional. It remains useful when layout is configuration-driven or translated layouts are loaded as resources.

```cpp
QRibbonHelper helper;

if (!helper.loadFromResources(ribbonPath, actionsPath)) {
    qWarning() << helper.errorString();
    return;
}

if (!helper.buildRibbon(ribbon)) {
    qWarning() << helper.errorString();
    return;
}
```

`actions.json` describes command metadata; `ribbon.json` describes placement.

Minimal action:

```json
{
  "actions": [
    {
      "id": "file.open",
      "name": "Open",
      "description": "Open a document",
      "icon": "qt:open",
      "shortcut": "Ctrl+O",
      "enabled": true,
      "visible": true,
      "checkable": false,
      "checked": false
    }
  ]
}
```

Minimal placement:

```json
{
  "ribbon": {
    "tabs": [
      {
        "id": "home",
        "title": "Home",
        "panels": [
          {
            "title": "Document",
            "items": [
              { "id": "file.open", "style": "large" }
            ]
          }
        ]
      }
    ]
  }
}
```

Supported placement sizes intentionally remain `large` and `small` only.

### When to use JSON

Use JSON when:

- layout must be externalized;
- language-specific resource layouts are required;
- the host already uses `QRibbonHelper` successfully.

Prefer direct QAction C++ construction when:

- the application already owns QAction objects;
- the Ribbon structure is known at compile time;
- minimal integration code is the priority.

## 15. `RibbonAction`

`RibbonAction` remains optional. It is useful when the host wants a framework-neutral command object that `QRibbonHelper` can bind to QAction.

It should not be introduced merely to use RibbonLib. If the application already has QAction or its own command layer, keep that architecture.

## 16. Styling and DPI

RibbonLib embeds its default Ribbon stylesheet and applies it locally.

Consumers should not:

- copy `ribbon.qss` into the application;
- call `Q_INIT_RESOURCE()` for RibbonLib resources;
- set RibbonLib QSS on `QApplication`;
- duplicate Ribbon metrics in application code;
- add special 125% or 150% button-height fixes.

If the host has a global application theme, test coexistence, but keep Ribbon-specific fixes inside RibbonLib rather than per consumer.

## 17. Migration from manual buttons

Old style:

```cpp
auto *button = new QRibbonButton(saveIcon,
                                 tr("Save"),
                                 QRibbonButtonSize::Large,
                                 group);
button->setEnabled(canSave);
connect(button, &QRibbonButton::clicked,
        this, &MainWindow::saveDocument);
group->addButton(button);
```

Recommended style:

```cpp
auto *saveAction = new QAction(saveIcon, tr("Save"), this);
connect(saveAction, &QAction::triggered,
        this, &MainWindow::saveDocument);

group->addAction(saveAction, QRibbonButtonSize::Large);
```

State update becomes:

```cpp
saveAction->setEnabled(canSave);
```

If the same command also appears in a menu or Quick Access bar, reuse the same QAction.

## 18. Build and tests

Linux:

```bash
./build.sh
./build.sh Release --tests
./build.sh Release --run
```

Windows:

```powershell
.\build.ps1
.\build.ps1 -Tests
.\build.ps1 -Run
```

Standard CMake:

```bash
cmake -S . -B build -DRIBBONLIB_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Important options:

- `RIBBONLIB_BUILD_EXAMPLE`
- `RIBBONLIB_BUILD_TESTS`
- `RIBBONLIB_BUILD_SHARED`
- `RIBBONLIB_ENABLE_INSTALL`

## 19. Recommended production rules

- Use QAction as the command-state source when possible.
- Let Ribbon code describe structure, not business logic.
- Reuse one QAction across Ribbon/Menu/Quick Access.
- Use `Large` and `Small`; do not create additional size categories in host code.
- Keep JSON optional.
- Keep Ribbon-specific styling inside RibbonLib.
- Use custom widgets only for controls that cannot be represented by QAction buttons.
- Do not add application architecture into RibbonLib.

Following these rules keeps integration small and makes RibbonLib easier to upgrade across multiple applications.
