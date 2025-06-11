#include "mainwindow.h"
#include <QMessageBox>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 设置窗口标题和大小
    setWindowTitle("扫雷游戏");
    resize(400, 500);
    
    // 初始化UI
    setupUI();
    
    // 初始化游戏难度
    initializeDifficulties();
    
    // 开始新游戏
    startNewGame();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI()
{
    // 创建中央部件和主布局
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    
    // 创建控制面板
    m_controlLayout = new QHBoxLayout();
    m_mainLayout->addLayout(m_controlLayout);
    
    // 创建地雷计数器
    m_mineCounterLabel = new QLabel("地雷: 0");
    m_controlLayout->addWidget(m_mineCounterLabel);
    
    // 创建新游戏按钮
    m_newGameButton = new QPushButton("新游戏");
    connect(m_newGameButton, &QPushButton::clicked, this, &MainWindow::startNewGame);
    m_controlLayout->addWidget(m_newGameButton);
    
    // 创建难度选择下拉框
    m_difficultyComboBox = new QComboBox();
    m_difficultyComboBox->addItem("初级");
    m_difficultyComboBox->addItem("中级");
    m_difficultyComboBox->addItem("高级");
    m_difficultyComboBox->addItem("自定义"); // 添加自定义选项
    m_controlLayout->addWidget(m_difficultyComboBox);

    // 创建自定义输入字段的布局
    QHBoxLayout* customInputLayout = new QHBoxLayout();
    m_controlLayout->addLayout(customInputLayout);
    
    // 创建行数输入框和标签
    QLabel* rowsLabel = new QLabel("行数:");
    customInputLayout->addWidget(rowsLabel);
    m_rowsInput = new QLineEdit("9");
    m_rowsInput->setPlaceholderText("1-30");
    m_rowsInput->setFixedWidth(50);
    customInputLayout->addWidget(m_rowsInput);

    // 创建列数输入框和标签
    QLabel* colsLabel = new QLabel("列数:");
    customInputLayout->addWidget(colsLabel);
    m_colsInput = new QLineEdit("9");
    m_colsInput->setPlaceholderText("1-30");
    m_colsInput->setFixedWidth(50);
    customInputLayout->addWidget(m_colsInput);

    // 创建雷数输入框和标签
    QLabel* minesLabel = new QLabel("雷数:");
    customInputLayout->addWidget(minesLabel);
    m_minesInput = new QLineEdit("10");
    m_minesInput->setPlaceholderText("1-899");
    m_minesInput->setFixedWidth(50);
    customInputLayout->addWidget(m_minesInput);
    
    // 连接难度选择的信号槽
    connect(m_difficultyComboBox, &QComboBox::currentTextChanged, this, [this](const QString &text) {
        bool isCustom = (text == "自定义");
        m_rowsInput->setEnabled(isCustom);
        m_colsInput->setEnabled(isCustom);
        m_minesInput->setEnabled(isCustom);
    });
    
    // 初始状态下禁用输入框
    m_rowsInput->setEnabled(false);
    m_colsInput->setEnabled(false);
    m_minesInput->setEnabled(false);
    
    // 创建计时器
    m_timerLabel = new QLabel("时间: 0");
    m_controlLayout->addWidget(m_timerLabel);
    
    // 创建游戏板
    m_gameBoard = new GameBoard(this);
    m_mainLayout->addWidget(m_gameBoard);
    
    // 添加底部信息布局
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    m_mainLayout->addLayout(bottomLayout);
    
    // 添加Debug模式提示（左对齐）
    QLabel* debugHintLabel = new QLabel("<i>提示: 连续按3次Delete键可以开启调试模式</i>");
    debugHintLabel->setStyleSheet("color: #888888; font-size: 10px;");
    bottomLayout->addWidget(debugHintLabel);
    
    // 添加弹簧
    bottomLayout->addStretch();
    
    // 添加开源地址链接（右对齐）
    QLabel *githubLabel = new QLabel("<a href=\"https://github.com/ZQDesigned/Minesweeper-Qt\">开源地址：Github</a>");
    githubLabel->setOpenExternalLinks(true);
    bottomLayout->addWidget(githubLabel);
    
    // 连接信号和槽
    connect(m_gameBoard, &GameBoard::gameOver, this, &MainWindow::onGameOver);
    connect(m_gameBoard, &GameBoard::updateMineCounter, this, &MainWindow::updateMineCounter);
    connect(m_gameBoard, &GameBoard::updateTimer, this, &MainWindow::updateTimer);
}

