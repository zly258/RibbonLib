# RibbonLib 详细使用文档

[README](../README.zh-CN.md) | [English](./USAGE.md)

本文档用于说明 RibbonLib 的稳定接入方式、公共 API 和布局规则。Example 适合做视觉验证，但正式项目应以公共 API 与本文档约定为准。

## 1. 设计目标

RibbonLib 有意保持轻量，只负责 Ribbon，不承担完整应用框架职责。

基本原则：

- 宿主负责业务逻辑、Document、Viewport 和应用状态；
- RibbonLib 负责 Ribbon 布局、命令呈现和局部样式；
- Ribbon 可以直接用 C++ 构建，也可以通过 JSON 构建；
- Ribbon 专用 QSS 必须局部生效，不要求修改整个 `QApplication`；
- 尺寸模型只保留 `Large` / `Small`；
- 布局指标固定可控，避免依赖不同平台 `sizeHint()` 的偶然差异；
- 命令 enabled / visible / checked 等状态优先由 `QAction` 或 `RibbonAction` 统一维护，不在多个 UI 控件中重复保存。

## 2. 环境要求

- CMake 3.16+
- C++17
- Qt 5.14.2+ 或 Qt 6
- Qt Widgets
- Windows / Linux

默认构建静态库，也支持共享库。

## 3. 集成方式

### 3.1 源码依赖

```cmake
add_subdirectory(thirdparty/RibbonLib)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

统一公共头文件：

```cpp
#include <RibbonLib.h>
```

宿主无需复制额外 Ribbon QSS。

### 3.2 安装后通过 `find_package` 使用

安装：

```bash
./build.sh Release --install
```

第三方：

```cmake
find_package(RibbonLib CONFIG REQUIRED)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

两种方式的 C++ API 完全一致。

## 4. 最小 C++ 示例

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

    auto *axes = new QRibbonButton(axesIcon,
                                   "坐标轴",
                                   QRibbonButtonSize::Small,
                                   document);
    axes->setCheckable(true);
    axes->setChecked(true);
    document->addButton(axes);

    connect(open, &QRibbonButton::clicked, this, &MainWindow::openDocument);
    connect(axes, &QRibbonButton::toggled, this, &MainWindow::setAxesVisible);
}
```

Ribbon 作为 `QMainWindow` 顶部区域使用时，推荐 `setMenuWidget(ribbon)`。Ribbon 本身仍然是普通 QWidget，如宿主有特殊布局也可自行嵌入。

## 5. 主要公共控件

### `QRibbonWidget`

Ribbon 根控件。常用 API：

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

它同时管理 Application Button、Quick Access Bar 与 Context Tab 显示信息。

### `QRibbonTab`

一个逻辑 Ribbon 页：

```cpp
auto *home = ribbon->addTab("主页", "home");
auto *document = home->addGroup("文档");
auto *view = home->addGroup("视图");
```

如果 Tab 需要通过代码选择、恢复状态或 JSON 管理，应使用稳定 `id`。

### `QRibbonGroup`

Tab 内的分组/面板。支持：

```cpp
group->addButton(button);
group->addSeparator();
group->addWidget(widget);
group->addLargeWidget(widget);
group->addSmallWidget(widget);
```

Small Widget 使用固定三行网格；Large Widget 使用大按钮区域。当前布局会为状态边框保留明确的上下安全距离，防止 checked / hover / pressed 底边被裁剪。

### `QRibbonButton`

只支持两种尺寸：

```cpp
QRibbonButtonSize::Large
QRibbonButtonSize::Small
```

常用状态：

```cpp
button->setEnabled(true);
button->setCheckable(true);
button->setChecked(true);
button->setShortcut(QKeySequence("Ctrl+S"));
```

信号：

```cpp
clicked();
toggled(bool checked);
```

Shortcut 属于命令元数据，不会直接绘制到 RibbonButton 表面。

### `QRibbonSplitButton`

主命令 + 下拉菜单：

```cpp
auto *menu = new QRibbonMenu(this);
auto *a = menu->addAction("方案 A");
auto *b = menu->addAction("方案 B");
menu->setDefaultAction(a);

auto *split = new QRibbonSplitButton(icon,
                                     "创建",
                                     QRibbonButtonSize::Large,
                                     group);
