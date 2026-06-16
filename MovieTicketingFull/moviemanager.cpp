#include "moviemanager.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <algorithm>

MovieManager::MovieManager() : m_dataPath("./data") {}

void MovieManager::setDataPath(const QString& path) { m_dataPath = path; QDir().mkpath(path); }

bool MovieManager::addMovie(const Movie& m) {
    for (auto& x : m_movies) if (x.getId() == m.getId()) return false;
    m_movies.append(m); return saveMovies();
}
bool MovieManager::updateMovie(const Movie& m) {
    for (auto& x : m_movies) if (x.getId() == m.getId()) { x = m; return saveMovies(); }
    return false;
}
bool MovieManager::removeMovie(int id) {
    for (int i = 0; i < m_movies.size(); ++i) if (m_movies[i].getId() == id) { m_movies.removeAt(i); return saveMovies(); }
    return false;
}
Movie* MovieManager::findMovieById(int id) {
    for (auto& m : m_movies) if (m.getId() == id) return &m;
    return nullptr;
}
QList<Movie> MovieManager::searchMovies(const QString& kw, const QString& genre, const QString& rating) const {
    QList<Movie> r;
    for (auto& m : m_movies) {
        bool k = kw.isEmpty() || m.getTitle().contains(kw, Qt::CaseInsensitive);
        bool g = genre.isEmpty() || m.getGenre() == genre;
        bool rt = rating.isEmpty() || m.getRating() == rating;
        if (k && g && rt) r.append(m);
    }
    return r;
}
QList<Movie> MovieManager::getAllMovies() const { return m_movies; }
int MovieManager::nextMovieId() const {
    int max = 0; for (auto& m : m_movies) if (m.getId() > max) max = m.getId(); return max + 1;
}

bool MovieManager::addHall(const ScreeningHall& h) {
    for (auto& x : m_halls) if (x.getId() == h.getId()) return false;
    m_halls.append(h); return saveHalls();
}
bool MovieManager::updateHall(const ScreeningHall& h) {
    for (auto& x : m_halls) if (x.getId() == h.getId()) { x = h; return saveHalls(); } return false;
}
bool MovieManager::removeHall(int id) {
    for (int i = 0; i < m_halls.size(); ++i) if (m_halls[i].getId() == id) { m_halls.removeAt(i); return saveHalls(); }
    return false;
}
ScreeningHall* MovieManager::findHallById(int id) {
    for (auto& h : m_halls) if (h.getId() == id) return &h; return nullptr;
}
QList<ScreeningHall> MovieManager::getAllHalls() const { return m_halls; }
int MovieManager::nextHallId() const {
    int max = 0; for (auto& h : m_halls) if (h.getId() > max) max = h.getId(); return max + 1;
}

bool MovieManager::addScreening(const Screening& s) {
    for (auto& x : m_screenings) if (x.getId() == s.getId()) return false;
    m_screenings.append(s);
    auto* hall = findHallById(s.getHallId());
    if (hall) initSeatsForScreening(s.getId(), hall->getRows(), hall->getColumns());
    return saveScreenings();
}
bool MovieManager::updateScreening(const Screening& s) {
    for (auto& x : m_screenings) if (x.getId() == s.getId()) { x = s; return saveScreenings(); } return false;
}
bool MovieManager::cancelScreening(int id) {
    m_seats.erase(std::remove_if(m_seats.begin(), m_seats.end(),
        [id](const Seat& s){ return s.getScreeningId() == id; }), m_seats.end());
    for (int i = 0; i < m_screenings.size(); ++i) if (m_screenings[i].getId() == id) {
        m_screenings.removeAt(i); saveScreenings(); saveSeats(); return true;
    }
    return false;
}
Screening* MovieManager::findScreeningById(int id) {
    for (auto& s : m_screenings) if (s.getId() == id) return &s; return nullptr;
}
QList<Screening> MovieManager::getScreeningsByMovie(int movieId) const {
    QList<Screening> r; for (auto& s : m_screenings) if (s.getMovieId() == movieId) r.append(s); return r;
}
QList<Screening> MovieManager::getScreeningsByDate(const QDate& date) const {
    QList<Screening> r; for (auto& s : m_screenings) if (s.getDate() == date) r.append(s); return r;
}
QList<Screening> MovieManager::getAllScreenings() const { return m_screenings; }
int MovieManager::nextScreeningId() const {
    int max = 0; for (auto& s : m_screenings) if (s.getId() > max) max = s.getId(); return max + 1;
}

