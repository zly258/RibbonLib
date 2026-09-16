# RibbonLib 详细使用文档

[README](../README.zh-CN.md) | [English](./USAGE.md)

本文档是 RibbonLib 的稳定接入说明。当前推荐模型是 **QAction 优先**：宿主程序负责命令和业务状态，RibbonLib 负责 Ribbon 的显示、布局、局部样式和 DPI 行为。

## 1. 设计模型

RibbonLib 有意保持轻量，不承担宿主应用框架职责。

职责划分：

- 宿主负责业务逻辑、Document、Viewport 和命令执行；
- `QAction` 是推荐的公共命令模型；
- RibbonLib 负责 Tab、Group、Button、状态显示和布局；
- 同一个 `QAction` 可以复用于 Ribbon、菜单、工具栏和 Quick Access；
- enabled / visible / checked 等命令状态不要在多个 UI 控件中重复维护；
- Ribbon 样式保持局部，不要求修改整个 `QApplication`；
- 公共尺寸模型只保留 `Large` / `Small`；
- 推荐直接 C++ 构建，JSON 作为可选能力保留。

最终目标是让业务侧 Ribbon 代码主要只描述“有哪些命令、怎么分组”。

## 2. 环境要求

- CMake 3.16+
- C++17
- Qt 5.14.2+ 或 Qt 6
- Qt Widgets
- Windows / Linux

默认静态库，也支持共享库。

## 3. 集成方式

### 3.1 源码依赖

```cmake
add_subdirectory(thirdparty/RibbonLib)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

统一头文件：

```cpp
#include <RibbonLib.h>
```

### 3.2 安装后使用

```bash
./build.sh Release --install
```

```cmake
find_package(RibbonLib CONFIG REQUIRED)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

两种方式都不需要宿主复制 Ribbon QSS。

## 4. 推荐的 QAction 优先用法

### 4.1 业务命令只定义一次

```cpp
auto *openAction = new QAction(openIcon, tr("打开"), this);
openAction->setShortcut(QKeySequence::Open);
openAction->setToolTip(tr("打开文档"));
connect(openAction, &QAction::triggered,
        this, &MainWindow::openDocument);

auto *wireframeAction = new QAction(wireframeIcon, tr("线框"), this);
wireframeAction->setCheckable(true);
wireframeAction->setChecked(false);
connect(wireframeAction, &QAction::toggled,
        this, &MainWindow::setWireframe);
```

### 4.2 Ribbon 只描述结构

```cpp
auto *ribbon = new QRibbonWidget(this);
setMenuWidget(ribbon);

auto *home = ribbon->addTab(tr("主页"), "home");

home->addGroup(tr("文档"), {
    openAction,
    saveAction,
    closeAction
});

home->addGroup(tr("视图"), {
    wireframeAction,
    axesAction,
    perspectiveAction
});
```

`QRibbonTab::addGroup(title, actions)` 默认把传入 QAction 作为 Small Button 加入分组。

### 4.3 需要时显式指定尺寸

```cpp
auto *create = home->addGroup(tr("创建"));

create->addAction(newAction, QRibbonButtonSize::Large);
create->addActions({lineAction, circleAction, arcAction},
                   QRibbonButtonSize::Small);
```

这应该作为正式项目的默认写法。

## 5. QAction 绑定与状态同步

绑定到 QAction 的 `QRibbonButton` 会自动同步：

- `text`；
- `icon`；
- `enabled`；
- `visible`；
- `checkable`；
- `checked`；
- `shortcut`；
- `toolTip`；
- `statusTip`。

示例：

```cpp
auto *button = group->addAction(wireframeAction,
                                QRibbonButtonSize::Small);
```

之后只修改 QAction：

```cpp
wireframeAction->setEnabled(false);
wireframeAction->setChecked(true);
wireframeAction->setText(tr("线框显示"));
```

RibbonButton 会自动更新。

点击 RibbonButton 时会执行 `QAction::trigger()`。

同一个 Action 可以安全复用：

```cpp
group->addAction(saveAction, QRibbonButtonSize::Large);
ribbon->addAccessBarAction(saveAction);
fileMenu->addAction(saveAction);
```

不要再为同一个命令分别维护 RibbonButton、Menu Action、Quick Access Button 的状态。

## 6. 主要公共控件

### 6.1 `QRibbonWidget`

Ribbon 根控件：

```cpp
QRibbonTab *addTab(const QString &title,
                   const QString &id = QString());
void removeTab(int index);
void removeTab(QRibbonTab *tab);
void setCurrentTab(int index);
bool setCurrentTab(const QString &id);
QRibbonTab *currentTab() const;
int indexOfTabId(const QString &id) const;
int tabCount() const;
int currentIndex() const;
```