split->setMenu(menu);
split->setDefaultAction(a);
group->addLargeWidget(split);
```

主区域执行默认 QAction，箭头区域打开菜单。默认 QAction 改变后，SplitButton 会同步图标、文字和状态。

### `QApplicationButton`

左上角 File/Application 按钮：

```cpp
auto *menu = new QRibbonMenu(this);
menu->addAction("打开");
menu->addAction("保存");
menu->addSeparator();
menu->addAction("退出");

ribbon->applicationButton()->setText("文件");
ribbon->applicationButton()->setApplicationMenu(menu);
```

### `QRibbonMenu`

Ribbon 专用局部样式 `QMenu`。Ribbon 内菜单优先使用它，避免影响宿主程序其他菜单。

## 6. 按钮布局规则

### 6.1 Small Button

Small 控件按每列三行排列。RibbonLib 使用固定 Row Metrics，不让每个子控件依据自己的 `sizeHint()` 随意决定纵向位置。

三行区域上下均保留安全距离，因此第一行和第三行的 checked / hover / pressed 边框不会贴父控件边界。

Small 适合高频、简短、一行即可说明的命令。

### 6.2 Large Button

Large Button 的纵向空间明确分配给：

1. 32px 图标区域；
2. 最多两行文字；
3. 上下状态边框安全距离。

Large 按钮左右 padding 已保持较紧凑，避免同组多个大按钮之间过于松散。

### 6.3 Large 两行文字

Large Label 的规则：

- 最多 2 个视觉行；
- 第一处显式 `\n` 会被保留；
- 第一处之后的更多换行符会转换为空格；
- 没有显式换行时，可以自动换行；
- 自动换行优先按单词边界，必要时允许字符边界，所以中文可正常换行；
- Large Label 不使用 `...` 省略；
- 按钮宽度会根据文本计算，保证完整文字最多两行显示；
- Large `QRibbonSplitButton` 使用同一套文字布局逻辑。

示例：

```cpp
new QRibbonButton(icon, "AI 建模助手", QRibbonButtonSize::Large);
new QRibbonButton(icon, "AI 建模\n助手", QRibbonButtonSize::Large);
```

第一种允许库自动决定换行位置；第二种由调用方明确指定第一处换行。

不要在 Large Button 中设计三行命令名。如果两行仍不够，应缩短命令名称，把说明放在 Tooltip。

## 7. 可勾选命令

直接 C++：

```cpp
auto *wireframe = new QRibbonButton(icon,
                                    "线框显示",
                                    QRibbonButtonSize::Small,
                                    group);
wireframe->setCheckable(true);
wireframe->setChecked(false);
connect(wireframe, &QRibbonButton::toggled,
        this, &MainWindow::setWireframe);