void MainWindow::initializeDifficulties()
{
    // 初始化难度设置
    m_difficulties.clear();
    
    // 初级: 9x9, 10个地雷
    m_difficulties.append({9, 9, 10, "初级"});
    
    // 中级: 16x16, 40个地雷
    m_difficulties.append({16, 16, 40, "中级"});
    
    // 高级: 16x30, 99个地雷
    m_difficulties.append({16, 30, 99, "高级"});
    // 为自定义难度添加一个占位符，实际值将从输入框读取
    m_difficulties.append({9, 9, 10, "自定义"}); 
}

void MainWindow::startNewGame()
{
    int rows, cols, mines;
    int index = m_difficultyComboBox->currentIndex();

    if (m_difficultyComboBox->currentText() == "自定义") {
        bool okRows, okCols, okMines;
        rows = m_rowsInput->text().toInt(&okRows);
        cols = m_colsInput->text().toInt(&okCols);
        mines = m_minesInput->text().toInt(&okMines);

        if (!okRows || !okCols || !okMines || rows <= 0 || cols <= 0 || mines <= 0 || mines >= rows * cols) {
            QMessageBox::warning(this, "输入无效", "请输入有效的行数、列数和地雷数量。地雷数量必须小于总单元格数。");
            // 恢复到上一个有效难度或默认难度
            if (index > 0 && index < m_difficulties.size() -1) { // m_difficulties.size() -1 is custom
                 m_difficultyComboBox->setCurrentIndex(index > 0 ? index -1 : 0); // revert to previous or first
            } else {
                 m_difficultyComboBox->setCurrentIndex(0); // default to first difficulty
            }
            // Optionally, clear or reset custom input fields
            const Difficulty &defaultDifficulty = m_difficulties[m_difficultyComboBox->currentIndex()];
            m_rowsInput->setText(QString::number(defaultDifficulty.rows));
            m_colsInput->setText(QString::number(defaultDifficulty.cols));
            m_minesInput->setText(QString::number(defaultDifficulty.mines));
            return;
        }
    } else {
        const Difficulty &difficulty = m_difficulties[index];
        rows = difficulty.rows;
        cols = difficulty.cols;
        mines = difficulty.mines;
        // 更新自定义输入框以反映所选难度
        m_rowsInput->setText(QString::number(rows));
        m_colsInput->setText(QString::number(cols));
        m_minesInput->setText(QString::number(mines));
    }
    
    // 初始化游戏板
    m_gameBoard->initializeBoard(rows, cols, mines);
    
    // 调整窗口大小
    int cellSize = 30; // 默认单元格大小
    int controlPanelHeight = 50; // 控制面板高度
    int windowWidth = cols * cellSize + 20; // 添加一些边距
    int windowHeight = rows * cellSize + controlPanelHeight + 40; // 添加控制面板高度和边距
    
    // 确保窗口不会太小
    windowWidth = qMax(windowWidth, 300);
    windowHeight = qMax(windowHeight, 350);
    
    resize(windowWidth, windowHeight);
    
    // 确保游戏板获得焦点，以便能接收键盘事件
    m_gameBoard->setFocus();
}

void MainWindow::onGameOver(bool won)
{
    // 显示游戏结果
    QString message = won ? "恭喜你赢了!" : "游戏结束!";
    QMessageBox::information(this, "游戏结束", message);
}

void MainWindow::updateMineCounter(int count)
{
    // 更新地雷计数器
    m_mineCounterLabel->setText(QString("地雷: %1").arg(count));
}

void MainWindow::updateTimer(int seconds)
{
    // 更新计时器
    m_timerLabel->setText(QString("时间: %1").arg(seconds));
}
