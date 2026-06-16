#include "movie.h"
#include <QStringList>

Movie::Movie() : m_id(0), m_duration(0), m_status(MovieStatus::ComingSoon) {}

Movie::Movie(int id, const QString& title, const QString& genre, int duration,
             const QString& rating, const QString& language, const QString& description,
             MovieStatus status)
    : m_id(id), m_title(title), m_genre(genre), m_rating(rating),
      m_language(language), m_description(description),
      m_duration(duration), m_status(status) {}

int Movie::getId() const { return m_id; }
QString Movie::getTitle() const { return m_title; }
QString Movie::getGenre() const { return m_genre; }
int Movie::getDuration() const { return m_duration; }
QString Movie::getRating() const { return m_rating; }
QString Movie::getLanguage() const { return m_language; }
QString Movie::getDescription() const { return m_description; }
MovieStatus Movie::getStatus() const { return m_status; }
QString Movie::getStatusString() const { return statusToString(m_status); }

void Movie::setTitle(const QString& t) { m_title = t; }
void Movie::setGenre(const QString& g) { m_genre = g; }
void Movie::setDuration(int d) { m_duration = d; }
void Movie::setRating(const QString& r) { m_rating = r; }
void Movie::setLanguage(const QString& l) { m_language = l; }
void Movie::setDescription(const QString& d) { m_description = d; }
void Movie::setStatus(MovieStatus s) { m_status = s; }

QString Movie::statusToString(MovieStatus s) {
    if (s == MovieStatus::NowShowing) return "NowShowing";
    if (s == MovieStatus::Ended) return "Ended";
    return "ComingSoon";
}

MovieStatus Movie::statusFromString(const QString& s) {
    if (s == "NowShowing") return MovieStatus::NowShowing;
    if (s == "Ended") return MovieStatus::Ended;
    return MovieStatus::ComingSoon;
}

QString Movie::toCsvLine() const {
    QStringList p;
    p << QString::number(m_id) << m_title << m_genre
      << QString::number(m_duration) << m_rating << m_language
      << m_description << statusToString(m_status);
    for (auto& x : p) x.replace(",", ";;");
    return p.join(",");
}

Movie Movie::fromCsvLine(const QString& line) {
    QStringList p = line.split(",");
    if (p.size() < 8) return Movie();
    for (auto& x : p) x.replace(";;", ",");
    Movie m;
    m.m_id = p[0].toInt();
    m.m_title = p[1]; m.m_genre = p[2];
    m.m_duration = p[3].toInt();
    m.m_rating = p[4]; m.m_language = p[5];
    m.m_description = p[6];
    m.m_status = statusFromString(p[7]);
    return m;
}
