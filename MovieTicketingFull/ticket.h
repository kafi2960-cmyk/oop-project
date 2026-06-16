#ifndef TICKET_H
#define TICKET_H

#include <QString>

class Ticket {
public:
    Ticket();
    Ticket(int id, int seatId, const QString& seatLabel, double basePrice);
    virtual ~Ticket() = default;

    int getTicketId() const;
    int getSeatId() const;
    QString getSeatLabel() const;
    double getBasePrice() const;

    virtual double getPrice() const = 0;
    virtual QString getTypeName() const = 0;

    QString toCsvLine() const;
    static Ticket* fromCsvLine(const QString& line);

protected:
    int m_ticketId, m_seatId;
    QString m_seatLabel;
    double m_basePrice;
};

class StandardTicket : public Ticket {
public:
    StandardTicket();
    StandardTicket(int id, int seatId, const QString& label, double base);
    double getPrice() const override;
    QString getTypeName() const override;
};

class VIPTicket : public Ticket {
public:
    VIPTicket();
    VIPTicket(int id, int seatId, const QString& label, double base);
    double getPrice() const override;
    QString getTypeName() const override;
};

class ChildTicket : public Ticket {
public:
    ChildTicket();
    ChildTicket(int id, int seatId, const QString& label, double base);
    double getPrice() const override;
    QString getTypeName() const override;
};

#endif
