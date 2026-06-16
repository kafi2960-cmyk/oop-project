#ifndef BOOKING_H
#define BOOKING_H

#include <QString>
#include <QList>
#include <QDateTime>
#include "ticket.h"

enum class BookingStatus { Active, Cancelled };

class Booking {
public:
    Booking();
    Booking(int id, int customerId, int screeningId, const QDateTime& dt);
    ~Booking();

    int getId() const;
    int getCustomerId() const;
    int getScreeningId() const;
    QString getReference() const;
    QDateTime getBookingTime() const;
    BookingStatus getStatus() const;
    const QList<Ticket*>& getTickets() const;
    double getTotal() const;

    void addTicket(Ticket* t);
    void cancel();

    QString toCsvLine() const;
    static Booking* fromCsvLine(const QString& line);
    static QString statusToString(BookingStatus s);
    static BookingStatus statusFromString(const QString& s);

private:
    int m_id, m_customerId, m_screeningId;
    QString m_reference;
    QDateTime m_bookingTime;
    BookingStatus m_status;
    QList<Ticket*> m_tickets;
};

#endif
