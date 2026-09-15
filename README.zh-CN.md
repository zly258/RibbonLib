# RibbonLib

[English](./README.md) | 中文

RibbonLib 是一个面向 Qt Widgets 的轻量 Ribbon 控件库，适合 CAD、BIM、工程设计及其他桌面工业软件。项目重点是布局可预测、第三方接入简单、样式隔离、公共 API 克制，而不是扩展成大型 UI 框架。

## 主要特点

- 支持 Qt 5.14.2+ 与 Qt 6。
- C++17，CMake 3.16+。
- 支持 Windows / Linux。
- Large / Small 两种 Ribbon 按钮。
- 支持可勾选按钮及 checked / pressed / disabled 状态。
- Large 按钮支持最多两行文字，不使用省略号折叠。
- Split Button 与 Ribbon Menu。
- Application Menu 与右侧 Quick Access Bar。
- Context Tab。
- 支持直接 C++ 构建，也支持可选 JSON 构建。
- 支持 `qt:<name>` Qt 原生标准图标。
- 默认 QSS 内嵌并且只作用于 RibbonLib，不污染宿主全局样式。
- 提供可选的状态保存与 `RibbonAction` 命令抽象。

## 快速接入

### 源码依赖

```cmake
add_subdirectory(thirdparty/RibbonLib)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

```cpp
#include <RibbonLib.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    auto *ribbon = new QRibbonWidget(this);
    setMenuWidget(ribbon);

    auto *home = ribbon->addTab("主页", "home");
    auto *document = home->addGroup("文档");

    auto *open = new QRibbonButton(openIcon,
                                   "打开",
                                   QRibbonButtonSize::Large,
                                   document);
    document->addButton(open);

    connect(open, &QRibbonButton::clicked, this, &MainWindow::openDocument);
}
```

到这里即可使用。第三方不需要复制 `ribbon.qss`、调用 `Q_INIT_RESOURCE()`、增加 ThemeManager，也不需要把 RibbonLib 样式设置给整个 `QApplication`。

### 安装后使用

```bash
./build.sh Release --install
```

```cmake
find_package(RibbonLib CONFIG REQUIRED)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

## 文档

README 只保留项目概览与最常用接入方式。完整说明见：

- [详细使用文档](./docs/USAGE.zh-CN.md)
- [Detailed usage guide](./docs/USAGE.md)
- [中文 Ribbon 示例配置](./example/resources/ribbon.zh-CN.json)
- [中文 Action 定义](./example/resources/actions.zh-CN.json)

详细文档包含：C++ 直接构建、Tab / Group、按钮布局规则、Large 两行文字、可勾选按钮、Split Button、Quick Access、Application Menu、Context Tab、自定义 Widget、JSON 规范、图标 fallback、`RibbonAction`、样式、构建选项和常见问题。

## Large 按钮文字规则

Large Button 使用固定纵向布局，并且最多显示两行文字。

- 文本中包含 `\n` 时，第一处换行符作为显式换行。
- 没有 `\n` 时，根据按钮宽度自动换行。
- 自动换行优先按单词边界，必要时允许字符边界换行，因此中文同样可正常处理。
- Large Button 不使用 `...` 截断标题。
- 按钮宽度会根据文本计算，保证最多两行可完整显示。
- 第一处之后的额外换行符会被转为空格，避免出现第三行。
- Small Button 仍保持紧凑的单行布局。

该规则同时适用于 `QRibbonButton` 与 Large `QRibbonSplitButton`。

## JSON 构建

JSON 是可选能力，直接 C++ 使用不依赖 JSON。

```cpp
QRibbonHelper helper;
if (!helper.loadFromResources(ribbonPath, actionsPath)) {
    qWarning() << helper.errorString();
    return;
}

helper.buildRibbon(ribbon);
```

最小 `actions.json`：

```json
{
  "actions": [
    {
      "id": "file.open",
      "name": "打开",
      "description": "打开文档",
      "icon": "qt:open",
      "shortcut": "Ctrl+O"
    },
    {
      "id": "view.axes",
      "name": "坐标轴",
      "icon": "qt:apply",
      "checkable": true,
      "checked": true
    }
  ]
}
```

最小 `ribbon.json`：

```json
{
  "ribbon": {
    "tabs": [
      {
        "id": "home",
        "title": "主页",
        "panels": [
          {
            "title": "文档",
            "items": [
              { "id": "file.open", "style": "large" },
              { "id": "view.axes", "style": "small" }
            ]
          }
        ]
      }
    ]
  }
}
```

`style` 有意只保留 `large` 和 `small` 两种。

## Qt 原生图标

JSON 中使用 `qt:<name>` 即可使用 Qt 标准图标。实际外观跟随当前 Qt 平台 Style，因此 Windows 下会得到与当前 Qt / Windows 风格一致的图标，不需要额外维护一套图标资源。

常用别名：

`file`、`folder`、`home`、`open`、`save`、`close`、`apply`、`cancel`、`reset`、`help`、`info`、`warning`、`error`、`question`、`back`、`forward`、`up`、`down`、`reload`、`stop`、`play`、`pause`、`trash`、`settings`、`list`、`maximize`。

同时支持 Qt `SP_...` 名称，例如 `SP_DialogSaveButton`。

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

也可以直接使用标准 CMake：

```bash
cmake -S . -B build
cmake --build build
```

测试默认不参与普通构建：

```bash
cmake -S . -B build -DRIBBONLIB_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

主要 CMake 选项：

- `RIBBONLIB_BUILD_EXAMPLE`
- `RIBBONLIB_BUILD_TESTS`
- `RIBBONLIB_BUILD_SHARED`
- `RIBBONLIB_ENABLE_INSTALL`

## Example

Example 不只是最小演示，还承担布局回归检查。`Tests` 页目前覆盖：

- 三行 Small 可勾选按钮；
- Large checked / unchecked / disabled 状态；
- Large / Small 混合布局；
- 长文字与 Large 两行文字；
- Split Button；
- Qt 原生图标；
- 中英文切换。

运行：

```bash
./build.sh Release --clean --run
```

## 项目边界

RibbonLib 不提供 Dock Framework、MVVM Framework、Command Bus、Plugin System、大型 Theme Engine 或宿主应用架构。它只负责把 Ribbon 本身做好，并允许大型工业软件以较低成本嵌入。

## License

RibbonLib 使用 [MIT License](./LICENSE)。
