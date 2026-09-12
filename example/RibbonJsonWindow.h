#ifndef RIBBONJSONWINDOW_H
#define RIBBONJSONWINDOW_H

#include <QMainWindow>
#include <QTextEdit>

class QRibbonWidget;
class QRibbonHelper;
class QAction;

/**
 * @brief Window demonstrating Ribbon UI loaded dynamically from JSON configuration files.
 *
 * Uses QRibbonHelper to load Ribbon structure from ribbon.json and action definitions
 * from actions.json. Supports runtime toggling between English (default) and Chinese.
 */
class RibbonJsonWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RibbonJsonWindow(QWidget *parent = nullptr);
    ~RibbonJsonWindow() override;

public slots:
    void setLanguage(bool chinese);
    void toggleLanguage();

private slots:
    // File operation slots
    void onNewFile();
    void onOpenFile();
    void onSaveFile();
    void onExit();
    
    // Edit operation slots
    void onCopy();
    void onPaste();
    void onCut();
    void onUndo();
    void onRedo();
    void onDelete();
    void onSelectAll();
    
    // Additional slots
    void onAbout();
    void onSettings();

private:
    void setupRibbon();
    void connectActions();
    void setupCentralWidget();
    void retranslateUi();

private:
    bool m_isChinese { false };
    QRibbonWidget *m_ribbonWidget { nullptr };
    QRibbonHelper *m_ribbonHelper { nullptr };
    QTextEdit *m_centralWidget { nullptr };
    QAction *m_langAction { nullptr };
};

#endif // RIBBONJSONWINDOW_H
