#ifndef ONLINE_H
#define ONLINE_H

#include <QWidget>

namespace Ui {
class online;
}

class online : public QWidget
{
    Q_OBJECT

public:
    explicit online(QWidget *parent = nullptr);
    ~online();

private:
    Ui::online *ui;
};

#endif // ONLINE_H
