# RibbonLib 详细使用文档

[README](../README.zh-CN.md) | [English](./USAGE.md)

RibbonLib 是一个专注于 Qt Widgets Ribbon 的轻量控件库。推荐采用 **QAction-first**：应用负责命令与业务状态，RibbonLib 负责 Ribbon 的显示、布局、几何与局部样式。

## 1. 集成

```cmake
add_subdirectory(thirdparty/RibbonLib)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

或安装后：

```cmake
find_package(RibbonLib CONFIG REQUIRED)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

统一头文件：

```cpp
#include <RibbonLib.h>
```

第三方不需要复制 RibbonLib 的 QSS，也不要把 RibbonLib 样式设置到整个 `QApplication`。

## 2. QAction-first 构建

业务命令只定义一次：

```cpp
auto *openAction = new QAction(openIcon, tr("打开"), this);
openAction->setShortcut(QKeySequence::Open);
connect(openAction, &QAction::triggered,
        this, &MainWindow::openDocument);
```

Ribbon 只描述结构：

```cpp
auto *ribbon = new QRibbonWidget(this);
setMenuWidget(ribbon);

auto *home = ribbon->addTab(tr("主页"), "home");
auto *document = home->addGroup(tr("文档"));
document->addAction(openAction, QRibbonButtonSize::Large);
document->addActions({saveAction, closeAction}, QRibbonButtonSize::Small);
```

也可以直接创建紧凑分组：

```cpp
home->addGroup(tr("编辑"),
               {undoAction, redoAction, findAction},
               QRibbonButtonSize::Small);
```

RibbonButton 会从绑定的 QAction 自动同步 `text`、`icon`、`enabled`、`visible`、`checkable`、`checked`、`shortcut`、`toolTip`、`statusTip`。点击 RibbonButton 会触发同一个 QAction。

## 3. Ribbon 专用 displayText

`QAction::text()` 应保持为干净的业务命令名称，因为同一个 QAction 可能同时出现在菜单、Quick Access、右键菜单等位置。

Ribbon 如果需要独立的显示文字，可以覆盖 display text：

```cpp
auto *button = group->addAction(settingsAction,
                                QRibbonButtonSize::Large);
button->setDisplayText(tr("模型\n设置"));
```

等价的快捷写法：

```cpp
group->addAction(settingsAction,
                 QRibbonButtonSize::Large,
                 tr("模型\n设置"));
```

使用 `clearDisplayText()` 可以恢复显示 `QAction::text()`。

## 4. Large 文字规则：只允许显式换行

Large Button 不再自动换行。

规则：

- 没有 `\n`：始终一行；
- 一个显式 `\n`：显示两行；
- 更多换行会归一化为空格；
- Small Button 始终单行；
- Large 长文字不会由库自动拆分，而是自然增加按钮宽度；
- 需要两行时，用 Ribbon-only displayText 明确指定换行位置。

这样中文、英文、不同字体和 DPI 下的布局都更可预测。

## 5. 宽度计算

Ribbon Button 不再有固定最小宽度。

普通 Button 的实际宽度来自：

- 图标宽度；
- displayText 实际宽度；
- 图标与文字同时存在时的间距；
- 水平 padding。

Split Button 额外保留下拉箭头区域。

因此短命令不会被固定最小宽度无意义撑大，长命令也会得到真正需要的宽度。`QRibbonGroup` 为了完整显示 Group 标题，仍可能比按钮内容更宽。

## 6. Split Button QAction-first

菜单继续直接使用已有 QAction：

```cpp
auto *menu = new QRibbonMenu(group);
menu->addAction(createPlateAction);
menu->addAction(createBeamAction);
menu->addAction(createColumnAction);
```

然后直接用 default QAction 创建 Split Button：

```cpp
auto *split = group->addSplitAction(createPlateAction,
                                    menu,
                                    QRibbonButtonSize::Large,
                                    tr("创建\n构件"));
```

也可以直接创建控件：

```cpp
auto *split = new QRibbonSplitButton(createPlateAction,
                                     menu,
                                     QRibbonButtonSize::Large,
                                     group);
```

主区域触发 `defaultAction()`；箭头区域展开菜单。Default QAction 改变后，图标和状态会自动同步；Ribbon-only displayText 不会污染 `QAction::text()`。

`setDefaultAction()` 现在不要求必须先设置菜单，因此命令绑定与菜单设置可以独立完成。

## 7. Large / Small 布局

Small 控件按固定三行布局。Large 控件占完整命令高度。高度、图标尺寸、纵向间距继续由 RibbonLib 控制，只有宽度按真实内容计算。

Large 用于视觉层级较高的命令，Small 用于紧凑命令组。不要为了“让按钮更宽”而滥用 Large。

## 8. 可勾选状态

推荐 QAction 作为唯一状态源：

```cpp
auto *wireframe = new QAction(wireframeIcon, tr("线框"), this);
wireframe->setCheckable(true);
wireframe->setChecked(false);

group->addAction(wireframe, QRibbonButtonSize::Small);
```

通过菜单、快捷键或业务代码改变 QAction 状态后，Ribbon 会自动更新。

## 9. Quick Access

```cpp
ribbon->addAccessBarAction(saveAction);
ribbon->addAccessBarAction(undoAction);
ribbon->addAccessBarAction(redoAction);
```

有图标时 Quick Access 只显示图标；没有图标时显示文字。

## 10. Application Menu

```cpp
auto *menu = new QRibbonMenu(ribbon);
menu->addAction(openAction);
menu->addAction(saveAction);
ribbon->applicationButton()->setApplicationMenu(menu);
```

同一个 QAction 可以同时用于 Ribbon Group、Application Menu 和 Quick Access。

## 11. 自定义 Widget

`addWidget`、`addLargeWidget`、`addSmallWidget` 继续保留，用于 ComboBox、SpinBox 等非命令控件。普通命令应优先使用 `addAction()` / `addActions()`。

## 12. JSON

JSON 构建仍通过 `QRibbonHelper` 支持，但它只是可选能力。宿主已经有命令层时，C++ + QAction 更简单直接。

```cpp
QRibbonHelper helper;
if (!helper.loadFromResources(ribbonPath, actionsPath)) {
    qWarning() << helper.errorString();
    return;
}
helper.buildRibbon(ribbon);
```

## 13. RibbonAction

`RibbonAction` 继续作为可选抽象存在。如果宿主已经使用 QAction 或自己的 Command 系统，不需要为了 RibbonLib 强行采用 RibbonAction。

## 14. 样式与 DPI

RibbonLib 负责自身局部 QSS 和确定性的纵向布局指标。第三方不要复制 QSS、手工调整按钮高度，也不要在业务层针对 125% / 150% 缩放做 Ribbon 补丁。

## 15. 迁移

旧写法：

```cpp
auto *button = new QRibbonButton(icon, tr("打开"), QRibbonButtonSize::Large, group);
connect(button, &QRibbonButton::clicked, this, &MainWindow::openDocument);
group->addButton(button);
```

推荐写法：

```cpp
auto *openAction = new QAction(icon, tr("打开"), this);
connect(openAction, &QAction::triggered, this, &MainWindow::openDocument);
group->addAction(openAction, QRibbonButtonSize::Large);
```

如果 Ribbon 需要手动两行文字，使用 `setDisplayText()`，不要把 `\n` 写进 `QAction::text()`。

## 16. 项目边界

RibbonLib 保持克制：不提供 Dock Framework、应用级 Command Bus、Plugin Framework、MVVM Framework 或全局 Theme Engine。
