# RibbonLib

[English](./README.md) | 中文

RibbonLib 是一个基于 Qt Widgets 的极简 Ribbon 控件库，适合 CAD、BIM、工程设计和其他桌面工业软件。项目定位保持克制：把基础 Ribbon 做稳定、第三方接入足够简单，不发展成大型 UI 框架。

## 1. 环境要求

- CMake 3.16+
- C++17
- Qt 5.14.2+ 或 Qt 6
- Windows / Linux

RibbonLib 运行时不需要额外复制 QSS。默认样式已经编译进库，并且只作用于 RibbonLib 自己的控件。

## 2. 最简单的第三方接入

源码依赖时只需要：

```cmake
add_subdirectory(thirdparty/RibbonLib)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

代码中直接使用：

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

到这里就可以工作。第三方**不需要**：

- 复制 `ribbon.qss`；
- 用 `QFile` 加载 QSS；
- 调用 `QApplication::setStyleSheet()`；
- 调用 `Q_INIT_RESOURCE()`；
- 配置 Ribbon 资源目录；
- 初始化 ThemeManager。

## 3. 安装后使用

安装 RibbonLib：

```bash
./build.sh Release --install
```

第三方标准 CMake：

```cmake
find_package(RibbonLib CONFIG REQUIRED)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

C++ 使用方式与 `add_subdirectory` 完全一样。

## 4. 主要控件

统一公共头文件：

```cpp
#include <RibbonLib.h>
```

主要公开类型：

- `QRibbonWidget`：Ribbon 根控件、Tab、ApplicationButton、快捷访问；
- `QRibbonTab`：一个 Ribbon Tab；
- `QRibbonGroup`：Tab 内分组；
- `QRibbonButton`：Large / Small 按钮；
- `QRibbonSplitButton`：主命令 + 下拉菜单；
- `QApplicationButton`：File/Application 菜单按钮；
- `QRibbonMenu`：Ribbon 专用局部样式菜单；
- `RibbonAction`：可选的宿主命令抽象；
- `QRibbonHelper`：可选 JSON 构建器；
- `QRibbonStateManager`：轻量当前 Tab 状态保存。

## 5. 直接用 C++ 构建

一个比较实际的基础结构：

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

快捷访问栏固定在右侧。有图标的 QAction 只显示图标；没有图标时显示文字。Action 名称和说明仍通过 Tooltip 可见，因此常用命令保持紧凑，语言切换这类无图标动作仍然清晰可读。

## 6. JSON 配置

JSON 是可选能力，不影响直接使用 C++ API。

```cpp
QRibbonHelper helper;
if (!helper.loadFromResources(ribbonPath, actionsPath)) {
    qWarning() << helper.errorString();
    return;
}
helper.buildRibbon(ribbon);
```

### actions.json

