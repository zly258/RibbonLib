#include "RibbonJsonWindow.h"
#include "ExampleRibbonAction.h"
#include "../src/QRibbonWidget.h"
#include "../src/QRibbonHelper.h"
#include <QTextEdit>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>
#include <QCoreApplication>
#include <QAction>
#include <QStyle>

RibbonJsonWindow::RibbonJsonWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_isChinese(false)
    , m_ribbonWidget(nullptr)
    , m_ribbonHelper(nullptr)
    , m_centralWidget(nullptr)
    , m_langAction(nullptr)
{
    setMinimumSize(1000, 700);

    setupCentralWidget();
    setupRibbon();
    retranslateUi();
}

RibbonJsonWindow::~RibbonJsonWindow()
{
    if (m_ribbonHelper) {
        delete m_ribbonHelper;
    }
}

void RibbonJsonWindow::setLanguage(bool chinese)
{
    if (m_isChinese != chinese) {
        m_isChinese = chinese;
        setupRibbon();
        retranslateUi();
    }
}

void RibbonJsonWindow::toggleLanguage()
{
    m_isChinese = !m_isChinese;
    setupRibbon();
    retranslateUi();
}

void RibbonJsonWindow::setupRibbon()
{
    // Clean up existing ribbon widget
    if (m_ribbonWidget) {
        setMenuWidget(nullptr);
        delete m_ribbonWidget;
        m_ribbonWidget = nullptr;
    }
    m_ribbonWidget = new QRibbonWidget(this);

    // Clean up existing helper
    if (m_ribbonHelper) {
        delete m_ribbonHelper;
        m_ribbonHelper = nullptr;
    }
    m_ribbonHelper = new QRibbonHelper(this);

    // Locate JSON resource files
    const QString resourceDir = QCoreApplication::applicationDirPath() + "/example/resources";
    const QString ribbonFile = m_isChinese ? "/ribbon_zh.json" : "/ribbon.json";
    const QString actionsFile = m_isChinese ? "/actions_zh.json" : "/actions.json";
    const QString ribbonPath = resourceDir + ribbonFile;
    const QString actionsPath = resourceDir + actionsFile;

    if (m_ribbonHelper->loadFromResources(ribbonPath, actionsPath)) {
        // Register business RibbonAction instances
        if (m_isChinese) {
            m_ribbonHelper->registerRibbonAction(new ExampleRibbonAction("new", "新建", m_centralWidget, m_ribbonHelper));
            m_ribbonHelper->registerRibbonAction(new ExampleRibbonAction("copy", "复制", m_centralWidget, m_ribbonHelper));
            m_ribbonHelper->registerRibbonAction(new ExampleRibbonAction("delete", "删除", m_centralWidget, m_ribbonHelper));
        } else {
            m_ribbonHelper->registerRibbonAction(new ExampleRibbonAction("new", "New", m_centralWidget, m_ribbonHelper));
            m_ribbonHelper->registerRibbonAction(new ExampleRibbonAction("copy", "Copy", m_centralWidget, m_ribbonHelper));
            m_ribbonHelper->registerRibbonAction(new ExampleRibbonAction("delete", "Delete", m_centralWidget, m_ribbonHelper));
        }

        // Build Ribbon from JSON structure
        m_ribbonHelper->buildRibbon(m_ribbonWidget);
    } else {
        const QString err = m_ribbonHelper->errorString();
        QMessageBox::warning(this, m_isChinese ? "错误" : "Error",
                             (m_isChinese ? "加载 JSON 失败:\n" : "Failed to load JSON:\n") + err);

        // Fallback default tab
        auto homeTab = m_ribbonWidget->addTab(m_isChinese ? "开始" : "Home");
        auto fileGroup = homeTab->addGroup(m_isChinese ? "文件" : "File");

        auto newBtn = new QRibbonButton(this);
        newBtn->setText(m_isChinese ? "新建" : "New");
        newBtn->setButtonSize(QRibbonButtonSize::Large);
        connect(newBtn, &QRibbonButton::clicked, this, &RibbonJsonWindow::onNewFile);
        fileGroup->addButton(newBtn);
    }

    // Add language toggle action in AccessBar
    m_langAction = new QAction(this);
    m_langAction->setText(m_isChinese ? "English" : "中文");
    m_langAction->setToolTip(m_isChinese ? "切换为英文 (Switch to English)" : "Switch to Chinese / 切换为中文");
    m_langAction->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    connect(m_langAction, &QAction::triggered, this, &RibbonJsonWindow::toggleLanguage);
    m_ribbonWidget->addAccessBarAction(m_langAction);

    setMenuWidget(m_ribbonWidget);
    connectActions();
}