void MovieManager::initSeatsForScreening(int screeningId, int rows, int cols) {
    int sid = 1;
    for (auto& s : m_seats) if (s.getId() >= sid) sid = s.getId() + 1;
    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
            m_seats.append(Seat(sid++, r, c, SeatCategory::Standard, screeningId));
    saveSeats();
}
QList<Seat> MovieManager::getSeatsForScreening(int screeningId) const {
    QList<Seat> r; for (auto& s : m_seats) if (s.getScreeningId() == screeningId) r.append(s); return r;
}
Seat* MovieManager::findSeat(int screeningId, int seatId) {
    for (auto& s : m_seats) if (s.getScreeningId() == screeningId && s.getId() == seatId) return &s;
    return nullptr;
}
bool MovieManager::bookSeat(int screeningId, int seatId) {
    Seat* s = findSeat(screeningId, seatId);
    if (!s || !s->isAvailable()) return false;
    s->setStatus(SeatStatus::Booked); return saveSeats();
}
bool MovieManager::releaseSeat(int screeningId, int seatId) {
    Seat* s = findSeat(screeningId, seatId);
    if (!s) return false;
    s->setStatus(SeatStatus::Available); return saveSeats();
}
int MovieManager::availableSeatCount(int screeningId) const {
    int c = 0; for (auto& s : m_seats) if (s.getScreeningId() == screeningId && s.isAvailable()) c++; return c;
}

bool MovieManager::saveAll() { return saveMovies() && saveHalls() && saveScreenings() && saveSeats(); }
bool MovieManager::loadAll() { return loadMovies() && loadHalls() && loadScreenings() && loadSeats(); }

bool MovieManager::saveMovies() {
    QFile f(m_dataPath + "/movies.csv");
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&f); for (auto& m : m_movies) out << m.toCsvLine() << "\n"; return true;
}
bool MovieManager::saveHalls() {
    QFile f(m_dataPath + "/halls.csv");
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&f); for (auto& h : m_halls) out << h.toCsvLine() << "\n"; return true;
}
bool MovieManager::saveScreenings() {
    QFile f(m_dataPath + "/screenings.csv");
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&f); for (auto& s : m_screenings) out << s.toCsvLine() << "\n"; return true;
}
bool MovieManager::saveSeats() {
    QFile f(m_dataPath + "/seats.csv");
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&f); for (auto& s : m_seats) out << s.toCsvLine() << "\n"; return true;
}
bool MovieManager::loadMovies() {
    m_movies.clear();
    QFile f(m_dataPath + "/movies.csv"); if (!f.exists()) return true;
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QTextStream in(&f);
    while (!in.atEnd()) { QString l = in.readLine().trimmed(); if (!l.isEmpty()) m_movies.append(Movie::fromCsvLine(l)); }
    return true;
}
bool MovieManager::loadHalls() {
    m_halls.clear();
    QFile f(m_dataPath + "/halls.csv"); if (!f.exists()) return true;
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QTextStream in(&f);
    while (!in.atEnd()) { QString l = in.readLine().trimmed(); if (!l.isEmpty()) m_halls.append(ScreeningHall::fromCsvLine(l)); }
    return true;
}
bool MovieManager::loadScreenings() {
    m_screenings.clear();
    QFile f(m_dataPath + "/screenings.csv"); if (!f.exists()) return true;
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QTextStream in(&f);
    while (!in.atEnd()) { QString l = in.readLine().trimmed(); if (!l.isEmpty()) m_screenings.append(Screening::fromCsvLine(l)); }
    return true;
}
bool MovieManager::loadSeats() {
    m_seats.clear();
    QFile f(m_dataPath + "/seats.csv"); if (!f.exists()) return true;
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QTextStream in(&f);
    while (!in.atEnd()) { QString l = in.readLine().trimmed(); if (!l.isEmpty()) m_seats.append(Seat::fromCsvLine(l)); }
    return true;
}
