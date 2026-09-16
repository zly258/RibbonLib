# RibbonLib

[English](./README.md) | 中文

RibbonLib 是一个面向 Qt Widgets 的轻量 Ribbon 控件库，适合 CAD、BIM、工程设计及其他桌面工业软件。推荐采用 **QAction-first**：宿主维护命令和业务状态，RibbonLib 负责 Ribbon 的显示、布局、DPI 与局部样式。

## 主要特点

- Qt 5.14.2+ 与 Qt 6。
- C++17，CMake 3.16+。
- Windows / Linux。
- `QAction` 驱动，自动同步 enabled / visible / checkable / checked / shortcut 等状态。
- Large / Small 两种 Ribbon Button。
- Split Button、Application Menu、Quick Access、Context Tab。
- RibbonLib 自带局部 QSS，第三方无需复制 `ribbon.qss`。
- 推荐 C++ 直接构建；JSON 保留为可选能力。

## 推荐用法

```cpp
auto *openAction = new QAction(openIcon, tr("打开"), this);
connect(openAction, &QAction::triggered,
        this, &MainWindow::openDocument);

auto *ribbon = new QRibbonWidget(this);
setMenuWidget(ribbon);

auto *home = ribbon->addTab(tr("主页"), "home");
auto *document = home->addGroup(tr("文档"));
document->addAction(openAction, QRibbonButtonSize::Large);
```

多个 Small 命令可以直接：

```cpp
home->addGroup(tr("编辑"), {
    undoAction,
    redoAction,
    findAction
});
```

## Ribbon 专用显示文字

`QAction::text()` 仍然是菜单、Quick Access、右键菜单等位置共用的业务命令名称。Ribbon 如果需要不同的展示文字，应使用 Ribbon-only display text，不要修改 QAction：

```cpp
auto *button = document->addAction(modelSettingsAction,
                                   QRibbonButtonSize::Large);
button->setDisplayText(tr("模型\n设置"));
```

也可以直接：

```cpp
document->addAction(modelSettingsAction,
                    QRibbonButtonSize::Large,
                    tr("模型\n设置"));
```

Large Button **不再自动换行**。只有 display text 明确包含 `\n` 时才显示两行，最多两行；没有手动换行时，按钮按完整文字自然扩宽。

按钮宽度不再使用固定最小宽度。实际宽度只由图标、文字、必要 padding，以及 Split Button 的下拉区域决定。Group 仍会为了完整显示分组标题而适当变宽。

## Split Button

Split Button 同样支持 QAction-first：

```cpp
auto *menu = new QRibbonMenu(group);
menu->addAction(createPlateAction);
menu->addAction(createBeamAction);
menu->addAction(createColumnAction);

auto *split = group->addSplitAction(createPlateAction,
                                    menu,
                                    QRibbonButtonSize::Large,
                                    tr("创建\n构件"));
```

主区域触发当前 default `QAction`；下拉区域显示菜单。QAction 状态自动同步，Ribbon-only display text 与 QAction 名称相互独立。

## Quick Access

```cpp
ribbon->addAccessBarAction(saveAction);
ribbon->addAccessBarAction(undoAction);
ribbon->addAccessBarAction(redoAction);
```

有图标时 Quick Access 只显示图标；没有图标时显示文字。

## JSON

JSON 构建仍然支持，但它是可选能力，不再作为最简单、最推荐的默认接入方式。

## 文档

- [详细使用文档](./docs/USAGE.zh-CN.md)
- [Detailed usage guide](./docs/USAGE.md)

## 构建

```bash
./build.sh Release --clean --run
```

```powershell
.\build.ps1 -Configuration Release -Clean -Run
```

## 项目边界

RibbonLib 不提供 Dock Framework、MVVM Framework、Command Bus、Plugin System 或大型 Theme Engine。它只负责 Ribbon，本身不接管宿主业务架构。

## License

RibbonLib 使用 [MIT License](./LICENSE)。
