#include "screening.h"
#include <QStringList>

Screening::Screening() : m_id(0), m_movieId(0), m_hallId(0),
    m_priceStd(0), m_priceVIP(0), m_priceChild(0) {}

Screening::Screening(int id, int movieId, int hallId, const QDate& date, const QTime& time,
                     double priceStd, double priceVIP, double priceChild)
    : m_id(id), m_movieId(movieId), m_hallId(hallId), m_date(date), m_showtime(time),
      m_priceStd(priceStd), m_priceVIP(priceVIP), m_priceChild(priceChild) {}

int Screening::getId() const { return m_id; }
int Screening::getMovieId() const { return m_movieId; }
int Screening::getHallId() const { return m_hallId; }
QDate Screening::getDate() const { return m_date; }
QTime Screening::getShowtime() const { return m_showtime; }
double Screening::getPriceStandard() const { return m_priceStd; }
double Screening::getPriceVIP() const { return m_priceVIP; }
double Screening::getPriceChild() const { return m_priceChild; }

void Screening::setDate(const QDate& d) { m_date = d; }
void Screening::setShowtime(const QTime& t) { m_showtime = t; }
void Screening::setPriceStandard(double p) { m_priceStd = p; }
void Screening::setPriceVIP(double p) { m_priceVIP = p; }
void Screening::setPriceChild(double p) { m_priceChild = p; }

QString Screening::toCsvLine() const {
    return QString("%1,%2,%3,%4,%5,%6,%7,%8")
        .arg(m_id).arg(m_movieId).arg(m_hallId)
        .arg(m_date.toString("yyyy-MM-dd"))
        .arg(m_showtime.toString("HH:mm"))
        .arg(m_priceStd).arg(m_priceVIP).arg(m_priceChild);
}

Screening Screening::fromCsvLine(const QString& line) {
    QStringList p = line.split(",");
    if (p.size() < 8) return Screening();
    return Screening(p[0].toInt(), p[1].toInt(), p[2].toInt(),
        QDate::fromString(p[3], "yyyy-MM-dd"),
        QTime::fromString(p[4], "HH:mm"),
        p[5].toDouble(), p[6].toDouble(), p[7].toDouble());
}
