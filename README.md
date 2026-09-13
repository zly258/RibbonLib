# RibbonLib

English | [中文](./README.zh-CN.md)

RibbonLib is a small Qt Widgets Ribbon library for CAD, BIM, engineering and other desktop applications. Its goal is deliberately narrow: provide a stable Ribbon, keep integration inexpensive, and avoid turning the library into a large UI framework.

## 1. Requirements

- CMake 3.16+
- C++17
- Qt 5.14.2+ or Qt 6
- Windows or Linux

RibbonLib does not require an external QSS file at runtime. Its default QSS is embedded in the library and scoped to RibbonLib widgets.

## 2. Fastest integration

For a source dependency:

```cmake
add_subdirectory(thirdparty/RibbonLib)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

Then create a Ribbon exactly like any other Qt widget:

```cpp
#include <RibbonLib.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    auto *ribbon = new QRibbonWidget(this);
    setMenuWidget(ribbon);

    auto *home = ribbon->addTab("Home", "home");
    auto *group = home->addGroup("Document");
    group->addButton(QIcon(), "New", QRibbonButtonSize::Large);
}
```

That is enough. A third-party application does **not** need to:

- copy `ribbon.qss`;
- load QSS with `QFile`;
- call `QApplication::setStyleSheet()`;
- call `Q_INIT_RESOURCE()`;
- set a Ribbon resource directory;
- initialize a theme manager.

## 3. Installed package

Install RibbonLib:

```bash
./build.sh Release --install
```

Then consume it with standard CMake package discovery:

```cmake
find_package(RibbonLib CONFIG REQUIRED)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

The C++ code is identical to the `add_subdirectory` case.

## 4. Main controls

The public umbrella header is:

```cpp
#include <RibbonLib.h>
```

The main public controls are:

- `QRibbonWidget` — Ribbon root, tabs, Application button and Quick Access;
- `QRibbonTab` — one Ribbon tab;
- `QRibbonGroup` — one group/panel inside a tab;
- `QRibbonButton` — Large or Small command button;
- `QRibbonSplitButton` — command plus dropdown menu;
- `QApplicationButton` — File/Application menu button;
- `QRibbonMenu` — locally styled Ribbon popup menu;
- `RibbonAction` — optional host command abstraction;
- `QRibbonHelper` — optional JSON builder;
- `QRibbonStateManager` — lightweight current-tab persistence.

## 5. Direct C++ construction

A minimal real-world structure looks like this:

```cpp
auto *ribbon = new QRibbonWidget(this);
setMenuWidget(ribbon);

auto *fileMenu = new QRibbonMenu(this);
fileMenu->addAction("Open");
ribbon->applicationButton()->setText("File");
ribbon->applicationButton()->setApplicationMenu(fileMenu);

auto *home = ribbon->addTab("Home", "home");
auto *document = home->addGroup("Document");

auto *open = new QRibbonButton(openIcon, "Open", QRibbonButtonSize::Large, document);
document->addButton(open);

QAction *saveAction = new QAction(saveIcon, "Save", this);
ribbon->addAccessBarAction(saveAction);
```

Quick Access is right-aligned. If an action has an icon, RibbonLib displays the icon only; if it has no icon, it displays text. The action text and description remain available through the tooltip, keeping common commands compact while text-only actions such as the language switch remain readable.

## 6. JSON construction

JSON is optional. Direct C++ use does not depend on it.

```cpp
QRibbonHelper helper;
if (!helper.loadFromResources(ribbonPath, actionsPath)) {
    qWarning() << helper.errorString();
    return;
}
helper.buildRibbon(ribbon);
```

### actions.json

Actions define text, tooltip, icon, shortcut and basic state:

```json
{
  "actions": [
    {
      "id": "file.open",
      "name": "Open",
      "description": "Open a document",
      "icon": "qt:open",
      "shortcut": "Ctrl+O"
    },
    {
      "id": "file.save",
      "name": "Save",
      "icon": "icons/save.svg",
      "standardIcon": "save",
      "shortcut": "Ctrl+S"
    },
    {
      "id": "view.readOnly",
      "name": "Read Only",
      "icon": "qt:info",
      "checkable": true,
      "checked": false
    }
  ]
}
```

Supported action fields are:

