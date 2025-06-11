#include "debugwindow.h"
#include "gameboard.h"
#include <QVBoxLayout>

DebugWindow::DebugWindow(GameBoard *gameBoard, QWidget *parent)
    : QDialog(parent), m_gameBoard(gameBoard)
{
    setWindowTitle("调试模式 - 地雷位置");
    setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(5);
    
    // 添加标题标签
    QLabel *titleLabel = new QLabel("红色 = 地雷");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // 创建网格布局用于显示地雷位置
    m_gridLayout = new QGridLayout();
    m_gridLayout->setSpacing(1); // 减小单元格之间的间距
    m_gridLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addLayout(m_gridLayout);
    
    // 初始化标签网格
    updateDisplay();
    
    // 调整窗口大小
    adjustSize();
    setMinimumSize(200, 200);
}

DebugWindow::~DebugWindow()
{
}

void DebugWindow::updateDisplay()
{
    int rows = m_gameBoard->getRows();
    int cols = m_gameBoard->getCols();
    
    // 如果行列数变化，重新创建标签网格
    if (m_labels.size() != rows || (rows > 0 && m_labels[0].size() != cols)) {
        // 清除现有标签
        for (auto &row : m_labels) {
            for (auto label : row) {
                delete label;
            }
        }
        m_labels.clear();
        
        // 清除布局中的所有项
        while (m_gridLayout->count() > 0) {
            QLayoutItem *item = m_gridLayout->takeAt(0);
            if (item->widget()) {
                delete item->widget();
            }
            delete item;
        }
        
        // 创建新的标签网格
        m_labels.resize(rows);
        for (int row = 0; row < rows; ++row) {
            m_labels[row].resize(cols);
            for (int col = 0; col < cols; ++col) {
                QLabel *label = new QLabel();
                label->setFixedSize(16, 16); // 减小标签大小
                label->setAlignment(Qt::AlignCenter);
                label->setStyleSheet("border: 1px solid #BBBBBB;");
                m_gridLayout->addWidget(label, row, col);
                m_labels[row][col] = label;
            }
        }
    }
    
    // 更新标签显示
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            if (m_gameBoard->isMineAt(row, col)) {
                m_labels[row][col]->setStyleSheet("background-color: #FF5252; border: 1px solid #BBBBBB;");
            } else {
                m_labels[row][col]->setStyleSheet("background-color: #E0E0E0; border: 1px solid #BBBBBB;");
            }
        }
    }
} 