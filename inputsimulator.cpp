#include "inputsimulator.h"
#include <QApplication>
#include <QClipboard>
#include <QDebug>

#ifdef Q_OS_WIN
    #include <windows.h>
#endif

#ifdef Q_OS_MAC
    #include <ApplicationServices/ApplicationServices.h>
#endif

InputSimulator::InputSimulator(QObject *parent) : QObject(parent)
{
}

bool InputSimulator::sendTextToActiveWindow(const QString &text)
{
    // First, save current clipboard content
    QClipboard *clipboard = QApplication::clipboard();
    QString savedText = clipboard->text();

    // Set the new text to the clipboard
    clipboard->setText(text);

    // Simulate Ctrl+V (or Command+V on Mac) to paste the content
#ifdef Q_OS_WIN
    // Get the foreground window
    HWND hwnd = GetForegroundWindow();
    if (!hwnd) {
        qWarning() << "Failed to get foreground window";
        return false;
    }

    // Simulate Ctrl+V keypress
    INPUT inputs[4] = {};
    
    // Press Ctrl
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_CONTROL;
    
    // Press V
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 'V';
    
    // Release V
    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = 'V';
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    
    // Release Ctrl
    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_CONTROL;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
    
    SendInput(4, inputs, sizeof(INPUT));
    
#elif defined(Q_OS_MAC)
    // Mac implementation using CGEventCreateKeyboardEvent
    CGEventSourceRef source = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
    
    // Command Down
    CGEventRef cmdDown = CGEventCreateKeyboardEvent(source, (CGKeyCode)55, true);
    // V Down
    CGEventRef vDown = CGEventCreateKeyboardEvent(source, (CGKeyCode)9, true);
    // V Up
    CGEventRef vUp = CGEventCreateKeyboardEvent(source, (CGKeyCode)9, false);
    // Command Up
    CGEventRef cmdUp = CGEventCreateKeyboardEvent(source, (CGKeyCode)55, false);
    
    CGEventPost(kCGHIDEventTap, cmdDown);
    CGEventPost(kCGHIDEventTap, vDown);
    CGEventPost(kCGHIDEventTap, vUp);
    CGEventPost(kCGHIDEventTap, cmdUp);
    
    CFRelease(cmdDown);
    CFRelease(vDown);
    CFRelease(vUp);
    CFRelease(cmdUp);
    CFRelease(source);
#else
    // Fallback for other platforms
    qWarning() << "Input simulation not implemented for this platform";
    return false;
#endif

    // Wait a bit to ensure paste completes
    QThread::msleep(50);
    
    // Restore original clipboard content
    clipboard->setText(savedText);
    
    return true;
}