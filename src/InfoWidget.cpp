/*
 * @Coding: gbk
 * @Author: vector-wlc
 * @Date: 2021-08-15 10:38:36
 * @Description: 
 */
#include "InfoWidget.h"

InfoWidget::InfoWidget()
{
    this->hide();
    // Êó±ê´©Í¸
    this->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    Qt::WindowFlags m_flags = windowFlags();
    // ±£³ÖÖÃ¶¥ÇÒÈ¥µô±ß¿ò
    this->setWindowFlags(m_flags | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::SubWindow);
    // ÉèÖÃÍ¸Ã÷¶È
    this->setWindowOpacity(0.6);
    this->label = new QLabel(this);
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(this->label);
    this->setLayout(layout);
}

void InfoWidget::setText(const QStringList& text_list)
{
    int height = text_list.size() * label->font().pointSize() * 1.05;
    QString text;
    int max_char_cnt = 0;
    for (const auto& str : text_list) {
        text += str;
        text += "\n";
        if (max_char_cnt < str.size()) {
            max_char_cnt = str.size();
        }
    }

    int width = max_char_cnt * label->font().pointSize() * 1.05;
    this->resize(width, height);
    this->label->setText(text);
}

void InfoWidget::setFont(const QFont& font)
{
    this->label->setFont(font);
}

void InfoWidget::setColor(const QColor& color)
{
    auto pattle = this->label->palette();
    pattle.setColor(QPalette::WindowText, color);
    this->label->setPalette(pattle);
}
