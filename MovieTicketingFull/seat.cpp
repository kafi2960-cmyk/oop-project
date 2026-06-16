#include "seat.h"
#include <QStringList>

Seat::Seat() : m_id(0), m_row(0), m_col(0), m_screeningId(0),
    m_category(SeatCategory::Standard), m_status(SeatStatus::Available) {}

Seat::Seat(int id, int row, int col, SeatCategory cat, int screeningId)
    : m_id(id), m_row(row), m_col(col), m_screeningId(screeningId),
      m_category(cat), m_status(SeatStatus::Available) {}

int Seat::getId() const { return m_id; }
int Seat::getRow() const { return m_row; }
int Seat::getCol() const { return m_col; }
SeatCategory Seat::getCategory() const { return m_category; }
SeatStatus Seat::getStatus() const { return m_status; }
int Seat::getScreeningId() const { return m_screeningId; }
bool Seat::isAvailable() const { return m_status == SeatStatus::Available; }
QString Seat::getSeatLabel() const { return QString(QChar('A' + m_row)) + QString::number(m_col + 1); }

void Seat::setCategory(SeatCategory c) { m_category = c; }
void Seat::setStatus(SeatStatus s) { m_status = s; }

QString Seat::categoryToString(SeatCategory c) {
    if (c == SeatCategory::VIP) return "VIP";
    if (c == SeatCategory::Child) return "Child";
    return "Standard";
}
SeatCategory Seat::categoryFromString(const QString& s) {
    if (s == "VIP") return SeatCategory::VIP;
    if (s == "Child") return SeatCategory::Child;
    return SeatCategory::Standard;
}
QString Seat::statusToString(SeatStatus s) { return s == SeatStatus::Booked ? "Booked" : "Available"; }
SeatStatus Seat::statusFromString(const QString& s) { return s == "Booked" ? SeatStatus::Booked : SeatStatus::Available; }

QString Seat::toCsvLine() const {
    return QString("%1,%2,%3,%4,%5,%6")
        .arg(m_id).arg(m_row).arg(m_col)
        .arg(categoryToString(m_category))
        .arg(statusToString(m_status))
        .arg(m_screeningId);
}

Seat Seat::fromCsvLine(const QString& line) {
    QStringList p = line.split(",");
    if (p.size() < 6) return Seat();
    Seat s(p[0].toInt(), p[1].toInt(), p[2].toInt(), categoryFromString(p[3]), p[5].toInt());
    s.m_status = statusFromString(p[4]);
    return s;
}
