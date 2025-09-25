#include "mainwindow.h"
#include <QApplication>
#include <QClipboard>
#include <QGroupBox>
#include <QJsonArray>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QScreen>
#include <QScrollArea>
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_maxButtonRowWidth(0)
{
    setupUi();
    
    // 加载默认JSON文件
    loadJsonData(":/resources/resume_data.json");
    
    // 初始设置为窗口置顶
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    m_alwaysOnTopCheckbox->setChecked(true);
    
    // 调整窗口大小
    adjustWindowSize();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    // 设置中央窗口部件
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    // 创建一个滚动区域
    QScrollArea *scrollArea = new QScrollArea(m_centralWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setFrameShape(QFrame::NoFrame);
    
    // 创建内容窗口部件
    QWidget *contentWidget = new QWidget();
    scrollArea->setWidget(contentWidget);
    
    // 创建主布局
    m_mainLayout = new QVBoxLayout(contentWidget);
    m_mainLayout->setSpacing(8);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    
    // 创建标题栏和控制按钮
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *titleLabel = new QLabel("秋招神器", this);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    
    m_alwaysOnTopCheckbox = new QCheckBox("置顶", this);
    connect(m_alwaysOnTopCheckbox, &QCheckBox::toggled, this, &MainWindow::onAlwaysOnTopToggled);
    
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_alwaysOnTopCheckbox);
    
    m_mainLayout->addLayout(headerLayout);
    
    // 设置中央窗口的布局
    QVBoxLayout *centralLayout = new QVBoxLayout(m_centralWidget);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->addWidget(scrollArea);
    
    // 设置窗口样式
    setWindowTitle("秋招神器");
    setStyleSheet(
        "QMainWindow {"
        "  background-color: #f5f5f5;"
        "}"
        "QGroupBox {"
        "  border: 1px solid #ddd;"
        "  border-radius: 4px;"
        "  margin-top: 8px;"
        "  background-color: #ffffff;"
        "}"
        "QGroupBox::title {"
        "  subcontrol-origin: margin;"
        "  left: 10px;"
        "  padding: 0 5px 0 5px;"
        "  background-color: #ffffff;"
        "}"
        "QPushButton {"
        "  background-color: #e6f2ff;"
        "  border: 1px solid #b3d9ff;"
        "  border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #cce5ff;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #99ccff;"
        "}"
    );
}

void MainWindow::loadJsonData(const QString &filePath)
{
    // 清除现有的按钮
    while (QLayoutItem *item = m_mainLayout->takeAt(1)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
    
    m_maxButtonRowWidth = 0;
    
    // 加载并解析JSON数据
    bool success = false;
    if (filePath.startsWith(":/")) {
        success = m_jsonReader.loadFromFile(filePath);
    } else {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString jsonData = file.readAll();
            file.close();
            success = m_jsonReader.loadFromString(jsonData);
        }
    }
    
    if (!success) {
        QMessageBox::critical(this, "错误", "无法加载或解析JSON数据");
        return;
    }
    
    // 创建按钮组
    createButtonGroups(m_jsonReader.getGroups());
    
    // 调整窗口大小
    adjustWindowSize();
}

void MainWindow::createButtonGroups(const QJsonArray &groups)
{
    for (int i = 0; i < groups.size(); ++i) {
        QJsonObject groupObj = groups[i].toObject();
        createGroupButtons(m_mainLayout, groupObj);
    }
}

void MainWindow::createGroupButtons(QVBoxLayout *mainLayout, const QJsonObject &group)
{
    QString groupName = group["Name"].toString();
    QJsonArray snippets = group["Snippets"].toArray();
    
    if (snippets.isEmpty()) {
        return;
    }
    
    // 创建分组框
    QGroupBox *groupBox = new QGroupBox(groupName);
    QVBoxLayout *groupLayout = new QVBoxLayout(groupBox);
    groupLayout->setSpacing(8);
    
    QHBoxLayout *currentRowLayout = new QHBoxLayout();
    int currentRowWidth = 0;
    const int buttonMargin = 16; // 每个按钮的额外边距
    
    // 为每个代码片段创建按钮
    for (int i = 0; i < snippets.size(); ++i) {
        QJsonObject snippet = snippets[i].toObject();
        QString title = snippet["Title"].toString();
        QString content = snippet["Content"].toString();
        bool wrap = snippet["wrap"].toString() == "true";
        
        // 创建按钮
        SnippetButton *button = new SnippetButton(title, content, wrap);
        connect(button, &QPushButton::clicked, this, &MainWindow::onButtonClicked);
        
        // 计算按钮宽度（估算值）
        QFontMetrics fontMetrics(button->font());
        int buttonWidth = fontMetrics.horizontalAdvance(title) + buttonMargin;
        
        // 添加按钮到当前行
        currentRowLayout->addWidget(button);
        currentRowWidth += buttonWidth;
        
        // 如果需要换行或者是最后一个按钮
        if (wrap || i == snippets.size() - 1) {
            currentRowLayout->addStretch();
            groupLayout->addLayout(currentRowLayout);
            
            // 更新最大行宽
            m_maxButtonRowWidth = qMax(m_maxButtonRowWidth, currentRowWidth);
            
            // 如果不是最后一个按钮，创建新的行布局
            if (i < snippets.size() - 1) {
                currentRowLayout = new QHBoxLayout();
                currentRowWidth = 0;
            }
        }
    }
    
    mainLayout->addWidget(groupBox);
}

void MainWindow::onButtonClicked()
{
    SnippetButton *button = qobject_cast<SnippetButton *>(sender());
    if (button) {
        // 将按钮内容复制到剪贴板
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(button->content());
        
        // 模拟Ctrl+V按键（剪贴板内容粘贴）
        // 注意：这种方法在某些系统中可能不工作，需要更复杂的实现
        // 这里只是将内容放到剪贴板上，用户可以手动粘贴
    }
}

void MainWindow::onAlwaysOnTopToggled(bool checked)
{
    Qt::WindowFlags flags = windowFlags();
    if (checked) {
        flags |= Qt::WindowStaysOnTopHint;
    } else {
        flags &= ~Qt::WindowStaysOnTopHint;
    }
    setWindowFlags(flags);
    show(); // 需要重新显示窗口以应用新标志
}

void MainWindow::adjustWindowSize()
{
    // 获取屏幕尺寸
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    
    // 计算窗口尺寸
    int width = m_maxButtonRowWidth + 60; // 添加额外空间以容纳滚动条和边距
    int height = screenGeometry.height() * 0.7; // 窗口高度为屏幕高度的70%
    
    // 确保宽度不会太小
    width = qMax(width, 300);
    
    // 调整窗口尺寸
    resize(width, height);
    
    // 将窗口移动到屏幕中央
    move(screenGeometry.center() - rect().center());
}