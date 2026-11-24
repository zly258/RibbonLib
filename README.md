<h1 align="center">RibbonLib</h1>

<p align="center">A compact Qt Widgets Ribbon library for CAD, BIM, and engineering desktop applications built around a QAction-first design.</p>

<p align="center">
  <a href="./README.md">English</a> | <a href="./README.zh-CN.md">简体中文</a>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-3776AB?style=flat-square" alt="C++17"> <img src="https://img.shields.io/badge/Qt-5.14.2%2B%20%7C%206-3776AB?style=flat-square" alt="Qt 5.14.2+ or Qt 6"> <img src="https://img.shields.io/badge/CMake-3.16%2B-3776AB?style=flat-square" alt="CMake 3.16+"> <img src="https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-4B5563?style=flat-square" alt="Windows and Linux"> <a href="./LICENSE"><img src="https://img.shields.io/badge/License-MIT-22C55E?style=flat-square" alt="License: MIT"></a>
</p>

RibbonLib separates command ownership from Ribbon presentation: your host application owns `QAction` instances and business state, while RibbonLib handles Ribbon layout, control presentation, scoped styling, and DPI-aware sizing.

## Highlights

| Highlight | Why it matters |
|---|---|
| `QAction`-first command binding | Ribbon buttons automatically mirror `QAction` text, icon, enabled, visible, checkable, checked, shortcut, tooltip, and status tip without duplicating state. |
| Large, Small, and Split buttons | Covers standard Ribbon controls (`QRibbonButtonSize::Large`, `QRibbonButtonSize::Small`, checkable commands, and `QRibbonSplitButton` with `QRibbonMenu`). |
| Complete Ribbon chrome | Includes `QApplicationButton`, Quick Access bar (`addAccessBarAction`), and context tabs (`setTabContext` / `clearTabContext`). |
| Explicit Large label layout | Large labels never wrap automatically and widths grow to fit content; use presentation-only `displayText` for explicit two-line breaks without modifying `QAction::text()`. |
| Embedded Ribbon-scoped QSS | Default styling (`resource/styles/ribbon.qss`) is bundled and applied only to Ribbon controls, with no external QSS copying, `Q_INIT_RESOURCE()`, or global stylesheet overrides required. |
| Optional JSON loading | `QRibbonHelper` supports loading action metadata and Ribbon layout from separate JSON files for data-driven applications. |

## Architecture

```text
┌─────────────────────────────────────────────────────────────────────────┐
│                            Host Application                             │
│  ┌──────────────────────────────┐     ┌──────────────────────────────┐  │
│  │  QAction Commands & State    │     │  Optional JSON Definitions   │  │
│  │  (text, icon, checked, slot) │     │  (actions.json, ribbon.json) │  │
│  └──────────────┬───────────────┘     └──────────────┬───────────────┘  │
└─────────────────┼────────────────────────────────────┼──────────────────┘
                  │ bind QAction                       │ QRibbonHelper
                  ▼                                    ▼
┌─────────────────────────────────────────────────────────────────────────┐
│               QRibbonWidget (QMainWindow::setMenuWidget)                │
│  ┌───────────────────────────────────────────────────────────────────┐  │
│  │ Top Bar: QApplicationButton | QTabBar (Context Tabs) | AccessBar  │  │
│  └───────────────────────────────────────────────────────────────────┘  │
│                                    │                                    │
│                                    ▼                                    │
│  ┌───────────────────────────────────────────────────────────────────┐  │
│  │ QRibbonTab (Scrollable Tab Content)                               │  │
│  │   └─▶ QRibbonGroup                                                │  │
│  │         ├─▶ QRibbonButton (Large / Small)                         │  │
│  │         └─▶ QRibbonSplitButton ──▶ QRibbonMenu                    │  │
│  └───────────────────────────────────────────────────────────────────┘  │
└────────────────────────────────────┬────────────────────────────────────┘
                                     │ scoped presentation
                                     ▼
┌─────────────────────────────────────────────────────────────────────────┐
│  Internal Presentation Layer                                            │
│  QRibbonStyle (embedded ribbon.qss) | QRibbonMetrics | QRibbonTextLayout│
└─────────────────────────────────────────────────────────────────────────┘
```

