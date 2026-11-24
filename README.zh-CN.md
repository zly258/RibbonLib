<h1 align="center">RibbonLib</h1>

<p align="center">面向 CAD、BIM 与工程桌面应用的轻量 Qt Widgets Ribbon 控件库，采用 QAction 优先设计。</p>

<p align="center">
  <a href="./README.md">English</a> | <a href="./README.zh-CN.md">简体中文</a>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-3776AB?style=flat-square" alt="C++17"> <img src="https://img.shields.io/badge/Qt-5.14.2%2B%20%7C%206-3776AB?style=flat-square" alt="Qt 5.14.2+ 或 Qt 6"> <img src="https://img.shields.io/badge/CMake-3.16%2B-3776AB?style=flat-square" alt="CMake 3.16+"> <img src="https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-4B5563?style=flat-square" alt="Windows 与 Linux"> <a href="./LICENSE"><img src="https://img.shields.io/badge/License-MIT-22C55E?style=flat-square" alt="License: MIT"></a>
</p>

RibbonLib 将命令所有权与 Ribbon 界面呈现解耦：宿主应用通过 `QAction` 管理业务命令与状态，RibbonLib 专注于 Ribbon 布局、控件呈现、局部样式和 DPI 适配。

## 核心特点

| 特性 | 价值说明 |
|---|---|
| `QAction` 优先绑定 | Ribbon 按钮自动同步 `QAction` 的文本、图标、启用、可见、可勾选、选中状态、快捷键、提示与状态栏文本，无需维护两套状态。 |
| Large / Small 与 Split 按钮 | 提供常用 Ribbon 控件形态（`QRibbonButtonSize::Large`、`QRibbonButtonSize::Small`、可勾选命令以及搭配 `QRibbonMenu` 的 `QRibbonSplitButton`）。 |
| 完整 Ribbon 顶栏结构 | 内置 `QApplicationButton`、Quick Access Bar（`addAccessBarAction`）以及上下文标签页（`setTabContext` / `clearTabContext`）。 |
| 显式 Large 双行标签规则 | Large 按钮不自动换行，宽度随图标与文本内容自然扩展；需要两行时可通过仅用于呈现的 `displayText` 显式换行，不修改 `QAction::text()`。 |
| 内嵌局部生效 QSS | 默认样式（`resource/styles/ribbon.qss`）随库内嵌并仅作用于 Ribbon 控件，接入方无需复制样式文件、调用 `Q_INIT_RESOURCE()` 或修改全局样式表。 |
| 可选 JSON 数据驱动加载 | 通过 `QRibbonHelper` 分别加载 Action 元数据与 Ribbon 布局 JSON，适配配置驱动型桌面应用。 |

## 架构概览

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

## 快速安装

### 环境要求

- CMake 3.16+
- 支持 C++17 的编译器
- Qt 5.14.2+ 或 Qt 6（`Qt Widgets`）
- Windows 或 Linux

### 通过 CMake 集成

作为源码子目录引入：

```cmake
add_subdirectory(thirdparty/RibbonLib)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

或链接已安装的 CMake 包：

```cmake
find_package(RibbonLib CONFIG REQUIRED)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

### 从源码构建

标准 CMake 构建：

```bash
cmake -S . -B build
cmake --build build
```

使用仓库自带构建脚本（默认同时构建 `RibbonExample` 示例程序）：

Linux：

```bash
./build.sh
./build.sh Release --clean --run
./build.sh Release --shared
./build.sh Release --install
```

Windows（PowerShell）：

```powershell
.\build.ps1
.\build.ps1 -Configuration Release -Clean -Run
.\build.ps1 -Shared
.\build.ps1 -Install
```

## 快速开始

引入聚合头文件 `<RibbonLib.h>`，先创建宿主应用的 `QAction`，再通过 `QMainWindow::setMenuWidget()` 挂载 `QRibbonWidget` 并添加标签页与分组：

