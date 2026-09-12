> **English | [中文](./README.zh-CN.md)** — English is the default language. / 英文为默认文档语言，点击顶部链接即可查看英文版。

# RibbonLib 使用说明

RibbonLib 是一个基于 **Qt Widgets** 的轻量 Ribbon 控件库。当前版本的定位是：**不扩展复杂控件，只把基础 Ribbon 做稳定、整洁、易集成、易换肤**。

它适合 CAD、BIM、工程设计、参数化建模、图纸工具等 Qt 桌面软件，用于替代传统菜单栏和工具栏。

## 1. 当前版本原则

本版基于最早的 RibbonLib 结构重新整理，遵循以下约束：

```text
1. 不新增复杂控件：不包含 ComboBox、SpinBox、Gallery 等扩展组件。
2. 样式集中维护：颜色、边框、hover、pressed、checked、disabled 全部放到 QSS。
3. C++ 只负责结构和布局：代码不写死主题颜色。
4. 快速访问栏只放高频动作：保存、撤销、重做等。
5. 上下文 Tab 不改变整体高度：不再添加顶部彩色横条，避免界面上下跳动。
6. 图标资源只保留 JSON / QSS 实际引用项。
7. 保持 Qt 5.14.2 + VS2017 可用。
```

## 2. 功能概览

当前支持：

```text
应用按钮       QApplicationButton，通常显示“文件”
Ribbon 根控件  QRibbonWidget
Ribbon 页      QRibbonTab
Ribbon 分组    QRibbonGroup
普通按钮       QRibbonButton，支持 Large / Small
分割按钮       QRibbonSplitButton，支持主体点击 + 下拉菜单
Ribbon 菜单    QRibbonMenu
快速访问栏     AccessBar，放在右上角
JSON 构建      QRibbonHelper
基础动作       RibbonAction，可绑定到 QAction / RibbonButton 执行
状态保存       QRibbonStateManager，保存当前 Tab
QSS 样式       example 使用 QFile 读取 styles/ribbon.qss
```

当前不包含：

```text
ComboBox
SpinBox
LineEdit
ColorButton
Gallery
复杂多主题系统
复杂状态规则系统
复杂布局折叠系统
```

需要这些控件时，建议在业务软件侧按实际需求单独实现，不建议继续把 RibbonLib 做成复杂 UI 框架。

## 3. 项目结构

```text
RibbonLib/
├─ CMakeLists.txt
├─ README.md
├─ src/
│  ├─ QRibbonButtonSize.h
│  │  按钮尺寸枚举：Large / Small
│  │
│  ├─ QRibbonWidget.h/.cpp
│  │  Ribbon 根控件，负责 ApplicationButton、TabBar、StackedWidget、AccessBar
│  │
│  ├─ QRibbonTab.h/.cpp
│  │  单个 Ribbon 页，内部横向放置多个 QRibbonGroup
│  │
│  ├─ QRibbonGroup.h/.cpp
│  │  Ribbon 分组，支持大按钮列、三行小按钮列、分隔线
│  │
│  ├─ QRibbonButton.h/.cpp
│  │  普通 Ribbon 按钮，自绘图标和文字，背景状态由 QSS 控制
│  │
│  ├─ QRibbonSplitButton.h/.cpp
│  │  分割按钮，主体点击触发默认动作，箭头区域弹出菜单
│  │
│  ├─ QApplicationButton.h/.cpp
│  │  左上角“文件”按钮，基于 QToolButton
│  │
│  ├─ QRibbonMenu.h/.cpp
│  │  Ribbon 菜单，可用于 ApplicationButton 或 SplitButton
│  │
│  ├─ QRibbonHelper.h/.cpp
│  │  从 actions.json / ribbon.json 构建 Ribbon
│  │
│  ├─ QRibbonStateManager.h/.cpp
│  │  保存和恢复当前 Tab 索引
│  │
│
├─ example/resources/
│  ├─ resources.qrc
│  │  示例程序资源清单，仅编译进 RibbonExample
│  │
│  ├─ styles/ribbon.qss
│  │  Ribbon 所有样式集中维护
│  │
│  ├─ actions.json
│  │  JSON 示例动作定义
│  │
│  ├─ ribbon.json
│  │  JSON 示例 Ribbon 结构定义
│  │
│  └─ icons/
│     只保留 actions.json / ribbon.json / qss 实际引用的图标
│
└─ example/
   ├─ CMakeLists.txt
   ├─ main.cpp
   ├─ MainWindow.h/.cpp
   │  代码方式创建 Ribbon 的示例
   │
   └─ RibbonJsonWindow.h/.cpp
      JSON 方式创建 Ribbon 的示例
```

## 4. 构建环境

推荐环境：

```text
Qt       5.14.2 或 5.15.x
CMake    3.16+
编译器   MSVC 2017 / MSVC 2019 / MSVC 2022
系统     Windows x64
```

MSVC 下项目已启用：

```text
/utf-8
/MP
/W3
/nologo
```

源码、JSON、QSS、README 均按 UTF-8 保存，避免中文乱码和 C4819。

## 5. 独立构建

```bash
cmake -S . -B build -G "Visual Studio 15 2017" -A x64
cmake --build build --config Debug
```

