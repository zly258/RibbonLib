#include "MainWindow.h"
#include "QRibbonWidget.h"
#include "QRibbonTab.h"
#include "QRibbonGroup.h"
#include "QRibbonButton.h"
#include "QRibbonButtonSize.h"
#include "QApplicationButton.h"
#include "RibbonJsonWindow.h"

#include <QTextEdit>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>
#include <QAction>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_isChinese(false)
    , m_ribbonWidget(nullptr)
    , m_appButton(nullptr)
    , m_homeTab(nullptr)
    , m_fileGroup(nullptr)
    , m_editGroup(nullptr)
    , m_newBtn(nullptr)
    , m_openBtn(nullptr)
    , m_saveBtn(nullptr)
    , m_jsonBtn(nullptr)
    , m_langBtn(nullptr)
    , m_copyBtn(nullptr)
    , m_pasteBtn(nullptr)
    , m_cutBtn(nullptr)
    , m_saveAction(nullptr)
    , m_undoAction(nullptr)
    , m_redoAction(nullptr)
    , m_langAction(nullptr)
    , m_centralWidget(nullptr)
    , m_jsonWindow(nullptr)
{
    setMinimumSize(860, 600);

    // Create central text editor
    m_centralWidget = new QTextEdit(this);
    setCentralWidget(m_centralWidget);

    setupRibbon();
    retranslateUi();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupRibbon()
{
    // Create root Ribbon control widget
    m_ribbonWidget = new QRibbonWidget(this);

    // Application Button (top-left)
    m_appButton = new QApplicationButton(this);
    m_appButton->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    m_ribbonWidget->setApplicationButton(m_appButton);

    // Access Bar actions (top-right quick access)
    m_saveAction = new QAction(this);
    m_saveAction->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::onSaveFile);
    m_ribbonWidget->addAccessBarAction(m_saveAction);

    m_undoAction = new QAction(this);
    m_undoAction->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
    connect(m_undoAction, &QAction::triggered, this, [this]() {
        if (m_centralWidget) m_centralWidget->undo();
    });
    m_ribbonWidget->addAccessBarAction(m_undoAction);

    m_redoAction = new QAction(this);
    m_redoAction->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    connect(m_redoAction, &QAction::triggered, this, [this]() {
        if (m_centralWidget) m_centralWidget->redo();
    });
    m_ribbonWidget->addAccessBarAction(m_redoAction);

    // Language toggle in access bar
    m_langAction = new QAction(this);
    m_langAction->setIcon(style()->standardIcon(QStyle::SP_FileDialogInfoView));
    connect(m_langAction, &QAction::triggered, this, &MainWindow::toggleLanguage);
    m_ribbonWidget->addAccessBarAction(m_langAction);

    // Home tab
    m_homeTab = m_ribbonWidget->addTab("Home");

    // File group
    m_fileGroup = m_homeTab->addGroup("File");

    m_newBtn = new QRibbonButton(this);
    m_newBtn->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));
    m_newBtn->setButtonSize(QRibbonButtonSize::Large);
    connect(m_newBtn, &QRibbonButton::clicked, this, &MainWindow::onNewFile);
    m_fileGroup->addButton(m_newBtn);

    m_openBtn = new QRibbonButton(this);
    m_openBtn->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    m_openBtn->setButtonSize(QRibbonButtonSize::Large);
    connect(m_openBtn, &QRibbonButton::clicked, this, &MainWindow::onOpenFile);
    m_fileGroup->addButton(m_openBtn);

    m_saveBtn = new QRibbonButton(this);
    m_saveBtn->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    m_saveBtn->setButtonSize(QRibbonButtonSize::Large);
    connect(m_saveBtn, &QRibbonButton::clicked, this, &MainWindow::onSaveFile);
    m_fileGroup->addButton(m_saveBtn);

    m_jsonBtn = new QRibbonButton(this);
    m_jsonBtn->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
    m_jsonBtn->setButtonSize(QRibbonButtonSize::Large);
    connect(m_jsonBtn, &QRibbonButton::clicked, this, &MainWindow::onOpenJsonWindow);
    m_fileGroup->addButton(m_jsonBtn);

    m_langBtn = new QRibbonButton(this);
    m_langBtn->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    m_langBtn->setButtonSize(QRibbonButtonSize::Large);
    connect(m_langBtn, &QRibbonButton::clicked, this, &MainWindow::toggleLanguage);
    m_fileGroup->addButton(m_langBtn);

    // Edit group
    m_editGroup = m_homeTab->addGroup("Edit");

    m_copyBtn = new QRibbonButton(this);
    m_copyBtn->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    m_copyBtn->setButtonSize(QRibbonButtonSize::Small);
    connect(m_copyBtn, &QRibbonButton::clicked, this, &MainWindow::onCopy);
    m_editGroup->addButton(m_copyBtn);

    m_pasteBtn = new QRibbonButton(this);
    m_pasteBtn->setIcon(style()->standardIcon(QStyle::SP_DialogYesButton));
    m_pasteBtn->setButtonSize(QRibbonButtonSize::Small);
    connect(m_pasteBtn, &QRibbonButton::clicked, this, &MainWindow::onPaste);
    m_editGroup->addButton(m_pasteBtn);

    m_cutBtn = new QRibbonButton(this);
    m_cutBtn->setIcon(style()->standardIcon(QStyle::SP_DialogDiscardButton));
    m_cutBtn->setButtonSize(QRibbonButtonSize::Small);
    connect(m_cutBtn, &QRibbonButton::clicked, this, &MainWindow::onCut);
    m_editGroup->addButton(m_cutBtn);

    // Set Ribbon as the main menu widget
    setMenuWidget(m_ribbonWidget);
}

