#ifndef DEBUGWINDOW_H
#define DEBUGWINDOW_H

#include <QDialog>
#include <QGridLayout>
#include <QLabel>

class GameBoard;

class DebugWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DebugWindow(GameBoard *gameBoard, QWidget *parent = nullptr);
    ~DebugWindow();

public slots:
    void updateDisplay();

private:
    GameBoard *m_gameBoard;
    QGridLayout *m_gridLayout;
    QVector<QVector<QLabel*>> m_labels;
};

#endif // DEBUGWINDOW_H 