关闭示例程序：

```bash
cmake -S . -B build -G "Visual Studio 15 2017" -A x64 -DRIBBONLIB_BUILD_EXAMPLE=OFF
cmake --build build --config Debug
```

## 6. 作为子项目集成

目录示例：

```text
YourApp/
├─ CMakeLists.txt
├─ thirdparty/
│  └─ RibbonLib/
└─ src/
```

主项目 CMake：

```cmake
add_subdirectory(thirdparty/RibbonLib)

target_link_libraries(YourApp PRIVATE RibbonLib Qt5::Widgets)
```

RibbonLib 的 `resources/resources.qrc` 已加入库目标，并在 `QFile 读取 qss 后 app.setStyleSheet()` 中显式初始化资源，因此业务工程通常不需要再单独添加该 qrc。

## 7. 快速上手：main.cpp

```cpp
#include <QApplication>
#include <QStyleFactory>
#include "MainWindow.h"
#include "QRibbonTheme.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setStyle(QStyleFactory::create("Fusion"));
    app.setStyleSheet(QFile 读取 qss 后 app.setStyleSheet());

    MainWindow w;
    w.show();
    return app.exec();
}
```

## 8. 快速上手：代码方式创建 Ribbon

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
    appButton->setText("文件");
    ribbon->setApplicationButton(appButton);

    auto *homeTab = ribbon->addTab("开始");
    auto *fileGroup = homeTab->addGroup("文件");

    auto *newButton = new QRibbonButton(style()->standardIcon(QStyle::SP_FileIcon),
                                        "新建",
                                        QRibbonButtonSize::Large,
                                        fileGroup);
    fileGroup->addButton(newButton);

    auto *copyButton = new QRibbonButton(style()->standardIcon(QStyle::SP_DialogApplyButton),
                                         "复制",
                                         QRibbonButtonSize::Small,
                                         fileGroup);
    fileGroup->addButton(copyButton);
}
```

## 9. 按钮尺寸规则

### Large

用于高频命令，例如：

```text
新建
打开
保存
导入
导出
生成
```

显示方式：

```text
图标在上
文字在下
适合放在分组左侧
```

### Small

用于普通命令，例如：

```text
复制
粘贴
剪切
移动
旋转
删除
```

显示方式：

```text
图标在左
文字在右
三行自动排列
```

## 10. 分组布局规则

`QRibbonGroup` 支持：

```text
addButton(Large)      大按钮横向排列
addButton(Small)      三行小按钮自动分列
addLargeWidget(...)   添加大尺寸自定义 QWidget
addSmallWidget(...)   添加小尺寸自定义 QWidget
addSeparator()        添加分组内分隔线
```

推荐布局：

```text
一个组内不要放太多大按钮
小按钮优先三行排列
不同功能域用不同 Group 区分
Group 标题保持 2~4 个汉字
```

## 11. 快速访问栏使用原则

快速访问栏位于右上角，仅建议放高频动作：

```text
保存
撤销
重做
```

示例：

```cpp
QAction *saveAction = new QAction(style()->standardIcon(QStyle::SP_DialogSaveButton), "保存", this);
ribbon->addAccessBarAction(saveAction);
```

不要把以下内容放到快速访问栏：

```text
主题切换
设置
关于
折叠 Ribbon
低频业务命令
```

## 12. ApplicationButton 文件菜单

```cpp
auto *appButton = new QApplicationButton(ribbon);
appButton->setText("文件");

QMenu *menu = new QRibbonMenu(appButton);
menu->addAction(newAction);
menu->addAction(openAction);
menu->addSeparator();
menu->addAction(exitAction);

appButton->setApplicationMenu(menu);
ribbon->setApplicationButton(appButton);
```

## 13. QRibbonSplitButton 用法

```cpp
auto *split = new QRibbonSplitButton(icon, "视图", QRibbonButtonSize::Large, group);

QMenu *menu = new QRibbonMenu(split);
menu->addAction(wireframeAction);
menu->addAction(shadedAction);
menu->setDefaultAction(wireframeAction);

