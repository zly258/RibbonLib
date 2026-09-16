# RibbonLib

[English](./README.md) | 中文

RibbonLib 是一个面向 CAD、BIM 和工程桌面软件的轻量 Qt Widgets Ribbon 控件库。推荐模式是 **QAction 优先**：宿主负责命令和业务状态，RibbonLib 只负责 Ribbon 的呈现、布局、局部样式和 DPI 行为。

## 特点

- Qt 5.14.2+ 与 Qt 6。
- C++17，CMake 3.16+。
- Windows / Linux。
- 以 `QAction` 为核心，自动同步命令状态。
- Large / Small 按钮、可勾选命令、Split Button。
- Application Menu、Quick Access Bar、Context Tab。
- 内嵌且局部生效的 QSS，第三方不需要复制样式文件。
- Large 文字不自动换行；需要两行时显式设置 Ribbon `displayText`。
- 按钮宽度由图标、文字、padding 自然决定，不再使用固定最小宽度。
- `QRibbonHelper` 保留可选 JSON Action/Layout 加载能力。

## 快速接入

```cmake
add_subdirectory(thirdparty/RibbonLib)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

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

`QAction::text()` 始终是业务命令名称，可继续用于菜单、右键菜单等位置；Ribbon 专用 `displayText` 只控制 Ribbon 显示，不污染 QAction。

## Large 文字规则

Large 按钮不会自动换行。没有显式换行时保持单行，并按文字自然扩宽。需要两行时显式设置：

```cpp
button->setDisplayText(tr("模型\n设置"));
```

或者：

```cpp
group->addAction(settingsAction,
                 QRibbonButtonSize::Large,
                 tr("模型\n设置"));
```

只保留第一处显式换行；Small 按钮始终保持单行。

## Split Button

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

默认 QAction 是命令源，`displayText` 只负责 Ribbon 展示。

## 样式

第三方不需要复制 `ribbon.qss`、不需要调用 `Q_INIT_RESOURCE()`、不需要安装 ThemeManager，也不需要把 RibbonLib 的样式应用到整个 `QApplication`。默认样式只作用于 RibbonLib 控件。

## 可选 JSON 加载

需要数据驱动时，Action 元数据和 Ribbon 布局分开加载：

```cpp
QRibbonHelper helper;
helper.loadActions(":/ui/actions.json");
helper.loadLayout(":/ui/ribbon.json");
helper.buildRibbon(ribbon);
```

普通项目优先直接使用 C++ + `QAction`。

## 构建

Linux：

```bash
./build.sh
./build.sh Release --clean --run
./build.sh Release --shared
./build.sh Release --install
```

Windows：

```powershell
.\build.ps1
.\build.ps1 -Configuration Release -Clean -Run
.\build.ps1 -Shared
.\build.ps1 -Install
```

也支持标准 CMake：

```bash
cmake -S . -B build
cmake --build build
```

主要选项：

- `RIBBONLIB_BUILD_EXAMPLE`
- `RIBBONLIB_BUILD_SHARED`
- `RIBBONLIB_ENABLE_INSTALL`

## 文档

- [Detailed usage guide](./docs/USAGE.md)
- [详细使用文档](./docs/USAGE.zh-CN.md)

## 范围

RibbonLib 不负责 Dock 框架、命令总线、插件系统、应用状态管理器或宿主应用架构。业务命令留在宿主，RibbonLib 只负责 Ribbon 呈现。

## License

RibbonLib 使用 [MIT License](./LICENSE)。
