#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>

class QRibbonWidget;
class QRibbonTab;
class QRibbonGroup;
class QRibbonButton;
class QApplicationButton;
class QAction;
class RibbonJsonWindow;

// Main window demonstrating code-based Ribbon creation and runtime language switching
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:
    void setLanguage(bool chinese);
    void toggleLanguage();

private slots:
    void onNewFile();
    void onOpenFile();
    void onSaveFile();
    void onCopy();
    void onPaste();
    void onCut();
    void onOpenJsonWindow();

private:
    void setupRibbon();
    void retranslateUi();

    bool m_isChinese { false };

    QRibbonWidget *m_ribbonWidget { nullptr };
    QApplicationButton *m_appButton { nullptr };
    QRibbonTab *m_homeTab { nullptr };
    QRibbonGroup *m_fileGroup { nullptr };
    QRibbonGroup *m_editGroup { nullptr };

    QRibbonButton *m_newBtn { nullptr };
    QRibbonButton *m_openBtn { nullptr };
    QRibbonButton *m_saveBtn { nullptr };
    QRibbonButton *m_jsonBtn { nullptr };
    QRibbonButton *m_langBtn { nullptr };

    QRibbonButton *m_copyBtn { nullptr };
    QRibbonButton *m_pasteBtn { nullptr };
    QRibbonButton *m_cutBtn { nullptr };

    QAction *m_saveAction { nullptr };
    QAction *m_undoAction { nullptr };
    QAction *m_redoAction { nullptr };
    QAction *m_langAction { nullptr };

    QTextEdit *m_centralWidget { nullptr };
    RibbonJsonWindow *m_jsonWindow { nullptr };
};

#endif // MAINWINDOW_H