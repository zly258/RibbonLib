# RibbonLib

> **English | [中文](README.zh-CN.md)** — English is the default language; click the link above to view the Chinese version.

A lightweight **Ribbon control library** built on **Qt Widgets**. The current version's goal is: **do not extend with complex controls — keep the basic Ribbon stable, clean, easy to integrate, and easy to re-skin**.

It suits Qt desktop software such as CAD, BIM, engineering design, parametric modeling, and drawing tools, as a modern replacement for the traditional menu bar and toolbar.

## 1. Current Version Principles

```text
1. No complex controls: no ComboBox, SpinBox, Gallery and other extended components.
2. Centralized styling: colors, borders, hover, pressed, checked, disabled are all kept in QSS.
3. C++ only handles structure and layout: code never hard-codes theme colors.
4. The quick access bar only holds high-frequency actions: Save, Undo, Redo, etc.
5. Contextual tabs do not change the overall height: no extra colored top bar, to avoid UI jumping.
6. Icon resources only keep items actually referenced by JSON / QSS.
7. Keeps Qt 5.14.2 + VS2017 usable.
```

## 2. Feature Overview

Currently supported:

```text
Application button    QApplicationButton, usually shows "File"
Ribbon root control   QRibbonWidget
Ribbon page           QRibbonTab
Ribbon group          QRibbonGroup
Normal button         QRibbonButton, supports Large / Small
Split button          QRibbonSplitButton, main click + dropdown menu
Ribbon menu           QRibbonMenu
Quick access bar      AccessBar, placed top-right
JSON construction     QRibbonHelper
Base action           RibbonAction, bindable to QAction / RibbonButton
State saving          QRibbonStateManager, saves the current tab
QSS styling           example reads styles/ribbon.qss through QFile
```

Currently NOT included:

```text
ComboBox
SpinBox
LineEdit
ColorButton
Gallery
Complex multi-theme system
Complex state-rule system
Complex collapsible layout system
```

If you need these controls, implement them in the business application as required; it is not recommended to keep growing RibbonLib into a complex UI framework.

## 3. Project Structure

```text
RibbonLib/
├─ CMakeLists.txt
├─ README.md
├─ src/
│  ├─ QRibbonButtonSize.h        Button size enum: Large / Small
│  ├─ QRibbonMetrics.h           Shared metrics constants
│  ├─ RibbonLibGlobal.h          Export macros (static/shared)
│  ├─ QRibbonWidget.h/.cpp       Ribbon root control: ApplicationButton, TabBar, StackedWidget, AccessBar
│  ├─ QRibbonTab.h/.cpp          A single Ribbon page holding multiple QRibbonGroup horizontally
│  ├─ QRibbonGroup.h/.cpp        Ribbon group: large-button column, three-row small-button column, separators
│  ├─ QRibbonButton.h/.cpp       Normal Ribbon button, self-drawn icon/text, background states via QSS
│  ├─ QRibbonSplitButton.h/.cpp  Split button: main area triggers default action, arrow area opens menu
│  ├─ QApplicationButton.h/.cpp  Top-left "File" button, based on QToolButton
│  ├─ QRibbonMenu.h/.cpp         Ribbon menu, usable by ApplicationButton or SplitButton
│  ├─ QRibbonHelper.h/.cpp       Builds a Ribbon from actions.json / ribbon.json
│  ├─ QRibbonStateManager.h/.cpp Saves and restores the current tab index
│  └─ RibbonAction.h/.cpp        Base action decoupled from business logic
│
├─ resource/
│  └─ styles/ribbon.qss          Library-level default style template
│
└─ example/
   ├─ CMakeLists.txt
   ├─ main.cpp
   ├─ MainWindow.h/.cpp          Code-based Ribbon creation example
   ├─ RibbonJsonWindow.h/.cpp    JSON-based Ribbon creation example
   ├─ ExampleRibbonAction.h/.cpp Business RibbonAction example
   └─ resources/
      ├─ actions.json            English action definitions (example default)
      ├─ ribbon.json             English Ribbon structure (example default)
      ├─ actions_zh.json         Chinese action definitions
      ├─ ribbon_zh.json          Chinese Ribbon structure
      ├─ styles/ribbon.qss       All Ribbon styles maintained centrally
      └─ icons/                  Only icons actually referenced by JSON / QSS
```

