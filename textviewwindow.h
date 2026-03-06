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

    bool loadTextFile(const QString& path);

protected:
    void closeEvent(QCloseEvent* event) override;

    void showEvent(QShowEvent* event) override;

private:
    Ui::TextViewWindow *ui;

    void scrollToTop();
};

#endif // TEXTVIEWWINDOW_H
