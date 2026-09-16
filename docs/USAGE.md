# RibbonLib Detailed Usage

[README](../README.md) | [中文](./USAGE.zh-CN.md)

RibbonLib is designed around one rule: the host application owns commands and state through `QAction`; RibbonLib only renders those commands as a Ribbon.

## 1. Requirements

- CMake 3.16+
- C++17
- Qt 5.14.2+ or Qt 6
- Qt Widgets
- Windows or Linux

## 2. Integration

Source dependency:

```cmake
add_subdirectory(thirdparty/RibbonLib)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

Installed package:

```cmake
find_package(RibbonLib CONFIG REQUIRED)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

Use the umbrella header:

```cpp
#include <RibbonLib.h>
```

No external Ribbon stylesheet is required.

## 3. QAction-first construction

Create normal application actions first:

```cpp
auto *openAction = new QAction(openIcon, tr("Open"), this);
openAction->setShortcut(QKeySequence::Open);
connect(openAction, &QAction::triggered,
        this, &MainWindow::openDocument);

auto *axesAction = new QAction(axesIcon, tr("Axes"), this);
axesAction->setCheckable(true);
axesAction->setChecked(true);
connect(axesAction, &QAction::toggled,
        this, &MainWindow::setAxesVisible);
```

Then describe Ribbon structure:

```cpp
auto *ribbon = new QRibbonWidget(this);
setMenuWidget(ribbon);

auto *home = ribbon->addTab(tr("Home"), "home");

auto *document = home->addGroup(tr("Document"));
document->addAction(openAction, QRibbonButtonSize::Large);

auto *view = home->addGroup(tr("View"));
view->addActions({axesAction, wireframeAction, perspectiveAction},
                 QRibbonButtonSize::Small);
```

The bound Ribbon button follows the action's text, icon, enabled, visible, checkable, checked, shortcut, tooltip and status tip. Triggering the Ribbon button triggers the same `QAction`.

## 4. Tabs and groups

```cpp
auto *home = ribbon->addTab(tr("Home"), "home");
auto *create = home->addGroup(tr("Create"));
auto *modify = home->addGroup(tr("Modify"));
```

For a compact group of actions:

```cpp
home->addGroup(tr("Edit"),
               {undoAction, redoAction, findAction},
               QRibbonButtonSize::Small);
```

Stable tab IDs are useful when selecting tabs in code:

```cpp
ribbon->setCurrentTab("home");
```

## 5. Large and Small buttons

RibbonLib intentionally exposes only two command sizes:

```cpp
QRibbonButtonSize::Large
QRibbonButtonSize::Small
```

Large buttons use a 32 px icon area above the text. Small buttons are arranged in three compact rows per column.

Button width is content-driven. RibbonLib does not impose a fixed minimum width on Large, Small or Split buttons. Width is calculated from icon width, text width, padding and the Split arrow area when present.

## 6. Explicit Large display text

Large labels never wrap automatically. Without an explicit line break, the label stays on one line and the button grows to fit it.

When a two-line Ribbon label is desirable, use presentation-only `displayText`:

```cpp
auto *button = group->addAction(settingsAction,
                                QRibbonButtonSize::Large);
button->setDisplayText(tr("Model\nSettings"));
```

or:

```cpp
group->addAction(settingsAction,
                 QRibbonButtonSize::Large,
                 tr("Model\nSettings"));
```

This does not change `QAction::text()`, so menus and other UI continue to display the normal command name.

Only the first explicit line break is preserved. Additional breaks are normalized into spaces. Small buttons use one line.

To return to the QAction text:

```cpp
button->clearDisplayText();
```

## 7. Checkable commands

Put checked state on the QAction:

```cpp
auto *wireframeAction = new QAction(icon, tr("Wireframe"), this);
wireframeAction->setCheckable(true);
wireframeAction->setChecked(false);

group->addAction(wireframeAction, QRibbonButtonSize::Small);
```