## 4. Build Requirements

```text
Qt       5.14.2 or 5.15.x
CMake    3.16+
Compiler MSVC 2017 / MSVC 2019 / MSVC 2022
System   Windows x64
```

MSVC flags already enabled:

```text
/utf-8
/MP
/W3
/nologo
```

Source, JSON, QSS, and README are all saved as UTF-8 to avoid mojibake and C4819.

## 5. Standalone Build

```bash
cmake -S . -B build -G "Visual Studio 15 2017" -A x64
cmake --build build --config Debug
```

Disable the example program:

```bash
cmake -S . -B build -G "Visual Studio 15 2017" -A x64 -DRIBBONLIB_BUILD_EXAMPLE=OFF
cmake --build build --config Debug
```

Build as a shared library:

```bash
cmake -S . -B build -DRIBBONLIB_BUILD_SHARED=ON
```

## 6. Integration as a Subproject

```text
YourApp/
├─ CMakeLists.txt
├─ thirdparty/
│  └─ RibbonLib/
└─ src/
```

```cmake
add_subdirectory(thirdparty/RibbonLib)

target_link_libraries(YourApp PRIVATE RibbonLib::RibbonLib)
```

The `RibbonLib` target only contains the library source; it does not compile the example's JSON, icons, QSS, or translation files, so it stays clean when shared with other projects.

### Resource Ownership

```text
src/                  RibbonLib library source
resource/             Library-level styles (ribbon.qss template)
example/resources/    QSS / JSON / icons used only by the example
```

The library does not bundle QSS or icons. Business projects decide how to provide style resources. To load the library template directly:

```cpp
app.setStyleSheet(QFile("config/ribbon.qss"));
```

For Qt built-in translations, load from the local Qt installation (e.g. `QLibraryInfo::location(QLibraryInfo::TranslationsPath)`); do not embed Qt's own `.qm` files into RibbonLib.

## 7. Quick Start: main.cpp

```cpp
#include <QApplication>
#include <QStyleFactory>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setStyle(QStyleFactory::create("Fusion"));

    // Load the example stylesheet
    QFile qssFile(QCoreApplication::applicationDirPath()
                  + "/example/resources/styles/ribbon.qss");
    if (qssFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        app.setStyleSheet(QString::fromUtf8(qssFile.readAll()));
    }

    MainWindow w;
    w.show();
    return app.exec();
}
```

## 8. Quick Start: Code-based Ribbon Creation

```cpp
#include "QRibbonWidget.h"
#include "QRibbonTab.h"
#include "QRibbonGroup.h"
#include "QRibbonButton.h"
#include "QApplicationButton.h"

void MainWindow::setupRibbon()
{
    auto *ribbon = new QRibbonWidget(this);
    setMenuWidget(ribbon);

    auto *appButton = new QApplicationButton(ribbon);
    appButton->setText("File");
    ribbon->setApplicationButton(appButton);

    auto *homeTab = ribbon->addTab("Home");
    auto *fileGroup = homeTab->addGroup("File");

    auto *newButton = new QRibbonButton(style()->standardIcon(QStyle::SP_FileIcon),
                                        "New",
                                        QRibbonButtonSize::Large,
                                        fileGroup);
    fileGroup->addButton(newButton);

    auto *copyButton = new QRibbonButton(style()->standardIcon(QStyle::SP_DialogApplyButton),
                                         "Copy",
                                         QRibbonButtonSize::Small,
                                         fileGroup);
    fileGroup->addButton(copyButton);
}
```

## 9. Button Size Rules

### Large

Used for high-frequency commands such as: New, Open, Save, Import, Export, Generate.

```text
Icon on top
Text below
Best placed on the left side of a group
```

### Small

Used for common commands such as: Copy, Paste, Cut, Move, Rotate, Delete.

```text
Icon on left
Text on right
Auto-arranged in three rows
```

## 10. Group Layout Rules

`QRibbonGroup` supports:

```text
addButton(Large)      Large buttons arranged horizontally
addButton(Small)      Three-row small buttons auto-columned
addLargeWidget(...)   Add a large custom QWidget
addSmallWidget(...)   Add a small custom QWidget
addSeparator()        Add an in-group separator
```

