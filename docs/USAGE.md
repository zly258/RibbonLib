# RibbonLib Detailed Usage Guide

[README](../README.md) | [中文文档](./USAGE.zh-CN.md)

This document describes the supported integration model and the layout conventions of RibbonLib. It is intended to be the stable reference for application developers. The example application is useful for visual verification, but production code should rely on the public API and the rules documented here.

## 1. Design goals

RibbonLib is intentionally small. It provides a Ribbon control layer, not a complete application framework.

The library is designed around the following rules:

- the host owns business logic, documents, viewports and application state;
- RibbonLib owns Ribbon layout, command presentation and local Ribbon styling;
- the host may build the Ribbon directly in C++ or from JSON;
- Ribbon-specific QSS stays local and must not require a global `QApplication` stylesheet;
- the public size model stays simple: `Large` and `Small`;
- layout metrics are deterministic so Windows/Linux and DPI scaling do not depend on accidental `sizeHint()` differences;
- command state should be driven by `QAction` or `RibbonAction`, not duplicated in UI-only state.

## 2. Requirements

- CMake 3.16 or newer
- C++17
- Qt 5.14.2 or newer, or Qt 6
- Qt Widgets
- Windows or Linux

The library can be built static or shared. Static is the default.

## 3. Integration

### 3.1 Add as a source dependency

```cmake
add_subdirectory(thirdparty/RibbonLib)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

The umbrella header is:

```cpp
#include <RibbonLib.h>
```

No external Ribbon QSS file is required by the host application.

### 3.2 Install and use with `find_package`

Install:

```bash
./build.sh Release --install
```

Consume:

```cmake
find_package(RibbonLib CONFIG REQUIRED)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

The C++ API is identical in both integration modes.

## 4. Minimal direct C++ Ribbon

```cpp
#include <RibbonLib.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    auto *ribbon = new QRibbonWidget(this);
    setMenuWidget(ribbon);

    auto *home = ribbon->addTab("Home", "home");
    auto *document = home->addGroup("Document");

    auto *open = new QRibbonButton(openIcon,
                                   "Open",
                                   QRibbonButtonSize::Large,
                                   document);
    document->addButton(open);

    auto *axes = new QRibbonButton(axesIcon,
                                   "Axes",
                                   QRibbonButtonSize::Small,
                                   document);
    axes->setCheckable(true);
    axes->setChecked(true);
    document->addButton(axes);

    connect(open, &QRibbonButton::clicked, this, &MainWindow::openDocument);
    connect(axes, &QRibbonButton::toggled, this, &MainWindow::setAxesVisible);
}
```

Use `QMainWindow::setMenuWidget()` when the Ribbon should occupy the normal menu-bar region. The Ribbon is still an ordinary QWidget and may be embedded differently when required by the host.

## 5. Public controls

### `QRibbonWidget`

The Ribbon root. Important operations include:

```cpp
QRibbonTab *addTab(const QString &title, const QString &id = QString());
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

### `QRibbonTab`

A logical Ribbon tab. It contains groups:

```cpp
auto *home = ribbon->addTab("Home", "home");
auto *document = home->addGroup("Document");
auto *view = home->addGroup("View");
```

Use a stable `id` when the tab must be selected or restored programmatically.

### `QRibbonGroup`

A group/panel inside a tab. Supported content includes:

```cpp
group->addButton(button);
group->addSeparator();
group->addWidget(widget);
group->addLargeWidget(widget);
group->addSmallWidget(widget);
```

Small widgets are placed in a deterministic three-row column. Large widgets occupy the large-button region and have explicit vertical clearance so hover/checked borders are not clipped.

### `QRibbonButton`

A command button with two supported sizes:

```cpp
QRibbonButtonSize::Large
QRibbonButtonSize::Small
```

State API:

```cpp
button->setEnabled(true);
button->setCheckable(true);
button->setChecked(true);
button->setShortcut(QKeySequence("Ctrl+S"));
```

Signals:

```cpp
clicked();
toggled(bool checked);
```

The shortcut is command metadata; the shortcut text is not painted on the Ribbon button surface.

### `QRibbonSplitButton`

A primary command plus a dropdown menu:

```cpp
auto *menu = new QRibbonMenu(this);
auto *a = menu->addAction("Option A");
auto *b = menu->addAction("Option B");

menu->setDefaultAction(a);

auto *split = new QRibbonSplitButton(icon,
                                     "Create",
                                     QRibbonButtonSize::Large,
                                     group);
