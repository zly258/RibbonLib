# RibbonLib

English | [中文](./README.zh-CN.md)

RibbonLib is a compact Qt Widgets Ribbon library for CAD, BIM, engineering and other desktop applications. It focuses on predictable layout, low integration cost, local styling and a small public API instead of becoming a general UI framework.

## Highlights

- Qt 5.14.2+ and Qt 6 support.
- C++17 and CMake 3.16+.
- Windows and Linux.
- Large and Small Ribbon buttons.
- Checkable buttons with checked / pressed / disabled states.
- Large buttons support up to two text lines without ellipsis.
- Split buttons and Ribbon menus.
- Application menu and right-aligned Quick Access bar.
- Context tabs.
- Direct C++ construction or optional JSON-driven construction.
- Qt standard icons through `qt:<name>` aliases.
- Embedded, Ribbon-scoped default QSS; no external stylesheet is required by consumers.
- Optional state persistence and `RibbonAction` command abstraction.

## Quick start

### Source dependency

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

    auto *home = ribbon->addTab("Home", "home");
    auto *document = home->addGroup("Document");

    auto *open = new QRibbonButton(openIcon,
                                   "Open",
                                   QRibbonButtonSize::Large,
                                   document);
    document->addButton(open);

    connect(open, &QRibbonButton::clicked, this, &MainWindow::openDocument);
}
```

That is sufficient. A consumer does not need to copy `ribbon.qss`, call `Q_INIT_RESOURCE()`, install a theme manager, or apply RibbonLib styling to the whole `QApplication`.

### Installed package

```bash
./build.sh Release --install
```

```cmake
find_package(RibbonLib CONFIG REQUIRED)
target_link_libraries(MyApp PRIVATE RibbonLib::RibbonLib)
```

## Documentation

The README is intentionally concise. Detailed integration and behavior documentation is available here:

- [Detailed usage guide](./docs/USAGE.md)
- [详细使用文档](./docs/USAGE.zh-CN.md)
- [English example configuration](./example/resources/ribbon.en.json)
- [English action definitions](./example/resources/actions.en.json)

The detailed guide covers direct C++ construction, tabs and groups, button layout rules, two-line large labels, checkable actions, split buttons, Quick Access, Application Menu, context tabs, custom widgets, JSON schema conventions, icon fallback, `RibbonAction`, styling, build options and troubleshooting.

## Large-button text behavior

Large buttons have a fixed vertical layout and support at most two visual text lines.

- An explicit `\n` requests a line break.
- Without `\n`, text may wrap automatically when the calculated button width requires it.
- Wrapping prefers word boundaries and can fall back to character boundaries.
- Large-button labels are not elided with `...`.
- The button width is calculated so the label fits in at most two lines.
- Additional explicit line breaks after the first are normalized into spaces.
- Small buttons remain compact single-row controls.

This behavior applies to both `QRibbonButton` and large `QRibbonSplitButton`.

## JSON-driven Ribbon

JSON support is optional. Direct C++ use has no JSON dependency.

```cpp
QRibbonHelper helper;
if (!helper.loadFromResources(ribbonPath, actionsPath)) {
    qWarning() << helper.errorString();
    return;
}

helper.buildRibbon(ribbon);
```

Minimal `actions.json`:

```json
{
  "actions": [
    {
      "id": "file.open",
      "name": "Open",
      "description": "Open a document",
      "icon": "qt:open",
      "shortcut": "Ctrl+O"
    },
    {
      "id": "view.axes",
      "name": "Axes",
      "icon": "qt:apply",
      "checkable": true,
      "checked": true
    }
  ]
}
```

Minimal `ribbon.json`:

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

`style` is intentionally limited to `large` and `small`.

## Qt standard icons

Use `qt:<name>` in JSON to request a Qt standard icon. The actual appearance follows the active Qt platform style, which gives appropriate native-looking icons on Windows without bundling a separate icon set.

Common aliases include:

`file`, `folder`, `home`, `open`, `save`, `close`, `apply`, `cancel`, `reset`, `help`, `info`, `warning`, `error`, `question`, `back`, `forward`, `up`, `down`, `reload`, `stop`, `play`, `pause`, `trash`, `settings`, `list`, `maximize`.

Qt `SP_...` names are also accepted, for example `SP_DialogSaveButton`.

## Build

Linux:

```bash
./build.sh
./build.sh Debug --clean
./build.sh Release --tests
./build.sh Release --run
./build.sh Release --shared
./build.sh Release --install
```

Windows PowerShell:

```powershell
.\build.ps1
.\build.ps1 -Configuration Debug -Clean
.\build.ps1 -Tests
.\build.ps1 -Run
.\build.ps1 -Shared
.\build.ps1 -Install
```

Standard CMake is also supported:

```bash
cmake -S . -B build
cmake --build build
```

Tests are opt-in:

```bash
cmake -S . -B build -DRIBBONLIB_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Important CMake options:

- `RIBBONLIB_BUILD_EXAMPLE`
- `RIBBONLIB_BUILD_TESTS`
- `RIBBONLIB_BUILD_SHARED`
- `RIBBONLIB_ENABLE_INSTALL`

## Example

The example intentionally contains more cases than a minimal application. In particular, the `Tests` tab exercises:

- checkable Small buttons in all three rows;
- checked, unchecked and disabled Large buttons;
- mixed Large / Small groups;
- long labels and two-line Large labels;
- Split buttons;
- Qt standard icons;
- English / Chinese switching.

Run it with:

```bash
./build.sh Release --clean --run
```

## Scope

RibbonLib intentionally does not provide a Dock framework, MVVM framework, command bus, plugin system, large theme engine or application architecture. It is meant to stay a focused Ribbon library that can be embedded into larger products without forcing the host to adopt unrelated infrastructure.

## License

RibbonLib is licensed under the [MIT License](./LICENSE).