void MainWindow::setLanguage(bool chinese)
{
    m_isChinese = chinese;
    retranslateUi();
}

void MainWindow::toggleLanguage()
{
    m_isChinese = !m_isChinese;
    retranslateUi();
    if (m_jsonWindow) {
        m_jsonWindow->setLanguage(m_isChinese);
    }
}

void MainWindow::retranslateUi()
{
    if (m_isChinese) {
        setWindowTitle("RibbonLib 示例程序");
        if (m_appButton) m_appButton->setText("文件");
        if (m_saveAction) m_saveAction->setText("保存");
        if (m_undoAction) m_undoAction->setText("撤销");
        if (m_redoAction) m_redoAction->setText("重做");
        if (m_langAction) {
            m_langAction->setText("English");
            m_langAction->setToolTip("切换为英文 (Switch to English)");
        }
        if (m_homeTab) m_homeTab->setTitle("开始");
        if (m_fileGroup) m_fileGroup->setTitle("文件");
        if (m_editGroup) m_editGroup->setTitle("编辑");

        if (m_newBtn) m_newBtn->setText("新建");
        if (m_openBtn) m_openBtn->setText("打开");
        if (m_saveBtn) m_saveBtn->setText("保存");
        if (m_jsonBtn) m_jsonBtn->setText("JSON窗口");
        if (m_langBtn) m_langBtn->setText("切换英文");

        if (m_copyBtn) m_copyBtn->setText("复制");
        if (m_pasteBtn) m_pasteBtn->setText("粘贴");
        if (m_cutBtn) m_cutBtn->setText("剪切");

        if (m_centralWidget && m_centralWidget->toPlainText().isEmpty()) {
            m_centralWidget->setPlainText(
                "欢迎使用 RibbonLib 示例程序！\n\n"
                "• 本界面通过 C++ 代码构建。\n"
                "• 点击“JSON窗口”查看基于 JSON 配置生成的 Ribbon。\n"
                "• 点击“切换英文”或右上角语言按钮可在中文与英文之间自由切换。\n"
                "• 支持 Large 与 Small 尺寸按钮、Office 2013 扁平风格与平滑滚轮横向滚动。"
            );
        }
    } else {
        setWindowTitle("RibbonLib Example");
        if (m_appButton) m_appButton->setText("File");
        if (m_saveAction) m_saveAction->setText("Save");
        if (m_undoAction) m_undoAction->setText("Undo");
        if (m_redoAction) m_redoAction->setText("Redo");
        if (m_langAction) {
            m_langAction->setText("中文");
            m_langAction->setToolTip("Switch to Chinese / 切换为中文");
        }
        if (m_homeTab) m_homeTab->setTitle("Home");
        if (m_fileGroup) m_fileGroup->setTitle("File");
        if (m_editGroup) m_editGroup->setTitle("Edit");

        if (m_newBtn) m_newBtn->setText("New");
        if (m_openBtn) m_openBtn->setText("Open");
        if (m_saveBtn) m_saveBtn->setText("Save");
        if (m_jsonBtn) m_jsonBtn->setText("JSON Window");
        if (m_langBtn) m_langBtn->setText("Switch to 中文");

        if (m_copyBtn) m_copyBtn->setText("Copy");
        if (m_pasteBtn) m_pasteBtn->setText("Paste");
        if (m_cutBtn) m_cutBtn->setText("Cut");

        if (m_centralWidget && m_centralWidget->toPlainText().isEmpty()) {
            m_centralWidget->setPlainText(
                "Welcome to the RibbonLib Example application!\n\n"
                "• This window is constructed using native C++ API.\n"
                "• Click 'JSON Window' to explore Ribbon creation driven by JSON configuration.\n"
                "• Click 'Switch to 中文' or the top-right language button to toggle between English and Chinese.\n"
                "• Supports Large/Small button sizes, Office 2013 flat styling, and mouse wheel horizontal scrolling."
            );
        }
    }
}

