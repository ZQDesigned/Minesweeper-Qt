#include "cell.h"
#include <QStyleOption>
#include <QPainter>

Cell::Cell(QWidget *parent) : QPushButton(parent)
{
    // setFixedSize(30, 30); // 移除固定大小，使其可自适应
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 设置大小策略为扩展
    setCheckable(true);   // 使按钮可以被选中
    reset();
    
    // 连接鼠标右键点击事件
    setContextMenuPolicy(Qt::CustomContextMenu);
}

QSize Cell::sizeHint() const
{
    // 获取父窗口的大小策略
    int size = 30; // 默认大小
    if (parentWidget()) {
        // 尝试获取可用空间并计算合适的大小
        size = qMin(parentWidget()->width(), parentWidget()->height()) / 10;
        size = qMax(size, 20); // 确保最小尺寸
    }
    return QSize(size, size); // 保持1:1的比例
}

QSize Cell::minimumSizeHint() const
{
    return QSize(20, 20); // 最小尺寸也保持1:1的比例
}

void Cell::reset()
{
    m_isMine = false;
    m_isRevealed = false;
    m_isFlagged = false;
    m_adjacentMines = 0;
    updateAppearance();
}

void Cell::updateAppearance()
{
    QString styleSheet = "QPushButton { border: 1px solid #BBBBBB; font-weight: bold; font-size: 14px; }";
    if (m_isFlagged) {
        // 显示旗帜
        setText("🚩");
        styleSheet += "QPushButton { background-color: #E0E0E0; color: #D32F2F; }"; // 旗帜颜色
    } else if (!m_isRevealed) {
        // 未揭开的单元格
        setText("");
        styleSheet += "QPushButton { background-color: #F0F0F0; }";
        styleSheet += "QPushButton:hover { background-color: #E0E0E0; }"; // 悬停效果
    } else if (m_isMine) {
        // 揭开的地雷
        setText("💣");
        styleSheet += "QPushButton { background-color: #FFCDD2; color: #B71C1C; }"; // 地雷背景和图标颜色
    } else {
        // 揭开的数字
        styleSheet += "QPushButton { background-color: #FFFFFF; }";
        if (m_adjacentMines > 0) {
            setText(QString::number(m_adjacentMines));
            QString colorStyle;
            switch (m_adjacentMines) {
                case 1: colorStyle = "color: #1976D2;"; break; // Blue
                case 2: colorStyle = "color: #388E3C;"; break; // Green
                case 3: colorStyle = "color: #D32F2F;"; break; // Red
                case 4: colorStyle = "color: #7B1FA2;"; break; // Purple
                case 5: colorStyle = "color: #FF8F00;"; break; // Orange
                case 6: colorStyle = "color: #0097A7;"; break; // Cyan
                case 7: colorStyle = "color: #424242;"; break; // Dark Gray
                case 8: colorStyle = "color: #9E9E9E;"; break; // Gray
            }
            styleSheet += "QPushButton { " + colorStyle + " }";
        } else {
            setText(""); // 空白单元格
            styleSheet += "QPushButton { background-color: #E8E8E8; border: 1px solid #DDDDDD; }"; // 稍暗的背景和不同的边框表示已揭开的空白
        }
    }
    setStyleSheet(styleSheet);
}