它同时负责 Application Button、Quick Access Bar 和 Context Tab 的显示。

### 6.2 `QRibbonTab`

基础写法：

```cpp
auto *home = ribbon->addTab(tr("主页"), "home");
auto *document = home->addGroup(tr("文档"));
```

快捷写法：

```cpp
home->addGroup(tr("文档"), {
    openAction,
    saveAction,
    closeAction
});
```

显式指定尺寸：

```cpp
home->addGroup(tr("创建"),
               {boxAction, cylinderAction, sphereAction},
               QRibbonButtonSize::Large);
```

如果需要程序化切换、状态恢复，应使用稳定 Tab ID。

### 6.3 `QRibbonGroup`

推荐的命令 API：

```cpp
QRibbonButton *addAction(QAction *action,
                         QRibbonButtonSize size = QRibbonButtonSize::Large);
void addActions(const QList<QAction*> &actions,
                QRibbonButtonSize size = QRibbonButtonSize::Small);
```

低层 API 仍保留：

```cpp
group->addButton(button);
group->addSeparator();
group->addWidget(widget);
group->addLargeWidget(widget);
group->addSmallWidget(widget);
```

普通业务命令已有 QAction 时，优先使用 `addAction/addActions`。

### 6.4 `QRibbonButton`

手动创建：

```cpp
auto *button = new QRibbonButton(icon,
                                 tr("自定义"),
                                 QRibbonButtonSize::Large,
                                 group);
```

直接绑定 QAction：

```cpp
auto *button = new QRibbonButton(saveAction,
                                 QRibbonButtonSize::Large,
                                 group);
```

也可以后绑定：

```cpp
button->setDefaultAction(saveAction);
```

读取当前 QAction：

```cpp
QAction *action = button->defaultAction();
```

没有绑定 QAction 时，原来的手动状态 API 仍然可用：

```cpp
button->setEnabled(true);
button->setCheckable(true);
button->setChecked(true);
button->setShortcut(QKeySequence("Ctrl+S"));
```

但普通应用命令推荐使用 QAction 绑定。

## 7. Large / Small 布局规则

### 7.1 Small Button

Small 控件按固定三行列布局。行高、间距和状态边框安全距离由 RibbonLib 自己控制。

Small 适合高频、简短、单行文字命令。

### 7.2 Large Button

Large Button 的垂直空间由 RibbonLib 分配给：

1. 图标区域；
2. 最多两行文字；
3. checked / hover / pressed 边框安全距离。

宿主不要手动设置按钮高度、图标尺寸或额外 margin。

### 7.3 Large 两行文字

规则：

- 最多两行；
- 第一处显式 `\n` 生效；
- 后续换行转换为空格；
- 没有显式换行时允许自动换行；
- 优先单词边界，必要时字符边界换行；
- 不用 `...` 截断；
- Large SplitButton 使用同一套规则。

## 8. 可勾选命令

推荐方式：

```cpp
auto *axesAction = new QAction(axesIcon, tr("坐标轴"), this);
axesAction->setCheckable(true);
axesAction->setChecked(true);

group->addAction(axesAction, QRibbonButtonSize::Small);
```

后续只修改 Action：

```cpp
axesAction->setChecked(false);
axesAction->setEnabled(document != nullptr);
```

同一个绑定命令不要同时调用 `action->setChecked()` 和 `button->setChecked()`。

## 9. Quick Access Bar

```cpp
ribbon->addAccessBarAction(saveAction);
ribbon->addAccessBarAction(undoAction);
ribbon->addAccessBarAction(redoAction);
```

显示策略：

- QAction 有图标：只显示图标；
- QAction 没图标：显示文字。

快捷键和状态仍由 QAction 统一维护。

## 10. Split Button 与菜单

```cpp
auto *menu = new QRibbonMenu(this);
auto *optionA = menu->addAction(tr("方案 A"));
auto *optionB = menu->addAction(tr("方案 B"));

menu->setDefaultAction(optionA);

auto *split = new QRibbonSplitButton(createIcon,
                                     tr("创建"),
                                     QRibbonButtonSize::Large,
                                     group);
split->setMenu(menu);
split->setDefaultAction(optionA);
group->addLargeWidget(split);
```

主区域执行默认 QAction，箭头区域打开菜单。默认 Action 的图标、文字、状态会自动同步。

Ribbon 内菜单优先使用 `QRibbonMenu`，避免样式污染宿主其他菜单。

## 11. Application Button

```cpp
auto *menu = new QRibbonMenu(this);
menu->addAction(openAction);
menu->addAction(saveAction);
menu->addSeparator();
menu->addAction(exitAction);

ribbon->applicationButton()->setText(tr("文件"));
ribbon->applicationButton()->setApplicationMenu(menu);
```

