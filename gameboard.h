#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QWidget>
#include <QGridLayout>
#include <QVector>
#include <QTimer>
#include <QElapsedTimer>
#include "cell.h"

class GameBoard : public QWidget
{
    Q_OBJECT

public:
    explicit GameBoard(QWidget *parent = nullptr);
    
    // 初始化游戏板
    void initializeBoard(int rows, int cols, int mineCount);
    
    // 重置游戏
    void resetGame();
    
    // 获取游戏状态
    bool isGameOver() const { return m_gameOver; }
    bool isGameWon() const { return m_gameWon; }
    int remainingMines() const { return m_mineCount - m_flaggedCount; }
    int elapsedSeconds() const { return (m_elapsedTime.elapsed() + m_timeOffset) / 1000; }
    
    // 保存和加载游戏
    bool saveGame(const QString &filename);
    bool loadGame(const QString &filename);
    
signals:
    void gameOver(bool won);
    void updateMineCounter(int count);
    void updateTimer(int seconds);
    
private slots:
    void onCellClicked();
    void onCellRightClicked();
    void updateTimerDisplay();
    
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
    
    // 游戏逻辑方法
    void placeMines(int firstRow, int firstCol);
    void calculateAdjacentMines();
    void revealCell(int row, int col);
    void revealAdjacentCells(int row, int col);
    void checkGameWon();
    bool isValidCell(int row, int col) const;
};

#endif // GAMEBOARD_H