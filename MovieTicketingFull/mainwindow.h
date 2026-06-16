#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QDateEdit>
#include <QTimeEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTextEdit>
#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QScrollArea>

#include "moviemanager.h"
#include "bookingmanager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void setUserRole(bool isAdmin, int customerId);
    BookingManager* getBookingManager() { return &m_bookingMgr; }

private slots:
    void onAddMovie();
    void onUpdateMovie();
    void onRemoveMovie();
    void onSearchMovies();
    void onMovieTableClicked(int row, int col);

    void onAddHall();
    void onRemoveHall();
    void onHallTableClicked(int row, int col);

    void onAddScreening();
    void onCancelScreening();
    void onScreeningTableClicked(int row, int col);

    void onAddCustomer();
    void onUpdateCustomer();
    void onDeleteCustomer();
    void onSearchCustomers();
    void onCustomerTableClicked(int row, int col);

    void onLoadSeats();
    void onSeatSelected(int row, int col);
    void onConfirmBooking();
    void onCancelBooking();

    void onBookingTableClicked(int row, int col);
    void onPrintTicket();
    void onLoadAllBookings();
    
    void onLogoutClicked();

private:
    void setupUI();
    QWidget* buildMovieTab();
    QWidget* buildHallTab();
    QWidget* buildScreeningTab();
    QWidget* buildCustomerTab();
    QWidget* buildBookingTab();
    QWidget* buildHistoryTab();

    void refreshMovieTable();
    void refreshHallTable();
    void refreshScreeningTable();
    void refreshCustomerTable();
    void refreshBookingTable();
    void refreshSeatMap(int screeningId);
    void clearMovieForm();
    void clearHallForm();
    void clearScreeningForm();
    void clearCustomerForm();
    void populateMovieForm(const Movie& m);
    void populateHallForm(const ScreeningHall& h);
    void populateCustomerForm(const Customer& c);
    void populateScreeningCombos();

    MovieManager m_movieMgr;
    BookingManager m_bookingMgr;

    int m_selectedMovieId = -1;
    int m_selectedHallId  = -1;
    int m_selectedScreeningId = -1;
    int m_selectedCustomerId  = -1;
    int m_currentBookingId    = -1;
    QList<int> m_selectedSeatIds;

    bool m_isAdmin = true;
    int m_loggedInCustomerId = -1;

    QWidget* m_movieAdminPanel;
    QWidget* m_bookCustomerSelectionWidget;

    QTabWidget* m_tabs;
    QWidget *m_movieTabW, *m_hallTabW, *m_screeningTabW, *m_customerTabW, *m_bookingTabW, *m_historyTabW;
    QPushButton* m_logoutBtn;

    QTableWidget* m_movieTable;
    QLineEdit *m_movieTitle, *m_movieGenre, *m_movieRating, *m_movieLang;
    QSpinBox* m_movieDuration;
    QTextEdit* m_movieDesc;
    QComboBox* m_movieStatus;

    QTableWidget* m_hallTable;
    QLineEdit* m_hallName;
    QSpinBox *m_hallRows, *m_hallCols;

    QTableWidget* m_screeningTable;
    QComboBox *m_scrMovieCombo, *m_scrHallCombo;
    QDateEdit* m_scrDate;
    QTimeEdit* m_scrTime;
    QDoubleSpinBox *m_scrPriceStd, *m_scrPriceVIP, *m_scrPriceChild;

    QTableWidget* m_customerTable;
    QLineEdit *m_custName, *m_custContact, *m_custEmail, *m_custSearch;

    QTableWidget* m_seatTable;
    QComboBox *m_bookScrCombo, *m_bookCustCombo;
    QLabel* m_bookTotalLabel;
    QLabel* m_bookRefLabel;
    QComboBox* m_seatCatCombo;

    QTableWidget* m_bookingTable;
    QLabel* m_histDetailLabel;
};

#endif
