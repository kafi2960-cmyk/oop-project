#include "screeninghall.h"
#include <QStringList>

ScreeningHall::ScreeningHall() : m_id(0), m_rows(0), m_cols(0) {}
ScreeningHall::ScreeningHall(int id, const QString& name, int rows, int cols)
    : m_id(id), m_name(name), m_rows(rows), m_cols(cols) {}

int ScreeningHall::getId() const { return m_id; }
QString ScreeningHall::getName() const { return m_name; }
int ScreeningHall::getRows() const { return m_rows; }
int ScreeningHall::getColumns() const { return m_cols; }
int ScreeningHall::getTotalSeats() const { return m_rows * m_cols; }

void ScreeningHall::setName(const QString& n) { m_name = n; }
void ScreeningHall::setRows(int r) { m_rows = r; }
void ScreeningHall::setColumns(int c) { m_cols = c; }

QString ScreeningHall::toCsvLine() const {
    return QString("%1,%2,%3,%4").arg(m_id).arg(m_name).arg(m_rows).arg(m_cols);
}

ScreeningHall ScreeningHall::fromCsvLine(const QString& line) {
    QStringList p = line.split(",");
    if (p.size() < 4) return ScreeningHall();
    return ScreeningHall(p[0].toInt(), p[1], p[2].toInt(), p[3].toInt());
}