Recommended layout:

```text
Do not put too many large buttons in one group
Keep small buttons in three rows
Separate feature domains into different groups
Group titles stay concise (2~4 short words in Chinese)
```

Group width is computed naturally from content; no fixed width is enforced.

## 11. Quick Access Bar Usage

The quick access bar sits on the top-right. Only put high-frequency actions there:

```text
Save
Undo
Redo
```

```cpp
QAction *saveAction = new QAction(style()->standardIcon(QStyle::SP_DialogSaveButton), "Save", this);
ribbon->addAccessBarAction(saveAction);
```

Do NOT put the following in the quick access bar:

```text
Theme switching
Settings
About
Collapse Ribbon
Low-frequency business commands
```

## 12. ApplicationButton File Menu

```cpp
auto *appButton = new QApplicationButton(ribbon);
appButton->setText("File");

QMenu *menu = new QRibbonMenu(appButton);
menu->addAction(newAction);
menu->addAction(openAction);
menu->addSeparator();
menu->addAction(exitAction);

appButton->setApplicationMenu(menu);
ribbon->setApplicationButton(appButton);
```

The `ApplicationButton` width adapts to its text (min 44px, max 96px, height 29px). Do not set a fixed width; use `QRibbonWidget::setApplicationButton()` to replace the default button.

## 13. QRibbonSplitButton Usage

```cpp
auto *split = new QRibbonSplitButton(icon, "View", QRibbonButtonSize::Large, group);

QMenu *menu = new QRibbonMenu(split);
menu->addAction(wireframeAction);
menu->addAction(shadedAction);
menu->setDefaultAction(wireframeAction);

split->setMenu(menu);
split->setDefaultAction(wireframeAction);
group->addLargeWidget(split);
```

Behavior:

```text
Click the main area: triggers the default action
Click the arrow area: opens the menu
When the menu default action changes: the button's text, icon, and enabled state sync accordingly
```

## 14. JSON-based Ribbon Creation

JSON is suitable for keeping the Ribbon configuration external to business software.

### actions.json

```json
{
  "actions": [
    {
      "id": "open",
      "name": "Open",
      "description": "Open a file",
      "shortcut": "Ctrl+O",
      "icon": "icons/file/open.png"
    }
  ]
}
```

Field reference:

```text
id           Stable command id, must be unique
name         Displayed text
description  Tooltip
shortcut     Shortcut, optional
icon         Icon path, optional
```

You can also express action state in JSON:

```json
{
  "id": "snap",
  "name": "Snap",
  "checkable": true,
  "checked": true,
  "enabled": true,
  "visible": true
}
```

### ribbon.json

```json
{
  "ribbon": {
    "applicationButton": {
      "title": "File",
      "menu": [
        { "id": "open" },
        "-"
      ]
    },
    "tabs": [
      {
        "id": "home",
        "title": "Home",
        "panels": [
          {
            "id": "file",
            "title": "File",
            "items": [
              { "id": "open", "style": "large" }
            ]
          }
        ]
      }
    ],
    "quickAccessBar": {
      "items": [
        { "id": "save" },
        { "id": "undo" },
        { "id": "redo" }
      ]
    }
  }
}
```

Field reference:

```text
applicationButton.title     Top-left button text
applicationButton.menu      File menu action list, "-" means separator
tabs                        Ribbon page list
panels                      Ribbon group list
items                       Buttons within a group
style                       large or small
quickAccessBar.items        Top-right quick access bar action list
```

Ribbon items and menu items can also carry execution parameters:

```json
{
  "id": "delete",
  "style": "large",
  "params": "selected"
}
```

```json
{
  "id": "export",
  "menu": [
    { "id": "exportPdf", "params": "pdf" },
    { "id": "exportDwg", "params": "dwg" }
  ]
}
```

### Loading JSON

```cpp
auto *ribbon = new QRibbonWidget(this);
setMenuWidget(ribbon);

auto *helper = new QRibbonHelper(this);
if (helper->loadFromResources(":/resources/ribbon.json", ":/resources/actions.json")) {
    helper->buildRibbon(ribbon);
}

connect(helper, &QRibbonHelper::actionTriggered,
        this, [](const QString &id) {
            qDebug() << "Ribbon action:" << id;
        });
```

