#include <initializer_list>

#include <QAction>
#include <QApplication>
#include <QDate>
#include <QHash>
#include <QMainWindow>
#include <QMessageBox>
#include <QStatusBar>
#include <QStyle>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QtAlgorithms>

#include <RibbonLib.h>

class DemoWindow final : public QMainWindow
{
public:
    DemoWindow()
    {
        resize(1180, 760);

        m_editor = new QTextEdit(this);
        m_editor->setStyleSheet(QStringLiteral(
            "QTextEdit { background: #f7f8fa; color: #202124; border: none; padding: 12px; }"));
        setCentralWidget(m_editor);

        m_ribbonHost = new QWidget(this);
        m_ribbonLayout = new QVBoxLayout(m_ribbonHost);
        m_ribbonLayout->setContentsMargins(0, 0, 0, 0);
        m_ribbonLayout->setSpacing(0);
        setMenuWidget(m_ribbonHost);

        rebuild();
    }

private:
    QString text(const char *english, const char *chinese) const
    {
        return m_chinese ? QString::fromUtf8(chinese) : QString::fromLatin1(english);
    }

    QIcon icon(QStyle::StandardPixmap pixmap) const
    {
        return style()->standardIcon(pixmap);
    }

    QAction *createAction(const QString &id,
                          const QString &title,
                          const QIcon &actionIcon = QIcon(),
                          const QKeySequence &shortcut = QKeySequence(),
                          bool checkable = false)
    {
        auto *result = new QAction(actionIcon, title, this);
        result->setObjectName(id);
        result->setShortcut(shortcut);
        result->setCheckable(checkable);
        connect(result, &QAction::triggered, this, [this, id]() { handleAction(id); });
        m_actions.insert(id, result);
        return result;
    }

    QAction *action(const char *id) const
    {
        return m_actions.value(QString::fromLatin1(id), nullptr);
    }

    QList<QAction*> actions(std::initializer_list<const char*> ids) const
    {
        QList<QAction*> result;
        for (const char *id : ids) {
            if (QAction *a = action(id)) result.append(a);
        }
        return result;
    }

    void createActions()
    {
        createAction("file.new", text("New", "新建"), icon(QStyle::SP_FileIcon), QKeySequence(QStringLiteral("Ctrl+N")));
        createAction("file.open", text("Open", "打开"), icon(QStyle::SP_DialogOpenButton), QKeySequence(QStringLiteral("Ctrl+O")));
        createAction("file.save", text("Save", "保存"), icon(QStyle::SP_DialogSaveButton), QKeySequence(QStringLiteral("Ctrl+S")));
        createAction("file.close", text("Close", "关闭"), icon(QStyle::SP_DialogCloseButton));

        createAction("edit.undo", text("Undo", "撤销"), icon(QStyle::SP_ArrowBack), QKeySequence(QStringLiteral("Ctrl+Z")));
        createAction("edit.redo", text("Redo", "重做"), icon(QStyle::SP_ArrowForward), QKeySequence(QStringLiteral("Ctrl+Y")));
        createAction("edit.cut", text("Cut", "剪切"), icon(QStyle::SP_DialogCancelButton), QKeySequence(QStringLiteral("Ctrl+X")));
        createAction("edit.copy", text("Copy", "复制"), icon(QStyle::SP_FileIcon), QKeySequence(QStringLiteral("Ctrl+C")));
        createAction("edit.paste", text("Paste", "粘贴"), icon(QStyle::SP_DialogApplyButton), QKeySequence(QStringLiteral("Ctrl+V")));
        createAction("edit.selectAll", text("Select All", "全选"), icon(QStyle::SP_FileDialogListView), QKeySequence(QStringLiteral("Ctrl+A")));
        createAction("edit.find", text("Find Ribbon", "查找 Ribbon"), icon(QStyle::SP_MessageBoxQuestion), QKeySequence(QStringLiteral("Ctrl+F")));

        createAction("insert.heading", text("Heading", "标题"), icon(QStyle::SP_FileIcon));
        createAction("insert.list", text("List", "列表"), icon(QStyle::SP_FileDialogListView));
        createAction("insert.table", text("Table", "表格"), icon(QStyle::SP_FileDialogDetailedView));
        createAction("insert.link", text("Link", "链接"), icon(QStyle::SP_ArrowForward));
        createAction("insert.date", text("Date", "日期"), icon(QStyle::SP_MessageBoxInformation));

        createAction("view.zoomIn", text("Zoom In", "放大"), icon(QStyle::SP_ArrowUp));
        createAction("view.zoomOut", text("Zoom Out", "缩小"), icon(QStyle::SP_ArrowDown));
        createAction("view.resetZoom", text("Reset Zoom", "重置缩放"), icon(QStyle::SP_DialogResetButton));
        createAction("view.readOnly", text("Read Only", "只读"), icon(QStyle::SP_MessageBoxInformation), QKeySequence(), true);
        createAction("view.fullscreen", text("Full Screen", "全屏"), icon(QStyle::SP_TitleBarMaxButton), QKeySequence(), true);

        createAction("tools.refresh", text("Refresh", "刷新"), icon(QStyle::SP_BrowserReload));
        createAction("tools.settings", text("Application Settings", "应用设置"), icon(QStyle::SP_FileDialogDetailedView));

        createAction("help.about", text("About", "关于"), icon(QStyle::SP_MessageBoxInformation));
        createAction("language.switch", m_chinese ? QStringLiteral("English") : QStringLiteral("中文"));
    }

