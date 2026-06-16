#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include "bookingmanager.h"

class RegisterWindow : public QDialog {
    Q_OBJECT

public:
    explicit RegisterWindow(BookingManager* bookingManager, QWidget* parent = nullptr);
    ~RegisterWindow();

    QString getRegisteredEmail() const;

private slots:
    void onRegisterClicked();
    void onCancelClicked();

private:
    BookingManager* m_bookingManager;
    QString m_registeredEmail;

    QLineEdit* m_nameEdit;
    QLineEdit* m_contactEdit;
    QLineEdit* m_emailEdit;
    QLineEdit* m_passwordEdit;
    QLineEdit* m_confirmPasswordEdit;

    QPushButton* m_registerBtn;
    QPushButton* m_cancelBtn;

    void setupUI();
};

#endif // REGISTERWINDOW_H
