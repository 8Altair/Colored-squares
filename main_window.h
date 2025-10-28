#ifndef COLORED_SQUARES_MAIN_WINDOW_H
#define COLORED_SQUARES_MAIN_WINDOW_H

#include <QMainWindow>  // Qt base class for main application windows


QT_BEGIN_NAMESPACE  // Begin Qt namespace block (matches ui header style)

namespace Ui { class MainWindow; }   // Forward declaration of auto-generated Ui class

QT_END_NAMESPACE    // End Qt namespace block

class MainWindow final : public QMainWindow // Main window class that publicly inherits QMainWindow class
{
    Q_OBJECT    // Enables Qt meta-object features (signals/slots, RTTI)

    Ui::MainWindow *ui; // Pointer to auto-generated UI helper (owns child widgets)

public:
    explicit MainWindow(QWidget *parent = nullptr); // Constructor; "explicit" avoids implicit conversions
    ~MainWindow() override; // Virtual destructor (overrides QObject destructor)
};


#endif //COLORED_SQUARES_MAIN_WINDOW_H