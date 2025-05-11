#include "gameboard.h"
#include <QRandomGenerator>
#include <QMessageBox>
#include <QFile>
#include <QDataStream>
#include <QFileDialog>

GameBoard::GameBoard(QWidget *parent) : QWidget(parent)
{
    // 初始化布局
    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setSpacing(1);
    setLayout(m_gridLayout);
    
    // 初始化计时器
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &GameBoard::updateTimerDisplay);
    m_timeOffset = 0;
}

void GameBoard::initializeBoard(int rows, int cols, int mineCount)
{
    // 清除旧的游戏板
    resetGame();
    
    // 设置新的游戏参数
    m_rows = rows;
    m_cols = cols;
    m_mineCount = mineCount;
    m_flaggedCount = 0;
    m_firstClick = true;
    m_gameOver = false;
    m_gameWon = false;
    
    // 创建单元格
    m_cells.resize(rows);
    for (int row = 0; row < rows; ++row) {
        m_cells[row].resize(cols);
        for (int col = 0; col < cols; ++col) {
            Cell *cell = new Cell(this);
            m_cells[row][col] = cell;
            
            // 连接信号和槽
            connect(cell, &QPushButton::clicked, this, &GameBoard::onCellClicked);
            connect(cell, &QPushButton::customContextMenuRequested, this, &GameBoard::onCellRightClicked);
            
            // 添加到布局
            m_gridLayout->addWidget(cell, row, col);
        }
    }
    
    // 计算合适的窗口大小
    int cellSize = 30; // 默认单元格大小
    int minWidth = cols * cellSize;
    int minHeight = rows * cellSize;
    setMinimumSize(minWidth, minHeight);
    
    // 发出信号更新地雷计数器
    emit updateMineCounter(m_mineCount);
}

void GameBoard::resetGame()
{
    // 停止计时器
    m_timer->stop();
    
    // 清除旧的单元格
    for (auto &row : m_cells) {
        for (auto cell : row) {
            if (cell) {
                m_gridLayout->removeWidget(cell);
                delete cell;
            }
        }
    }
    m_cells.clear();
    
    // 重置游戏状态
    m_firstClick = true;
    m_gameOver = false;
    m_gameWon = false;
    m_flaggedCount = 0;
    
    // 发出信号更新计数器
    emit updateMineCounter(m_mineCount);
    emit updateTimer(0);
}

void GameBoard::placeMines(int firstRow, int firstCol)
{
    // 确保第一次点击的位置及其周围没有地雷
    QVector<QPair<int, int>> safeCells;
    for (int r = firstRow - 1; r <= firstRow + 1; ++r) {
        for (int c = firstCol - 1; c <= firstCol + 1; ++c) {
            if (isValidCell(r, c)) {
                safeCells.append(qMakePair(r, c));
            }
        }
    }
    
    // 随机放置地雷
    int minesPlaced = 0;
    while (minesPlaced < m_mineCount) {
        int row = QRandomGenerator::global()->bounded(m_rows);
        int col = QRandomGenerator::global()->bounded(m_cols);
        
        // 检查是否是安全区域
        bool isSafeCell = false;
        for (const auto &safeCell : safeCells) {
            if (safeCell.first == row && safeCell.second == col) {
                isSafeCell = true;
                break;
            }
        }
        
        // 如果不是安全区域且没有地雷，则放置地雷
        if (!isSafeCell && !m_cells[row][col]->isMine()) {
            m_cells[row][col]->setMine(true);
            minesPlaced++;
        }
    }
    
    // 计算每个单元格周围的地雷数量
    calculateAdjacentMines();
}

void GameBoard::calculateAdjacentMines()
{
    for (int row = 0; row < m_rows; ++row) {
        for (int col = 0; col < m_cols; ++col) {
            if (!m_cells[row][col]->isMine()) {
                int count = 0;
                
                // 检查周围8个方向的单元格
                for (int r = row - 1; r <= row + 1; ++r) {
                    for (int c = col - 1; c <= col + 1; ++c) {
                        if (isValidCell(r, c) && m_cells[r][c]->isMine()) {
                            count++;
                        }
                    }
                }
                
                m_cells[row][col]->setAdjacentMines(count);
            }
        }
    }
}

void GameBoard::onCellClicked()
{
    // 获取被点击的单元格
    Cell *cell = qobject_cast<Cell*>(sender());
    if (!cell || m_gameOver) {
        return;
    }
    
    // 找到单元格的位置
    int row = -1, col = -1;
    for (int r = 0; r < m_rows; ++r) {
        for (int c = 0; c < m_cols; ++c) {
            if (m_cells[r][c] == cell) {
                row = r;
                col = c;
                break;
            }
        }
        if (row != -1) break;
    }
    
    // 如果单元格已标记或已揭示，则不做任何操作
    if (cell->isFlagged() || cell->isRevealed()) {
        return;
    }
    
    // 如果是第一次点击，放置地雷并开始计时
    if (m_firstClick) {
        placeMines(row, col);
        m_firstClick = false;
        m_elapsedTime.start();
        m_timer->start(1000); // 每秒更新一次
    }
    
    // 揭示单元格
    revealCell(row, col);
}

