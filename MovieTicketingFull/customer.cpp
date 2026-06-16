#include "customer.h"
#include <QStringList>

Customer::Customer() : m_id(0) {}
Customer::Customer(int id, const QString& name, const QString& contact, const QString& email, const QString& password)
    : m_id(id), m_name(name), m_contact(contact), m_email(email), m_password(password) {}

int Customer::getId() const { return m_id; }
QString Customer::getName() const { return m_name; }
QString Customer::getContact() const { return m_contact; }
QString Customer::getEmail() const { return m_email; }
QString Customer::getPassword() const { return m_password; }
QList<int> Customer::getBookingHistory() const { return m_bookingHistory; }

void Customer::setName(const QString& n) { m_name = n; }
void Customer::setContact(const QString& c) { m_contact = c; }
void Customer::setEmail(const QString& e) { m_email = e; }
void Customer::setPassword(const QString& p) { m_password = p; }
void Customer::addBookingRef(int id) { m_bookingHistory.append(id); }
void Customer::removeBookingRef(int id) { m_bookingHistory.removeAll(id); }

QString Customer::toCsvLine() const {
    QStringList bids;
    for (int id : m_bookingHistory) bids << QString::number(id);
    return QString("%1,%2,%3,%4,%5,%6")
        .arg(m_id).arg(m_name).arg(m_contact).arg(m_email).arg(m_password).arg(bids.join("|"));
}

Customer Customer::fromCsvLine(const QString& line) {
    QStringList p = line.split(",");
    if (p.size() < 5) return Customer();
    
    // Handle old format (5 parts) and new format (6 parts)
    QString pwd = "";
    QString bidsStr = "";
    if (p.size() >= 6) {
        pwd = p[4];
        bidsStr = p[5];
    } else {
        bidsStr = p[4];
    }
    
    Customer c(p[0].toInt(), p[1], p[2], p[3], pwd);
    if (!bidsStr.isEmpty())
        for (auto& b : bidsStr.split("|"))
            if (!b.isEmpty()) c.m_bookingHistory.append(b.toInt());
    return c;
}
