#include "bookingmanager.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDateTime>

BookingManager::BookingManager(MovieManager* mm) : m_movieManager(mm), m_dataPath("./data"), m_ticketIdCounter(1) {}
BookingManager::~BookingManager() { qDeleteAll(m_bookings); }

void BookingManager::setDataPath(const QString& path) { m_dataPath = path; QDir().mkpath(path); }

bool BookingManager::addCustomer(const Customer& c) {
    for (auto& x : m_customers) if (x.getId() == c.getId()) return false;
    m_customers.append(c); return saveCustomers();
}
bool BookingManager::updateCustomer(const Customer& c) {
    for (auto& x : m_customers) if (x.getId() == c.getId()) { x = c; return saveCustomers(); }
    return false;
}
bool BookingManager::deleteCustomer(int id) {
    for (int i = 0; i < m_customers.size(); ++i) if (m_customers[i].getId() == id) { m_customers.removeAt(i); return saveCustomers(); }
    return false;
}
Customer* BookingManager::findCustomerById(int id) {
    for (auto& c : m_customers) if (c.getId() == id) return &c; return nullptr;
}

Customer* BookingManager::authenticateCustomer(const QString& email, const QString& password) {
    for (auto& c : m_customers) {
        if (c.getEmail().compare(email, Qt::CaseInsensitive) == 0 && c.getPassword() == password) {
            return &c;
        }
    }
    return nullptr;
}

QList<Customer> BookingManager::searchCustomers(const QString& kw) const {
    QList<Customer> r;
    for (auto& c : m_customers)
        if (c.getName().contains(kw, Qt::CaseInsensitive) || QString::number(c.getId()).contains(kw))
            r.append(c);
    return r;
}
QList<Customer> BookingManager::getAllCustomers() const { return m_customers; }
int BookingManager::nextCustomerId() const {
    int max = 0; for (auto& c : m_customers) if (c.getId() > max) max = c.getId(); return max + 1;
}

Booking* BookingManager::createBooking(int customerId, int screeningId) {
    int id = nextBookingId();
    Booking* b = new Booking(id, customerId, screeningId, QDateTime::currentDateTime());
    m_bookings.append(b);
    return b;
}
bool BookingManager::addTicketToBooking(int bookingId, const QString& seatLabel, int seatId, SeatCategory cat) {
    Booking* b = findBookingById(bookingId);
    if (!b) return false;
    Screening* scr = m_movieManager->findScreeningById(b->getScreeningId());
    if (!scr) return false;
    double price = 0;
    if (cat == SeatCategory::Standard) price = scr->getPriceStandard();
    else if (cat == SeatCategory::VIP)  price = scr->getPriceVIP();
    else                                price = scr->getPriceChild();
    Ticket* t = nullptr;
    if (cat == SeatCategory::Standard) t = new StandardTicket(m_ticketIdCounter++, seatId, seatLabel, price);
    else if (cat == SeatCategory::VIP)  t = new VIPTicket(m_ticketIdCounter++, seatId, seatLabel, price);
    else                                t = new ChildTicket(m_ticketIdCounter++, seatId, seatLabel, price);
    b->addTicket(t);
    m_movieManager->bookSeat(b->getScreeningId(), seatId);
    return true;
}
bool BookingManager::finalizeBooking(int bookingId) {
    Booking* b = findBookingById(bookingId);
    if (!b) return false;
    Customer* c = findCustomerById(b->getCustomerId());
    if (c) c->addBookingRef(bookingId);
    saveCustomers(); saveBookings(); return true;
}
bool BookingManager::cancelBooking(int bookingId) {
    Booking* b = findBookingById(bookingId);
    if (!b || b->getStatus() == BookingStatus::Cancelled) return false;
    for (auto* t : b->getTickets())
        m_movieManager->releaseSeat(b->getScreeningId(), t->getSeatId());
    b->cancel();
    Customer* c = findCustomerById(b->getCustomerId());
    if (c) c->removeBookingRef(bookingId);
    saveCustomers(); saveBookings(); return true;
}
Booking* BookingManager::findBookingById(int id) {
    for (auto* b : m_bookings) if (b->getId() == id) return b; return nullptr;
}
QList<Booking*> BookingManager::getBookingsForCustomer(int customerId) const {
    QList<Booking*> r; for (auto* b : m_bookings) if (b->getCustomerId() == customerId) r.append(b); return r;
}
QList<Booking*> BookingManager::getAllBookings() const { return m_bookings; }
int BookingManager::nextBookingId() const {
    int max = 0; for (auto* b : m_bookings) if (b->getId() > max) max = b->getId(); return max + 1;
}
int BookingManager::nextTicketId() const { return m_ticketIdCounter; }

bool BookingManager::generateTicketFile(int bookingId, const QString& movieTitle,
                                         const QString& hallName, const QString& outputPath) {
    Booking* b = findBookingById(bookingId);
    if (!b) return false;
    Screening* scr = m_movieManager->findScreeningById(b->getScreeningId());
    if (!scr) return false;
    QFile f(outputPath + "/ticket_" + b->getReference() + ".txt");
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&f);
    out << "========================================\n";
    out << "         MOVIE TICKET\n";
    out << "========================================\n";
    out << "Booking Ref : " << b->getReference() << "\n";
    out << "Movie       : " << movieTitle << "\n";
    out << "Hall        : " << hallName << "\n";
    out << "Date        : " << scr->getDate().toString("dd MMM yyyy") << "\n";
    out << "Showtime    : " << scr->getShowtime().toString("hh:mm AP") << "\n";
    out << "----------------------------------------\n";
    for (auto* t : b->getTickets())
        out << "Seat " << t->getSeatLabel() << "  [" << t->getTypeName() << "]  $" << QString::number(t->getPrice(), 'f', 2) << "\n";
    out << "----------------------------------------\n";
    out << "TOTAL       : $" << QString::number(b->getTotal(), 'f', 2) << "\n";
    out << "========================================\n";
    return true;
}

bool BookingManager::saveAll() { return saveCustomers() && saveBookings(); }
bool BookingManager::loadAll() { return loadCustomers() && loadBookings(); }

bool BookingManager::saveCustomers() {
    QFile f(m_dataPath + "/customers.csv");
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&f); for (auto& c : m_customers) out << c.toCsvLine() << "\n"; return true;
}
bool BookingManager::saveBookings() {
    QFile f(m_dataPath + "/bookings.csv");
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&f); for (auto* b : m_bookings) out << b->toCsvLine() << "\n"; return true;
}
bool BookingManager::loadCustomers() {
    m_customers.clear();
    QFile f(m_dataPath + "/customers.csv"); if (!f.exists()) return true;
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QTextStream in(&f);
    while (!in.atEnd()) { QString l = in.readLine().trimmed(); if (!l.isEmpty()) m_customers.append(Customer::fromCsvLine(l)); }
    return true;
}
bool BookingManager::loadBookings() {
    qDeleteAll(m_bookings); m_bookings.clear();
    QFile f(m_dataPath + "/bookings.csv"); if (!f.exists()) return true;
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QTextStream in(&f);
    while (!in.atEnd()) {
        QString l = in.readLine().trimmed();
        if (!l.isEmpty()) { Booking* b = Booking::fromCsvLine(l); if (b) m_bookings.append(b); }
    }
    if (!m_bookings.isEmpty()) {
        for (auto* b : m_bookings)
            for (auto* t : b->getTickets())
                if (t->getTicketId() >= m_ticketIdCounter) m_ticketIdCounter = t->getTicketId() + 1;
    }
    return true;
}
