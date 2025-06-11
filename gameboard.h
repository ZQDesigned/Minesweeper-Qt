#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QWidget>
#include <QGridLayout>
#include <QVector>
#include <QTimer>
#include <QElapsedTimer>
#include <QKeyEvent>
#include "cell.h"

class DebugWindow;

class GameBoard : public QWidget
{
    Q_OBJECT

public:
    explicit GameBoard(QWidget *parent = nullptr);
    ~GameBoard();
    
    // 初始化游戏板
    void initializeBoard(int rows, int cols, int mineCount);
    
    // 重置游戏
    void resetGame();
    
    // 获取游戏状态
    bool isGameOver() const { return m_gameOver; }
    bool isGameWon() const { return m_gameWon; }
    int remainingMines() const { return m_mineCount - m_flaggedCount; }
    int elapsedSeconds() const { return m_elapsedTime.elapsed() / 1000; }
    
    // 获取地雷位置信息
    int getRows() const { return m_rows; }
    int getCols() const { return m_cols; }
    bool isMineAt(int row, int col) const;
    
signals:
    void gameOver(bool won);
    void updateMineCounter(int count);
    void updateTimer(int seconds);
    
protected:
    // 添加键盘事件处理
    void keyPressEvent(QKeyEvent *event) override;
    
private slots:
    void onCellClicked();
    void onCellRightClicked();
    void updateTimerDisplay();
    void toggleDebugWindow();
    
private:
    // 游戏参数
    int m_rows = 0;
    int m_cols = 0;
    int m_mineCount = 0;
    int m_flaggedCount = 0;
    bool m_firstClick = true;
    bool m_gameOver = false;
    bool m_gameWon = false;
    
    // 布局和单元格
    QGridLayout *m_gridLayout = nullptr;
    QVector<QVector<Cell*>> m_cells;
    
    // 计时器
    QTimer *m_timer = nullptr;
    QElapsedTimer m_elapsedTime;
    qint64 m_timeOffset = 0;
    
    // Debug相关
    QVector<QDateTime> m_deleteKeyPresses;
    DebugWindow *m_debugWindow = nullptr;
    
    // 游戏逻辑方法
    void placeMines(int firstRow, int firstCol);
    void calculateAdjacentMines();
    void revealCell(int row, int col);
    void revealAdjacentCells(int row, int col);
    void checkGameWon();
    bool isValidCell(int row, int col) const;
};

#endif // GAMEBOARD_H