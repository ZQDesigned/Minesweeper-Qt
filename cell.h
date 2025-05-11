#ifndef CELL_H
#define CELL_H

#include <QPushButton>

class Cell : public QPushButton
{
    Q_OBJECT

public:
    explicit Cell(QWidget *parent = nullptr);
    
    // 单元格状态
    bool isMine() const { return m_isMine; }
    bool isRevealed() const { return m_isRevealed; }
    bool isFlagged() const { return m_isFlagged; }
    int adjacentMines() const { return m_adjacentMines; }
    
    // 设置单元格状态
    void setMine(bool isMine) { m_isMine = isMine; }
    void setRevealed(bool isRevealed) { m_isRevealed = isRevealed; }
    void setFlagged(bool isFlagged) { m_isFlagged = isFlagged; }
    void setAdjacentMines(int count) { m_adjacentMines = count; }
    
    // 重置单元格
    void reset();
    
    // 更新单元格显示
    void updateAppearance();
    
    // 确保单元格保持正方形比例
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    
private:
    bool m_isMine = false;        // 是否是地雷
    bool m_isRevealed = false;    // 是否已揭开
    bool m_isFlagged = false;     // 是否已标记
    int m_adjacentMines = 0;      // 相邻地雷数量
};

#endif // CELL_H