优先复用已有 QAction，不要为菜单重新创建一套命令。

## 12. Context Tab

```cpp
const int index = ribbon->indexOfTabId("tools");
ribbon->setTabContext(index, tr("工具"), QColor("#0078d4"));
```

清除：

```cpp
ribbon->clearTabContext(index);
```

RibbonLib 只处理显示，何时出现或激活由宿主业务决定。

## 13. 自定义 Widget

```cpp
auto *combo = new QComboBox(group);
combo->addItems({"A", "B", "C"});
group->addSmallWidget(combo);
```

建议：

- QAction 能表达的普通命令不要改成自定义 Widget；
- 使用 Qt SizePolicy，不要手写绝对 geometry；
- 自定义样式保持局部；
- 纵向槽位尺寸交给 RibbonLib。

## 14. JSON 构建

JSON 是可选能力。适合布局需要配置驱动、语言资源独立加载，或者现有项目已经稳定使用 `QRibbonHelper` 的情况。

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

`actions.json` 描述命令元数据，`ribbon.json` 描述布局。

最小 Action：

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

最小布局：

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
              { "id": "file.open", "style": "large" }
            ]
          }
        ]
      }
    ]
  }
}
```

尺寸仍有意只支持 `large` / `small`。

### 什么时候使用 JSON

适合：

- Ribbon 布局需要外部配置；
- 不同语言需要独立 Resource；
- 项目已经使用 `QRibbonHelper` 且稳定。

更适合直接 QAction C++ 的情况：

- 应用已经有 QAction 命令层；
- Ribbon 结构编译期明确；
- 更关注最少代码和易维护性。

## 15. `RibbonAction`

`RibbonAction` 继续作为可选抽象存在，适合希望使用独立命令对象，再由 `QRibbonHelper` 绑定到 QAction 的项目。

不要为了使用 RibbonLib 强行引入 `RibbonAction`。如果宿主已经有 QAction 或已有 Command 系统，继续使用现有架构即可。

## 16. 样式和 DPI

RibbonLib 内嵌默认 QSS，并局部作用于 Ribbon 控件。

宿主不应该：

- 复制 `ribbon.qss`；
- 调用 `Q_INIT_RESOURCE()` 初始化 RibbonLib 样式资源；
- 把 RibbonLib QSS 设置到整个 `QApplication`；
- 在业务代码重复 Ribbon Metrics；
- 为 125% / 150% DPI 单独修改按钮高度。

如果宿主有全局主题，应测试兼容性，但 Ribbon 专用修复应回到 RibbonLib，而不是分散到各业务项目。

## 17. 从旧按钮写法迁移

旧写法：

```cpp
auto *button = new QRibbonButton(saveIcon,
                                 tr("保存"),
                                 QRibbonButtonSize::Large,
                                 group);
button->setEnabled(canSave);
connect(button, &QRibbonButton::clicked,
        this, &MainWindow::saveDocument);
group->addButton(button);
```

推荐写法：

```cpp
auto *saveAction = new QAction(saveIcon, tr("保存"), this);
connect(saveAction, &QAction::triggered,
        this, &MainWindow::saveDocument);

group->addAction(saveAction, QRibbonButtonSize::Large);
```

状态更新只写：

```cpp
saveAction->setEnabled(canSave);
```

如果同一个命令还需要放到菜单或 Quick Access，继续复用这个 QAction。

## 18. 构建与测试

Linux：

```bash
./build.sh
./build.sh Release --tests
./build.sh Release --run
```

Windows：

```powershell
.\build.ps1
.\build.ps1 -Tests
.\build.ps1 -Run
```

标准 CMake：

```bash
cmake -S . -B build -DRIBBONLIB_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

主要选项：

- `RIBBONLIB_BUILD_EXAMPLE`
- `RIBBONLIB_BUILD_TESTS`
- `RIBBONLIB_BUILD_SHARED`
- `RIBBONLIB_ENABLE_INSTALL`

## 19. 正式项目推荐规则

- 优先让 QAction 成为命令状态唯一来源；
- Ribbon 代码只描述结构，不承载业务逻辑；
- Ribbon / Menu / Quick Access 复用同一个 QAction；
- 尺寸只使用 `Large` / `Small`；
- JSON 保持可选，不作为必经路径；
- Ribbon 专用样式和 DPI 修复放在 RibbonLib 内；
- 只有 QAction 无法表达的控件才使用自定义 Widget；
- 不向 RibbonLib 引入宿主应用架构。

按这套方式使用后，RibbonLib 的接入代码会保持很薄，也更适合多个工业软件项目共享升级。
