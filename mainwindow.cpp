#include "mainwindow.h"
#include <QApplication>
#include <QScreen>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QSettings>
#include <QFontMetrics>
#include <QTimer>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_alwaysOnTop(true),
    m_isDragging(false),
    m_maxButtonRowWidth(0)
{
    setWindowTitle("秋招神器");
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    // Set minimum size
    setMinimumHeight(100);
    setMinimumWidth(200);

    // Create geometry save timer
    m_geometrySaveTimer = new QTimer(this);
    m_geometrySaveTimer->setSingleShot(true);
    m_geometrySaveTimer->setInterval(500); // 500ms debounce
    connect(m_geometrySaveTimer, &QTimer::timeout, this, &MainWindow::saveWindowGeometry);

    // Load snippet data
    if (!m_snippetManager.loadFromFile(":/resources/resume_data.json")) {
        QMessageBox::critical(this, "错误", "无法加载简历数据文件。");
    }

    loadSettings();
    createUI();
    calculateOptimalSize();
}

MainWindow::~MainWindow()
{
    saveWindowGeometry();
}

void MainWindow::createUI()
{
    // Create central widget and main layout
    m_centralWidget = new QWidget(this);
    m_centralWidget->setObjectName("centralWidget");
    m_centralWidget->setStyleSheet(
        "QWidget#centralWidget { background-color: #f5f5f5; border: 1px solid #cccccc; }"
        "QGroupBox { font-weight: bold; border: 1px solid #cccccc; border-radius: 5px; margin-top: 1ex; }"
        "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top center; }"
        "QPushButton { background-color: #e6e6e6; border: 1px solid #cccccc; border-radius: 3px; padding: 3px; }"
        "QPushButton:hover { background-color: #d9d9d9; }"
        "QPushButton:pressed { background-color: #c2c2c2; }"
    );

    setCentralWidget(m_centralWidget);

    // Create scroll area
    m_scrollArea = new QScrollArea(m_centralWidget);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Create container widget for scroll area
    QWidget *scrollWidget = new QWidget(m_scrollArea);
    m_scrollArea->setWidget(scrollWidget);

    // Create layout for scroll widget
    m_mainLayout = new QVBoxLayout(scrollWidget);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);

    // Add a header with drag area and controls
    QWidget *headerWidget = new QWidget(this);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(5, 5, 5, 5);

    QLabel *titleLabel = new QLabel("秋招神器", headerWidget);
    titleLabel->setStyleSheet("font-weight: bold; color: #333333;");

    QPushButton *pinButton = new QPushButton(m_alwaysOnTop ? "取消置顶" : "置顶", headerWidget);
    pinButton->setFixedSize(60, 20);
    connect(pinButton, &QPushButton::clicked, [this, pinButton]() {
        toggleAlwaysOnTop();
        pinButton->setText(m_alwaysOnTop ? "取消置顶" : "置顶");
    });

    QPushButton *closeButton = new QPushButton("关闭", headerWidget);
    closeButton->setFixedSize(50, 20);
    connect(closeButton, &QPushButton::clicked, this, &QMainWindow::close);

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(pinButton);
    headerLayout->addWidget(closeButton);

    m_mainLayout->addWidget(headerWidget);

    // Add a separator line
    QFrame *separator = new QFrame(scrollWidget);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    m_mainLayout->addWidget(separator);

    // Create group boxes and buttons for each snippet group
    const QVector<SnippetGroup> &groups = m_snippetManager.getGroups();
    for (const SnippetGroup &group : groups) {
        QGroupBox *groupBox = new QGroupBox(group.name, scrollWidget);
        QVBoxLayout *groupLayout = new QVBoxLayout(groupBox);
        groupLayout->setSpacing(5);

        QHBoxLayout *currentRowLayout = new QHBoxLayout();
        currentRowLayout->setSpacing(5);

        for (int i = 0; i < group.snippets.size(); ++i) {
            const Snippet &snippet = group.snippets[i];

            QPushButton *button = new QPushButton(snippet.title, groupBox);
            button->setToolTip(snippet.content);
            button->setProperty("snippetContent", snippet.content);

            connect(button, &QPushButton::clicked, this, &MainWindow::onSnippetButtonClicked);

            currentRowLayout->addWidget(button);

            // Add to new row if wrap is true or it's the last snippet
            if (snippet.wrap || i == group.snippets.size() - 1) {
                currentRowLayout->addStretch();
                groupLayout->addLayout(currentRowLayout);

                // Only create a new layout if this isn't the last snippet
                if (i < group.snippets.size() - 1) {
                    currentRowLayout = new QHBoxLayout();
                    currentRowLayout->setSpacing(5);
                }
            }
        }

        m_mainLayout->addWidget(groupBox);
    }

    m_mainLayout->addStretch();

    // Set up the main layout for the central widget
    QVBoxLayout *centralLayout = new QVBoxLayout(m_centralWidget);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->addWidget(m_scrollArea);
}

