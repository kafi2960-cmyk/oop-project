#ifndef SCREENINGHALL_H
#define SCREENINGHALL_H

#include <QString>

class ScreeningHall {
public:
    ScreeningHall();
    ScreeningHall(int id, const QString& name, int rows, int cols);

    int getId() const;
    QString getName() const;
    int getRows() const;
    int getColumns() const;
    int getTotalSeats() const;

    void setName(const QString& n);
    void setRows(int r);
    void setColumns(int c);

    QString toCsvLine() const;
    static ScreeningHall fromCsvLine(const QString& line);

private:
    int m_id, m_rows, m_cols;
    QString m_name;
};

#endif
