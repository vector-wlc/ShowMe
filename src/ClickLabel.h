/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-12-05 16:43:23
 * @Description: 
 */
#include <QLabel>
#include <QMouseEvent>

class ClickLabel : public QLabel {
    Q_OBJECT

protected:
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void enterEvent(QEvent* event) override;
    virtual void leaveEvent(QEvent* event) override;

signals:
    void clicked();
};
