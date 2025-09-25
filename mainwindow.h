#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QJsonArray>
#include <QJsonObject>
#include <QScreen>
#include <QCheckBox>
#include "snippetbutton.h"
#include "jsonreader.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private slots:
    void onButtonClicked();
    void onAlwaysOnTopToggled(bool checked);
    void loadJsonData(const QString &filePath);
    
private:
    void setupUi();
    void createButtonGroups(const QJsonArray &groups);
    void createGroupButtons(QVBoxLayout *mainLayout, const QJsonObject &group);
    void adjustWindowSize();
    
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    QCheckBox *m_alwaysOnTopCheckbox;
    JsonReader m_jsonReader;
    int m_maxButtonRowWidth;
};

#endif // MAINWINDOW_H