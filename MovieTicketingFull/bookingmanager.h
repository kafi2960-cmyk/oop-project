#ifndef BOOKINGMANAGER_H
#define BOOKINGMANAGER_H

#include <QList>
#include <QString>
#include "booking.h"
#include "customer.h"
#include "moviemanager.h"

class BookingManager {
public:
    BookingManager(MovieManager* mm);
    ~BookingManager();

    bool addCustomer(const Customer& c);
    bool updateCustomer(const Customer& c);
    bool deleteCustomer(int id);
    Customer* findCustomerById(int id);
    Customer* authenticateCustomer(const QString& email, const QString& password);
    QList<Customer> searchCustomers(const QString& kw) const;
    QList<Customer> getAllCustomers() const;
    int nextCustomerId() const;

    Booking* createBooking(int customerId, int screeningId);
    bool addTicketToBooking(int bookingId, const QString& seatLabel, int seatId, SeatCategory cat);
    bool finalizeBooking(int bookingId);
    bool cancelBooking(int bookingId);
    Booking* findBookingById(int id);
    QList<Booking*> getBookingsForCustomer(int customerId) const;
    QList<Booking*> getAllBookings() const;
    int nextBookingId() const;
    int nextTicketId() const;

    bool generateTicketFile(int bookingId, const QString& movieTitle,
                            const QString& hallName, const QString& outputPath);

    void setDataPath(const QString& path);
    bool saveAll();
    bool loadAll();

private:
    bool saveCustomers();
    bool saveBookings();
    bool loadCustomers();
    bool loadBookings();

    QList<Customer> m_customers;
    QList<Booking*> m_bookings;
    MovieManager* m_movieManager;
    QString m_dataPath;
    int m_ticketIdCounter;
};

#endif