split->setMenu(menu);
split->setDefaultAction(a);
group->addLargeWidget(split);
```

The main area triggers the default action. The arrow area opens the menu. When a menu action becomes the default action, icon/text/state are synchronized from that `QAction`.

### `QApplicationButton`

The top-left File/Application button:

```cpp
auto *menu = new QRibbonMenu(this);
menu->addAction("Open");
menu->addAction("Save");
menu->addSeparator();
menu->addAction("Exit");

ribbon->applicationButton()->setText("File");
ribbon->applicationButton()->setApplicationMenu(menu);
```

### `QRibbonMenu`

A locally styled `QMenu` for Ribbon-owned menus. Prefer it for Ribbon menus so menu styling remains isolated from unrelated host menus.

## 6. Button layout rules

### 6.1 Small buttons

Small controls are arranged in three rows per column. RibbonLib uses fixed row metrics rather than allowing each child widget to independently determine vertical geometry.

The library intentionally leaves vertical clearance around the three-row content so a checked/hover/pressed border is fully visible on the first and third rows.

A Small button should be used for frequent compact commands where one-line text is appropriate.

### 6.2 Large buttons

A Large button reserves vertical space for:

1. a 32 px icon area;
2. up to two lines of command text;
3. border clearance around the button.

Large buttons also have reduced horizontal padding so adjacent large commands remain compact.

### 6.3 Two-line Large labels

Large labels follow these rules:

- maximum visual line count: 2;
- first explicit `\n`: honored as an explicit line break;
- additional explicit line breaks: converted to spaces;
- no explicit line break: automatic wrapping may be used;
- wrap mode: word boundary first, character boundary when necessary;
- no `...` elision for Large labels;
- button width is calculated so the complete label fits within two lines;
- Large Split Buttons use the same text-layout helper.

Examples:

```cpp
new QRibbonButton(icon, "AI Modeling Assistant", QRibbonButtonSize::Large);
new QRibbonButton(icon, "AI Modeling\nAssistant", QRibbonButtonSize::Large);
```

The first allows automatic wrapping. The second explicitly requests the line break position.

Do not encode three meaningful lines into a Large command. If a command name requires that much text, shorten the command name and move the explanation into a tooltip.

## 7. Checkable commands

Direct C++:

```cpp
auto *wireframe = new QRibbonButton(icon,
                                    "Wireframe",
                                    QRibbonButtonSize::Small,
                                    group);
wireframe->setCheckable(true);
wireframe->setChecked(false);
connect(wireframe, &QRibbonButton::toggled,
        this, &MainWindow::setWireframe);
