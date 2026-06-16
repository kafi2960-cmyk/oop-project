#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <QString>
#include <QList>

class Customer {
public:
    Customer();
    Customer(int id, const QString& name, const QString& contact, const QString& email, const QString& password = "");

    int getId() const;
    QString getName() const;
    QString getContact() const;
    QString getEmail() const;
    QString getPassword() const;
    QList<int> getBookingHistory() const;

    void setName(const QString& n);
    void setContact(const QString& c);
    void setEmail(const QString& e);
    void setPassword(const QString& p);
    void addBookingRef(int id);
    void removeBookingRef(int id);

    QString toCsvLine() const;
    static Customer fromCsvLine(const QString& line);

private:
    int m_id;
    QString m_name, m_contact, m_email, m_password;
    QList<int> m_bookingHistory;
};

#endif