    void buildApplicationMenu()
    {
        QApplicationButton *button = m_ribbon->applicationButton();
        button->setText(text("File", "文件"));

        auto *menu = new QRibbonMenu(button);
        for (QAction *a : actions({"file.new", "file.open", "file.save"})) menu->addAction(a);
        menu->addSeparator();
        menu->addAction(action("language.switch"));
        menu->addAction(action("help.about"));
        menu->addSeparator();
        menu->addAction(action("file.close"));
        button->setApplicationMenu(menu);
    }

    void buildQuickAccess()
    {
        for (QAction *a : actions({"file.save", "edit.undo", "edit.redo", "language.switch"})) {
            m_ribbon->addAccessBarAction(a);
        }
    }

    void buildHomeTab()
    {
        QRibbonTab *tab = m_ribbon->addTab(text("Home", "主页"), QStringLiteral("home"));

        auto *document = tab->addGroup(text("Document", "文档"));
        document->addActions(actions({"file.new", "file.open", "file.save"}), QRibbonButtonSize::Large);

        auto *clipboard = tab->addGroup(text("Clipboard", "剪贴板"));
        clipboard->addAction(action("edit.paste"), QRibbonButtonSize::Large);
        clipboard->addActions(actions({"edit.cut", "edit.copy", "edit.selectAll"}), QRibbonButtonSize::Small);

        tab->addGroup(text("Edit", "编辑"),
                      actions({"edit.undo", "edit.redo", "edit.find"}),
                      QRibbonButtonSize::Small);
    }

    void buildInsertTab()
    {
        QRibbonTab *tab = m_ribbon->addTab(text("Insert", "插入"), QStringLiteral("insert"));
        auto *group = tab->addGroup(text("Content", "内容"));

        auto *menu = new QRibbonMenu(group);
        menu->addAction(action("insert.heading"));
        menu->addAction(action("insert.list"));
        menu->addAction(action("insert.table"));
        group->addSplitAction(action("insert.heading"),
                              menu,
                              QRibbonButtonSize::Large,
                              text("Insert\nContent", "插入\n内容"));

        group->addAction(action("insert.link"), QRibbonButtonSize::Large);
        group->addAction(action("insert.date"), QRibbonButtonSize::Large);
    }

    void buildViewTab()
    {
        QRibbonTab *tab = m_ribbon->addTab(text("View", "视图"), QStringLiteral("view"));
        tab->addGroup(text("Zoom", "缩放"),
                      actions({"view.zoomIn", "view.zoomOut", "view.resetZoom"}),
                      QRibbonButtonSize::Small);

        auto *window = tab->addGroup(text("Window", "窗口"));
        window->addActions(actions({"view.fullscreen", "view.readOnly"}), QRibbonButtonSize::Large);
    }

    void buildToolsTab()
    {
        QRibbonTab *tab = m_ribbon->addTab(text("Tools", "工具"), QStringLiteral("tools"));
        auto *group = tab->addGroup(text("Utilities", "实用工具"));
        group->addAction(action("tools.refresh"), QRibbonButtonSize::Large);
        group->addAction(action("tools.settings"),
                         QRibbonButtonSize::Large,
                         text("Application\nSettings", "应用\n设置"));
    }

    void buildHelpTab()
    {
        QRibbonTab *tab = m_ribbon->addTab(text("Help", "帮助"), QStringLiteral("help"));
        auto *group = tab->addGroup(text("Help", "帮助"));
        group->addAction(action("help.about"), QRibbonButtonSize::Large);
        group->addAction(action("language.switch"), QRibbonButtonSize::Small);
    }

    void updateDescription()
    {
        m_editor->setPlainText(m_chinese
            ? QStringLiteral(
                "RibbonLib 示例\n\n"
                "示例直接使用 C++ + QAction 构建 Ribbon。\n"
                "业务命令只维护一份 QAction，Ribbon、文件菜单和快速访问栏复用同一命令状态。\n"
                "Large 按钮不会自动换行；需要两行时，通过 Ribbon displayText 显式指定换行。\n"
                "RibbonLib 自己处理布局、局部样式、状态显示和 DPI。")
            : QStringLiteral(
                "RibbonLib Example\n\n"
                "The example builds the Ribbon directly with C++ + QAction.\n"
                "Application commands live in one QAction and are reused by the Ribbon, File menu and Quick Access bar.\n"
                "Large buttons never wrap automatically; use Ribbon displayText when an explicit two-line label is wanted.\n"
                "RibbonLib owns layout, local styling, state presentation and DPI behavior."));
    }