group->addButton(wireframe);
```

如果程序本身已经使用 `QAction`，应优先让 `QAction` 成为 enabled / visible / checkable / checked 的唯一状态源。通过 JSON 构建时，`QRibbonHelper` 会负责 QAction 与 RibbonButton 状态同步。

checked、pressed、disabled 是不同视觉状态。默认 QSS 保证状态切换时按钮 box model 不变，避免勾选后尺寸发生跳动。

## 8. Quick Access Bar

直接增加 QAction：

```cpp
auto *save = new QAction(saveIcon, "保存", this);
ribbon->addAccessBarAction(save);
```

显示规则：

- QAction 有图标：Quick Access 只显示图标；
- QAction 没有图标：显示文字；
- QAction 的文字和说明仍可通过 Qt Tooltip/Action 信息保留。

这样保存/撤销/重做保持紧凑，而语言切换这类命令仍可只显示文字。

## 9. Context Tab

RibbonLib 支持上下文 Tab 信息：

```cpp
const int index = ribbon->indexOfTabId("tools");
ribbon->setTabContext(index, "上下文", QColor("#0078d4"));
```

清除：

```cpp
ribbon->clearTabContext(index);
```

RibbonLib 只负责上下文视觉信息。何时显示、激活这些 Tab 仍由宿主业务逻辑决定。

## 10. 自定义 Widget

Group 中如需加入 ComboBox、SpinBox 等宿主控件，可以使用：

```cpp
auto *combo = new QComboBox(group);
combo->addItems({"A", "B", "C"});
group->addSmallWidget(combo);
```

建议：

- 控件高度应适配目标 Large / Small 区域；
- 普通命令不要为了特殊布局而绕过 Large / Small 模型；
- 宿主控件样式保持局部；
- 使用 Qt SizePolicy，不要手写绝对 geometry。

## 11. JSON 模型

RibbonLib 把命令定义与布局分开：

- `actions.json`：命令 ID、文字、说明、图标、快捷键与状态；
- `ribbon.json`：Ribbon Tab、Group、按钮位置和尺寸。

这样命令身份不会与翻译文字或显示位置耦合。

### 11.1 `actions.json`

```json
{
  "actions": [
    {
      "id": "file.open",
      "name": "打开",
      "description": "打开文档",
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

字段：

| 字段 | 说明 |
| --- | --- |
| `id` | 稳定命令标识。 |
| `name` | 显示文字。 |
| `description` | Tooltip/说明。 |
| `icon` | 文件/资源路径或 `qt:<name>`。 |
| `standardIcon` | 自定义图标缺失时使用的 Qt 标准 fallback。 |
| `shortcut` | 例如 `Ctrl+O`。 |
| `enabled` | QAction 初始启用状态。 |
| `visible` | QAction 初始可见状态。 |
| `checkable` | 是否可勾选。 |
| `checked` | 可勾选 Action 的初始勾选状态。 |

不同语言版本中 `id` 必须保持不变，只翻译 `name` 和 `description`。

### 11.2 图标解析

解析顺序：

1. `icon` 指向有效自定义文件/Resource：使用自定义图标；
2. 自定义图标不存在：尝试 `standardIcon`；
3. `icon` 为 `qt:<name>`：直接使用 Qt 标准图标；
4. 最终没有图标：退化成文字 Action，不使用问号占位图标。

相对自定义图标路径以当前 `actions.json` 所在目录为基准，Qt Resource 路径同样支持。

常用标准别名：

`file`、`folder`、`home`、`open`、`save`、`close`、`apply`、`cancel`、`reset`、`help`、`info`、`warning`、`error`、`question`、`back`、`forward`、`up`、`down`、`reload`、`stop`、`play`、`pause`、`trash`、`settings`、`list`、`maximize`。

也支持 Qt `SP_...` 名称，例如 `SP_DialogSaveButton`。

### 11.3 `ribbon.json`

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
        { "id": "edit.redo" }
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

Placement 尺寸只支持 `large` / `small`，不增加复杂的中间尺寸体系。

### 11.4 JSON Split Button

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

### 11.5 Placement 参数

同一个命令可以在多个位置出现，每个位置使用独立 `params`：

```json
{ "id": "view.set", "style": "small", "params": "front" }
{ "id": "view.set", "style": "small", "params": "top" }
```

适合一个宿主命令处理一组同类操作。

## 12. `QRibbonHelper`

典型流程：

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

根据 ID 获取 QAction：

```cpp
QAction *save = helper.action("file.save");
```

简单命令分发：

```cpp
connect(&helper, &QRibbonHelper::actionTriggered,
        this, [this](const QString &id) {
            dispatchCommand(id);
        });
```

加载或构建失败时应检查 `errorString()`，正式程序不要静默忽略配置错误。

## 13. `RibbonAction`

`RibbonAction` 是可选命令抽象，适合宿主希望命令对象与具体 Document/View 解耦的情况。

```cpp
class OpenAction final : public RibbonAction
{
public:
    using RibbonAction::RibbonAction;

    QString id() const override { return "file.open"; }

    void execute(const QString &params) override
    {
        Q_UNUSED(params);
        // 宿主命令实现
    }

    void cleanup() override
    {
        // 需要时取消临时交互/预览
    }
};
```

注册：

```cpp
auto *openAction = new OpenAction(this);
openAction->setName("打开");
openAction->setIcon(openIcon);
helper.registerRibbonAction(openAction);
```

`RibbonAction::trigger()` 会先执行 validate，再进入 execute，并提供 executed / executeRejected 等通知。属性变化通过 `changed()` 通知 `QRibbonHelper` 同步 QAction/Button 状态。

如果宿主已有自己的 Command 系统，不需要为了使用 RibbonLib 强制改成 `RibbonAction`。

## 14. 状态保存

`QRibbonStateManager` 提供轻量 Ribbon 状态保存：

```cpp
QRibbonStateManager stateManager(ribbon, this);
stateManager.restoreState();
```

退出前：

```cpp
stateManager.saveState();
```

底层使用 `QSettings`。它不是完整 Workspace/Layout 持久化框架。

## 15. 样式

默认 QSS 已内嵌到 RibbonLib 并局部应用。

重要规则：除非宿主明确准备接管全局主题，否则不要把 RibbonLib QSS 放进 `QApplication::setStyleSheet()`。

仅修改 Ribbon：

```cpp
ribbon->setStyleSheet(myRibbonStyle);
```

Popup Menu 是独立 Qt Window，应对 `QRibbonMenu` / Application Menu 单独设置局部样式，不要写过宽的全局选择器。

自定义 QSS 时要保持 normal / hover / checked / pressed 的 border 宽度一致。推荐 normal 状态用透明边框，例如 `border: 1px solid transparent`，状态变化只改颜色，不要让按钮尺寸变化。

## 16. 国际化

RibbonLib 不强制翻译框架。

Example 使用两套 JSON：

```text
example/resources/actions.en.json
example/resources/actions.zh-CN.json
example/resources/ribbon.en.json
example/resources/ribbon.zh-CN.json
```

正式项目也可以用 `QTranslator` 并直接 C++ 构建 Ribbon。

命令 ID 不应随语言变化。

## 17. 构建选项

标准 CMake：

```bash
cmake -S . -B build \
  -DRIBBONLIB_BUILD_EXAMPLE=ON \
  -DRIBBONLIB_BUILD_TESTS=ON \
  -DRIBBONLIB_BUILD_SHARED=OFF \
  -DRIBBONLIB_ENABLE_INSTALL=ON
cmake --build build
```

主要选项：

| 选项 | 说明 |
| --- | --- |
| `RIBBONLIB_BUILD_EXAMPLE` | 构建 Example。 |
| `RIBBONLIB_BUILD_TESTS` | 构建 smoke tests，默认 OFF。 |
| `RIBBONLIB_BUILD_SHARED` | 构建动态库而不是静态库。 |
| `RIBBONLIB_ENABLE_INSTALL` | 启用安装/Package Target。 |

辅助脚本支持 `Qt5_DIR`、`Qt6_DIR`、`CMAKE_PREFIX_PATH`。

## 18. Example 与回归验证

Example 的 `Tests` 页用于覆盖容易回归的视觉场景：

- 第一行 Small checked；
- 第三行 Small checked；
- 多列 Small；
- Large checked / unchecked / disabled；
- Large 与 Small 混合；
- 长 Small 文本；
- Large 两行长文本；
- Large / Small Split Button；
- Qt 标准图标。

Smoke Test 还会对关键几何和状态同步做程序化验证。

## 19. 推荐规范

项目使用时建议：

- 命令 ID 使用稳定的小写点分隔形式，如 `file.open`、`view.axes`、`model.extrude`；
- Group 标题保持简短；
- 高频紧凑命令使用 Small，重要入口使用 Large；
- Large 标题控制在一到两行；
- 解释性内容放 Tooltip，不要塞进按钮文字；
- `QAction` 或 `RibbonAction` 作为命令状态唯一来源；
- Qt 标准图标能够清楚表达语义时优先使用，否则使用产品自己的专用图标；
- 宿主业务 Widget 与业务样式不要放入 RibbonLib；
- 普通本地 build 可以不跑测试，但 CI / 发布验证建议启用测试。

## 20. 常见问题

### 可勾选按钮边框被裁掉

不要在宿主 QSS 中强行覆盖 Ribbon Row/Button 固定高度。RibbonLib 已为 Small 三行和 Large Button 预留状态边框安全区，额外增大边框或固定高度可能破坏这些指标。

### 勾选后按钮跳动

normal 与 checked 必须保持相同 border 宽度。normal 使用透明边框，checked/hover 只改变颜色。

### Large 文本太宽

允许自动两行换行，或者在合适位置放一个显式 `\n`。不要通过增加空格控制按钮宽度。

### 需要第三行文字

缩短命令名。Large Button 有意限制最多两行。

### Quick Access 没显示文字

QAction 有图标时，Quick Access 有意采用 Icon Only。需要文字时不要给该 QAction 设置图标。

### 自定义图标不存在

检查绝对路径、Qt Resource 路径或相对 `actions.json` 路径；也可以提供 `standardIcon`，或者直接使用 `qt:<name>`。

### 宿主程序其他控件样式被影响

不要全局应用 RibbonLib QSS。RibbonLib 默认样式本身已经局部化并内嵌。

## 21. 项目边界

RibbonLib 不提供：

- Dock/Window 管理；
- Document/View 架构；
- MVVM Framework；
- Plugin Framework；
- 应用级 Command Bus；
- 大型 Theme System；
- CAD/BIM 业务对象。

这些职责应由宿主应用负责。

## 22. License

RibbonLib 使用 MIT License，详见 [`LICENSE`](../LICENSE)。
