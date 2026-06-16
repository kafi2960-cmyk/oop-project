#ifndef SCREENING_H
#define SCREENING_H

#include <QString>
#include <QDate>
#include <QTime>

class Screening {
public:
    Screening();
    Screening(int id, int movieId, int hallId, const QDate& date, const QTime& time,
              double priceStd, double priceVIP, double priceChild);

    int getId() const;
    int getMovieId() const;
    int getHallId() const;
    QDate getDate() const;
    QTime getShowtime() const;
    double getPriceStandard() const;
    double getPriceVIP() const;
    double getPriceChild() const;

    void setDate(const QDate& d);
    void setShowtime(const QTime& t);
    void setPriceStandard(double p);
    void setPriceVIP(double p);
    void setPriceChild(double p);

    QString toCsvLine() const;
    static Screening fromCsvLine(const QString& line);

private:
    int m_id, m_movieId, m_hallId;
    QDate m_date;
    QTime m_showtime;
    double m_priceStd, m_priceVIP, m_priceChild;
};

#endif
