# RibbonLib 详细使用文档

[README](../README.zh-CN.md) | [English](./USAGE.md)

RibbonLib 的核心原则只有一个：宿主应用通过 `QAction` 维护命令与状态，RibbonLib 只负责把这些命令呈现为 Ribbon。

## 1. 环境要求

- CMake 3.16+
- C++17
- Qt 5.14.2+ 或 Qt 6
- Qt Widgets
- Windows / Linux

## 2. 集成

源码依赖：

```cmake
add_subdirectory(thirdparty/RibbonLib)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

安装后：

```cmake
find_package(RibbonLib CONFIG REQUIRED)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

统一头文件：

```cpp
#include <RibbonLib.h>
```

第三方不需要复制 Ribbon QSS。

## 3. QAction-first 构建

先创建应用自己的 QAction：

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

然后只描述 Ribbon 结构：

```cpp
auto *ribbon = new QRibbonWidget(this);
setMenuWidget(ribbon);

auto *home = ribbon->addTab(tr("主页"), "home");

auto *document = home->addGroup(tr("文档"));
document->addAction(openAction, QRibbonButtonSize::Large);

auto *view = home->addGroup(tr("视图"));
view->addActions({axesAction, wireframeAction, perspectiveAction},
                 QRibbonButtonSize::Small);
```

Ribbon 按钮自动同步 QAction 的文字、图标、enabled、visible、checkable、checked、shortcut、tooltip 和 status tip。点击 Ribbon 按钮就是触发同一个 QAction。

## 4. Tab 与 Group

```cpp
auto *home = ribbon->addTab(tr("主页"), "home");
auto *create = home->addGroup(tr("创建"));
auto *modify = home->addGroup(tr("修改"));
```

一组 Small 命令可直接：

```cpp
home->addGroup(tr("编辑"),
               {undoAction, redoAction, findAction},
               QRibbonButtonSize::Small);
```

Tab 使用稳定 ID 后可直接选择：

```cpp
ribbon->setCurrentTab("home");
```

## 5. Large 与 Small

RibbonLib 只保留两种命令尺寸：

```cpp
QRibbonButtonSize::Large
QRibbonButtonSize::Small
```

Large 使用 32 px 图标区域，文字在下方。Small 按钮按每列三行排列。

按钮宽度完全按内容计算，不设置 Large、Small 或 Split 的固定最小宽度。实际宽度来自图标、文字、padding，以及 Split Button 的箭头区域。

## 6. Large 显式显示文字

Large 不自动换行。没有显式换行时保持单行，并自动扩宽到足够显示完整文字。

需要两行时使用 Ribbon 专用 `displayText`：

```cpp
auto *button = group->addAction(settingsAction,
                                QRibbonButtonSize::Large);
button->setDisplayText(tr("模型\n设置"));
```

或者：

```cpp
group->addAction(settingsAction,
                 QRibbonButtonSize::Large,
                 tr("模型\n设置"));
```

这不会修改 `QAction::text()`，所以菜单、右键菜单和其他 UI 仍然显示正常的命令名。

只保留第一处显式换行；更多换行会转换为空格。Small 始终保持单行。

恢复 QAction 原始文字：

```cpp
button->clearDisplayText();
```

## 7. 可勾选命令

勾选状态放在 QAction：

```cpp
auto *wireframeAction = new QAction(icon, tr("线框"), this);
wireframeAction->setCheckable(true);
wireframeAction->setChecked(false);

group->addAction(wireframeAction, QRibbonButtonSize::Small);
```

业务层不要再为 Ribbon 控件维护第二份 checked 状态。

## 8. Split Button

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

主区域执行当前默认 QAction，箭头打开菜单。选择菜单项后，该 QAction 成为新的默认动作。可选 `displayText` 始终只控制 Ribbon 显示。

也可以直接创建：

```cpp
auto *split = new QRibbonSplitButton(defaultAction,
                                     menu,
                                     QRibbonButtonSize::Large,
                                     group);
```

