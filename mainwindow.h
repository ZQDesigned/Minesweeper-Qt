#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include "gameboard.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startNewGame();
    void onGameOver(bool won);
    void updateMineCounter(int count);
    void updateTimer(int seconds);

private:
    // 游戏组件
    GameBoard *m_gameBoard;
    
    // 界面元素
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_controlLayout;
    QLabel *m_mineCounterLabel;
    QLabel *m_timerLabel;
    QPushButton *m_newGameButton;
    QComboBox *m_difficultyComboBox;
    QLineEdit *m_rowsInput;
    QLineEdit *m_colsInput;
    QLineEdit *m_minesInput;
    QPushButton *m_customGameButton;
    
    // 游戏难度设置
    struct Difficulty {
        int rows;
        int cols;
        int mines;
        QString name;
    };
    QVector<Difficulty> m_difficulties;
    
    void setupUI();
    void initializeDifficulties();
};
#endif // MAINWINDOW_H
