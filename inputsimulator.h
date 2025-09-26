#ifndef INPUTSIMULATOR_H
#define INPUTSIMULATOR_H

#include <QString>
#include <QObject>

class InputSimulator : public QObject
{
    Q_OBJECT
public:
    explicit InputSimulator(QObject *parent = nullptr);

    // Send text to the active window
    bool sendTextToActiveWindow(const QString &text);
};

#endif // INPUTSIMULATOR_H