- `id` — stable command identifier;
- `name` — displayed text;
- `description` — tooltip;
- `icon` — custom file/resource path, or `qt:<name>` for a Qt standard icon;
- `standardIcon` — Qt standard fallback when a custom icon does not exist;
- `shortcut` — Qt key sequence such as `Ctrl+S`; QAction handles it, but Ribbon buttons do not draw shortcut text on their surface;
- `enabled`, `visible`, `checkable`, `checked` — basic QAction state.

Relative custom icon paths are resolved relative to `actions.json`, including when `actions.json` itself lives in a Qt resource (`:/...`).

### Icon fallback

The resolution order is intentionally simple:

1. use `icon` when the referenced file/resource exists;
2. otherwise use `standardIcon`;
3. if `icon` is written as `qt:save`, use the Qt standard icon directly;
4. if nothing resolves, show a text-only action.

Useful built-in aliases include:

`file`, `folder`, `home`, `open`, `save`, `close`, `apply`, `cancel`, `reset`, `help`, `info`, `warning`, `error`, `question`, `back`, `forward`, `up`, `down`, `reload`, `stop`, `play`, `pause`, `trash`, `settings`, `list`, `maximize`.

The corresponding `SP_...` names are also accepted, for example `SP_DialogSaveButton`.

### ribbon.json

Ribbon layout is independent of actions:

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

`style` is intentionally limited to `large` or `small`.

A split button is defined by adding a `menu` to an item:

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

The same action may appear in multiple places. Optional per-placement `params` are supported without overwriting each other.

## 7. Actions and business logic

RibbonLib does not own application business logic. The host may either connect to the generated `QAction`s:

```cpp
connect(&helper, &QRibbonHelper::actionTriggered,
        this, [this](const QString &id) {
            // dispatch in the host application
        });
```

or subclass `RibbonAction` and register it with `QRibbonHelper`.

This keeps RibbonLib independent of a Document, Viewport, CAD model, BIM model, command bus or application framework.

## 8. Language switching

RibbonLib does not force a translation framework. Use the host application's normal i18n strategy.

The example deliberately demonstrates a simple JSON approach:

```text
example/resources/actions.en.json
example/resources/actions.zh-CN.json
example/resources/ribbon.en.json
example/resources/ribbon.zh-CN.json
```

The right-side Quick Access keeps the language action text-only. Switching language reloads the matching JSON pair and rebuilds the example Ribbon. A production application may instead use `QTranslator`; RibbonLib does not prevent either approach.

## 9. Styling

The default style is embedded and local to RibbonLib. It does not change the host `QApplication` stylesheet.

To replace the Ribbon body style:

```cpp
ribbon->setStyleSheet(myRibbonStyle);
```

Qt popup menus are separate windows. If a custom application needs different popup styling, set local QSS on its `QRibbonMenu` or Application Menu rather than applying Ribbon-specific QSS globally.

## 10. Build scripts

Linux:

```bash
./build.sh
./build.sh Debug --clean
./build.sh Release --tests
./build.sh Release --run
./build.sh Release --shared
./build.sh Release --install
./build.sh Release --install --prefix /opt/RibbonLib
```

Windows PowerShell:

```powershell
.\build.ps1
.\build.ps1 -Configuration Debug -Clean
.\build.ps1 -Tests
.\build.ps1 -Run
.\build.ps1 -Shared
.\build.ps1 -Install
.\build.ps1 -Install -Prefix C:\RibbonLib
```

The scripts honor `Qt5_DIR`, `Qt6_DIR` and `CMAKE_PREFIX_PATH`.

Standard CMake is also supported:

```bash
cmake -S . -B build
cmake --build build
```

Tests remain opt-in:

```bash
cmake -S . -B build -DRIBBONLIB_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 11. Example

The Example intentionally contains more functionality than the library's minimum integration sample. It demonstrates:

- Home / Insert / View / Tools / Help tabs;
- Application menu;
- right-aligned Quick Access with icon-only common commands and a text language switch;
- Qt standard icons and missing-resource fallback;
- Large and Small buttons;
- SplitButton and menus;
- checkable actions;
- English / Chinese JSON switching;
- Action dispatch into ordinary host widgets.

Run it with:

```bash
./build.sh Release --clean --run
```

## 12. Scope

RibbonLib intentionally does **not** try to provide a Gallery framework, Dock framework, MVVM framework, command bus, plugin system, or large theme engine.

The priorities are stability, simple integration, style isolation, predictable layout, Qt5/Qt6 compatibility and low maintenance cost.