## Quick Install

### Prerequisites

- CMake 3.16+
- C++17 compiler
- Qt 5.14.2+ or Qt 6 (`Qt Widgets`)
- Windows or Linux

### Integrate via CMake

Add RibbonLib as a source subdirectory:

```cmake
add_subdirectory(thirdparty/RibbonLib)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

Or consume an installed CMake package:

```cmake
find_package(RibbonLib CONFIG REQUIRED)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

### Build from Source

Standard CMake:

```bash
cmake -S . -B build
cmake --build build
```

Using the repository helper scripts (which also build the example target `RibbonExample`):

Linux:

```bash
./build.sh
./build.sh Release --clean --run
./build.sh Release --shared
./build.sh Release --install
```

Windows (PowerShell):

```powershell
.\build.ps1
.\build.ps1 -Configuration Release -Clean -Run
.\build.ps1 -Shared
.\build.ps1 -Install
```

## Quick Start

Include the umbrella header `<RibbonLib.h>`, create your application `QAction` objects, mount `QRibbonWidget` on a `QMainWindow` with `setMenuWidget()`, and add tabs and groups:

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

When the window runs, `QRibbonWidget` renders the `Home` tab with `Document` and `Tools` groups using its embedded stylesheet. `QAction::text()` stays intact for menus and other UI while `displayText` controls only the two-line Ribbon presentation.

## API Example

### Explicit Two-Line Large Labels

Large buttons do not wrap automatically; a single-line label expands button width to fit. Specify an explicit line break via `displayText` (only the first line break is kept; Small buttons stay single-line):

```cpp
button->setDisplayText(tr("Model\nSettings"));
```

Or pass `displayText` directly when adding the action:

```cpp
group->addAction(settingsAction,
                 QRibbonButtonSize::Large,
                 tr("Model\nSettings"));
```

### Split Button

Attach a default `QAction` and a `QRibbonMenu` with `addSplitAction()`:

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

### Optional JSON Loading

For data-driven UIs, action metadata and Ribbon layout are loaded separately through `QRibbonHelper`:

```cpp
QRibbonHelper helper;
helper.loadActions(":/ui/actions.json");
helper.loadLayout(":/ui/ribbon.json");
helper.buildRibbon(ribbon);
```

Direct C++ with `QAction` remains the recommended path for most applications.

## Configuration and Integrations

### CMake Options

| Option | Default | Description |
|---|---|---|
| `RIBBONLIB_BUILD_EXAMPLE` | `ON` (top-level) / `OFF` (subproject) | Build the `RibbonExample` demo application under `example/`. |
| `RIBBONLIB_BUILD_SHARED` | `OFF` | Build `RibbonLib` as a shared library instead of a static library. |
| `RIBBONLIB_ENABLE_INSTALL` | `ON` (top-level) / `OFF` (subproject) | Enable CMake install targets and `RibbonLibConfig.cmake` package export. |

### Styling and Scope

- **Scoped styling**: Consumers do not need to copy `ribbon.qss`, call `Q_INIT_RESOURCE()`, install a theme manager, or apply RibbonLib styles to `QApplication`. Default styles apply only to RibbonLib widgets.
- **Intentional scope**: RibbonLib focuses strictly on Ribbon presentation and intentionally omits dock frameworks, command buses, plugin systems, and application state managers.

## Documentation

| Topic | What it covers | Link |
|---|---|---|
| Detailed usage guide (English) | Full integration, tabs/groups, button sizing, `displayText`, checkable actions, Split Buttons, context tabs, and `QRibbonHelper` JSON schemas. | [docs/USAGE.md](./docs/USAGE.md) |
| Detailed usage guide (Simplified Chinese) | Complete Chinese reference covering the same APIs, styling rules, and JSON loading workflow. | [docs/USAGE.zh-CN.md](./docs/USAGE.zh-CN.md) |

## License

RibbonLib is licensed under the [MIT License](./LICENSE).
