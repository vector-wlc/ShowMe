/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-12-04 12:25:40
 * @Description: 
 */

#include "MutexWidget.h"
#include <QContextMenuEvent>
#include <QInputDialog>
#include <QMenu>
#include <set>

MutexWidget::MutexWidget()
    : QTableWidget()
{
    this->add_mutex = new QAction("添加项");
    this->erase_mutex = new QAction("删除选定项");

    connect(add_mutex, &QAction::triggered, [=]() {
        bool is_ok = false;
        int index = QInputDialog::getInt(this, "添加项", "请输入对象编号", 0, 0, INT_MAX, 1, &is_ok);
        if (is_ok) {
            int row_cnt = this->rowCount();
            this->setRowCount(row_cnt + 1);
            auto item = new QTableWidgetItem(QString("%1").arg(index));
            item->setTextAlignment(Qt::AlignCenter);
            this->setItem(row_cnt, 0, item);

            item = new QTableWidgetItem(QString("未知"));
            item->setTextAlignment(Qt::AlignCenter);
            this->setItem(row_cnt, 1, item);

            item = new QTableWidgetItem(QString("未知"));
            item->setTextAlignment(Qt::AlignCenter);
            this->setItem(row_cnt, 2, item);
            emit rowChanged();
        }
    });

    connect(erase_mutex, &QAction::triggered, [=]() {
        const auto& selected_items = this->selectedItems();
        std::set<int> remove_rows;
        for (const auto& selected_item : selected_items) {
            int row = this->row(selected_item);
            if (remove_rows.find(row) == remove_rows.end()) {
                remove_rows.insert(row);
            }
        }

        int i = 0;
        for (const auto& row : remove_rows) {
            this->removeRow(row - i);
            ++i;
        }

        emit rowChanged();
    });
}

void MutexWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton) {
        return;
    }

    QTableWidget::mousePressEvent(event);
}

void MutexWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton) {
        QMenu menu(this);
        menu.addAction(add_mutex);
        menu.addAction(erase_mutex);
        menu.exec(QCursor::pos());
    }

    QTableWidget::mouseReleaseEvent(event);
}
