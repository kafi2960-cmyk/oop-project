#ifndef MOVIEMANAGER_H
#define MOVIEMANAGER_H

#include <QList>
#include <QString>
#include "movie.h"
#include "screeninghall.h"
#include "screening.h"
#include "seat.h"

class MovieManager {
public:
    MovieManager();

    bool addMovie(const Movie& m);
    bool updateMovie(const Movie& m);
    bool removeMovie(int id);
    Movie* findMovieById(int id);
    QList<Movie> searchMovies(const QString& kw, const QString& genre = "", const QString& rating = "") const;
    QList<Movie> getAllMovies() const;
    int nextMovieId() const;

    bool addHall(const ScreeningHall& h);
    bool updateHall(const ScreeningHall& h);
    bool removeHall(int id);
    ScreeningHall* findHallById(int id);
    QList<ScreeningHall> getAllHalls() const;
    int nextHallId() const;

    bool addScreening(const Screening& s);
    bool updateScreening(const Screening& s);
    bool cancelScreening(int id);
    Screening* findScreeningById(int id);
    QList<Screening> getScreeningsByMovie(int movieId) const;
    QList<Screening> getScreeningsByDate(const QDate& date) const;
    QList<Screening> getAllScreenings() const;
    int nextScreeningId() const;

    void initSeatsForScreening(int screeningId, int rows, int cols);
    QList<Seat> getSeatsForScreening(int screeningId) const;
    Seat* findSeat(int screeningId, int seatId);
    bool bookSeat(int screeningId, int seatId);
    bool releaseSeat(int screeningId, int seatId);
    int availableSeatCount(int screeningId) const;

    void setDataPath(const QString& path);
    bool saveAll();
    bool loadAll();

private:
    bool saveMovies(); bool saveHalls(); bool saveScreenings(); bool saveSeats();
    bool loadMovies(); bool loadHalls(); bool loadScreenings(); bool loadSeats();

    QList<Movie> m_movies;
    QList<ScreeningHall> m_halls;
    QList<Screening> m_screenings;
    QList<Seat> m_seats;
    QString m_dataPath;
};

#endif