group->addButton(wireframe);
```

For applications that already use `QAction`, prefer a single source of truth for `enabled`, `visible`, `checkable` and `checked` state. `QRibbonHelper` performs this synchronization for JSON-created controls.

Checked state, pressed state and disabled state are separate visual states. QSS is written so changing state does not change the geometry of the button box.

## 8. Quick Access bar

Add an action directly:

```cpp
auto *save = new QAction(saveIcon, "Save", this);
ribbon->addAccessBarAction(save);
```

Presentation policy:

- action has an icon -> icon-only Quick Access button;
- action has no icon -> text-only Quick Access button;
- command text/description remains available through Qt action/tool-tip behavior.

This keeps Save/Undo/Redo compact while allowing actions such as a language switch to remain readable.

## 9. Context tabs

RibbonLib supports contextual tab metadata:

```cpp
const int index = ribbon->indexOfTabId("tools");
ribbon->setTabContext(index, "Context", QColor("#0078d4"));
```

Clear it with:

```cpp
ribbon->clearTabContext(index);
```

Context metadata affects Ribbon tab presentation only. The host remains responsible for deciding when contextual tabs should be visible or active.

## 10. Custom widgets

Use `addWidget`, `addLargeWidget` or `addSmallWidget` when a group needs a host control that is not a Ribbon button.

```cpp
auto *combo = new QComboBox(group);
combo->addItems({"A", "B", "C"});
group->addSmallWidget(combo);
```

Guidelines:

- keep custom widgets vertically compatible with the requested slot;
- do not use custom widgets to bypass the Large/Small model for ordinary commands;
- keep application-specific styling local;
- use standard Qt size policies instead of manual geometry.

## 11. JSON model

RibbonLib separates command definitions from layout.

- `actions.json` describes command identity and state.
- `ribbon.json` describes placement and Ribbon structure.

This prevents translated labels and visual placement from becoming application command identity.

### 11.1 `actions.json`

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

Supported fields:

| Field | Purpose |
| --- | --- |
| `id` | Stable command identifier. |
| `name` | Displayed text. |
| `description` | Tooltip/description. |
| `icon` | File/resource path or `qt:<name>`. |
| `standardIcon` | Qt standard fallback when a custom icon is unavailable. |
| `shortcut` | Qt key sequence such as `Ctrl+O`. |
| `enabled` | Initial QAction enabled state. |
| `visible` | Initial QAction visible state. |
| `checkable` | Whether the action toggles. |
| `checked` | Initial checked state for a checkable action. |

`id` should remain stable across languages. Translate `name` and `description`, not the ID.

### 11.2 Icon resolution

Resolution order:

1. valid custom `icon` file/resource;
2. `standardIcon` fallback;
3. direct `qt:<name>` standard icon;
4. text-only action when no icon resolves.

Relative custom icon paths are resolved relative to the loaded `actions.json`, including Qt resource paths.

Common standard aliases:

`file`, `folder`, `home`, `open`, `save`, `close`, `apply`, `cancel`, `reset`, `help`, `info`, `warning`, `error`, `question`, `back`, `forward`, `up`, `down`, `reload`, `stop`, `play`, `pause`, `trash`, `settings`, `list`, `maximize`.

Qt `SP_...` names such as `SP_DialogSaveButton` are also accepted.

### 11.3 `ribbon.json`

```json
{
  "ribbon": {
    "applicationButton": {
      "title": "File",
      "menu": [
        { "id": "file.open" },
        { "id": "file.save" }
      ]
    },
    "quickAccessBar": {
      "items": [
        { "id": "file.save" },
        { "id": "edit.undo" },
        { "id": "edit.redo" }
      ]
    },
    "tabs": [
      {
        "id": "home",
        "title": "Home",
        "panels": [
          {
            "title": "Document",
            "items": [
              { "id": "file.open", "style": "large" },
              { "id": "file.save", "style": "small" }
            ]
          }
        ]
      }
    ]
  }
}
```

The supported placement size is intentionally only `large` or `small`.

### 11.4 Split button in JSON

```json
{
  "id": "insert.content",
  "style": "large",
  "defaultId": "insert.heading",
  "menu": [
    { "id": "insert.heading" },
    { "id": "insert.list" },
    { "id": "insert.table" }
  ]
}
```

### 11.5 Placement parameters

The same command may appear in more than one place. A placement may provide independent `params` without changing the base action:

```json
{ "id": "view.set", "style": "small", "params": "front" }
{ "id": "view.set", "style": "small", "params": "top" }
```

This is useful when one host command handles a family of closely related placements.

## 12. `QRibbonHelper`

Typical JSON flow:

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

Retrieve an action by ID:

```cpp
QAction *save = helper.action("file.save");
```

Simple dispatch:

```cpp
connect(&helper, &QRibbonHelper::actionTriggered,
        this, [this](const QString &id) {
            dispatchCommand(id);
        });
```

Use `errorString()` when loading/building fails. Avoid silently ignoring configuration errors in production builds.

## 13. `RibbonAction`

`RibbonAction` is optional. It is useful when the host wants command objects that can be bound to the Ribbon but remain independent of a specific Document/View implementation.

```cpp
class OpenAction final : public RibbonAction
{
public:
    using RibbonAction::RibbonAction;

    QString id() const override { return "file.open"; }

    void execute(const QString &params) override
    {
        Q_UNUSED(params);
        // host command implementation
    }

