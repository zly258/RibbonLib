# RibbonLib

English | [中文](./README.zh-CN.md)

RibbonLib is a compact Qt Widgets Ribbon library for CAD, BIM, engineering and other desktop applications. The recommended integration model is now **QAction first**: the host owns commands and state, while RibbonLib owns Ribbon presentation, layout, DPI behavior and local styling.

## Highlights

- Qt 5.14.2+ and Qt 6.
- C++17 and CMake 3.16+.
- Windows and Linux.
- `QAction`-first construction with automatic state synchronization.
- Large / Small Ribbon buttons.
- Checkable, enabled, visible and shortcut state synchronized from `QAction`.
- Split buttons and Ribbon menus.
- Application menu, Quick Access bar and context tabs.
- Embedded Ribbon-scoped QSS; consumers do not copy or load `ribbon.qss`.
- Direct C++ construction is the recommended path; JSON remains optional.
- Optional `RibbonAction` abstraction and state persistence.

## Recommended usage

### 1. Link RibbonLib

```cmake
add_subdirectory(thirdparty/RibbonLib)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

or after installation:

```cmake
find_package(RibbonLib CONFIG REQUIRED)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

### 2. Define application commands as `QAction`

```cpp
auto *openAction = new QAction(openIcon, tr("Open"), this);
openAction->setShortcut(QKeySequence::Open);
connect(openAction, &QAction::triggered,
        this, &MainWindow::openDocument);

auto *axesAction = new QAction(axesIcon, tr("Axes"), this);
axesAction->setCheckable(true);
axesAction->setChecked(true);
connect(axesAction, &QAction::toggled,
        this, &MainWindow::setAxesVisible);
```

### 3. Only describe Ribbon structure

```cpp
#include <RibbonLib.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    auto *ribbon = new QRibbonWidget(this);
    setMenuWidget(ribbon);

    auto *home = ribbon->addTab(tr("Home"), "home");

    home->addGroup(tr("Document"), {
        openAction,
        saveAction
    });

    home->addGroup(tr("View"), {
        axesAction,
        wireframeAction,
        perspectiveAction
    });
}
```

For a specific button size:

```cpp
auto *document = home->addGroup(tr("Document"));
document->addAction(openAction, QRibbonButtonSize::Large);
document->addActions({saveAction, closeAction}, QRibbonButtonSize::Small);
```

RibbonLib automatically synchronizes `text`, `icon`, `enabled`, `visible`, `checkable`, `checked`, `shortcut`, `toolTip` and `statusTip` from the bound `QAction`. Triggering the Ribbon button triggers the same `QAction`.

This is the preferred model: **one QAction, multiple UI entry points, one source of command state**.

## Low-level button API

`QRibbonButton` can still be created manually when there is no application `QAction`:

```cpp
auto *button = new QRibbonButton(icon,
                                 tr("Custom"),
                                 QRibbonButtonSize::Large,
                                 group);
connect(button, &QRibbonButton::clicked,
        this, &MainWindow::runCustomCommand);
group->addButton(button);
```

It can also bind an existing action directly:

```cpp
auto *button = new QRibbonButton(openAction,
                                 QRibbonButtonSize::Large,
                                 group);
group->addButton(button);
```

For normal application commands, prefer `group->addAction()` instead of constructing the button yourself.

## Quick Access

The same `QAction` can be reused:

```cpp
ribbon->addAccessBarAction(saveAction);
ribbon->addAccessBarAction(undoAction);
ribbon->addAccessBarAction(redoAction);
```

If an action has an icon, Quick Access shows the icon only. If it has no icon, text is shown.

## Styling

Consumers do **not** need to:

- copy `ribbon.qss`;
- call `Q_INIT_RESOURCE()` for RibbonLib resources;
- install a theme manager;
- apply RibbonLib QSS to the whole `QApplication`;
- manually fix button height, icon size, check-state geometry or group spacing.

RibbonLib keeps its default style and layout behavior local to Ribbon controls.

## JSON construction

JSON remains supported for applications that need data-driven Ribbon definitions, but it is optional and no longer the simplest path.

```cpp
QRibbonHelper helper;
if (!helper.loadFromResources(ribbonPath, actionsPath)) {
    qWarning() << helper.errorString();
    return;
}

if (!helper.buildRibbon(ribbon)) {
    qWarning() << helper.errorString();
}
```

Use direct `QAction` construction when the Ribbon structure is known in C++ and the application already has a command layer.

## Documentation

- [Detailed usage guide](./docs/USAGE.md)
- [详细使用文档](./docs/USAGE.zh-CN.md)
- [English JSON example](./example/resources/ribbon.en.json)
- [English action definitions](./example/resources/actions.en.json)

The detailed guide covers QAction-first construction, state synchronization, tabs/groups, Large/Small layout, checkable commands, split buttons, Quick Access, Application Menu, context tabs, custom widgets, JSON, `RibbonAction`, styling, migration and build options.

## Build

Linux:

```bash
./build.sh
./build.sh Debug --clean
./build.sh Release --tests
./build.sh Release --run
./build.sh Release --shared
./build.sh Release --install
```

Windows PowerShell:

```powershell
.\build.ps1
.\build.ps1 -Configuration Debug -Clean
.\build.ps1 -Tests
.\build.ps1 -Run
.\build.ps1 -Shared
.\build.ps1 -Install
```

Standard CMake is also supported:

```bash
cmake -S . -B build
cmake --build build
```

## Scope

RibbonLib intentionally does not provide a Dock framework, MVVM framework, command bus, plugin system, large theme engine or host-application architecture. It stays focused on Ribbon presentation and lets the host keep ownership of business logic and commands.

## License

RibbonLib is licensed under the [MIT License](./LICENSE).