    void rebuild()
    {
        if (m_ribbon) {
            delete m_ribbon;
            m_ribbon = nullptr;
        }
        qDeleteAll(m_actions);
        m_actions.clear();

        createActions();
        m_ribbon = new QRibbonWidget(m_ribbonHost);
        m_ribbonLayout->addWidget(m_ribbon);

        buildApplicationMenu();
        buildQuickAccess();
        buildHomeTab();
        buildInsertTab();
        buildViewTab();
        buildToolsTab();
        buildHelpTab();
        updateDescription();

        setWindowTitle(m_chinese ? QStringLiteral("RibbonLib 示例") : QStringLiteral("RibbonLib Example"));
    }

    void switchLanguage()
    {
        m_chinese = !m_chinese;
        QTimer::singleShot(0, this, [this]() { rebuild(); });
    }

    void resetZoom()
    {
        if (m_zoomSteps > 0) m_editor->zoomOut(m_zoomSteps);
        else if (m_zoomSteps < 0) m_editor->zoomIn(-m_zoomSteps);
        m_zoomSteps = 0;
    }

    void message(const QString &english, const QString &chinese)
    {
        statusBar()->showMessage(m_chinese ? chinese : english, 2500);
    }

    void handleAction(const QString &id)
    {
        if (id == QStringLiteral("language.switch")) { switchLanguage(); return; }
        if (id == QStringLiteral("file.new")) { m_editor->clear(); return; }
        if (id == QStringLiteral("file.open")) {
            m_editor->setPlainText(m_chinese
                ? QStringLiteral("示例文档\n\n通过同一个 QAction 执行打开命令。")
                : QStringLiteral("Sample document\n\nThe Open command is executed through the same QAction."));
            return;
        }
        if (id == QStringLiteral("file.save")) { message(QStringLiteral("Saved"), QStringLiteral("已保存")); return; }
        if (id == QStringLiteral("file.close")) { close(); return; }
        if (id == QStringLiteral("edit.undo")) { m_editor->undo(); return; }
        if (id == QStringLiteral("edit.redo")) { m_editor->redo(); return; }
        if (id == QStringLiteral("edit.cut")) { m_editor->cut(); return; }
        if (id == QStringLiteral("edit.copy")) { m_editor->copy(); return; }
        if (id == QStringLiteral("edit.paste")) { m_editor->paste(); return; }
        if (id == QStringLiteral("edit.selectAll")) { m_editor->selectAll(); return; }
        if (id == QStringLiteral("edit.find")) { m_editor->find(QStringLiteral("Ribbon")); return; }
        if (id == QStringLiteral("insert.heading")) { m_editor->insertPlainText(QStringLiteral("\n# Ribbon\n")); return; }
        if (id == QStringLiteral("insert.list")) { m_editor->insertPlainText(QStringLiteral("\n- A\n- B\n- C\n")); return; }
        if (id == QStringLiteral("insert.table")) { m_editor->insertPlainText(QStringLiteral("\n| A | B |\n|---|---|\n| 1 | 2 |\n")); return; }
        if (id == QStringLiteral("insert.link")) { m_editor->insertPlainText(QStringLiteral("https://www.qt.io/")); return; }
        if (id == QStringLiteral("insert.date")) { m_editor->insertPlainText(QDate::currentDate().toString(Qt::ISODate)); return; }
        if (id == QStringLiteral("view.zoomIn")) { m_editor->zoomIn(1); ++m_zoomSteps; return; }
        if (id == QStringLiteral("view.zoomOut")) { m_editor->zoomOut(1); --m_zoomSteps; return; }
        if (id == QStringLiteral("view.resetZoom")) { resetZoom(); return; }
        if (id == QStringLiteral("view.readOnly")) { m_editor->setReadOnly(action("view.readOnly")->isChecked()); return; }
        if (id == QStringLiteral("view.fullscreen")) {
            if (action("view.fullscreen")->isChecked()) showFullScreen(); else showNormal();
            return;
        }
        if (id == QStringLiteral("tools.refresh")) { message(QStringLiteral("Refreshed"), QStringLiteral("已刷新")); return; }
        if (id == QStringLiteral("tools.settings")) {
            QMessageBox::information(this,
                                     text("Settings", "设置"),
                                     text("Application settings stay in the host application.",
                                          "应用设置仍由宿主程序维护。"));
            return;
        }
        if (id == QStringLiteral("help.about")) {
            QMessageBox::about(this,
                               text("About RibbonLib", "关于 RibbonLib"),
                               text("A compact QAction-first Qt Widgets Ribbon library.",
                                    "一个以 QAction 为核心的轻量 Qt Widgets Ribbon 控件库。"));
        }
    }

    QTextEdit *m_editor {nullptr};
    QWidget *m_ribbonHost {nullptr};
    QVBoxLayout *m_ribbonLayout {nullptr};
    QRibbonWidget *m_ribbon {nullptr};
    QHash<QString, QAction*> m_actions;
    bool m_chinese {false};
    int m_zoomSteps {0};
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("RibbonLib Example"));

    DemoWindow window;
    window.show();
    return app.exec();
}
