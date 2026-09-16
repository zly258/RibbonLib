# RibbonLib

[English](./README.md) | 中文

RibbonLib 是一个面向 Qt Widgets 的轻量 Ribbon 控件库，适合 CAD、BIM、工程设计及其他桌面工业软件。当前推荐的接入方式是 **QAction 优先**：宿主负责命令和业务状态，RibbonLib 负责 Ribbon 的显示、布局、DPI、自身样式和状态呈现。

## 主要特点

- 支持 Qt 5.14.2+ 与 Qt 6。
- C++17，CMake 3.16+。
- Windows / Linux。
- 以 `QAction` 为核心的直接构建方式。
- 自动同步 `enabled / visible / checkable / checked / shortcut` 等状态。
- Large / Small 两种 Ribbon 按钮。
- Split Button、Ribbon Menu、Application Menu。
- Quick Access Bar 与 Context Tab。
- 默认 QSS 内嵌并且只作用于 RibbonLib，第三方无需复制样式文件。
- 推荐直接 C++ 构建；JSON 仍作为可选能力保留。
- 可选 `RibbonAction` 命令抽象与状态保存。

## 推荐用法

### 1. 引入 RibbonLib

```cmake
add_subdirectory(thirdparty/RibbonLib)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

安装后也可以：

```cmake
find_package(RibbonLib CONFIG REQUIRED)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

### 2. 业务命令统一使用 `QAction`

```cpp
auto *openAction = new QAction(openIcon, tr("打开"), this);
openAction->setShortcut(QKeySequence::Open);
connect(openAction, &QAction::triggered,
        this, &MainWindow::openDocument);

auto *axesAction = new QAction(axesIcon, tr("坐标轴"), this);
axesAction->setCheckable(true);
axesAction->setChecked(true);
connect(axesAction, &QAction::toggled,
        this, &MainWindow::setAxesVisible);
```

### 3. Ribbon 代码只描述结构

```cpp
#include <RibbonLib.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    auto *ribbon = new QRibbonWidget(this);
    setMenuWidget(ribbon);

    auto *home = ribbon->addTab(tr("主页"), "home");

    home->addGroup(tr("文档"), {
        openAction,
        saveAction
    });

    home->addGroup(tr("视图"), {
        axesAction,
        wireframeAction,
        perspectiveAction
    });
}
```

需要指定按钮尺寸时：

```cpp
auto *document = home->addGroup(tr("文档"));
document->addAction(openAction, QRibbonButtonSize::Large);
document->addActions({saveAction, closeAction}, QRibbonButtonSize::Small);
```

RibbonLib 会自动从 `QAction` 同步：

`text / icon / enabled / visible / checkable / checked / shortcut / toolTip / statusTip`

点击 RibbonButton 时会直接触发同一个 `QAction`。

推荐原则就是：**一个 QAction，多处 UI 复用，命令状态只维护一份。**

## 低层按钮 API

如果某个控件并不是标准应用命令，也仍然可以直接创建 `QRibbonButton`：

```cpp
auto *button = new QRibbonButton(icon,
                                 tr("自定义"),
                                 QRibbonButtonSize::Large,
                                 group);
connect(button, &QRibbonButton::clicked,
        this, &MainWindow::runCustomCommand);
group->addButton(button);
```

也可以直接绑定已有 QAction：

```cpp
auto *button = new QRibbonButton(openAction,
                                 QRibbonButtonSize::Large,
                                 group);
group->addButton(button);
```

但普通业务命令优先使用：

```cpp
group->addAction(openAction);
```

这样业务代码最少。

## Quick Access Bar

同一套 QAction 可以直接复用：

```cpp
ribbon->addAccessBarAction(saveAction);
ribbon->addAccessBarAction(undoAction);
ribbon->addAccessBarAction(redoAction);
```

显示规则：

- QAction 有图标：Quick Access 只显示图标；
- QAction 没有图标：显示文字。

快捷键仍属于 QAction，不会额外绘制在按钮表面。

## 样式与布局

第三方项目不需要：

- 复制 `ribbon.qss`；
- 调用 `Q_INIT_RESOURCE()` 初始化 RibbonLib 资源；
- 增加 ThemeManager；
- 把 RibbonLib QSS 设置到整个 `QApplication`；
- 手动处理按钮高度、图标尺寸、checked 边框、Group 间距；
- 针对 125% / 150% DPI 在业务代码里补特殊尺寸。

这些都应该由 RibbonLib 自己负责。

## JSON 构建

JSON 仍然保留，适合需要配置驱动 Ribbon 的项目，但不再作为最简单的默认接入方式。

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

如果 Ribbon 结构在 C++ 中已经明确，并且程序本身已有 QAction 命令层，推荐直接使用 QAction API。

## 文档

- [详细使用文档](./docs/USAGE.zh-CN.md)
- [Detailed usage guide](./docs/USAGE.md)
- [中文 JSON 示例](./example/resources/ribbon.zh-CN.json)
- [中文 Action 定义](./example/resources/actions.zh-CN.json)

详细文档包含 QAction 优先接入、状态同步、Tab / Group、Large / Small 布局、可勾选命令、Split Button、Quick Access、Application Menu、Context Tab、自定义 Widget、JSON、`RibbonAction`、样式、迁移方式和构建选项。

## 构建

Linux：

```bash
./build.sh
./build.sh Debug --clean
./build.sh Release --tests
./build.sh Release --run
./build.sh Release --shared
./build.sh Release --install
```

Windows PowerShell：

```powershell
.\build.ps1
.\build.ps1 -Configuration Debug -Clean
.\build.ps1 -Tests
.\build.ps1 -Run
.\build.ps1 -Shared
.\build.ps1 -Install
```

标准 CMake 同样支持：

```bash
cmake -S . -B build
cmake --build build
```

## 项目边界

RibbonLib 不提供 Dock Framework、MVVM Framework、Command Bus、Plugin System、大型 Theme Engine 或宿主业务架构。它只负责把 Ribbon 做好，业务逻辑和命令仍由宿主拥有。

## License

RibbonLib 使用 [MIT License](./LICENSE)。
