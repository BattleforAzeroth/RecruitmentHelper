#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QTimer>
#include "snippetmanager.h"
#include "inputsimulator.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void onSnippetButtonClicked();
    void toggleAlwaysOnTop();
    void saveWindowGeometry();
    
private:
    void createUI();
    void loadSettings();
    void calculateOptimalSize();
    int calculateMaxRowWidth(const QVector<Snippet> &snippets);
    
    QScrollArea *m_scrollArea;
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    
    SnippetManager m_snippetManager;
    InputSimulator m_inputSimulator;
    
    bool m_alwaysOnTop;
    QPoint m_dragPosition;
    bool m_isDragging;
    QTimer *m_geometrySaveTimer;
    int m_maxButtonRowWidth;
};

#endif // MAINWINDOW_H