void GameBoard::onCellRightClicked()
{
    // 获取被右键点击的单元格
    Cell *cell = qobject_cast<Cell*>(sender());
    if (!cell || m_gameOver) {
        return;
    }
    
    // 如果单元格已揭示，则不做任何操作
    if (cell->isRevealed()) {
        return;
    }
    
    // 切换标记状态
    bool flagged = cell->isFlagged();
    cell->setFlagged(!flagged);
    cell->updateAppearance();
    
    // 更新标记计数
    m_flaggedCount += flagged ? -1 : 1;
    emit updateMineCounter(m_mineCount - m_flaggedCount);
}

void GameBoard::revealCell(int row, int col)
{
    // 检查单元格是否有效
    if (!isValidCell(row, col)) {
        return;
    }
    
    Cell *cell = m_cells[row][col];
    
    // 如果单元格已揭示或已标记，则不做任何操作
    if (cell->isRevealed() || cell->isFlagged()) {
        return;
    }
    
    // 揭示单元格
    cell->setRevealed(true);
    cell->updateAppearance();
    
    // 如果是地雷，游戏结束
    if (cell->isMine()) {
        // 显示所有地雷
        for (int r = 0; r < m_rows; ++r) {
            for (int c = 0; c < m_cols; ++c) {
                if (m_cells[r][c]->isMine()) {
                    m_cells[r][c]->setRevealed(true);
                    m_cells[r][c]->updateAppearance();
                }
            }
        }
        
        m_gameOver = true;
        m_timer->stop();
        emit gameOver(false);
    }
    // 如果是空白单元格（周围没有地雷），自动揭示周围的单元格
    else if (cell->adjacentMines() == 0) {
        revealAdjacentCells(row, col);
    }
    
    // 检查是否赢得游戏
    checkGameWon();
}

void GameBoard::revealAdjacentCells(int row, int col)
{
    // 揭示周围8个方向的单元格
    for (int r = row - 1; r <= row + 1; ++r) {
        for (int c = col - 1; c <= col + 1; ++c) {
            if (r != row || c != col) { // 跳过当前单元格
                revealCell(r, c);
            }
        }
    }
}

void GameBoard::checkGameWon()
{
    // 检查是否所有非地雷单元格都已揭示
    for (int row = 0; row < m_rows; ++row) {
        for (int col = 0; col < m_cols; ++col) {
            Cell *cell = m_cells[row][col];
            if (!cell->isMine() && !cell->isRevealed()) {
                return; // 还有非地雷单元格未揭示，游戏未结束
            }
        }
    }
    
    // 所有非地雷单元格都已揭示，游戏胜利
    m_gameOver = true;
    m_gameWon = true;
    m_timer->stop();
    
    // 标记所有地雷
    for (int row = 0; row < m_rows; ++row) {
        for (int col = 0; col < m_cols; ++col) {
            Cell *cell = m_cells[row][col];
            if (cell->isMine() && !cell->isFlagged()) {
                cell->setFlagged(true);
                cell->updateAppearance();
                m_flaggedCount++;
            }
        }
    }
    
    emit updateMineCounter(0);
    emit gameOver(true);
}

void GameBoard::updateTimerDisplay()
{
    emit updateTimer((m_elapsedTime.elapsed() + m_timeOffset) / 1000);
}

bool GameBoard::isValidCell(int row, int col) const
{
    return row >= 0 && row < m_rows && col >= 0 && col < m_cols;
}

bool GameBoard::saveGame(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    QDataStream out(&file);
    
    // 保存游戏参数
    out << m_rows << m_cols << m_mineCount << m_flaggedCount;
    out << m_firstClick << m_gameOver << m_gameWon;
    out << (m_elapsedTime.elapsed() + m_timeOffset);
    
    // 保存单元格状态
    for (int row = 0; row < m_rows; ++row) {
        for (int col = 0; col < m_cols; ++col) {
            Cell *cell = m_cells[row][col];
            out << cell->isMine() << cell->isRevealed() << cell->isFlagged() << cell->adjacentMines();
        }
    }
    
    file.close();
    return true;
}

bool GameBoard::loadGame(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QDataStream in(&file);
    
    // 读取游戏参数
    int rows, cols, mineCount, flaggedCount;
    bool firstClick, gameOver, gameWon;
    qint64 elapsedTime;
    
    in >> rows >> cols >> mineCount >> flaggedCount;
    in >> firstClick >> gameOver >> gameWon;
    in >> elapsedTime;
    
    // 初始化游戏板
    initializeBoard(rows, cols, mineCount);
    
    m_flaggedCount = flaggedCount;
    m_firstClick = firstClick;
    m_gameOver = gameOver;
    m_gameWon = gameWon;
    
    // 读取单元格状态
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            bool isMine, isRevealed, isFlagged;
            int adjacentMines;
            
            in >> isMine >> isRevealed >> isFlagged >> adjacentMines;
            
            Cell *cell = m_cells[row][col];
            cell->setMine(isMine);
            cell->setRevealed(isRevealed);
            cell->setFlagged(isFlagged);
            cell->setAdjacentMines(adjacentMines);
            cell->updateAppearance();
        }
    }
    
    // 设置计时器
    if (!m_firstClick && !m_gameOver) {
        m_timeOffset = elapsedTime;
        m_elapsedTime.start();
        m_timer->start(1000);
    }
    
    // 更新显示
    emit updateMineCounter(m_mineCount - m_flaggedCount);
    emit updateTimer(elapsedTime / 1000);
    
    file.close();
    return true;
}
