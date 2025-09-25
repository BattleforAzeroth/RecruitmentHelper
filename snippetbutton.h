#ifndef SNIPPETBUTTON_H
#define SNIPPETBUTTON_H

#include <QPushButton>
#include <QString>

class SnippetButton : public QPushButton
{
    Q_OBJECT
    
public:
    explicit SnippetButton(const QString &title, const QString &content, 
                           bool wrap = false, QWidget *parent = nullptr);
    
    QString content() const;
    bool shouldWrap() const;
    
private:
    QString m_content;
    bool m_wrap;
};

#endif // SNIPPETBUTTON_H