split->setMenu(menu);
split->setDefaultAction(wireframeAction);
group->addLargeWidget(split);
```

行为：

```text
点击主体区域：触发默认动作
点击箭头区域：弹出菜单
菜单动作变化：默认动作文本、图标、启用状态同步到按钮
```

## 14. JSON 方式创建 Ribbon

JSON 方式适合业务软件把 Ribbon 配置外置。

### actions.json

```json
{
  "actions": [
    {
      "id": "open",
      "name": "打开",
      "description": "打开文件",
      "shortcut": "Ctrl+O",
      "icon": ":/icons/file/open.png"
    }
  ]
}
```

字段说明：

```text
id           稳定命令 id，必须唯一
name         显示文本
description  tooltip
shortcut     快捷键，可选
icon         Qt 资源路径，可选
```

### ribbon.json

```json
{
  "ribbon": {
    "applicationButton": {
      "title": "文件",
      "menu": [
        { "id": "open" },
        "-"
      ]
    },
    "tabs": [
      {
        "id": "home",
        "title": "开始",
        "panels": [
          {
            "id": "file",
            "title": "文件",
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

字段说明：

```text
applicationButton.title     左上角按钮文本
applicationButton.menu      文件菜单动作列表，"-" 表示分隔线
tabs                         Ribbon 页列表
panels                       Ribbon 分组列表
items                        分组内按钮列表
style                        large 或 small
quickAccessBar.items         右上角快速访问栏动作列表
```

### 加载 JSON

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

## 15. 样式维护

所有样式集中在：

```text
example/resources/styles/ribbon.qss
```

可调整内容包括：

```text
Ribbon 背景
文件按钮颜色
Tab 普通 / hover / selected
快速访问栏按钮
Ribbon 内容区边界
Group 边界和标题
QRibbonButton 状态
QRibbonSplitButton 状态
QMenu 状态
QToolTip
```

按钮状态通过动态属性暴露给 QSS：

```text
hovered="true"
pressed="true"
checked="true"
```

示例：

```css
QRibbonWidget#RibbonWidget QWidget#RibbonButton[hovered="true"] {
    background: #eef6ff;
    border-color: #b7d4f6;
}
```

维护要求：

```text
不要在 paintEvent 中写背景色
不要在业务代码中给 Ribbon 子控件单独 setStyleSheet
优先通过 ribbon.qss 统一调整
如果新增 objectName，需要同步到 qss
```

## 16. 图标资源维护

图标位于：

```text
resources/icons/
```

维护规则：

```text
1. 图标路径写在 actions.json 中。
2. resources.qrc 只登记实际使用文件。
3. 删除无用图标后必须同步 resources.qrc。
4. 图标建议使用英文路径，不建议使用中文文件名。
5. 同一类图标放同一目录，例如 file/edit/view/dim/3d。
```

推荐路径：

```text
:/icons/file/open.png
:/icons/edit/copy.png
:/icons/view/wireframe.png
```

## 17. 上下文 Tab

`QRibbonWidget::setTabContext()` 可为某个 Tab 设置上下文标记。

```cpp
ribbon->setTabContext(index, "图片工具", QColor("#d89b00"));
```

当前实现原则：

```text
不增加顶部彩色条
不改变 Ribbon 总高度
只改变 Tab 文字颜色和 tooltip
避免界面上下跳动
```

## 18. 状态保存

`QRibbonStateManager` 当前只保存当前 Tab：

```cpp
auto *state = new QRibbonStateManager(ribbon, this);
state->restoreState();

// 程序退出或窗口析构前
state->saveState();
```

保持轻量，不保存复杂用户布局。

## 19. 常见问题

### 19.1 样式没有生效

检查是否执行：

```cpp
app.setStyle(QStyleFactory::create("Fusion"));
app.setStyleSheet(QFile 读取 qss 后 app.setStyleSheet());
```

### 19.2 QSS 为空

当前 `resources.qrc` 已编译进 RibbonLib，并在 `QFile 读取 qss 后 app.setStyleSheet()` 中显式初始化资源。若你修改了 qrc 名称，需要同步修改 `QRibbonTheme.cpp` 中的 `Q_INIT_RESOURCE(resources)`。

### 19.3 中文乱码或 C4819

确认：

```text
源码保存为 UTF-8
MSVC 使用 /utf-8
不要用系统代码页保存 cpp/h/json/qss
```

### 19.4 Tab 文字显示不全

优先检查：

```text
QTabBar 高度
ribbon.qss 中 QTabBar::tab height/min-height
字体大小
```

### 19.5 图标缺失

检查：

```text
actions.json 中 icon 路径是否正确
resources.qrc 是否登记该文件
文件是否真实存在
```

### 19.6 快速访问栏太乱

快速访问栏只放 2~4 个最高频动作，其他命令放到 Ribbon 分组或文件菜单。

## 20. 后续维护建议

建议继续保持这个方向：

```text
只维护基础 Ribbon
不扩展复杂输入控件
不增加过多主题
样式只改 qss
布局尺寸少量集中在代码常量中
资源保持干净
example 保持简单
README 同步更新
```

不建议：

```text
把业务控件塞进 RibbonLib
把主题切换做成复杂系统
把低频设置放进快速访问栏
在多个 cpp 里散落颜色和边框
```

## 21. 最小示例

```cpp
QRibbonWidget *ribbon = new QRibbonWidget(this);
setMenuWidget(ribbon);

QRibbonTab *tab = ribbon->addTab("开始");
QRibbonGroup *group = tab->addGroup("文件");

QRibbonButton *btn = new QRibbonButton(QIcon(":/icons/file/open.png"),
                                       "打开",
                                       QRibbonButtonSize::Large,
                                       group);
group->addButton(btn);

connect(btn, &QRibbonButton::clicked, this, []() {
    qDebug() << "open clicked";
});
```


### 顶部栏显示规范

当前版本采用 Office 风格的一体化顶栏：

- `ApplicationButton`、`QTabBar`、快速访问栏处于同一行。
- 顶栏背景由 `RibbonTopBar` 统一控制。
- 未激活 Tab 也保留浅边界，避免顶部区域断裂。
- 选中 Tab 使用白底并与下方 Ribbon 内容区衔接。
- 不要在业务代码中额外创建第二个“文件”按钮；应通过 `QRibbonWidget::setApplicationButton()` 替换默认按钮。


### ApplicationButton 与 Tab 边界规范

- ApplicationButton 宽度按文本内容自适应，保留最小/最大宽度，只表达“文件”入口，不承担普通 Tab 的宽度。
- ApplicationButton 不应重复创建；JSON 或代码设置时通过 `setApplicationButton()` 替换默认按钮。
- 顶部边界由 `RibbonTopBar` 提供，横向贯通整个 Ribbon。
- 未激活 Tab 只保留轻量分隔线，避免相邻 Tab 双边框重叠。
- 激活 Tab 使用白底、蓝色顶边、白色下边，与下方 Ribbon 内容区自然衔接。
- 应用菜单使用 `QMenu#RibbonApplicationMenu` 单独控制内边距，避免菜单过宽。


### ApplicationButton 宽度自适应

`ApplicationButton` 不应写死固定宽度。当前实现通过 `QApplicationButton::sizeHint()` 根据文本宽度计算推荐宽度，并通过 QSS 限制最小/最大宽度：

- 最小宽度：44px
- 最大宽度：96px
- 高度：29px
- 横向 padding 由 `example/resources/styles/ribbon.qss` 控制

这样中文“文件”、英文 “File”、更长的本地化文本都能保持合理显示。


### 边框职责划分

当前 Ribbon 边框按以下规则维护：

- `RibbonTopBar`：负责顶栏背景、顶部横线、底部横线。
- `ApplicationButton`：负责蓝色文件入口和右侧轻量分隔，不画重复外框。
- 未激活 `QTabBar::tab`：只保留右侧轻量分隔线和底边线，不画完整矩形。
- 激活 `QTabBar::tab`：画蓝色顶边、左右边界、白色底边，与内容区衔接。
- `RibbonContent`：负责内容区完整顶部线和底部线。
- `RibbonGroup`：只负责右侧分隔线，不再画顶部线，避免横线断裂或重复。
- `RibbonApplicationMenu`：使用单独 QSS 规则压缩图标与文字间距。

维护时不要让多个层级同时画同一方向的边界线。


### 最新边框规则

当前版本采用更简洁的 Ribbon 边框职责：

- `QTabBar::tab`：只画顶部、左侧、右侧边界；不画底部边界。
- 激活 Tab：使用蓝色顶边 + 左右边界；底部无边框，由内容区顶部线统一衔接。
- 未激活 Tab：只保留轻量右侧分隔线，hover 时补轻量顶部/左右边界。
- `RibbonTopBar`：只负责顶部背景和顶部横线，不负责底部横线。
- `RibbonContent`：统一负责内容区顶部横线和底部横线。
- `RibbonGroup`：只负责右侧分隔线，不画顶部和底部边界。
- `RibbonGroupTitle`：只显示标题，不画边界线。

维护原则：横向边界由容器统一画，竖向分隔由 Tab/Group 自己画。


### Office 2013 对齐说明

当前 QSS 按 Office 2013 的扁平风格收敛：

- 文件按钮使用纯蓝色入口，不参与普通 Tab 的边框体系。
- Tab 只画顶部和左右边界，不画底部边界。
- 激活 Tab 使用蓝色顶边，内容区顶部线由 `RibbonContent` 统一提供。
- Group 不画顶部/底部边界，只画右侧分隔线。
- ApplicationButton 菜单使用 `QMenu#RibbonApplicationMenu` 单独压缩 padding，图标大小固定 16px，减少图标列和文字列之间的空隙。
- 不在 C++ 中写颜色和状态样式，颜色继续集中维护在 `example/resources/styles/ribbon.qss`。


## RibbonAction 基础动作绑定

RibbonLib 提供一个轻量基础动作类 `RibbonAction`，用于把业务动作和 Ribbon 按钮解耦。

设计边界：

```text
RibbonLib 只提供基础 RibbonAction
不依赖 Document / Viewport / ApplicationContext
业务工程可以继承 RibbonAction，并在 execute() 中访问自己的上下文
按钮点击 -> QAction::triggered -> RibbonAction::execute(params)
```

### 1. 定义业务 Action

```cpp
#include "RibbonAction.h"

class DeleteAction : public RibbonAction
{
    Q_OBJECT
public:
    explicit DeleteAction(QObject *parent = nullptr)
        : RibbonAction(parent)
    {
        setName("删除");
        setDescription("删除当前选中对象");
    }

    QString id() const override
    {
        return "delete";
    }

    void execute(const QString &params) override
    {
        Q_UNUSED(params)
        // 在这里调用业务删除逻辑
    }

    void cleanup() override
    {
        // 清理临时状态
    }
};
```

### 2. 注册到 QRibbonHelper

```cpp
auto helper = new QRibbonHelper(this);
helper->loadFromResources();

helper->registerRibbonAction(new DeleteAction(helper));

helper->buildRibbon(ribbonWidget);
```

注册后，如果 `actions.json` 中存在同 id 的动作，例如：

```json
{
  "id": "delete",
  "name": "删除",
  "icon": ":/resources/icons/edit/delete.png",
  "shortcut": "Del",
  "description": "删除当前选中对象"
}
```

则 `QRibbonHelper` 会复用内部 `QAction`，并把 `RibbonAction` 的属性同步到该 `QAction`。按钮仍然通过 JSON 创建，但点击后会执行 `RibbonAction::execute()`。

### 3. 动作属性同步

`RibbonAction` 支持：

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

修改属性后会发出 `changed()`，`QRibbonHelper` 会同步到内部 `QAction`，进而更新按钮文本、图标、启用状态等。

### 4. 仍保留 actionTriggered

没有注册 `RibbonAction` 的项目，仍然可以使用原来的方式：

```cpp
connect(helper, &QRibbonHelper::actionTriggered,
        this, &MainWindow::executeCommand);
```

因此两种方式可以并存：

```text
简单项目：监听 actionTriggered(id)
复杂项目：继承并注册 RibbonAction
```


## 作为源码库/子项目引用

RibbonLib 当前按“源码库”方式整理，推荐直接作为子项目引用：

```cmake
add_subdirectory(thirdparty/RibbonLib)

target_link_libraries(YourApp PRIVATE RibbonLib::RibbonLib)
```

库目标 `RibbonLib` 只包含源码，不再编译 example 的 JSON、图标、QSS、翻译文件等资源。这样作为第三方库给别人使用时更干净，不会把示例资源强行带入业务程序。

### 资源归属

当前资源归属规则：

```text
src/                  RibbonLib 库源码
example/resources/    仅供示例程序使用的 qss / json / icons / qrc
```

业务项目需要样式和图标时有两种方式：

### 方式一：业务项目自己编译 qrc

业务项目可以复制 `example/example/resources/styles/ribbon.qss` 到自己的资源目录，并在自己的 `.qrc` 中注册：

```xml
<qresource prefix="/">
    <file alias="example/resources/styles/ribbon.qss">styles/ribbon.qss</file>
</qresource>
```

然后：

```cpp
app.setStyleSheet(QFile 读取 qss 后 app.setStyleSheet());
```

默认读取：

```text
:/example/resources/styles/ribbon.qss
```

### 方式二：直接从文件加载 qss

不使用 qrc 时：

```cpp
app.setStyleSheet(QFile("config/ribbon.qss"));
```

### Qt 翻译文件

已删除示例中的：

```text
qt_zh_CN.qm
qtbase_zh_CN.qm
```

如果业务软件需要 Qt 内置翻译，应从本机 Qt 安装目录加载，例如：

```cpp
QLibraryInfo::location(QLibraryInfo::TranslationsPath)
```

不要把 Qt 自带 qm 固化进 RibbonLib。


## 本轮优化说明

本轮主要围绕源码库可维护性继续优化：

```text
1. RibbonAction 支持 params。
2. checkable / checked 支持 QAction 与 RibbonAction 双向同步。
3. QRibbonHelper 增加 errorString()。
4. JSON 加载失败、缺少 action、重复 id 会输出 qWarning。
5. ribbon.json item/menu/applicationMenu 支持 params 字段。
6. Office 2013 QSS 继续微调，Group 分隔线更浅，ApplicationMenu 更紧凑。
```

### ribbon.json 传递 params

Ribbon item 可以直接配置执行参数：

```json
{
  "id": "delete",
  "style": "large",
  "params": "selected"
}
```

下拉菜单项也支持：

```json
{
  "id": "export",
  "menu": [
    { "id": "exportPdf", "params": "pdf" },
    { "id": "exportDwg", "params": "dwg" }
  ]
}
```

点击按钮后执行链路为：

```text
QAction::triggered
  -> QRibbonHelper 查找 RibbonAction
  -> RibbonAction::validate()
  -> RibbonAction::execute(params)
  -> QRibbonHelper::actionTriggered(id)
```

### checked 双向同步

如果业务 Action 是可勾选动作：

```cpp
action->setCheckable(true);
action->setChecked(true);
```

`QRibbonHelper` 会同步到内部 `QAction`。用户点击按钮改变 checked 状态时，也会同步回 `RibbonAction`。

### 错误信息

加载失败时可以读取：

```cpp
if (!helper->loadFromResources()) {
    qWarning() << helper->errorString();
}
```

构建失败时也可以读取同一个 `errorString()`。


## 继续优化内容

本轮继续做库质量优化，保持不新增复杂控件：

```text
1. RibbonAction 增加 trigger(params) 统一执行入口。
2. RibbonAction 执行成功/拒绝统一由 trigger 发出 executed / executeRejected。
3. QRibbonHelper 增加 QAction 与 QRibbonButton / QRibbonSplitButton 的统一绑定函数。
4. actions.json 支持 enabled / visible / checkable / checked。
5. QRibbonStateManager 支持 settingsPrefix，避免多个 Ribbon 共用 QSettings 时 key 冲突。
6. CMake 增加 RIBBONLIB_BUILD_SHARED，可选择静态库或动态库。
7. ApplicationMenu 间距继续压缩，Office 2013 样式继续细调。
```

### RibbonAction::trigger

业务侧通常只需要实现：

```cpp
void execute(const QString &params) override;
void cleanup() override;
```

执行时可以统一调用：

```cpp
action->trigger("selected");
```

内部会自动：

```text
validate()
execute(params)
executed(id, params)
executeRejected(id, params)
```

### actions.json 状态字段

`actions.json` 支持：

```json
{
  "id": "snap",
  "name": "捕捉",
  "checkable": true,
  "checked": true,
  "enabled": true,
  "visible": true
}
```

### 多 Ribbon 状态隔离

```cpp
QRibbonStateManager manager(ribbon);
manager.setSettingsPrefix("MainWindow/Ribbon");
manager.restoreState();
```

### 构建动态库

```bash
cmake -S . -B build -DRIBBONLIB_BUILD_SHARED=ON
```


## 最终库结构建议

当前版本已经按源码库方式收敛，推荐作为第三方子项目使用：

```text
RibbonLib/
├─ src/                  库源码与公开头文件
├─ example/              示例程序
│  └─ resources/         仅示例使用的 qss/json/icons/qrc
├─ CMakeLists.txt
└─ README.md
```

库本体不依赖 example 资源，不内置 Qt qm 翻译文件，不绑定业务上下文。

### 动态库导出

当前版本增加了：

```text
src/RibbonLibGlobal.h
```

公开类使用 `RIBBONLIB_EXPORT`，支持：

```text
静态库：RIBBONLIB_STATIC
动态库：RIBBONLIB_BUILD_LIBRARY
```

CMake 会根据 `RIBBONLIB_BUILD_SHARED` 自动设置。

### 安装导出

如果需要安装：

```bash
cmake -S . -B build -DRIBBONLIB_ENABLE_INSTALL=ON
cmake --build build --config Release
cmake --install build --config Release
```

安装后会生成：

```text
RibbonLibTargets.cmake
```

### 资源使用原则

RibbonLib 不强制携带 QSS 和图标。业务项目可以：

```cpp
app.setStyleSheet(QFile 读取 qss 后 app.setStyleSheet(":/resources/styles/ribbon.qss"));
```

或者：

```cpp
app.setStyleSheet(QFile("config/ribbon.qss"));
```

### 完成状态

当前版本已经完成以下收敛：

```text
1. 基础 Ribbon 控件稳定。
2. Office 2013 风格 QSS 独立维护。
3. RibbonAction 基础动作体系。
4. QAction / RibbonAction / Button 同步。
5. JSON 构建和错误提示。
6. example 资源独立。
7. qm 移除。
8. 源码库 / 子项目 / install/export 支持。
```


## 构建修复记录

针对 Qt 5.14.2 + VS2017 构建修复：

```text
1. QRibbonHelper 补齐 m_errorString 成员，修复 errorString()/warn()/loadFromResources() 编译错误。
2. 移除 QMenu::setIconSize() 调用，避免 Qt 5.14 环境中 QMenu 接口不兼容。
3. 菜单图标尺寸与间距继续通过 QAction 图标和 QSS 控制，不在 QMenu API 中硬控。
4. 检查并避免导出宏误用于前向声明。
```


## 文件系统资源模式

示例程序不再把 `json/png/qss` 编译进 qrc，改为构建后复制到 exe 输出目录：

```text
resource/
  styles/ribbon.qss
example/
  resources/
    actions.json
    ribbon.json
    icons/...
```

`example/CMakeLists.txt` 中通过 post-build 命令复制：

```cmake
add_custom_command(TARGET RibbonExample POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/resources
            $<TARGET_FILE_DIR:RibbonExample>/resources
)
```

示例程序读取：

```cpp
const QString resourceDir = QCoreApplication::applicationDirPath() + "/resources";
helper->loadFromResources(resourceDir + "/ribbon.json",
                          resourceDir + "/actions.json");

app.setStyleSheet(QFile(
    QCoreApplication::applicationDirPath() + "/resources/styles/ribbon.qss"));
```

`actions.json` 中的图标路径使用相对路径：

```json
{
  "id": "open",
  "icon": "icons/file/open.png"
}
```

`QRibbonHelper` 会以 `actions.json` 所在目录作为基准目录解析相对图标路径。

这种方式更适合给别人作为源码库使用：库本体不携带资源，业务项目可以自由决定资源目录、qss 和图标。


## RibbonLib 资源与 example 资源分离

当前资源分层如下：

```text
RibbonLib/
├─ src/
│  └─ 库源码
│
├─ resource/
│  └─ styles/ribbon.qss
│     RibbonLib 库级样式资源，与 src 同级
│
└─ example/
   └─ resources/
      ├─ actions.json
      ├─ ribbon.json
      └─ icons/
         示例使用的 JSON 和图标资源
```

也就是说：

```text
RibbonLib/resource        属于库
RibbonLib/example/resources 属于示例
```

两者不再混放。

### 示例输出目录

示例程序构建后输出：

```text
example/
  resources/
    actions.json
    ribbon.json
    styles/ribbon.qss
    icons/
```

示例中 QSS 读取：

```cpp
app.setStyleSheet(QFile(
    QCoreApplication::applicationDirPath() + "/example/resources/styles/ribbon.qss"));
```

示例中 JSON 读取：

```cpp
const QString resourceDir =
    QCoreApplication::applicationDirPath() + "/example/resources";

helper->loadFromResources(resourceDir + "/ribbon.json",
                          resourceDir + "/actions.json");
```

业务项目也可以按这个约定复制 RibbonLib 的 `resource` 目录。


## example qss 资源位置修正

示例程序运行时只依赖一个示例资源目录：

```text
example/
  resources/
    actions.json
    ribbon.json
    styles/ribbon.qss
    icons/
```

因此 example 中的 QSS 读取路径为：

```cpp
QFile(
    QCoreApplication::applicationDirPath()
    + "/example/resources/styles/ribbon.qss");
```

`example/resources/styles/ribbon.qss` 是示例运行资源，构建后会被复制到输出目录。

根目录 `resource/styles/ribbon.qss` 可作为库的默认样式模板保留，供业务项目复制或参考，但 example 不再依赖输出目录下的 `resource/`。


## QRibbonTheme 删除说明

当前版本已经删除 `QRibbonTheme.h/.cpp`。

原因：

```text
1. 当前采用文件系统资源模式，qss 不再编译进 RibbonLib。
2. QRibbonTheme 只剩读取 qss 文件的薄包装，作为库 API 价值不大。
3. 样式资源应该由业务项目或 example 自己管理。
```

示例程序现在直接读取：

```cpp
QFile qssFile(QCoreApplication::applicationDirPath()
              + "/example/resources/styles/ribbon.qss");

if (qssFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    app.setStyleSheet(QString::fromUtf8(qssFile.readAll()));
}
```

业务项目集成 RibbonLib 时，也建议自行决定 qss 放在哪里、如何加载。


## build 发布目录

当前 CMake 会在构建目录下生成便于引用的发布结构：

```text
build/
├─ bin/
│  ├─ RibbonLib.dll          # 动态库模式时
│  ├─ *.pdb                  # Debug/符号文件
│
├─ lib/
│  └─ RibbonLib.lib          # 静态库或导入库
│
├─ inc/
│  └─ RibbonLib/
│     ├─ QRibbonWidget.h
│     ├─ QRibbonButton.h
│     ├─ QRibbonHelper.h
│     ├─ RibbonAction.h
│     └─ ...
│
└─ resource/
   └─ styles/ribbon.qss      # RibbonLib 库级 qss 模板
```

对应 CMake 变量：

```cmake
RIBBONLIB_PUBLISH_ROOT
RIBBONLIB_BIN_DIR
RIBBONLIB_LIB_DIR
RIBBONLIB_INC_DIR
RIBBONLIB_RESOURCE_DIR
```

默认：

```cmake
RIBBONLIB_PUBLISH_ROOT = ${CMAKE_BINARY_DIR}
```

也可以自定义：

```bash
cmake -S . -B build -DRIBBONLIB_PUBLISH_ROOT=D:/RibbonLibPublish
```

业务项目可以直接引用：

```cmake
include_directories(<RibbonLib-build>/inc)
link_directories(<RibbonLib-build>/lib)
```

更推荐的方式仍然是源码子项目引用：

```cmake
add_subdirectory(thirdparty/RibbonLib)
target_link_libraries(YourApp PRIVATE RibbonLib::RibbonLib)
```


## example 不参与发布目录

当前发布目录只发布 RibbonLib 本体：

```text
build/
├─ bin/
│  ├─ RibbonLib.dll          # 动态库模式时
│  └─ *.pdb
│
├─ lib/
│  └─ RibbonLib.lib          # 静态库或导入库
│
├─ inc/
│  └─ RibbonLib/
│     └─ 公开头文件
│
└─ resource/
   └─ styles/ribbon.qss
```

`RibbonExample` 仍然可以构建和运行，但输出到 example 自己的构建目录：

```text
build/example/bin/
    example/resources/
```

这样 `build/bin / build/lib / build/inc / build/resource` 可以直接作为 RibbonLib 发布包，不会混入示例程序和示例资源。


## 发布目录最终规则

发布目录只包含 RibbonLib 本体，不包含 example，也不生成 `.in` 相关配置文件。

```text
build/
├─ bin/
│  ├─ RibbonLib.dll          # 动态库模式时
│  └─ *.pdb
│
├─ lib/
│  └─ RibbonLib.lib          # 静态库或导入库
│
├─ inc/
│  └─ RibbonLib/
│     └─ 公开头文件
│
└─ resource/
   └─ styles/ribbon.qss
```

example 仅用于本仓库演示，输出在：

```text
build/example/bin/
```

不进入 RibbonLib 发布目录。


## 发布头文件复制修复

针对 VS2017 生成器修复 `RibbonLibPublish`：

```text
Error copying file (if different) from "src/xxx.h"
```

原因是自定义命令执行目录不一定是源码根目录，`src/xxx.h` 相对路径在 `RibbonLibPublish.vcxproj` 中无法解析。

现在 CMake 会先把公开头文件转换为绝对路径：

```cmake
foreach(_header ${RIBBON_PUBLIC_HEADERS})
    list(APPEND RIBBON_PUBLIC_HEADER_FILES
         "${CMAKE_CURRENT_SOURCE_DIR}/${_header}")
endforeach()
```

再执行：

```cmake
cmake -E copy_if_different ${RIBBON_PUBLIC_HEADER_FILES} ...
```

可兼容 Visual Studio 2017 生成器。

## 最新完成版说明

本版整合：

```text
1. 发布目录为 build/publish/inc、build/publish/lib、build/publish/resource。
2. inc 下直接放公开头文件，不生成 inc/RibbonLib。
3. resource 下直接放 ribbon.qss，不生成 resource/styles。
4. example 不进入发布目录。
5. 删除 QRibbonTheme，example 自己用 QFile 读取 qss。
6. 删除 .in 文件与 configure_package_config_file。
7. Ribbon 主按钮宽度按内容自适应，不设置固定宽度。
8. Ribbon 图标按标准分级：Large=32x32，Small=16x16，Access=16x16。
```


## 隐藏横向滚动条

Ribbon panel 区域内容超出宽度时：

```text
1. 不显示横向滚动条。
2. 不允许上下滚动。
3. 鼠标滚轮会转换为横向滚动。
```

实现方式：

```cpp
m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
```

滚轮事件中仍然使用隐藏的 `horizontalScrollBar()` 作为内部滚动位置控制器：

```cpp
QScrollBar *hbar = m_scrollArea->horizontalScrollBar();
hbar->setValue(hbar->value() - delta / 2);
```

## 本版修复重点

```text
1. QRibbonGroup 宽度按内容自然计算，不设置固定宽度。
2. QRibbonGroup 只提供 sizeHint/minimumSizeHint，右侧 stretch 只吃剩余空间。
3. 小按钮列宽度按列内最大按钮自然宽度计算。
4. QRibbonTabContent 宽度按所有 group 总宽度计算。
5. Ribbon panel 不显示横向滚动条，但鼠标滚轮可横向滚动。
6. 全库无 QStringLiteral。
7. 发布目录为 build/publish/inc、lib、resource，example 不发布。
```

## 代码注释与冗余清理

本版补充并整理了核心代码中文注释：

```text
1. QRibbonWidget：说明整体 Ribbon 结构与职责。
2. QRibbonTab：说明隐藏滚动条、滚轮横向滚动、内容宽度计算。
3. QRibbonGroup：说明宽度按内容自然计算，不固定宽度。
4. QRibbonButton / QRibbonSplitButton：说明按钮宽度自适应和图标分级。
5. QRibbonHelper / RibbonAction：说明 JSON 构建和动作绑定边界。
6. QRibbonMetrics：说明尺寸常量与 QSS 职责边界。
```

同时清理了：

```text
1. 重复 include。
2. 过期注释。
3. QRibbonSplitButton 旧固定宽度缓存相关残留。
4. 主按钮 setFixedWidth / setMaximumWidth 相关逻辑。
5. 全库 QStringLiteral。
```

## 问号占位图标

当按钮没有设置图标，或者图标路径不存在导致 `QIcon` 为空时：

```text
1. Large 按钮使用 Qt 内置问号图标，占位尺寸 32x32。
2. Small 按钮使用 Qt 内置问号图标，占位尺寸 16x16。
3. 不额外引入 png/qrc 资源。
4. 占位图标来自当前 Qt Style：QStyle::SP_MessageBoxQuestion。
```

## 第一次显示宽度刷新

Ribbon 分组中的按钮通常在 `addGroup()` 之后继续添加。旧逻辑只在 `addGroup()` 时计算一次
`RibbonTabContent` 宽度，导致第一次显示时 group 宽度偏小，需要改变窗口大小才刷新。

本版修复：

```text
1. QRibbonGroup 内容宽度变化时发出 layoutChanged。
2. QRibbonTab 连接 layoutChanged，并通过 QTimer::singleShot(0) 延迟刷新内容总宽度。
3. QRibbonTab 在 showEvent / resizeEvent / tabChanged 时刷新布局。
4. 右侧 stretch 仍只吃空白，不参与 group 自然宽度计算。
```

## RibbonButton 内容左对齐

本版调整按钮绘制：

```text
1. QRibbonButton 大按钮图标从左侧 padding 开始绘制，不再居中。
2. QRibbonButton 大按钮文字左对齐。
3. QRibbonButton 小按钮保持图标在左，文字左对齐并垂直居中。
4. QRibbonSplitButton 同步采用左对齐规则。
```

## 分割按钮对齐与菜单紧凑

```text
1. QRibbonSplitButton 的下拉箭头统一放在右侧窄区域。
2. Large / Small 分割按钮的图标和文字与普通按钮左对齐规则一致。
3. 下拉箭头不再占用底部一整行，避免与普通按钮行高不一致。
4. QMenu 菜单项高度、padding、separator margin 收紧，避免菜单过高过松散。
```

## 菜单图标文字间距

本版进一步收紧 QMenu 图标列与文字列间距：

```text
1. SplitMenu 菜单项左侧 padding 从 20px 调整为 18px。
2. 菜单项右侧 padding 从 14px 调整为 10px。
3. QMenu::icon / RibbonSplitMenu::icon 的 margin 与 padding 均置 0。
4. 不使用 QMenu::setIconSize，避免 Qt 5.14 / VS2017 下接口不兼容。
```

## 空 RibbonGroup 隐藏

本版处理空分组：

```text
1. QRibbonGroup 创建后默认隐藏。
2. 只有调用 addButton / addLargeWidget / addSmallWidget / addWidget / addSeparator 添加内容后才显示。
3. 只有标题但没有内容的分组不显示、不占宽度。
4. QRibbonTab 计算内容总宽度时继续跳过隐藏分组。
```