```cpp
#include <RibbonLib.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    auto *openAction = new QAction(openIcon, tr("打开"), this);
    connect(openAction, &QAction::triggered,
            this, &MainWindow::openDocument);

    auto *settingsAction = new QAction(settingsIcon, tr("应用设置"), this);

    auto *ribbon = new QRibbonWidget(this);
    setMenuWidget(ribbon);

    auto *home = ribbon->addTab(tr("主页"), "home");
    auto *document = home->addGroup(tr("文档"));
    document->addAction(openAction, QRibbonButtonSize::Large);

    auto *tools = home->addGroup(tr("工具"));
    tools->addAction(settingsAction,
                     QRibbonButtonSize::Large,
                     tr("应用\n设置"));
}
```

运行后，`QRibbonWidget` 会使用内嵌样式渲染包含“文档”和“工具”分组的“主页”标签页。`QAction::text()` 保持原始业务命令名供菜单等位置复用，`displayText` 仅控制 Ribbon 中的双行显示。

## API 示例

### Large 按钮显式双行文本

Large 按钮不会自动换行；未设置换行符时保持单行并按内容自然扩宽。需要双行显示时，通过 `displayText` 显式指定换行（仅保留第一处显式换行，Small 按钮始终保持单行）：

```cpp
button->setDisplayText(tr("模型\n设置"));
```

或在添加 Action 时直接传入 `displayText`：

```cpp
group->addAction(settingsAction,
                 QRibbonButtonSize::Large,
                 tr("模型\n设置"));
```

### Split Button

使用 `addSplitAction()` 绑定默认 `QAction` 与下拉 `QRibbonMenu`：

```cpp
auto *menu = new QRibbonMenu(group);
menu->addAction(headingAction);
menu->addAction(listAction);
menu->addAction(tableAction);

group->addSplitAction(headingAction,
                      menu,
                      QRibbonButtonSize::Large,
                      tr("插入\n内容"));
```

### 可选 JSON 加载

在需要数据驱动的场景下，通过 `QRibbonHelper` 分别加载 Action 定义和 Ribbon 布局：

```cpp
QRibbonHelper helper;
helper.loadActions(":/ui/actions.json");
helper.loadLayout(":/ui/ribbon.json");
helper.buildRibbon(ribbon);
```

普通桌面项目推荐直接使用 C++ 与 `QAction` 构建。

## 配置与集成

### CMake 选项

| 选项 | 默认值 | 说明 |
|---|---|---|
| `RIBBONLIB_BUILD_EXAMPLE` | 顶层构建 `ON` / 子项目 `OFF` | 是否构建 `example/` 下的 `RibbonExample` 示例程序。 |
| `RIBBONLIB_BUILD_SHARED` | `OFF` | 是否以动态库形式构建 `RibbonLib`（默认构建静态库）。 |
| `RIBBONLIB_ENABLE_INSTALL` | 顶层构建 `ON` / 子项目 `OFF` | 是否启用安装目标并导出 `RibbonLibConfig.cmake`。 |

### 样式与职责边界

- **局部样式隔离**：接入方无需复制 `ribbon.qss`、无需调用 `Q_INIT_RESOURCE()`、无需引入 ThemeManager，也无需向整个 `QApplication` 注入样式。默认样式仅作用于 RibbonLib 控件。
- **职责边界**：RibbonLib 专注于 Ribbon 控件呈现，刻意不包含 Dock 框架、命令总线、插件系统或宿主应用状态管理。

## 文档导航

| 主题 | 内容说明 | 链接 |
|---|---|---|
| 详细使用文档（中文） | 完整介绍接入方式、标签页与分组、按钮尺寸、`displayText`、可勾选命令、Split Button、上下文标签页及 `QRibbonHelper` JSON 结构。 | [docs/USAGE.zh-CN.md](./docs/USAGE.zh-CN.md) |
| Detailed usage guide (English) | 与中文文档对应的英文完整 API 与集成参考。 | [docs/USAGE.md](./docs/USAGE.md) |

## 许可证

RibbonLib 基于 [MIT License](./LICENSE) 开源。