void MainWindow::onNewFile()
{
    if (m_centralWidget) {
        m_centralWidget->clear();
    }
    if (m_isChinese) {
        QMessageBox::information(this, "新建", "创建新文件");
    } else {
        QMessageBox::information(this, "New", "Create a new file");
    }
}

void MainWindow::onOpenFile()
{
    const QString title = m_isChinese ? "打开文件" : "Open File";
    const QString filter = m_isChinese ? "文本文件 (*.txt)" : "Text Files (*.txt)";
    QString fileName = QFileDialog::getOpenFileName(this, title, "", filter);
    if (!fileName.isEmpty()) {
        const QString msg = (m_isChinese ? "打开文件: " : "Opened file: ") + fileName;
        QMessageBox::information(this, title, msg);
    }
}

void MainWindow::onSaveFile()
{
    const QString title = m_isChinese ? "保存文件" : "Save File";
    const QString filter = m_isChinese ? "文本文件 (*.txt)" : "Text Files (*.txt)";
    QString fileName = QFileDialog::getSaveFileName(this, title, "", filter);
    if (!fileName.isEmpty()) {
        const QString msg = (m_isChinese ? "保存文件: " : "Saved file: ") + fileName;
        QMessageBox::information(this, title, msg);
    }
}

void MainWindow::onCopy()
{
    if (m_centralWidget) {
        m_centralWidget->copy();
    }
}

void MainWindow::onPaste()
{
    if (m_centralWidget) {
        m_centralWidget->paste();
    }
}

void MainWindow::onCut()
{
    if (m_centralWidget) {
        m_centralWidget->cut();
    }
}

void MainWindow::onOpenJsonWindow()
{
    if (!m_jsonWindow) {
        m_jsonWindow = new RibbonJsonWindow(this);
    }
    m_jsonWindow->setLanguage(m_isChinese);
    m_jsonWindow->show();
    m_jsonWindow->raise();
    m_jsonWindow->activateWindow();
}