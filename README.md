# RibbonLib

English | [中文](./README.zh-CN.md)

RibbonLib is a compact Qt Widgets Ribbon library for CAD, BIM and engineering desktop applications. The recommended model is **QAction first**: the host owns commands and business state; RibbonLib owns Ribbon presentation, layout, local styling and DPI behavior.

## Highlights

- Qt 5.14.2+ and Qt 6.
- C++17 and CMake 3.16+.
- Windows and Linux.
- `QAction`-first construction and state synchronization.
- Large / Small buttons, checkable commands and Split Buttons.
- Application Menu, Quick Access bar and context tabs.
- Embedded Ribbon-scoped QSS; consumers do not copy QSS files.
- Large labels never wrap automatically. Use explicit Ribbon `displayText` when two lines are wanted.
- Button width follows icon/text/padding content instead of a fixed minimum width.
- Optional JSON action/layout loading through `QRibbonHelper`.

## Quick start

```cmake
add_subdirectory(thirdparty/RibbonLib)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

```cpp
#include <RibbonLib.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    auto *openAction = new QAction(openIcon, tr("Open"), this);
    connect(openAction, &QAction::triggered,
            this, &MainWindow::openDocument);

    auto *settingsAction = new QAction(settingsIcon, tr("Application Settings"), this);

    auto *ribbon = new QRibbonWidget(this);
    setMenuWidget(ribbon);

    auto *home = ribbon->addTab(tr("Home"), "home");
    auto *document = home->addGroup(tr("Document"));
    document->addAction(openAction, QRibbonButtonSize::Large);

    auto *tools = home->addGroup(tr("Tools"));
    tools->addAction(settingsAction,
                     QRibbonButtonSize::Large,
                     tr("Application\nSettings"));
}
```

`QAction::text()` remains the command name used by menus and other UI. Ribbon-only `displayText` controls the Ribbon presentation without changing the action text.

## Large label rule

Large buttons do not automatically wrap. A label stays on one line and the button grows to fit it. To request two lines, set an explicit Ribbon display label:

```cpp
button->setDisplayText(tr("Model\nSettings"));
```

or:

```cpp
group->addAction(settingsAction,
                 QRibbonButtonSize::Large,
                 tr("Model\nSettings"));
```

Only the first explicit line break is kept. Small buttons remain single-line.

## Split Button

```cpp
auto *menu = new QRibbonMenu(group);
menu->addAction(headingAction);
menu->addAction(listAction);
menu->addAction(tableAction);

group->addSplitAction(headingAction,
                      menu,
                      QRibbonButtonSize::Large,
                      tr("Insert\nContent"));
```

The default action remains the command source; `displayText` is presentation-only.

## Styling

Consumers do not need to copy `ribbon.qss`, call `Q_INIT_RESOURCE()`, install a theme manager, or apply RibbonLib styling to the whole application. RibbonLib applies its embedded style only to Ribbon controls.

## Optional JSON loading

For data-driven applications, action metadata and Ribbon layout are deliberately separate:

```cpp
QRibbonHelper helper;
helper.loadActions(":/ui/actions.json");
helper.loadLayout(":/ui/ribbon.json");
helper.buildRibbon(ribbon);
```

Direct C++ + `QAction` is the recommended and simplest path.

## Build

Linux:

```bash
./build.sh
./build.sh Release --clean --run
./build.sh Release --shared
./build.sh Release --install
```

Windows:

```powershell
.\build.ps1
.\build.ps1 -Configuration Release -Clean -Run
.\build.ps1 -Shared
.\build.ps1 -Install
```

Standard CMake is also supported:

```bash
cmake -S . -B build
cmake --build build
```

Main options:

- `RIBBONLIB_BUILD_EXAMPLE`
- `RIBBONLIB_BUILD_SHARED`
- `RIBBONLIB_ENABLE_INSTALL`

## Documentation

- [Detailed usage guide](./docs/USAGE.md)
- [详细使用文档](./docs/USAGE.zh-CN.md)

## Scope

RibbonLib intentionally does not provide a Dock framework, command bus, plugin system, application state manager or host-application architecture. Keep business commands in the host and use RibbonLib only for Ribbon presentation.

## License

RibbonLib is licensed under the [MIT License](./LICENSE).
