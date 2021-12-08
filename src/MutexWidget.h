/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-12-04 12:21:57
 * @Description: 
 */

#include <QAction>
#include <QMenu>
#include <qtablewidget.h>

class MutexWidget : public QTableWidget {
    Q_OBJECT
private:
    QAction* add_mutex;
    QAction* erase_mutex;

protected:
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;

signals:
    void rowChanged();

public:
    MutexWidget();
};