void RibbonJsonWindow::setupCentralWidget()
{
    m_centralWidget = new QTextEdit(this);
    setCentralWidget(m_centralWidget);
}

void RibbonJsonWindow::retranslateUi()
{
    if (m_isChinese) {
        setWindowTitle("RibbonLib JSON配置示例");
        m_centralWidget->setPlainText(
            "这是一个使用 JSON 配置文件加载 Ribbon 界面的示例窗口。\n\n"
            "Ribbon 界面由以下 JSON 配置文件驱动：\n"
            "• ribbon.json / ribbon_zh.json: 定义 Ribbon 选项卡、面板及按钮布局\n"
            "• actions.json / actions_zh.json: 定义动作属性（名称、图标、快捷键等）\n\n"
            "点击右上角快速访问栏中的语言切换按钮可在中英文之间自由切换。\n"
            "所有动作均由 QRibbonHelper 动态构建并与 QAction / RibbonAction 双向绑定。"
        );
    } else {
        setWindowTitle("RibbonLib JSON Configuration Example");
        m_centralWidget->setPlainText(
            "This window demonstrates a Ribbon interface dynamically loaded from JSON configuration files.\n\n"
            "The Ribbon UI is driven by:\n"
            "• ribbon.json / ribbon_zh.json: defines Ribbon structure, tabs, panels, and item layout\n"
            "• actions.json / actions_zh.json: defines action properties including labels, icons, and shortcuts\n\n"
            "Click the language toggle icon in the top-right access bar to switch between English and Chinese.\n"
            "All actions are dynamically constructed and dispatched via QRibbonHelper."
        );
    }
}

void RibbonJsonWindow::connectActions()
{
    if (!m_ribbonHelper) return;

    const QMap<QString, QAction*> &actions = m_ribbonHelper->actions();

    // File actions
    if (auto action = actions.value("new", nullptr)) {
        connect(action, &QAction::triggered, this, &RibbonJsonWindow::onNewFile);
    }
    if (auto action = actions.value("open", nullptr)) {
        connect(action, &QAction::triggered, this, &RibbonJsonWindow::onOpenFile);
    }
    if (auto action = actions.value("save", nullptr)) {
        connect(action, &QAction::triggered, this, &RibbonJsonWindow::onSaveFile);
    }
    if (auto action = actions.value("exit", nullptr)) {
        connect(action, &QAction::triggered, this, &RibbonJsonWindow::onExit);
    }

    // Edit actions
    if (auto action = actions.value("copy", nullptr)) {
        connect(action, &QAction::triggered, this, &RibbonJsonWindow::onCopy);
    }
    if (auto action = actions.value("paste", nullptr)) {
        connect(action, &QAction::triggered, this, &RibbonJsonWindow::onPaste);
    }
    if (auto action = actions.value("cut", nullptr)) {
        connect(action, &QAction::triggered, this, &RibbonJsonWindow::onCut);
    }
    if (auto action = actions.value("undo", nullptr)) {
        connect(action, &QAction::triggered, this, &RibbonJsonWindow::onUndo);
    }
    if (auto action = actions.value("redo", nullptr)) {
        connect(action, &QAction::triggered, this, &RibbonJsonWindow::onRedo);
    }
    if (auto action = actions.value("delete", nullptr)) {
        connect(action, &QAction::triggered, this, &RibbonJsonWindow::onDelete);
    }
    if (auto action = actions.value("selectall", nullptr)) {
        connect(action, &QAction::triggered, this, &RibbonJsonWindow::onSelectAll);
    }

    // Additional actions
    if (auto action = actions.value("about", nullptr)) {
        connect(action, &QAction::triggered, this, &RibbonJsonWindow::onAbout);
    }
    if (auto action = actions.value("settings", nullptr)) {
        connect(action, &QAction::triggered, this, &RibbonJsonWindow::onSettings);
    }
}

