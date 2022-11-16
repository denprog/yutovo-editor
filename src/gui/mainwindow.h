#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include "ui_mainwindow.h"
#include <cstring>
#include "document_widget.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void SetupGui();
    void CreateActions();
    void CreateStatusBar();

    void New();
    void Open();
    void Save();
    void SaveAs();
    void Exit();

    void Cut();
    void Copy();
    void Paste();

    void Undo();
    void Redo();

    void About();

    void Bold();
    void Italic();
    void Underline();

private slots:
    void OnCurrentFontChanged(const QFont& font);
    void OnCurrentSizeChanged(const QString& size);

private:
    void FillSizes(const QFont& font);

private:
    Ui::MainWindow *ui;
    DocumentWidget* document_widget;
    QComboBox* size_combo = nullptr;
};

#endif
