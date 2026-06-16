#include "ticket.h"
#include <QStringList>

Ticket::Ticket() : m_ticketId(0), m_seatId(0), m_basePrice(0) {}
Ticket::Ticket(int id, int seatId, const QString& label, double base)
    : m_ticketId(id), m_seatId(seatId), m_seatLabel(label), m_basePrice(base) {}

int Ticket::getTicketId() const { return m_ticketId; }
int Ticket::getSeatId() const { return m_seatId; }
QString Ticket::getSeatLabel() const { return m_seatLabel; }
double Ticket::getBasePrice() const { return m_basePrice; }

QString Ticket::toCsvLine() const {
    return QString("%1,%2,%3,%4,%5").arg(m_ticketId).arg(m_seatId).arg(m_seatLabel).arg(m_basePrice).arg(getTypeName());
}

Ticket* Ticket::fromCsvLine(const QString& line) {
    QStringList p = line.split(",");
    if (p.size() < 5) return nullptr;
    int tid = p[0].toInt(); int sid = p[1].toInt();
    QString lbl = p[2]; double base = p[3].toDouble(); QString type = p[4];
    if (type == "VIP")   return new VIPTicket(tid, sid, lbl, base);
    if (type == "Child") return new ChildTicket(tid, sid, lbl, base);
    return new StandardTicket(tid, sid, lbl, base);
}

StandardTicket::StandardTicket() {}
StandardTicket::StandardTicket(int id, int sid, const QString& l, double b) : Ticket(id, sid, l, b) {}
double StandardTicket::getPrice() const { return m_basePrice; }
QString StandardTicket::getTypeName() const { return "Standard"; }

VIPTicket::VIPTicket() {}
VIPTicket::VIPTicket(int id, int sid, const QString& l, double b) : Ticket(id, sid, l, b) {}
double VIPTicket::getPrice() const { return m_basePrice * 1.5; }
QString VIPTicket::getTypeName() const { return "VIP"; }

ChildTicket::ChildTicket() {}
ChildTicket::ChildTicket(int id, int sid, const QString& l, double b) : Ticket(id, sid, l, b) {}
double ChildTicket::getPrice() const { return m_basePrice * 0.6; }
QString ChildTicket::getTypeName() const { return "Child"; }
