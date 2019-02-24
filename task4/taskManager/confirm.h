#ifndef CONFIRM_H
#define CONFIRM_H

#include <QDialog>

namespace Ui {
class Confirm;
}

class Confirm : public QDialog
{
    Q_OBJECT

public:
    explicit Confirm(QWidget *parent = nullptr);
    ~Confirm();

private:
    Ui::Confirm *ui;
private slots:
    void Ok();
    void Cancel();
};

#endif // CONFIRM_H
