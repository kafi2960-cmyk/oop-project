#include "loginwindow.h"
#include "registerwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

LoginWindow::LoginWindow(BookingManager* bookingManager, QWidget* parent)
    : QDialog(parent), m_bookingManager(bookingManager), m_isAdmin(false), m_loggedInCustomerId(-1) {
    setupUI();
    setWindowTitle("Login - Movie Ticketing System");
    setMinimumWidth(350);
}

LoginWindow::~LoginWindow() {}

bool LoginWindow::isAdmin() const {
    return m_isAdmin;
}

int LoginWindow::getLoggedInCustomerId() const {
    return m_loggedInCustomerId;
}

void LoginWindow::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QLabel* titleLabel = new QLabel("Welcome to Movie Ticketing");
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(10);

    QFormLayout* formLayout = new QFormLayout;
    m_emailEdit = new QLineEdit;
    m_emailEdit->setPlaceholderText("Email");
    m_passwordEdit = new QLineEdit;
    m_passwordEdit->setPlaceholderText("Password");
    m_passwordEdit->setEchoMode(QLineEdit::Password);

    formLayout->addRow("Email:", m_emailEdit);
    formLayout->addRow("Password:", m_passwordEdit);
    mainLayout->addLayout(formLayout);

    mainLayout->addSpacing(15);

    QHBoxLayout* btnLayout = new QHBoxLayout;
    m_loginBtn = new QPushButton("Login");
    m_loginBtn->setStyleSheet("background-color: #2980b9; color: white; font-weight: bold; padding: 6px;");
    m_registerBtn = new QPushButton("Register");
    
    btnLayout->addWidget(m_registerBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(m_loginBtn);
    mainLayout->addLayout(btnLayout);

    connect(m_loginBtn, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(m_registerBtn, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginWindow::onLoginClicked);
}

void LoginWindow::onLoginClicked() {
    QString email = m_emailEdit->text().trimmed();
    QString password = m_passwordEdit->text();

    if (email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter both email and password.");
        return;
    }

    if (email == "admin" && password == "admin") {
        m_isAdmin = true;
        m_loggedInCustomerId = -1;
        accept();
        return;
    }

    Customer* cust = m_bookingManager->authenticateCustomer(email, password);
    if (cust) {
        m_isAdmin = false;
        m_loggedInCustomerId = cust->getId();
        accept();
    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid credentials. Please try again or register.");
    }
}

void LoginWindow::onRegisterClicked() {
    RegisterWindow regWindow(m_bookingManager, this);
    if (regWindow.exec() == QDialog::Accepted) {
        m_emailEdit->setText(regWindow.getRegisteredEmail());
        m_passwordEdit->clear();
        QMessageBox::information(this, "Success", "Registration successful! Please log in.");
    }
}