Action 负责定义文字、提示、图标、快捷键和基础状态：

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
      "id": "file.save",
      "name": "保存",
      "icon": "icons/save.svg",
      "standardIcon": "save",
      "shortcut": "Ctrl+S"
    },
    {
      "id": "view.readOnly",
      "name": "只读",
      "icon": "qt:info",
      "checkable": true,
      "checked": false
    }
  ]
}
```

支持字段：

- `id`：稳定的命令 ID；
- `name`：显示文字；
- `description`：Tooltip；
- `icon`：自定义文件/Qt Resource 路径，或 `qt:<name>`；
- `standardIcon`：自定义图标缺失时使用的 Qt 原生 fallback；
- `shortcut`：例如 `Ctrl+S`，由 QAction 处理，但不会绘制在 Ribbon 按钮表面；
- `enabled`、`visible`、`checkable`、`checked`：QAction 基础状态。

相对图标路径以 `actions.json` 所在目录为基准；`actions.json` 放在 `:/...` Qt Resource 中时也一样。

### 图标规则

图标解析顺序保持简单：

1. `icon` 指向的自定义文件/资源存在时，使用自定义图标；
2. 自定义图标不存在时，使用 `standardIcon`；
3. `icon: "qt:save"` 表示直接使用 Qt 原生标准图标；
4. 最终都找不到时，退化成文字按钮，不使用问号占位图标。

内置常用别名包括：

`file`、`folder`、`home`、`open`、`save`、`close`、`apply`、`cancel`、`reset`、`help`、`info`、`warning`、`error`、`question`、`back`、`forward`、`up`、`down`、`reload`、`stop`、`play`、`pause`、`trash`、`settings`、`list`、`maximize`。

同时也接受对应的 Qt `SP_...` 名称，例如 `SP_DialogSaveButton`。

### ribbon.json

Ribbon 布局与 Action 定义分离：

```json
{
  "ribbon": {
    "applicationButton": {
      "title": "文件",
      "menu": [
        { "id": "file.open" },
        { "id": "file.save" }
      ]
    },
    "quickAccessBar": {
      "items": [
        { "id": "file.save" },
        { "id": "edit.undo" },
        { "id": "edit.redo" },
        { "id": "language.switch" }
      ]
    },
    "tabs": [
      {
        "id": "home",
        "title": "主页",
        "panels": [
          {
            "title": "文档",
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

`style` 有意只保留 `large` 和 `small` 两种，不增加复杂尺寸体系。

SplitButton：

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

同一个 Action 可以出现在多个位置，并且每个位置可通过 `params` 带独立参数，不会互相覆盖。

## 7. Action 与业务逻辑

RibbonLib 不接管宿主业务逻辑。最简单可以监听：

```cpp
connect(&helper, &QRibbonHelper::actionTriggered,
        this, [this](const QString &id) {
            // 在宿主程序里执行命令
        });
```

也可以继承 `RibbonAction` 并注册到 `QRibbonHelper`。

因此 RibbonLib 不依赖 Document、Viewport、CAD/BIM 模型、Command Bus 或其他应用框架。

## 8. 语言切换

RibbonLib 不强制任何国际化框架。

Example 使用最简单、最直观的双 JSON 方法：

```text
example/resources/actions.en.json
example/resources/actions.zh-CN.json
example/resources/ribbon.en.json
example/resources/ribbon.zh-CN.json
```

右上角快捷访问栏保留文字语言切换动作。点击后加载另一组 JSON 并重建 Example Ribbon，所以 Tab、Group、Action、菜单和 Tooltip 会一起切换。正式项目也可以继续使用 Qt `QTranslator`，RibbonLib 不限制宿主的国际化方案。

## 9. 样式

默认样式已内嵌，并且不会修改宿主 `QApplication` 的全局样式。

只修改 Ribbon 主体时：

```cpp
ribbon->setStyleSheet(myRibbonStyle);
```

Qt Popup Menu 是独立窗口；如果需要自定义菜单，只对相应 `QRibbonMenu` 或 Application Menu 设置局部 QSS，不要把 Ribbon 专用 QSS 设置给整个 `QApplication`。

## 10. 构建

Linux：

```bash
./build.sh
./build.sh Debug --clean
./build.sh Release --tests
./build.sh Release --run
./build.sh Release --shared
./build.sh Release --install
./build.sh Release --install --prefix /opt/RibbonLib
```

Windows PowerShell：

```powershell
.\build.ps1
.\build.ps1 -Configuration Debug -Clean
.\build.ps1 -Tests
.\build.ps1 -Run
.\build.ps1 -Shared
.\build.ps1 -Install
.\build.ps1 -Install -Prefix C:\RibbonLib
```

脚本会自动使用 `Qt5_DIR`、`Qt6_DIR`、`CMAKE_PREFIX_PATH`。

标准 CMake 同样支持：

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

## 11. Example 展示内容

Example 比最小第三方接入案例更完整，当前用于验证：

- Home / Insert / View / Tools / Help 多个 Tab；
- Application Menu；
- 右上角 Quick Access：常用命令用纯图标，语言切换保留文字；
- 常用演示命令使用 Qt 原生或 fallback 图标；
- 自定义图标不存在时回退 Qt 原生图标；
- Large / Small Button；
- SplitButton / Menu；
- Checkable Action；
- 中文 / English 切换；
- JSON Ribbon 配置；
- 普通 Qt Widgets 宿主命令分发。

运行：

```bash
./build.sh Release --clean --run
```

## 12. 项目边界

RibbonLib 不计划加入 Gallery Framework、Dock Framework、MVVM Framework、Command Bus、Plugin System 或大型 Theme Engine。

优先级始终是：稳定、第三方使用简单、样式隔离、布局可预测、Qt5/Qt6 兼容、维护成本低。