## 9. Application Menu

```cpp
auto *menu = new QRibbonMenu(ribbon);
menu->addAction(openAction);
menu->addAction(saveAction);
menu->addSeparator();
menu->addAction(closeAction);

ribbon->applicationButton()->setText(tr("文件"));
ribbon->applicationButton()->setApplicationMenu(menu);
```

## 10. Quick Access Bar

```cpp
ribbon->addAccessBarAction(saveAction);
ribbon->addAccessBarAction(undoAction);
ribbon->addAccessBarAction(redoAction);
```

有图标的 QAction 在 Quick Access 中只显示图标；没有图标时显示文字。

## 11. Context Tab

```cpp
const int index = ribbon->indexOfTabId("tools");
ribbon->setTabContext(index, tr("上下文"), QColor("#0078d4"));
```

清除：

```cpp
ribbon->clearTabContext(index);
```

何时出现和激活由宿主业务逻辑决定。

## 12. 自定义 Widget

ComboBox、SpinBox 等特殊控件可以：

```cpp
group->addSmallWidget(comboBox);
group->addLargeWidget(customWidget);
```

普通命令仍应优先使用 `addAction()`。

## 13. 可选 JSON 加载

`QRibbonHelper` 仅用于确实需要数据驱动 UI 的项目，不是普通项目的必经路径。

Action 元数据和 Ribbon Layout 独立加载：

```cpp
QRibbonHelper helper;

if (!helper.loadActions(":/ui/actions.json")) {
    qWarning() << helper.errorString();
}
if (!helper.loadLayout(":/ui/ribbon.json")) {
    qWarning() << helper.errorString();
}
helper.buildRibbon(ribbon);
```

最小 actions JSON：

```json
{
  "actions": [
    {
      "id": "file.open",
      "name": "打开",
      "description": "打开文档",
      "icon": "qt:open",
      "shortcut": "Ctrl+O"
    }
  ]
}
```

最小 layout JSON：

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

Placement 字段保持极简：`id`、`style`、可选 `displayText`、可选 `menu`。`style` 只有 `large` 和 `small`。

显式两行：

```json
{
  "id": "tools.settings",
  "style": "large",
  "displayText": "应用\n设置"
}
```

## 14. JSON 中的 Qt 标准图标

Action 元数据中使用 `qt:<name>`。常用别名：

`file`、`folder`、`home`、`open`、`save`、`close`、`apply`、`cancel`、`reset`、`help`、`info`、`warning`、`error`、`question`、`back`、`forward`、`up`、`down`、`reload`、`stop`、`play`、`pause`、`trash`、`settings`、`list`、`maximize`。

也支持 RibbonLib 已映射的 Qt `SP_...` 名称。

## 15. 样式与 DPI

RibbonLib 内嵌自身 QSS，并只对 Ribbon 控件局部生效。第三方无需复制 QSS、无需 `Q_INIT_RESOURCE()`，也不要把 Ribbon 样式应用到整个 `QApplication`。

Ribbon 的高度、图标尺寸、Small 三行布局、状态边框和 Group 间距都由库内部控制。业务项目不应针对 125%/150% 单独修补这些数值。

## 16. 构建

Linux：

```bash
./build.sh
./build.sh Debug --clean
./build.sh Release --run
./build.sh Release --shared
./build.sh Release --install
```

Windows：

```powershell
.\build.ps1
.\build.ps1 -Configuration Debug -Clean
.\build.ps1 -Run
.\build.ps1 -Shared
.\build.ps1 -Install
```

CMake 选项：

- `RIBBONLIB_BUILD_EXAMPLE`
- `RIBBONLIB_BUILD_SHARED`
- `RIBBONLIB_ENABLE_INSTALL`

## 17. 推荐边界

RibbonLib 应保持为纯 UI 呈现库。Document 状态、应用设置、命令路由、插件和业务逻辑都放在宿主中。如果现有 `QAction` 已经能表示一个命令，就直接复用它，不再引入额外命令抽象。
