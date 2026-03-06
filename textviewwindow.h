#ifndef TEXTVIEWWINDOW_H
#define TEXTVIEWWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QString>

namespace Ui {
class TextViewWindow;
}

/// A window that can be used to view text files.
class TextViewWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TextViewWindow(QWidget* parent = nullptr);
    ~TextViewWindow();

    /// Loads the file specified by path as text file.
    ///
    /// Returns true, if text was successfully loaded.
    /// Returns false otherwise.
    bool loadTextFile(const QString& path);

protected:
    void closeEvent(QCloseEvent* event) override;

    void showEvent(QShowEvent* event) override;

private slots:
    void actionPrintTriggered();
    void actionChangeFontTriggered();

private:
    Ui::TextViewWindow *ui;

    /// Scrolls the text widget back to the top.
    void scrollToTop();

    /// Set the system's recommended fixed-size font for the text widget.
    void setMonospacedFont();
};

#endif // TEXTVIEWWINDOW_H