void RibbonJsonWindow::onNewFile()
{
    if (m_centralWidget) {
        m_centralWidget->clear();
    }
    QMessageBox::information(this,
                             m_isChinese ? "新建" : "New",
                             m_isChinese ? "创建新文件" : "Create a new file");
}

void RibbonJsonWindow::onOpenFile()
{
    const QString title = m_isChinese ? "打开文件" : "Open File";
    const QString filter = m_isChinese ? "文本文件 (*.txt);;所有文件 (*.*)" : "Text Files (*.txt);;All Files (*.*)";
    QString fileName = QFileDialog::getOpenFileName(this, title, "", filter);
    if (!fileName.isEmpty()) {
        QMessageBox::information(this, title, (m_isChinese ? "打开文件: " : "Opened file: ") + fileName);
    }
}

void RibbonJsonWindow::onSaveFile()
{
    const QString title = m_isChinese ? "保存文件" : "Save File";
    const QString filter = m_isChinese ? "文本文件 (*.txt);;所有文件 (*.*)" : "Text Files (*.txt);;All Files (*.*)";
    QString fileName = QFileDialog::getSaveFileName(this, title, "", filter);
    if (!fileName.isEmpty()) {
        QMessageBox::information(this, title, (m_isChinese ? "保存文件: " : "Saved file: ") + fileName);
    }
}

void RibbonJsonWindow::onExit()
{
    close();
}

void RibbonJsonWindow::onCopy()
{
    if (m_centralWidget) {
        m_centralWidget->copy();
    }
}

void RibbonJsonWindow::onPaste()
{
    if (m_centralWidget) {
        m_centralWidget->paste();
    }
}

void RibbonJsonWindow::onCut()
{
    if (m_centralWidget) {
        m_centralWidget->cut();
    }
}

void RibbonJsonWindow::onUndo()
{
    if (m_centralWidget) {
        m_centralWidget->undo();
    }
}

void RibbonJsonWindow::onRedo()
{
    if (m_centralWidget) {
        m_centralWidget->redo();
    }
}

void RibbonJsonWindow::onDelete()
{
    if (m_centralWidget) {
        m_centralWidget->textCursor().removeSelectedText();
    }
}

void RibbonJsonWindow::onSelectAll()
{
    if (m_centralWidget) {
        m_centralWidget->selectAll();
    }
}

void RibbonJsonWindow::onAbout()
{
    if (m_isChinese) {
        QMessageBox::about(this, "关于",
            "RibbonLib JSON配置示例\n\n"
            "这个窗口展示了如何使用 QRibbonHelper 从 JSON 配置文件加载 Ribbon 界面。\n\n"
            "特性：\n"
            "• 从 ribbon.json / ribbon_zh.json 加载界面结构\n"
            "• 从 actions.json / actions_zh.json 加载动作定义\n"
            "• 支持图标、快捷键、工具提示等\n"
            "• 支持运行时双语切换\n"
            "• 完全可配置的 Ribbon 界面\n\n"
            "版本：1.0");
    } else {
        QMessageBox::about(this, "About",
            "RibbonLib JSON Configuration Example\n\n"
            "This window demonstrates how to load a Ribbon UI from JSON files using QRibbonHelper.\n\n"
            "Features:\n"
            "• Load UI layout from ribbon.json / ribbon_zh.json\n"
            "• Load action definitions from actions.json / actions_zh.json\n"
            "• Supports icons, shortcuts, tooltips, and styles\n"
            "• Runtime bilingual switching between English and Chinese\n"
            "• Fully configurable Ribbon structure\n\n"
            "Version: 1.0");
    }
}

void RibbonJsonWindow::onSettings()
{
    QMessageBox::information(this,
                             m_isChinese ? "设置" : "Settings",
                             m_isChinese ? "打开设置对话框" : "Open settings dialog");
}