On failure, read the same `errorString()`:

```cpp
if (!helper->loadFromResources()) {
    qWarning() << helper->errorString();
}
```

## 15. Style Maintenance

All styles are centralized in:

```text
example/resources/styles/ribbon.qss
```

Adjustable content includes Ribbon background, file-button color, tab normal / hover / selected states, quick access bar buttons, content-area borders, group borders and titles, `QRibbonButton` and `QRibbonSplitButton` states, `QMenu` states, and `QToolTip`.

Button states are exposed to QSS via dynamic properties:

```text
hovered="true"
pressed="true"
checked="true"
```

Example:

```css
QRibbonWidget#RibbonWidget QWidget#RibbonButton[hovered="true"] {
    background: #eef6ff;
    border-color: #b7d4f6;
}
```

Maintenance rules:

```text
Do not write background colors in paintEvent
Do not call setStyleSheet on individual Ribbon child widgets in business code
Prefer adjusting everything in ribbon.qss
If you add an objectName, sync it to the qss
```

Border responsibilities are divided so horizontal borders are drawn by containers and vertical separators by tabs/groups:

```text
QTabBar::tab         Draws only top, left, right borders (no bottom)
Active tab           Blue top edge + left/right borders; bottom is joined by the content area top line
Inactive tab         Only a light right separator; on hover add a light top/left/right border
RibbonTopBar         Only the top background and top line
RibbonContent        Draws the content area top and bottom lines
RibbonGroup          Only the right separator; no top/bottom borders
RibbonGroupTitle     Only shows the title, no borders
```

## 16. Icon Resource Maintenance

Icons live in:

```text
resources/icons/
```

Rules:

```text
1. Icon paths are written in actions.json.
2. resources.qrc only registers actually-used files.
3. After deleting unused icons, always sync resources.qrc.
4. Prefer English icon paths; avoid Chinese filenames.
5. Group similar icons into one directory, e.g. file / edit / view / dim / 3d.
```

Icon paths in `actions.json` use relative paths; `QRibbonHelper` resolves them against the directory containing `actions.json`:

```json
{
  "id": "open",
  "icon": "icons/file/open.png"
}
```

Recommended paths:

```text
icons/file/open.png
icons/edit/copy.png
icons/view/wireframe.png
```

## 17. Contextual Tabs

`QRibbonWidget::setTabContext()` sets a contextual marker for a tab.

```cpp
ribbon->setTabContext(index, "Picture Tools", QColor("#d89b00"));
```

Current behavior:

```text
No extra colored top bar
No change to total Ribbon height
Only changes the tab text color and tooltip
Avoids vertical UI jumping
```

## 18. State Saving

`QRibbonStateManager` currently saves only the active tab:

```cpp
auto *state = new QRibbonStateManager(ribbon, this);
state->restoreState();

// Before program exit or window destruction
state->saveState();
```

To isolate multiple Ribbon instances that share `QSettings`, use a prefix:

```cpp
QRibbonStateManager manager(ribbon);
manager.setSettingsPrefix("MainWindow/Ribbon");
manager.restoreState();
```

It stays lightweight and does not persist complex user layouts.

## 19. RibbonAction Basic Action Binding

RibbonLib provides a lightweight base action class `RibbonAction` that decouples business actions from Ribbon buttons.

Design boundary:

```text
RibbonLib only provides the base RibbonAction
No dependency on Document / Viewport / ApplicationContext
Business projects may subclass RibbonAction and access their own context in execute()
Button click -> QAction::triggered -> RibbonAction::execute(params)
```

### Define a Business Action

```cpp
#include "RibbonAction.h"

class DeleteAction : public RibbonAction
{
    Q_OBJECT
public:
    explicit DeleteAction(QObject *parent = nullptr)
        : RibbonAction(parent)
    {
        setName("Delete");
        setDescription("Delete the currently selected object");
    }

    QString id() const override
    {
        return "delete";
    }

    void execute(const QString &params) override
    {
        Q_UNUSED(params)
        // call your business delete logic here
    }

    void cleanup() override
    {
        // free temporary state
    }
};
```

