#include "registerwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QLabel>

RegisterWindow::RegisterWindow(BookingManager* bookingManager, QWidget* parent)
    : QDialog(parent), m_bookingManager(bookingManager) {
    setupUI();
    setWindowTitle("Register - Movie Ticketing System");
    setMinimumWidth(350);
}

RegisterWindow::~RegisterWindow() {}

QString RegisterWindow::getRegisteredEmail() const {
    return m_registeredEmail;
}

void RegisterWindow::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QLabel* titleLabel = new QLabel("Create a New Account");
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(10);

    QFormLayout* formLayout = new QFormLayout;
    m_nameEdit = new QLineEdit;
    m_contactEdit = new QLineEdit;
    m_emailEdit = new QLineEdit;
    m_passwordEdit = new QLineEdit;
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_confirmPasswordEdit = new QLineEdit;
    m_confirmPasswordEdit->setEchoMode(QLineEdit::Password);

    formLayout->addRow("Full Name:", m_nameEdit);
    formLayout->addRow("Contact Number:", m_contactEdit);
    formLayout->addRow("Email Address:", m_emailEdit);
    formLayout->addRow("Password:", m_passwordEdit);
    formLayout->addRow("Confirm Password:", m_confirmPasswordEdit);
    mainLayout->addLayout(formLayout);

    mainLayout->addSpacing(15);

    QHBoxLayout* btnLayout = new QHBoxLayout;
    m_registerBtn = new QPushButton("Register");
    m_registerBtn->setStyleSheet("background-color: #27ae60; color: white; font-weight: bold;");
    m_cancelBtn = new QPushButton("Cancel");

    btnLayout->addStretch();
    btnLayout->addWidget(m_cancelBtn);
    btnLayout->addWidget(m_registerBtn);
    mainLayout->addLayout(btnLayout);

    connect(m_registerBtn, &QPushButton::clicked, this, &RegisterWindow::onRegisterClicked);
    connect(m_cancelBtn, &QPushButton::clicked, this, &RegisterWindow::onCancelClicked);
}

void RegisterWindow::onRegisterClicked() {
    QString name = m_nameEdit->text().trimmed();
    QString contact = m_contactEdit->text().trimmed();
    QString email = m_emailEdit->text().trimmed();
    QString password = m_passwordEdit->text();
    QString confirmPassword = m_confirmPasswordEdit->text();

    if (name.isEmpty() || contact.isEmpty() || email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "All fields are required.");
        return;
    }

    if (password != confirmPassword) {
        QMessageBox::warning(this, "Error", "Passwords do not match.");
        return;
    }

    // Check if email already exists
    for (const Customer& c : m_bookingManager->getAllCustomers()) {
        if (c.getEmail().compare(email, Qt::CaseInsensitive) == 0) {
            QMessageBox::warning(this, "Error", "An account with this email already exists.");
            return;
        }
    }

    Customer newCustomer(m_bookingManager->nextCustomerId(), name, contact, email, password);
    if (m_bookingManager->addCustomer(newCustomer)) {
        m_registeredEmail = email;
        accept();
    } else {
        QMessageBox::warning(this, "Error", "Failed to create account. Please try again.");
    }
}

void RegisterWindow::onCancelClicked() {
    reject();
}
