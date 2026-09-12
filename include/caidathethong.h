#ifndef CAIDATHETHONG_H
#define CAIDATHETHONG_H

#include <QDialog>

class QLabel;
class QSpinBox;
class QPushButton;

class CaiDatHeThong : public QDialog
{
    Q_OBJECT

public:
    explicit CaiDatHeThong(
        QWidget *cha = nullptr
    );

private slots:
    // Luu cau hinh vao file JSON
    void xuLyLuuCauHinh();

    // Dong cua so ma khong luu
    void xuLyHuy();

private:
    void luuCauHinhTuGiaoDien();

    // Doc cau hinh hien tai va dua len giao dien
    void taiCauHinhHienTai();

    QSpinBox *spnDoorCloseTime;

    QPushButton *btnSave;
    QPushButton *btnCancel;

    QLabel *lblNotification;
};

#endif