Do not maintain a second checked state in application code for the Ribbon control.

## 8. Split Button

```cpp
auto *menu = new QRibbonMenu(group);
menu->addAction(headingAction);
menu->addAction(listAction);
menu->addAction(tableAction);

group->addSplitAction(headingAction,
                      menu,
                      QRibbonButtonSize::Large,
                      tr("Insert\nContent"));
```

The main area triggers the default action. The arrow opens the menu. Selecting a menu action makes it the current default action. The optional Ribbon `displayText` remains presentation-only.

You can also construct the control directly:

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

ribbon->applicationButton()->setText(tr("File"));
ribbon->applicationButton()->setApplicationMenu(menu);
```

## 10. Quick Access bar

```cpp
ribbon->addAccessBarAction(saveAction);
ribbon->addAccessBarAction(undoAction);
ribbon->addAccessBarAction(redoAction);
```

Actions with icons render as icon-only Quick Access buttons. Text-only actions remain text-only.

## 11. Context tabs

```cpp
const int index = ribbon->indexOfTabId("tools");
ribbon->setTabContext(index, tr("Context"), QColor("#0078d4"));
```

Clear it with:

```cpp
ribbon->clearTabContext(index);
```

The host decides when context tabs should appear or become active.

## 12. Custom widgets

For controls such as combo boxes or spin boxes:

```cpp
group->addSmallWidget(comboBox);
group->addLargeWidget(customWidget);
```

Normal commands should still use `addAction()`.

## 13. Optional JSON loader

`QRibbonHelper` is optional. It is intended only for applications that genuinely need data-driven UI definitions.

Action metadata and layout are loaded independently:

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

Minimal actions JSON:

```json
{
  "actions": [
    {
      "id": "file.open",
      "name": "Open",
      "description": "Open a document",
      "icon": "qt:open",
      "shortcut": "Ctrl+O"
    }
  ]
}
```

Minimal layout JSON:

```json
{
  "ribbon": {
    "tabs": [
      {
        "id": "home",
        "title": "Home",
        "panels": [
          {
            "title": "Document",
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

Supported placement fields are intentionally small: `id`, `style`, optional `displayText`, and optional `menu`. `style` is `large` or `small`.

Example explicit two-line placement:

```json
{
  "id": "tools.settings",
  "style": "large",
  "displayText": "Application\nSettings"
}
```

## 14. Qt standard icons in JSON

Use `qt:<name>` in action metadata. Common aliases include:

`file`, `folder`, `home`, `open`, `save`, `close`, `apply`, `cancel`, `reset`, `help`, `info`, `warning`, `error`, `question`, `back`, `forward`, `up`, `down`, `reload`, `stop`, `play`, `pause`, `trash`, `settings`, `list`, `maximize`.

Qt `SP_...` names supported by RibbonLib are also accepted.

## 15. Styling and DPI

RibbonLib embeds its own QSS resource and applies it locally. Consumers do not copy the QSS, call `Q_INIT_RESOURCE()`, or apply Ribbon styles globally.

The library owns Ribbon heights, icon metrics, three-row Small layout, state borders and group spacing. Application code should not patch these values for 125% or 150% scaling.

## 16. Build

Linux:

```bash
./build.sh
./build.sh Debug --clean
./build.sh Release --run
./build.sh Release --shared
./build.sh Release --install
```

Windows:

```powershell
.\build.ps1
.\build.ps1 -Configuration Debug -Clean
.\build.ps1 -Run
.\build.ps1 -Shared
.\build.ps1 -Install
```

CMake options:

- `RIBBONLIB_BUILD_EXAMPLE`
- `RIBBONLIB_BUILD_SHARED`
- `RIBBONLIB_ENABLE_INSTALL`

## 17. Recommended boundary

RibbonLib should remain a presentation library. Keep document state, application settings, command routing, plugins and business logic outside it. If an existing `QAction` can represent a command, reuse that action instead of introducing another command abstraction.
