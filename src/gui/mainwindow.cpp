#include "mainwindow.h"
#include <QMenu>
#include <QToolBar>
#include <QFontComboBox>
#include "editor/util.h"

//MainWindow

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qRegisterMetaType<Rect>("Rect");

    SetupGui();
    CreateActions();
    CreateStatusBar();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetupGui()
{
    document_widget = new DocumentWidget(ui->centralwidget);
    document_widget->setObjectName(QStringLiteral("document_widget"));
    ui->verticalLayout->addWidget(document_widget);

    document_widget->InsertText("Text", std::make_shared<StringFormat>("Arial", 22, false, false, false));
    document_widget->InsertText("Italic", std::make_shared<StringFormat>("Times New Roman", 18, false, true, false));
    document_widget->InsertText("Bold", std::make_shared<StringFormat>("Times New Roman", 34, true, false, false));
    document_widget->InsertText("String1 String2 String3", std::make_shared<StringFormat>("Arial", 20, false, false, false));
}

void MainWindow::CreateActions()
{
    //file menu and toolbar
    QMenu *file_menu = menuBar()->addMenu(tr("&File"));
    QToolBar *file_toolbar = addToolBar(tr("File"));

    QAction* action = new QAction(QIcon(":/icons/images/new.png"), tr("&New"), this);
    action->setShortcuts(QKeySequence::New);
    action->setStatusTip(tr("Create a new document"));
    connect(action, &QAction::triggered, this, &MainWindow::New);
    file_menu->addAction(action);
    file_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/open.png"), tr("&Open..."), this);
    action->setShortcuts(QKeySequence::Open);
    action->setStatusTip(tr("Open an existing file"));
    connect(action, &QAction::triggered, this, &MainWindow::Open);
    file_menu->addAction(action);
    file_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/save.png"), tr("&Save"), this);
    action->setShortcuts(QKeySequence::Save);
    action->setStatusTip(tr("Save the document to disk"));
    connect(action, &QAction::triggered, this, &MainWindow::Save);
    file_menu->addAction(action);
    file_toolbar->addAction(action);

    action = file_menu->addAction(QIcon(":/images/new.png"), tr("Save &As..."), this, &MainWindow::SaveAs);
    action->setShortcuts(QKeySequence::SaveAs);
    action->setStatusTip(tr("Save the document under a new name"));

    file_menu->addSeparator();

    action = file_menu->addAction(QIcon::fromTheme("application-exit"), tr("E&xit"), this, &MainWindow::Exit);
    action->setShortcuts(QKeySequence::Quit);
    action->setStatusTip(tr("Exit the application"));

    //edit menu and toolbar
    QMenu *edit_menu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *edit_toolbar = addToolBar(tr("Edit"));

    action = new QAction(QIcon(":/icons/images/undo.png"), tr("U&ndo"), this);
    action->setShortcuts(QKeySequence::Undo);
    action->setStatusTip(tr("Undo the last operation"));
    connect(action, &QAction::triggered, this, &MainWindow::Undo);
    edit_menu->addAction(action);
    edit_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/redo.png"), tr("&Redo"), this);
    action->setShortcuts(QKeySequence::Redo);
    action->setStatusTip(tr("Redo the last operation"));
    connect(action, &QAction::triggered, this, &MainWindow::Redo);
    edit_menu->addAction(action);
    edit_toolbar->addAction(action);

    edit_menu->addSeparator();
    edit_toolbar->addSeparator();

    action = new QAction(QIcon(":/icons/images/cut.png"), tr("Cu&t"), this);
    action->setShortcuts(QKeySequence::Cut);
    action->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
    connect(action, &QAction::triggered, this, &MainWindow::Cut);
    edit_menu->addAction(action);
    edit_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/copy.png"), tr("&Copy"), this);
    action->setShortcuts(QKeySequence::Copy);
    action->setStatusTip(tr("Copy the current selection's contents to the clipboard"));
    connect(action, &QAction::triggered, this, &MainWindow::Copy);
    edit_menu->addAction(action);
    edit_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/paste.png"), tr("&Paste"), this);
    action->setShortcuts(QKeySequence::Paste);
    action->setStatusTip(tr("Paste the clipboard's contents into the current selection"));
    connect(action, &QAction::triggered, this, &MainWindow::Paste);
    edit_menu->addAction(action);
    edit_toolbar->addAction(action);

    //fonts toolbar
    QToolBar *fonts_toolbar = addToolBar(tr("Fonts"));
    fonts_toolbar->setStyleSheet("QToolBar{spacing:4px;}");

    QFontComboBox* font_combo = new QFontComboBox;
    connect(font_combo, &QFontComboBox::currentFontChanged, this, &MainWindow::OnCurrentFontChanged);
    fonts_toolbar->addWidget(font_combo);

    size_combo = new QComboBox;
    connect(size_combo, &QComboBox::currentTextChanged, this, &MainWindow::OnCurrentSizeChanged);
    fonts_toolbar->addWidget(size_combo);
    FillSizes(font_combo->currentFont());

    action = new QAction(QIcon(":/icons/images/bold.png"), tr("Bold"), this);
    connect(action, &QAction::triggered, this, &MainWindow::Bold);
    fonts_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/italic.png"), tr("Italic"), this);
    connect(action, &QAction::triggered, this, &MainWindow::Italic);
    fonts_toolbar->addAction(action);

    action = new QAction(QIcon(":/icons/images/underline.png"), tr("Underline"), this);
    connect(action, &QAction::triggered, this, &MainWindow::Underline);
    fonts_toolbar->addAction(action);

    //help menu
    QMenu* help_menu = menuBar()->addMenu(tr("&Help"));
    action = help_menu->addAction(tr("&About"), this, &MainWindow::About);
    help_menu->setStatusTip(tr("Show the application's About box"));
}

void MainWindow::CreateStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::New()
{
}

void MainWindow::Open()
{
}

void MainWindow::Save()
{
}

void MainWindow::SaveAs()
{
}

void MainWindow::Exit()
{
    close();
}

void MainWindow::Cut()
{
}

void MainWindow::Copy()
{
}

void MainWindow::Paste()
{
}

void MainWindow::Undo()
{
    document_widget->document.Undo();
}

void MainWindow::Redo()
{
    document_widget->document.Redo();
}

void MainWindow::About()
{
}

void MainWindow::Bold()
{
}

void MainWindow::Italic()
{
}

void MainWindow::Underline()
{
}

void MainWindow::OnCurrentFontChanged(const QFont& font)
{
    FillSizes(font);
}

void MainWindow::OnCurrentSizeChanged(const QString& size)
{

}

void MainWindow::FillSizes(const QFont& font)
{
    QFontDatabase font_database;
    QString current_size = size_combo->currentText();

    {
        const QSignalBlocker blocker(size_combo);
        // sizeCombo signals are now blocked until end of scope
        size_combo->clear();

        if (font_database.isSmoothlyScalable(font.family(), font_database.styleString(font)))
        {
            const QList<int> sizes = QFontDatabase::standardSizes();
            for (const int size : sizes)
            {
                size_combo->addItem(QVariant(size).toString());
                size_combo->setEditable(true);
            }
        }
        else
        {
            const QList<int> sizes = font_database.smoothSizes(font.family(), font_database.styleString(font));
            for (const int size : sizes)
            {
                size_combo->addItem(QVariant(size).toString());
                size_combo->setEditable(false);
            }
        }
    }

    int i = size_combo->findText(current_size);

    if (i == -1)
        size_combo->setCurrentIndex(qMax(0, size_combo->count() / 3));
    else
        size_combo->setCurrentIndex(i);
}
