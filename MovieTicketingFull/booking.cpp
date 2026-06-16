#include "booking.h"
#include <QStringList>

Booking::Booking() : m_id(0), m_customerId(0), m_screeningId(0), m_status(BookingStatus::Active) {}

Booking::Booking(int id, int customerId, int screeningId, const QDateTime& dt)
    : m_id(id), m_customerId(customerId), m_screeningId(screeningId),
      m_bookingTime(dt), m_status(BookingStatus::Active) {
    m_reference = QString("BK%1%2").arg(dt.toString("yyyyMMddHHmm")).arg(id, 4, 10, QChar('0'));
}

Booking::~Booking() { qDeleteAll(m_tickets); }

int Booking::getId() const { return m_id; }
int Booking::getCustomerId() const { return m_customerId; }
int Booking::getScreeningId() const { return m_screeningId; }
QString Booking::getReference() const { return m_reference; }
QDateTime Booking::getBookingTime() const { return m_bookingTime; }
BookingStatus Booking::getStatus() const { return m_status; }
const QList<Ticket*>& Booking::getTickets() const { return m_tickets; }

double Booking::getTotal() const {
    double t = 0;
    for (auto* tk : m_tickets) t += tk->getPrice();
    return t;
}

void Booking::addTicket(Ticket* t) { m_tickets.append(t); }
void Booking::cancel() { m_status = BookingStatus::Cancelled; }

QString Booking::statusToString(BookingStatus s) { return s == BookingStatus::Cancelled ? "Cancelled" : "Active"; }
BookingStatus Booking::statusFromString(const QString& s) { return s == "Cancelled" ? BookingStatus::Cancelled : BookingStatus::Active; }

QString Booking::toCsvLine() const {
    QStringList tparts;
    for (auto* t : m_tickets) tparts << t->toCsvLine().replace(",", ":");
    return QString("%1,%2,%3,%4,%5,%6,%7")
        .arg(m_id).arg(m_customerId).arg(m_screeningId)
        .arg(m_reference)
        .arg(m_bookingTime.toString("yyyy-MM-dd HH:mm"))
        .arg(statusToString(m_status))
        .arg(tparts.join(";"));
}

Booking* Booking::fromCsvLine(const QString& line) {
    QStringList p = line.split(",");
    if (p.size() < 7) return nullptr;
    Booking* b = new Booking();
    b->m_id = p[0].toInt();
    b->m_customerId = p[1].toInt();
    b->m_screeningId = p[2].toInt();
    b->m_reference = p[3];
    b->m_bookingTime = QDateTime::fromString(p[4] + "," + p[5], "yyyy-MM-dd HH:mm");
    b->m_status = statusFromString(p[6]);
    if (p.size() > 7 && !p[7].isEmpty()) {
        for (auto& td : p[7].split(";")) {
            if (!td.isEmpty()) {
                Ticket* t = Ticket::fromCsvLine(td.replace(":", ","));
                if (t) b->m_tickets.append(t);
            }
        }
    }
    return b;
}