    void cleanup() override
    {
        // cancel transient interaction if necessary
    }
};
```

Register:

```cpp
auto *openAction = new OpenAction(this);
openAction->setName("Open");
openAction->setIcon(openIcon);
helper.registerRibbonAction(openAction);
```

`RibbonAction::trigger()` performs validation before execution and emits execution/rejection notifications. Property changes emit `changed()`, allowing `QRibbonHelper` to synchronize bound QAction/button state.

Use this abstraction only if it matches the host architecture. It is not required for RibbonLib.

## 14. State persistence

`QRibbonStateManager` provides lightweight Ribbon state persistence:

```cpp
QRibbonStateManager stateManager(ribbon, this);
stateManager.restoreState();
```

Before shutdown:

```cpp
stateManager.saveState();
```

It uses `QSettings`. It is intentionally not a general workspace/layout persistence system.

## 15. Styling

RibbonLib embeds its default QSS in the library and applies it locally.

Important rule: do not copy RibbonLib QSS into `QApplication::setStyleSheet()` unless the host deliberately wants to take ownership of global styling.

To replace Ribbon body styling locally:

```cpp
ribbon->setStyleSheet(myRibbonStyle);
```

Popup menus are separate Qt windows. Style Ribbon-owned menus locally through `QRibbonMenu`/the Application menu instead of broad global selectors.

When extending QSS, preserve the button box model across normal/hover/checked/pressed states. In particular, do not add a border only in the checked state while removing it in the normal state; use a transparent border in the normal state so geometry does not shift.

## 16. Internationalization

RibbonLib does not mandate an i18n system.

The example demonstrates two JSON sets:

```text
example/resources/actions.en.json
example/resources/actions.zh-CN.json
example/resources/ribbon.en.json
example/resources/ribbon.zh-CN.json
```

A production application may instead use `QTranslator` and construct the Ribbon directly in C++.

Keep command IDs language-independent.

## 17. Build options

Direct CMake:

```bash
cmake -S . -B build \
  -DRIBBONLIB_BUILD_EXAMPLE=ON \
  -DRIBBONLIB_BUILD_TESTS=ON \
  -DRIBBONLIB_BUILD_SHARED=OFF \
  -DRIBBONLIB_ENABLE_INSTALL=ON
cmake --build build
```

Options:

| Option | Meaning |
| --- | --- |
| `RIBBONLIB_BUILD_EXAMPLE` | Build the example application. |
| `RIBBONLIB_BUILD_TESTS` | Build smoke tests. Default is OFF. |
| `RIBBONLIB_BUILD_SHARED` | Build shared instead of static. |
| `RIBBONLIB_ENABLE_INSTALL` | Enable install/package targets. |

The helper scripts also honor `Qt5_DIR`, `Qt6_DIR` and `CMAKE_PREFIX_PATH`.

## 18. Example and regression coverage

The `Tests` tab in the example is intentionally visual. It covers layout cases that are easy to regress:

- checked Small button in row 1;
- checked Small button in row 3;
- multiple Small columns;
- checked/unchecked/disabled Large states;
- mixed Large and Small controls;
- long Small label;
- long two-line Large label;
- Large and Small Split buttons;
- Qt standard icons.

Smoke tests additionally verify important geometry/state behavior programmatically.

## 19. Recommended usage conventions

For application code, prefer these conventions:

- use stable lowercase dotted IDs such as `file.open`, `view.axes`, `model.extrude`;
- keep Group titles short;
- use Small for compact frequent actions and Large for visually important actions;
- keep Large labels to one or two concise lines;
- put explanations in tooltips, not in button labels;
- use `QAction` or `RibbonAction` as the source of truth for command state;
- use Qt standard icons when they communicate the command well, otherwise provide a dedicated product icon;
- keep host-specific widgets and styles outside RibbonLib;
- keep tests enabled in CI/release validation even though they are opt-in for normal local builds.

## 20. Troubleshooting

### Checked border is clipped

Do not override Ribbon row/button heights with host QSS. RibbonLib reserves explicit top/bottom clearance for state borders. Host styles that force a larger border or fixed height can invalidate those metrics.

### Buttons shift when checked

Ensure normal and checked states use the same border width. Prefer `border: 1px solid transparent` in normal state and only change the border color for checked/hover states.

### Long Large label is too wide

Allow automatic two-line wrapping or provide one explicit `\n`. Do not append arbitrary spaces to force geometry.

### A third line is needed

Shorten the command name. Large buttons intentionally support two lines only.

### Quick Access text is not visible

If the QAction has an icon, Quick Access intentionally uses icon-only presentation. Remove the icon when a text-only Quick Access action is desired.

### Custom icon is missing

Use a valid absolute/resource/relative path, add `standardIcon`, or use `qt:<name>` directly. Relative paths are resolved from `actions.json`.

### Host application styling changed unexpectedly

Do not apply RibbonLib QSS globally. RibbonLib's default style is already scoped and embedded.

## 21. Scope

RibbonLib intentionally does not provide:

- dock/window management;
- document/view architecture;
- MVVM framework;
- plugin framework;
- application command bus;
- large theme system;
- CAD/BIM domain objects.

Those responsibilities belong to the host application.

## 22. License

RibbonLib is available under the MIT License. See [`LICENSE`](../LICENSE).
