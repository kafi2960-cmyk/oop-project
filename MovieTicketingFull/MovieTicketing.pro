QT += core gui widgets

CONFIG += c++11

TARGET = MovieTicketing
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    movie.cpp \
    screeninghall.cpp \
    screening.cpp \
    customer.cpp \
    seat.cpp \
    ticket.cpp \
    booking.cpp \
    moviemanager.cpp \
    bookingmanager.cpp \
    loginwindow.cpp \
    registerwindow.cpp

HEADERS += \
    mainwindow.h \
    movie.h \
    screeninghall.h \
    screening.h \
    customer.h \
    seat.h \
    ticket.h \
    booking.h \
    moviemanager.h \
    bookingmanager.h \
    loginwindow.h \
    registerwindow.h
