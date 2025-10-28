#include "main_window.h"    // Header for this class (declaration of MainWindow)
#include "ui_main_window.h" // Auto-generated header from MainWindow.ui (defines Ui::MainWindow)
#include "view.h"   // OpenGL widget class (QOpenGLWidget subclass)


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)   // Initialize base QMainWindow and allocate UI helper
{
    ui->setupUi(this);  // Build widgets defined in MainWindow.ui and attach to this window

    auto *scene = new View(this);   // Create the GL scene widget; parent = this so Qt owns and deletes it
    ui->verticalLayout->addWidget(scene);   // Insert the scene widget into the layout from the .ui
}

MainWindow::~MainWindow()
{
    delete ui;  // Destroy the auto-generated UI wrapper (child widgets get deleted by Qt)
}