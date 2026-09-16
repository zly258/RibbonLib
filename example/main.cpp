#include <initializer_list>

#include <QAction>
#include <QApplication>
#include <QDate>
#include <QMainWindow>
#include <QMessageBox>
#include <QStatusBar>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <RibbonLib.h>

class DemoWindow final : public QMainWindow
{
public:
    DemoWindow()
        : m_helper(new QRibbonHelper(this))
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

        connect(m_helper, &QRibbonHelper::actionTriggered, this, [this](const QString &id) {
            handleAction(id);
        });

        rebuildRibbon();
    }

private:
    bool isChinese() const
    {
        return m_language == QStringLiteral("zh-CN");
    }

    QAction *action(const char *id) const
    {
        return m_helper->action(QString::fromLatin1(id));
    }

    QList<QAction*> actions(std::initializer_list<const char*> ids) const
    {
        QList<QAction*> result;
        result.reserve(static_cast<int>(ids.size()));
        for (const char *id : ids) {
            if (QAction *a = action(id)) {
                result.append(a);
            }
        }
        return result;
    }

    QRibbonSplitButton *addSplitButton(QRibbonGroup *group,
                                       const char *displayActionId,
                                       const char *defaultActionId,
                                       std::initializer_list<const char*> menuActionIds,
                                       QRibbonButtonSize size)
    {
        QAction *displayAction = action(displayActionId);
        QAction *defaultAction = action(defaultActionId);
        if (!group || !displayAction || !defaultAction) {
            return nullptr;
        }

        auto *menu = new QRibbonMenu(group);
        for (QAction *menuAction : actions(menuActionIds)) {
            menu->addAction(menuAction);
        }

        auto *button = new QRibbonSplitButton(displayAction->icon(),
                                              displayAction->text(),
                                              size,
                                              group);
        button->setMenu(menu);
        button->setDefaultAction(defaultAction);
        // Keep placement text/icon independent from the command used by the primary area.
        button->setIcon(displayAction->icon());
        button->setText(displayAction->text());

        if (size == QRibbonButtonSize::Large) {
            group->addLargeWidget(button);
        } else {
            group->addSmallWidget(button);
        }
        return button;
    }

    void buildApplicationMenu()
    {
        auto *button = m_ribbon->applicationButton();
        if (!button) return;

        button->setText(isChinese() ? QStringLiteral("文件") : QStringLiteral("File"));
        auto *menu = new QRibbonMenu(button);
        for (QAction *a : actions({"file.new", "file.open", "file.save"})) menu->addAction(a);
        menu->addSeparator();
        for (QAction *a : actions({"language.switch", "help.about"})) menu->addAction(a);
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
        auto *tab = m_ribbon->addTab(isChinese() ? QStringLiteral("主页") : QStringLiteral("Home"),
                                     QStringLiteral("home"));

        auto *document = tab->addGroup(isChinese() ? QStringLiteral("文档") : QStringLiteral("Document"));
        document->addActions(actions({"file.new", "file.open", "file.save"}), QRibbonButtonSize::Large);

        auto *clipboard = tab->addGroup(isChinese() ? QStringLiteral("剪贴板") : QStringLiteral("Clipboard"));
        clipboard->addAction(action("edit.paste"), QRibbonButtonSize::Large);
        clipboard->addActions(actions({"edit.cut", "edit.copy", "edit.selectAll"}), QRibbonButtonSize::Small);

        tab->addGroup(isChinese() ? QStringLiteral("编辑") : QStringLiteral("Edit"),
                      actions({"edit.undo", "edit.redo", "edit.find"}),
                      QRibbonButtonSize::Small);
    }

    void buildInsertTab()
    {
        auto *tab = m_ribbon->addTab(isChinese() ? QStringLiteral("插入") : QStringLiteral("Insert"),
                                     QStringLiteral("insert"));
        auto *group = tab->addGroup(isChinese() ? QStringLiteral("内容") : QStringLiteral("Content"));
        addSplitButton(group,
                       "insert.content",
                       "insert.heading",
                       {"insert.heading", "insert.list", "insert.table"},
                       QRibbonButtonSize::Large);
        group->addAction(action("insert.link"), QRibbonButtonSize::Large);
        group->addAction(action("insert.date"), QRibbonButtonSize::Large);
    }

    void buildViewTab()
    {
        auto *tab = m_ribbon->addTab(isChinese() ? QStringLiteral("视图") : QStringLiteral("View"),
                                     QStringLiteral("view"));
        tab->addGroup(isChinese() ? QStringLiteral("缩放") : QStringLiteral("Zoom"),
                      actions({"view.zoomIn", "view.zoomOut", "view.resetZoom"}),
                      QRibbonButtonSize::Small);

        auto *window = tab->addGroup(isChinese() ? QStringLiteral("窗口") : QStringLiteral("Window"));
        window->addActions(actions({"view.fullscreen", "view.readOnly"}), QRibbonButtonSize::Large);
    }

    void buildTestsTab()
    {
        auto *tab = m_ribbon->addTab(isChinese() ? QStringLiteral("测试") : QStringLiteral("Tests"),
                                     QStringLiteral("tests"));

        tab->addGroup(isChinese() ? QStringLiteral("小按钮勾选") : QStringLiteral("Small checks"),
                      actions({"test.checkedTop", "test.uncheckedMiddle", "test.checkedBottom",
                               "test.toggleTop", "test.toggleMiddle", "test.toggleBottom"}),
                      QRibbonButtonSize::Small);

        tab->addGroup(isChinese() ? QStringLiteral("大按钮状态") : QStringLiteral("Large states"),
                      actions({"test.largeChecked", "test.largeToggle", "test.disabledChecked", "test.disabled"}),
                      QRibbonButtonSize::Large);

        auto *mixed = tab->addGroup(isChinese() ? QStringLiteral("混合布局") : QStringLiteral("Mixed layout"));
        mixed->addAction(action("test.nativeFolder"), QRibbonButtonSize::Large);
        mixed->addActions(actions({"test.longSmall", "test.nativeReload", "test.nativeSettings"}),
                          QRibbonButtonSize::Small);
        mixed->addAction(action("test.longLarge"), QRibbonButtonSize::Large);

        auto *split = tab->addGroup(isChinese() ? QStringLiteral("拆分按钮") : QStringLiteral("Split buttons"));
        addSplitButton(split,
                       "test.splitLarge",
                       "insert.heading",
                       {"insert.heading", "insert.list", "insert.table"},
                       QRibbonButtonSize::Large);
        addSplitButton(split,
                       "test.splitSmall",
                       "view.zoomIn",
                       {"view.zoomIn", "view.zoomOut", "view.resetZoom"},
                       QRibbonButtonSize::Small);

        tab->addGroup(isChinese() ? QStringLiteral("Qt 原生图标") : QStringLiteral("Qt native icons"),
                      actions({"test.nativeFile", "test.nativeFolder", "test.nativeHome",
                               "test.nativeReload", "test.nativeSettings", "test.nativeTrash"}),
                      QRibbonButtonSize::Small);
    }

    void buildToolsTab()
    {
        auto *tab = m_ribbon->addTab(isChinese() ? QStringLiteral("工具") : QStringLiteral("Tools"),
                                     QStringLiteral("tools"));
        tab->addGroup(isChinese() ? QStringLiteral("实用工具") : QStringLiteral("Utilities"),
                      actions({"tools.refresh", "tools.settings"}),
                      QRibbonButtonSize::Large);
    }

    void buildHelpTab()
    {
        auto *tab = m_ribbon->addTab(isChinese() ? QStringLiteral("帮助") : QStringLiteral("Help"),
                                     QStringLiteral("help"));
        auto *group = tab->addGroup(isChinese() ? QStringLiteral("帮助") : QStringLiteral("Help"));
        group->addAction(action("help.about"), QRibbonButtonSize::Large);
        // Text-only language switching is intentionally Small; this avoids producing a tall,
        // narrow Large command just because it has no icon.
        group->addAction(action("language.switch"), QRibbonButtonSize::Small);
    }

    void buildRibbonFromActions()
    {
        buildApplicationMenu();
        buildQuickAccess();
        buildHomeTab();
        buildInsertTab();
        buildViewTab();
        buildTestsTab();
        buildToolsTab();
        buildHelpTab();
    }

    void updateExampleText()
    {
        m_editor->setPlainText(isChinese()
            ? QStringLiteral(
                "RibbonLib 示例\n\n"
                "本示例使用 C++ + QAction 构建 Ribbon 布局。\n"
                "Action 的文字、图标和快捷键仍从 JSON 加载，仅用于保持双语示例简洁；JSON 不再负责 Ribbon 的 Tab/Group 布局。\n"
                "主页、插入、视图展示常规用法；测试页覆盖三行 Small、可勾选状态、禁用状态、Large/Small 混排、Split Button、长文字和 Qt 原生图标。\n\n"
                "推荐业务项目维护 QAction，RibbonLib 只负责展示、布局和状态同步。")
            : QStringLiteral(
                "RibbonLib Example\n\n"
                "This example builds Ribbon layout with C++ + QAction.\n"
                "Action text, icons and shortcuts are still loaded from JSON only to keep the bilingual example compact; JSON no longer owns Tab/Group layout.\n"
                "Home, Insert and View show normal usage. Tests covers three-row Small buttons, checkable/disabled states, mixed Large/Small layout, Split Buttons, long labels and Qt native icons.\n\n"
                "Recommended application code owns QActions; RibbonLib owns presentation, layout and state synchronization."));
    }

    void rebuildRibbon()
    {
        if (m_ribbon) {
            delete m_ribbon;
            m_ribbon = nullptr;
        }

        const QString suffix = isChinese() ? QStringLiteral("zh-CN") : QStringLiteral("en");
        const QString actionsPath = QStringLiteral(":/RibbonExample/actions_%1.json").arg(suffix);
        const QString ribbonPath = QStringLiteral(":/RibbonExample/ribbon_%1.json").arg(suffix);

        // QRibbonHelper currently loads action metadata and layout metadata together.
        // The example intentionally ignores JSON layout and uses the public QAction-first API.
        if (!m_helper->loadFromResources(ribbonPath, actionsPath)) {
            statusBar()->showMessage(m_helper->errorString());
            return;
        }

        m_ribbon = new QRibbonWidget(m_ribbonHost);
        m_ribbonLayout->addWidget(m_ribbon);
        buildRibbonFromActions();
        updateExampleText();

        setWindowTitle(isChinese()
            ? QStringLiteral("RibbonLib 示例")
            : QStringLiteral("RibbonLib Example"));
        statusBar()->showMessage(isChinese()
            ? QStringLiteral("Ribbon 已使用 C++ + QAction API 构建")
            : QStringLiteral("Ribbon built with the C++ + QAction API"),
            2500);
    }

    void scheduleLanguageSwitch()
    {
        m_language = isChinese() ? QStringLiteral("en") : QStringLiteral("zh-CN");
        QTimer::singleShot(0, this, [this]() { rebuildRibbon(); });
    }

    void resetZoom()
    {
        if (m_zoomSteps > 0) {
            m_editor->zoomOut(m_zoomSteps);
        } else if (m_zoomSteps < 0) {
            m_editor->zoomIn(-m_zoomSteps);
        }
        m_zoomSteps = 0;
    }

    void showSimpleStatus(const QString &english, const QString &chinese)
    {
        statusBar()->showMessage(isChinese() ? chinese : english, 2500);
    }

    void handleAction(const QString &id)
    {
        if (id == QStringLiteral("language.switch")) { scheduleLanguageSwitch(); return; }
        if (id == QStringLiteral("file.new")) {
            m_editor->clear();
            showSimpleStatus(QStringLiteral("New document"), QStringLiteral("已新建文档"));
            return;
        }
        if (id == QStringLiteral("file.open")) {
            m_editor->setPlainText(isChinese()
                ? QStringLiteral("示例文档\n\n这是通过 QAction 驱动的 Ribbon“打开”命令载入的示例内容。")
                : QStringLiteral("Sample document\n\nThis content was loaded by the QAction-driven Ribbon Open command."));
            showSimpleStatus(QStringLiteral("Sample document opened"), QStringLiteral("已打开示例文档"));
            return;
        }
        if (id == QStringLiteral("file.save")) {
            showSimpleStatus(QStringLiteral("Document saved"), QStringLiteral("文档已保存"));
            return;
        }
        if (id == QStringLiteral("file.close")) { QTimer::singleShot(0, this, [this]() { close(); }); return; }
        if (id == QStringLiteral("edit.undo")) { m_editor->undo(); return; }
        if (id == QStringLiteral("edit.redo")) { m_editor->redo(); return; }
        if (id == QStringLiteral("edit.cut")) { m_editor->cut(); return; }
        if (id == QStringLiteral("edit.copy")) { m_editor->copy(); return; }
        if (id == QStringLiteral("edit.paste")) { m_editor->paste(); return; }
        if (id == QStringLiteral("edit.selectAll")) { m_editor->selectAll(); return; }
        if (id == QStringLiteral("edit.find")) {
            const bool found = m_editor->find(QStringLiteral("Ribbon"));
            showSimpleStatus(found ? QStringLiteral("Found Ribbon") : QStringLiteral("Ribbon not found"),
                             found ? QStringLiteral("已找到 Ribbon") : QStringLiteral("未找到 Ribbon"));
            return;
        }
        if (id == QStringLiteral("insert.heading")) { m_editor->insertPlainText(QStringLiteral("\n# Ribbon heading\n")); return; }
        if (id == QStringLiteral("insert.list")) { m_editor->insertPlainText(QStringLiteral("\n- Item 1\n- Item 2\n- Item 3\n")); return; }
        if (id == QStringLiteral("insert.table")) { m_editor->insertPlainText(QStringLiteral("\n| A | B |\n|---|---|\n| 1 | 2 |\n")); return; }
        if (id == QStringLiteral("insert.link")) { m_editor->insertPlainText(QStringLiteral("https://www.qt.io/")); return; }
        if (id == QStringLiteral("insert.date")) { m_editor->insertPlainText(QDate::currentDate().toString(Qt::ISODate)); return; }
        if (id == QStringLiteral("view.zoomIn")) { m_editor->zoomIn(1); ++m_zoomSteps; return; }
        if (id == QStringLiteral("view.zoomOut")) { m_editor->zoomOut(1); --m_zoomSteps; return; }
        if (id == QStringLiteral("view.resetZoom")) { resetZoom(); return; }
        if (id == QStringLiteral("view.readOnly")) {
            QAction *a = action("view.readOnly");
            m_editor->setReadOnly(a && a->isChecked());
            showSimpleStatus(m_editor->isReadOnly() ? QStringLiteral("Read-only mode enabled") : QStringLiteral("Read-only mode disabled"),
                             m_editor->isReadOnly() ? QStringLiteral("已启用只读模式") : QStringLiteral("已关闭只读模式"));
            return;
        }
        if (id == QStringLiteral("view.fullscreen")) {
            QAction *a = action("view.fullscreen");
            if (a && a->isChecked()) showFullScreen(); else showNormal();
            return;
        }
        if (id.startsWith(QStringLiteral("test."))) {
            QAction *a = m_helper->action(id);
            if (a && a->isCheckable()) {
                const bool checked = a->isChecked();
                showSimpleStatus(QStringLiteral("%1: %2").arg(a->text(), checked ? QStringLiteral("checked") : QStringLiteral("unchecked")),
                                 QStringLiteral("%1：%2").arg(a->text(), checked ? QStringLiteral("已勾选") : QStringLiteral("未勾选")));
            } else {
                showSimpleStatus(QStringLiteral("Test action: %1").arg(id), QStringLiteral("测试动作：%1").arg(id));
            }
            return;
        }
        if (id == QStringLiteral("tools.refresh")) {
            showSimpleStatus(QStringLiteral("Example state refreshed"), QStringLiteral("示例状态已刷新"));
            return;
        }
        if (id == QStringLiteral("tools.settings")) {
            QMessageBox::information(this,
                                     isChinese() ? QStringLiteral("设置") : QStringLiteral("Settings"),
                                     isChinese()
                                         ? QStringLiteral("RibbonLib 只负责 Ribbon；应用设置继续由宿主维护。")
                                         : QStringLiteral("RibbonLib owns Ribbon presentation; application settings remain in the host."));
            return;
        }
        if (id == QStringLiteral("help.about")) {
            QMessageBox::about(this,
                               isChinese() ? QStringLiteral("关于 RibbonLib") : QStringLiteral("About RibbonLib"),
                               isChinese()
                                   ? QStringLiteral("一个 QAction-first、轻量、可嵌入的 Qt Widgets Ribbon 控件库。")
                                   : QStringLiteral("A lightweight, QAction-first, embeddable Qt Widgets Ribbon library."));
        }
    }

    QTextEdit *m_editor { nullptr };
    QWidget *m_ribbonHost { nullptr };
    QVBoxLayout *m_ribbonLayout { nullptr };
    QRibbonWidget *m_ribbon { nullptr };
    QRibbonHelper *m_helper { nullptr };
    QString m_language { QStringLiteral("en") };
    int m_zoomSteps { 0 };
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("RibbonLib Example"));

    DemoWindow window;
    window.show();
    return app.exec();
}
