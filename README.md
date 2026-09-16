# RibbonLib

English | [中文](./README.zh-CN.md)

RibbonLib is a compact Qt Widgets Ribbon library for CAD, BIM, engineering and other desktop applications. The recommended integration model is **QAction first**: the host owns commands and state, while RibbonLib owns Ribbon presentation, layout, DPI behavior and local styling.

## Highlights

- Qt 5.14.2+ and Qt 6.
- C++17 and CMake 3.16+.
- Windows and Linux.
- `QAction`-first construction with automatic state synchronization.
- Large / Small Ribbon buttons.
- Checkable, enabled, visible and shortcut state synchronized from `QAction`.
- Split buttons and Ribbon menus.
- Application menu, Quick Access bar and context tabs.
- Embedded Ribbon-scoped QSS; consumers do not copy or load `ribbon.qss`.
- Direct C++ construction is the recommended path; JSON remains optional.
- Optional `RibbonAction` abstraction and state persistence.

## Recommended usage

```cmake
add_subdirectory(thirdparty/RibbonLib)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

```cpp
#include <RibbonLib.h>

auto *openAction = new QAction(openIcon, tr("Open"), this);
connect(openAction, &QAction::triggered,
        this, &MainWindow::openDocument);

auto *ribbon = new QRibbonWidget(this);
setMenuWidget(ribbon);

auto *home = ribbon->addTab(tr("Home"), "home");
auto *document = home->addGroup(tr("Document"));
document->addAction(openAction, QRibbonButtonSize::Large);
```

For multiple compact commands:

```cpp
home->addGroup(tr("Edit"), {
    undoAction,
    redoAction,
    findAction
});
```

## Ribbon display text

`QAction::text()` remains the command name shared by menus, Quick Access and other Qt UI. Ribbon-only presentation text can be overridden without modifying the action:

```cpp
auto *button = document->addAction(modelSettingsAction,
                                   QRibbonButtonSize::Large);
button->setDisplayText(tr("Model\nSettings"));
```

Or directly:

```cpp
document->addAction(modelSettingsAction,
                    QRibbonButtonSize::Large,
                    tr("Model\nSettings"));
```

Large buttons **do not wrap automatically**. A line break appears only when the Ribbon display text explicitly contains `\n`. At most two lines are displayed. Without an explicit break, the button grows to the natural text width.

Button width has no fixed minimum width. Width is derived from actual icon/text content, required padding and, for split buttons, the dropdown area. Group width may still be wider when required by the group title.

## Split buttons

Split buttons can also be QAction-first:

```cpp
auto *menu = new QRibbonMenu(group);
menu->addAction(createPlateAction);
menu->addAction(createBeamAction);
menu->addAction(createColumnAction);

auto *split = group->addSplitAction(createPlateAction,
                                    menu,
                                    QRibbonButtonSize::Large,
                                    tr("Create\nMember"));
```

The primary area triggers the current default `QAction`; the dropdown uses the supplied menu. Action state is synchronized automatically, while Ribbon-only display text remains independent.

## Quick Access

The same actions can be reused:

```cpp
ribbon->addAccessBarAction(saveAction);
ribbon->addAccessBarAction(undoAction);
ribbon->addAccessBarAction(redoAction);
```

If an action has an icon, Quick Access shows the icon only. If it has no icon, text is shown.

## Styling

Consumers do not need to copy `ribbon.qss`, call `Q_INIT_RESOURCE()` for RibbonLib resources, install a theme manager, apply RibbonLib QSS to the whole `QApplication`, or manually fix Ribbon button height / icon size / group spacing.

## JSON construction

JSON remains supported for applications that need data-driven Ribbon definitions, but it is optional and not the recommended default integration path.

## Documentation

- [Detailed usage guide](./docs/USAGE.md)
- [详细使用文档](./docs/USAGE.zh-CN.md)

## Build

```bash
./build.sh
./build.sh Release --clean --run
```

```powershell
.\build.ps1
.\build.ps1 -Configuration Release -Clean -Run
```

## Scope

RibbonLib intentionally does not provide a Dock framework, MVVM framework, command bus, plugin system, large theme engine or host-application architecture. It stays focused on Ribbon presentation and lets the host keep ownership of business logic and commands.

## License

RibbonLib is licensed under the [MIT License](./LICENSE).
