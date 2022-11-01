#include "mainwindow.h"
#include "editor/util.h"

//MainWindow

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qRegisterMetaType<Rect>("Rect");

    SetupGui();
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
