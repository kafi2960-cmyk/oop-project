#ifndef SEAT_H
#define SEAT_H

#include <QString>

enum class SeatCategory { Standard, VIP, Child };
enum class SeatStatus   { Available, Booked };

class Seat {
public:
    Seat();
    Seat(int id, int row, int col, SeatCategory cat, int screeningId);

    int getId() const;
    int getRow() const;
    int getCol() const;
    SeatCategory getCategory() const;
    SeatStatus getStatus() const;
    int getScreeningId() const;
    QString getSeatLabel() const;
    bool isAvailable() const;

    void setCategory(SeatCategory c);
    void setStatus(SeatStatus s);

    QString toCsvLine() const;
    static Seat fromCsvLine(const QString& line);
    static QString categoryToString(SeatCategory c);
    static SeatCategory categoryFromString(const QString& s);
    static QString statusToString(SeatStatus s);
    static SeatStatus statusFromString(const QString& s);

private:
    int m_id, m_row, m_col, m_screeningId;
    SeatCategory m_category;
    SeatStatus m_status;
};

#endif
