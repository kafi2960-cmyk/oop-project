#ifndef MOVIE_H
#define MOVIE_H

#include <QString>

enum class MovieStatus { ComingSoon, NowShowing, Ended };

class Movie {
public:
    Movie();
    Movie(int id, const QString& title, const QString& genre, int duration,
          const QString& rating, const QString& language, const QString& description,
          MovieStatus status = MovieStatus::ComingSoon);

    int getId() const;
    QString getTitle() const;
    QString getGenre() const;
    int getDuration() const;
    QString getRating() const;
    QString getLanguage() const;
    QString getDescription() const;
    MovieStatus getStatus() const;
    QString getStatusString() const;

    void setTitle(const QString& t);
    void setGenre(const QString& g);
    void setDuration(int d);
    void setRating(const QString& r);
    void setLanguage(const QString& l);
    void setDescription(const QString& d);
    void setStatus(MovieStatus s);

    QString toCsvLine() const;
    static Movie fromCsvLine(const QString& line);
    static QString statusToString(MovieStatus s);
    static MovieStatus statusFromString(const QString& s);

private:
    int m_id;
    QString m_title, m_genre, m_rating, m_language, m_description;
    int m_duration;
    MovieStatus m_status;
};

#endif
