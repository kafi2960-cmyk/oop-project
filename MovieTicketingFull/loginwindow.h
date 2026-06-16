#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "bookingmanager.h"

class LoginWindow : public QDialog {
    Q_OBJECT

public:
    explicit LoginWindow(BookingManager* bookingManager, QWidget* parent = nullptr);
    ~LoginWindow();

    bool isAdmin() const;
    int getLoggedInCustomerId() const;

private slots:
    void onLoginClicked();
    void onRegisterClicked();

private:
    BookingManager* m_bookingManager;
    bool m_isAdmin;
    int m_loggedInCustomerId;

    QLineEdit* m_emailEdit;
    QLineEdit* m_passwordEdit;
    QPushButton* m_loginBtn;
    QPushButton* m_registerBtn;

    void setupUI();
};

#endif // LOGINWINDOW_H