void MainWindow::calculateOptimalSize()
{
    // Get the current screen
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();

    // Calculate max height (70% of screen height)
    int maxHeight = screenGeometry.height() * 0.7;

    // Calculate width based on the widest row of buttons
    QFontMetrics fm(font());
    const QVector<SnippetGroup> &groups = m_snippetManager.getGroups();

    m_maxButtonRowWidth = 0;
    for (const SnippetGroup &group : groups) {
        int rowWidth = calculateMaxRowWidth(group.snippets);
        if (rowWidth > m_maxButtonRowWidth) {
            m_maxButtonRowWidth = rowWidth;
        }
    }

    // Add padding, margins, and scrollbar width
    int optimalWidth = m_maxButtonRowWidth + 50;

    // Ensure the width is reasonable
    optimalWidth = qMax(300, qMin(optimalWidth, screenGeometry.width() - 100));

    // Resize the window
    resize(optimalWidth, maxHeight);

    // Center on screen
    setGeometry(QStyle::alignedRect(
        Qt::LeftToRight,
        Qt::AlignCenter,
        size(),
        screenGeometry
    ));
}

int MainWindow::calculateMaxRowWidth(const QVector<Snippet> &snippets)
{
    QFontMetrics fm(font());
    int currentRowWidth = 0;
    int maxRowWidth = 0;

    for (const Snippet &snippet : snippets) {
        // Estimate button width: text width + padding
        int buttonWidth = fm.horizontalAdvance(snippet.title) + 20;

        currentRowWidth += buttonWidth + 5; // Add button spacing

        if (snippet.wrap) {
            maxRowWidth = qMax(maxRowWidth, currentRowWidth);
            currentRowWidth = 0;
        }
    }

    // Check the last row if it didn't end with a wrap
    maxRowWidth = qMax(maxRowWidth, currentRowWidth);

    return maxRowWidth;
}

void MainWindow::onSnippetButtonClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        QString content = button->property("snippetContent").toString();
        m_inputSimulator.sendTextToActiveWindow(content);
    }
}

void MainWindow::toggleAlwaysOnTop()
{
    m_alwaysOnTop = !m_alwaysOnTop;

    Qt::WindowFlags flags = windowFlags();
    if (m_alwaysOnTop) {
        flags |= Qt::WindowStaysOnTopHint;
    } else {
        flags &= ~Qt::WindowStaysOnTopHint;
    }

    setWindowFlags(flags);
    show(); // Window needs to be shown again after changing flags

    QSettings settings;
    settings.setValue("AlwaysOnTop", m_alwaysOnTop);
}

void MainWindow::loadSettings()
{
    QSettings settings;
    m_alwaysOnTop = settings.value("AlwaysOnTop", true).toBool();

    // Restore geometry if available
    if (settings.contains("WindowGeometry")) {
        restoreGeometry(settings.value("WindowGeometry").toByteArray());
    }
}

void MainWindow::saveWindowGeometry()
{
    QSettings settings;
    settings.setValue("WindowGeometry", saveGeometry());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveWindowGeometry();
    event->accept();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        m_isDragging = true;
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && m_isDragging) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
        m_geometrySaveTimer->start(); // Start timer to save position
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        event->accept();
    }
}