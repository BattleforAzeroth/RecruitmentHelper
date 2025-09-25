#include "snippetbutton.h"

SnippetButton::SnippetButton(const QString &title, const QString &content, 
                             bool wrap, QWidget *parent)
    : QPushButton(title, parent), m_content(content), m_wrap(wrap)
{
    // 设置按钮样式和大小策略
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setStyleSheet("QPushButton { padding: 4px 8px; }");
}

QString SnippetButton::content() const
{
    return m_content;
}

bool SnippetButton::shouldWrap() const
{
    return m_wrap;
}