### Register with QRibbonHelper

```cpp
auto helper = new QRibbonHelper(this);
helper->loadFromResources();

helper->registerRibbonAction(new DeleteAction(helper));

helper->buildRibbon(ribbonWidget);
```

Once registered with the same id as an action in `actions.json`, `QRibbonHelper` reuses the internal `QAction` and syncs `RibbonAction` properties onto it. Buttons are still created from JSON, but clicking them runs `RibbonAction::execute()`.

### Action Property Synchronization

`RibbonAction` supports:

```text
id
name
description
icon
shortcut
defaultParams
enabled
visible
checkable
checked
```

Changing a property emits `changed()`; `QRibbonHelper` syncs it to the internal `QAction`, which updates the button text, icon, and enabled state. If an action is checkable, the checked state is synchronized bidirectionally between `QAction` and `RibbonAction`.

### Unified Execution Entry Point

Business code usually only implements:

```cpp
void execute(const QString &params) override;
void cleanup() override;
```

Then invoke uniformly:

```cpp
action->trigger("selected");
```

Internally it runs: `validate()` → `execute(params)`, then emits `executed(id, params)` or `executeRejected(id, params)`.

### Kept actionTriggered

Projects that do not register a `RibbonAction` may keep using:

```cpp
connect(helper, &QRibbonHelper::actionTriggered,
        this, &MainWindow::executeCommand);
```

Both approaches may coexist:

```text
Simple projects: listen to actionTriggered(id)
Complex projects: subclass and register RibbonAction
```

## 20. Publish / Install

By default the CMake build produces a publish directory under the build tree:

```text
build/publish/
├─ bin/
│  └─ RibbonLib.dll          # when shared
├─ lib/
│  └─ RibbonLib.lib          # static or import library
├─ inc/
│  └─ public headers
└─ resource/
   └─ ribbon.qss
```

Example output only, everything is kept away from the publish directory:

```text
build/example/bin/
    example/resources/
```

For install/export:

```bash
cmake -S . -B build -DRIBBONLIB_ENABLE_INSTALL=ON
cmake --build build --config Release
cmake --install build --config Release
```

This generates `RibbonLibTargets.cmake` and a `RibbonLib::RibbonLib` package target.

## 21. Behaviors Worth Noting

```text
1. Horizontal scrollbar is hidden; the mouse wheel is converted to horizontal scrolling.
2. Empty Ribbon groups (no buttons/separators) are hidden by default and take no width.
3. Buttons without an icon (or with a missing icon file) use the Qt built-in placeholder:
   Large = QStyle::SP_MessageBoxQuestion (32x32), Small = same (16x16).
4. Large/ small buttons are content-aligned: large button icon starts at the left padding,
   and both icon and text are left-aligned; the split button arrow sits in a narrow right zone.
5. Group width is computed naturally from content; the right stretch only takes remaining space.
```

## 22. FAQ

### 22.1 Styles Not Applied

Make sure you run:

```cpp
app.setStyle(QStyleFactory::create("Fusion"));
app.setStyleSheet(CONTENT_OF_ribbon.qss);
```

### 22.2 Mojibake or C4819

```text
Save the source as UTF-8
Use /utf-8 under MSVC
Do not save cpp/h/json/qss in the system code page
```

### 22.3 Tab Text Truncated

```text
QTabBar height
QTabBar::tab height / min-height in ribbon.qss
font size
```

### 22.4 Missing Icon

```text
Check the icon path in actions.json
Check whether resources.qrc registers the file
Check whether the file actually exists
```

### 22.5 Quick Access Bar Too Cluttered

Keep only 2~4 highest-frequency actions in the quick access bar; put the rest in Ribbon groups or the file menu.

## 23. Example Program & Bilingual Switching

The example defaults to English and can switch to Chinese at runtime:

- `MainWindow` and `RibbonJsonWindow` default `m_isChinese = false` (English).
- Click the language toggle in the quick access bar (or the "JSON Window" page) to switch.
- The JSON window loads `ribbon.json` / `actions.json` (English) by default, and changes to `ribbon_zh.json` / `actions_zh.json` (Chinese) after switching.

## 24. License

Released as a source-code library intended for third-party subproject use. See the repository for the source and headers.