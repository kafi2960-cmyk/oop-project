#include "mainwindow.h"
#include "loginwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QHeaderView>
#include <QFormLayout>
#include <QSplitter>
#include <QApplication>
#include <QStyle>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_bookingMgr(&m_movieMgr) {

    QString dataPath = QApplication::applicationDirPath() + "/data";
    m_movieMgr.setDataPath(dataPath);
    m_bookingMgr.setDataPath(dataPath);
    m_movieMgr.loadAll();
    m_bookingMgr.loadAll();

    setupUI();
    setWindowTitle("Movie Ticketing Management System");
    setMinimumSize(1100, 720);
    refreshMovieTable();
    refreshHallTable();
    refreshScreeningTable();
    refreshCustomerTable();
    refreshBookingTable();
    refreshBookingTable();
    populateScreeningCombos();
}

void MainWindow::setUserRole(bool isAdmin, int customerId) {
    m_isAdmin = isAdmin;
    m_loggedInCustomerId = customerId;

    m_tabs->clear();
    m_tabs->addTab(m_movieTabW, "Movies");
    if (m_isAdmin) {
        m_tabs->addTab(m_hallTabW, "Halls");
        m_tabs->addTab(m_screeningTabW, "Screenings");
        m_tabs->addTab(m_customerTabW, "Customers");
    }
    m_tabs->addTab(m_bookingTabW, "Book Tickets");
    m_tabs->addTab(m_historyTabW, "Booking History");

    if (!m_isAdmin) {
        if (m_movieAdminPanel) m_movieAdminPanel->hide();
        if (m_bookCustomerSelectionWidget) m_bookCustomerSelectionWidget->hide();

        // Lock the customer combobox to the logged in user
        int index = m_bookCustCombo->findData(m_loggedInCustomerId);
        if (index >= 0) m_bookCustCombo->setCurrentIndex(index);
    } else {
        if (m_movieAdminPanel) m_movieAdminPanel->show();
        if (m_bookCustomerSelectionWidget) m_bookCustomerSelectionWidget->show();
    }

    refreshBookingTable();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    QWidget* central = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(5,5,5,5);

    QHBoxLayout* topBar = new QHBoxLayout;
    topBar->addStretch();
    m_logoutBtn = new QPushButton("Logout", this);
    m_logoutBtn->setStyleSheet("background-color: #e74c3c; color: white; font-weight: bold; padding: 5px 15px;");
    topBar->addWidget(m_logoutBtn);
    mainLayout->addLayout(topBar);

    m_tabs = new QTabWidget(this);
    m_movieTabW = buildMovieTab();
    m_hallTabW = buildHallTab();
    m_screeningTabW = buildScreeningTab();
    m_customerTabW = buildCustomerTab();
    m_bookingTabW = buildBookingTab();
    m_historyTabW = buildHistoryTab();

    mainLayout->addWidget(m_tabs);
    setCentralWidget(central);

    connect(m_logoutBtn, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
}

void MainWindow::onLogoutClicked() {
    this->hide();
    LoginWindow login(&m_bookingMgr, this);
    if (login.exec() == QDialog::Accepted) {
        setUserRole(login.isAdmin(), login.getLoggedInCustomerId());
        this->show();
    } else {
        this->close();
    }
}

QWidget* MainWindow::buildMovieTab() {
    QWidget* w = new QWidget;
    QHBoxLayout* main = new QHBoxLayout(w);

    QGroupBox* formBox = new QGroupBox("Movie Details");
    QFormLayout* form = new QFormLayout(formBox);
    m_movieTitle    = new QLineEdit; form->addRow("Title:",       m_movieTitle);
    m_movieGenre    = new QLineEdit; form->addRow("Genre:",       m_movieGenre);
    m_movieDuration = new QSpinBox; m_movieDuration->setRange(1,300); m_movieDuration->setSuffix(" min");
    form->addRow("Duration:", m_movieDuration);
    m_movieRating = new QLineEdit; form->addRow("Rating:",    m_movieRating);
    m_movieLang   = new QLineEdit; form->addRow("Language:",  m_movieLang);
    m_movieDesc   = new QTextEdit; m_movieDesc->setMaximumHeight(80);
    form->addRow("Description:", m_movieDesc);
    m_movieStatus = new QComboBox;
    m_movieStatus->addItems({"ComingSoon","NowShowing","Ended"});
    form->addRow("Status:", m_movieStatus);

    QHBoxLayout* btns = new QHBoxLayout;
    QPushButton *btnAdd = new QPushButton("Add"),
                *btnUpd = new QPushButton("Update"),
                *btnDel = new QPushButton("Remove"),
                *btnClr = new QPushButton("Clear");
    btns->addWidget(btnAdd); btns->addWidget(btnUpd);
    btns->addWidget(btnDel); btns->addWidget(btnClr);
    QVBoxLayout* leftLayout = new QVBoxLayout;
    leftLayout->addWidget(formBox);
    leftLayout->addLayout(btns);

    QGroupBox* tableBox = new QGroupBox("Movies");
    QVBoxLayout* tableLayout = new QVBoxLayout(tableBox);
    QHBoxLayout* searchRow = new QHBoxLayout;
    QLineEdit* searchEdit = new QLineEdit; searchEdit->setPlaceholderText("Search title/genre/rating...");
    QPushButton* btnSearch = new QPushButton("Search");
    QPushButton* btnAll    = new QPushButton("Show All");
    searchRow->addWidget(searchEdit); searchRow->addWidget(btnSearch); searchRow->addWidget(btnAll);
    m_movieTable = new QTableWidget(0,8);
    m_movieTable->setHorizontalHeaderLabels({"ID","Title","Genre","Duration","Rating","Language","Status","Description"});
    m_movieTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_movieTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_movieTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableLayout->addLayout(searchRow);
    tableLayout->addWidget(m_movieTable);

    m_movieAdminPanel = new QWidget; m_movieAdminPanel->setLayout(leftLayout); m_movieAdminPanel->setMaximumWidth(320);
    main->addWidget(m_movieAdminPanel);
    main->addWidget(tableBox);

    connect(btnAdd,    &QPushButton::clicked, this, &MainWindow::onAddMovie);
    connect(btnUpd,    &QPushButton::clicked, this, &MainWindow::onUpdateMovie);
    connect(btnDel,    &QPushButton::clicked, this, &MainWindow::onRemoveMovie);
    connect(btnClr,    &QPushButton::clicked, this, &MainWindow::clearMovieForm);
    connect(btnSearch, &QPushButton::clicked, this, &MainWindow::onSearchMovies);
    connect(btnAll,    &QPushButton::clicked, this, &MainWindow::refreshMovieTable);
    connect(m_movieTable, &QTableWidget::cellClicked, this, &MainWindow::onMovieTableClicked);
    return w;
}

QWidget* MainWindow::buildHallTab() {
    QWidget* w = new QWidget;
    QHBoxLayout* main = new QHBoxLayout(w);

    QGroupBox* formBox = new QGroupBox("Hall Details");
    QFormLayout* form = new QFormLayout(formBox);
    m_hallName = new QLineEdit; form->addRow("Name:", m_hallName);
    m_hallRows = new QSpinBox; m_hallRows->setRange(1,26); form->addRow("Rows:", m_hallRows);
    m_hallCols = new QSpinBox; m_hallCols->setRange(1,30); form->addRow("Columns:", m_hallCols);

    QHBoxLayout* btns = new QHBoxLayout;
    QPushButton *btnAdd = new QPushButton("Add"), *btnDel = new QPushButton("Remove");
    btns->addWidget(btnAdd); btns->addWidget(btnDel);
    QVBoxLayout* leftLayout = new QVBoxLayout;
    leftLayout->addWidget(formBox); leftLayout->addLayout(btns); leftLayout->addStretch();

    QGroupBox* tableBox = new QGroupBox("Screening Halls");
    QVBoxLayout* tl = new QVBoxLayout(tableBox);
    m_hallTable = new QTableWidget(0,4);
    m_hallTable->setHorizontalHeaderLabels({"ID","Name","Rows","Columns"});
    m_hallTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_hallTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_hallTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tl->addWidget(m_hallTable);

    QWidget* left = new QWidget; left->setLayout(leftLayout); left->setMaximumWidth(280);
    main->addWidget(left); main->addWidget(tableBox);

    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::onAddHall);
    connect(btnDel, &QPushButton::clicked, this, &MainWindow::onRemoveHall);
    connect(m_hallTable, &QTableWidget::cellClicked, this, &MainWindow::onHallTableClicked);
    return w;
}

QWidget* MainWindow::buildScreeningTab() {
    QWidget* w = new QWidget;
    QHBoxLayout* main = new QHBoxLayout(w);

    QGroupBox* formBox = new QGroupBox("Screening Details");
    QFormLayout* form = new QFormLayout(formBox);
    m_scrMovieCombo = new QComboBox; form->addRow("Movie:",    m_scrMovieCombo);
    m_scrHallCombo  = new QComboBox; form->addRow("Hall:",     m_scrHallCombo);
    m_scrDate = new QDateEdit(QDate::currentDate()); m_scrDate->setCalendarPopup(true);
    form->addRow("Date:", m_scrDate);
    m_scrTime = new QTimeEdit(QTime(18,0)); form->addRow("Showtime:", m_scrTime);
    m_scrPriceStd   = new QDoubleSpinBox; m_scrPriceStd->setRange(0,9999); m_scrPriceStd->setPrefix("$");
    m_scrPriceVIP   = new QDoubleSpinBox; m_scrPriceVIP->setRange(0,9999); m_scrPriceVIP->setPrefix("$");
    m_scrPriceChild = new QDoubleSpinBox; m_scrPriceChild->setRange(0,9999); m_scrPriceChild->setPrefix("$");
    form->addRow("Price Standard:", m_scrPriceStd);
    form->addRow("Price VIP:",      m_scrPriceVIP);
    form->addRow("Price Child:",    m_scrPriceChild);

    QHBoxLayout* btns = new QHBoxLayout;
    QPushButton *btnAdd = new QPushButton("Add Screening"), *btnCancel = new QPushButton("Cancel Screening");
    btns->addWidget(btnAdd); btns->addWidget(btnCancel);
    QVBoxLayout* leftLayout = new QVBoxLayout;
    leftLayout->addWidget(formBox); leftLayout->addLayout(btns); leftLayout->addStretch();

    QGroupBox* tableBox = new QGroupBox("Screenings");
    QVBoxLayout* tl = new QVBoxLayout(tableBox);
    m_screeningTable = new QTableWidget(0,8);
    m_screeningTable->setHorizontalHeaderLabels({"ID","Movie","Hall","Date","Time","Std Price","VIP Price","Child Price"});
    m_screeningTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_screeningTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_screeningTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tl->addWidget(m_screeningTable);

    QWidget* left = new QWidget; left->setLayout(leftLayout); left->setMaximumWidth(300);
    main->addWidget(left); main->addWidget(tableBox);

    connect(btnAdd,    &QPushButton::clicked, this, &MainWindow::onAddScreening);
    connect(btnCancel, &QPushButton::clicked, this, &MainWindow::onCancelScreening);
    connect(m_screeningTable, &QTableWidget::cellClicked, this, &MainWindow::onScreeningTableClicked);
    return w;
}

QWidget* MainWindow::buildCustomerTab() {
    QWidget* w = new QWidget;
    QHBoxLayout* main = new QHBoxLayout(w);

    QGroupBox* formBox = new QGroupBox("Customer Details");
    QFormLayout* form = new QFormLayout(formBox);
    m_custName    = new QLineEdit; form->addRow("Name:",    m_custName);
    m_custContact = new QLineEdit; form->addRow("Contact:", m_custContact);
    m_custEmail   = new QLineEdit; form->addRow("Email:",   m_custEmail);

    QHBoxLayout* btns = new QHBoxLayout;
    QPushButton *btnAdd = new QPushButton("Add"), *btnUpd = new QPushButton("Update"),
                *btnDel = new QPushButton("Delete"), *btnClr = new QPushButton("Clear");
    btns->addWidget(btnAdd); btns->addWidget(btnUpd); btns->addWidget(btnDel); btns->addWidget(btnClr);
    QVBoxLayout* leftLayout = new QVBoxLayout;
    leftLayout->addWidget(formBox); leftLayout->addLayout(btns); leftLayout->addStretch();

    QGroupBox* tableBox = new QGroupBox("Customers");
    QVBoxLayout* tl = new QVBoxLayout(tableBox);
    QHBoxLayout* searchRow = new QHBoxLayout;
    m_custSearch = new QLineEdit; m_custSearch->setPlaceholderText("Search by name or ID...");
    QPushButton *btnSearch = new QPushButton("Search"), *btnAll = new QPushButton("Show All");
    searchRow->addWidget(m_custSearch); searchRow->addWidget(btnSearch); searchRow->addWidget(btnAll);
    m_customerTable = new QTableWidget(0,4);
    m_customerTable->setHorizontalHeaderLabels({"ID","Name","Contact","Email"});
    m_customerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_customerTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_customerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tl->addLayout(searchRow); tl->addWidget(m_customerTable);

    QWidget* left = new QWidget; left->setLayout(leftLayout); left->setMaximumWidth(280);
    main->addWidget(left); main->addWidget(tableBox);

    connect(btnAdd,    &QPushButton::clicked, this, &MainWindow::onAddCustomer);
    connect(btnUpd,    &QPushButton::clicked, this, &MainWindow::onUpdateCustomer);
    connect(btnDel,    &QPushButton::clicked, this, &MainWindow::onDeleteCustomer);
    connect(btnClr,    &QPushButton::clicked, this, &MainWindow::clearCustomerForm);
    connect(btnSearch, &QPushButton::clicked, this, &MainWindow::onSearchCustomers);
    connect(btnAll,    &QPushButton::clicked, this, &MainWindow::refreshCustomerTable);
    connect(m_customerTable, &QTableWidget::cellClicked, this, &MainWindow::onCustomerTableClicked);
    return w;
}

QWidget* MainWindow::buildBookingTab() {
    QWidget* w = new QWidget;
    QVBoxLayout* main = new QVBoxLayout(w);

    QGroupBox* selBox = new QGroupBox("Select Screening & Customer");
    QHBoxLayout* selLayout = new QHBoxLayout(selBox);
    m_bookScrCombo  = new QComboBox; m_bookScrCombo->setMinimumWidth(280);
    m_bookCustCombo = new QComboBox; m_bookCustCombo->setMinimumWidth(200);
    m_seatCatCombo  = new QComboBox;
    m_seatCatCombo->addItems({"Standard","VIP","Child"});
    QPushButton* btnLoad = new QPushButton("Load Seat Map");
    selLayout->addWidget(new QLabel("Screening:")); selLayout->addWidget(m_bookScrCombo);
    
    m_bookCustomerSelectionWidget = new QWidget;
    QHBoxLayout* custLayout = new QHBoxLayout(m_bookCustomerSelectionWidget);
    custLayout->setContentsMargins(0,0,0,0);
    custLayout->addWidget(new QLabel("Customer:"));
    custLayout->addWidget(m_bookCustCombo);
    selLayout->addWidget(m_bookCustomerSelectionWidget);

    selLayout->addWidget(new QLabel("Category:")); selLayout->addWidget(m_seatCatCombo);
    selLayout->addWidget(btnLoad);

    QGroupBox* seatBox = new QGroupBox("Seat Map  (click to select/deselect — Green=Available, Red=Booked, Blue=Selected)");
    QVBoxLayout* seatLayout = new QVBoxLayout(seatBox);
    m_seatTable = new QTableWidget(0,0);
    m_seatTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_seatTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_seatTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    seatLayout->addWidget(m_seatTable);

    QHBoxLayout* bookRow = new QHBoxLayout;
    m_bookTotalLabel = new QLabel("Total: $0.00");
    m_bookTotalLabel->setStyleSheet("font-weight:bold; font-size:14px;");
    m_bookRefLabel   = new QLabel("Reference: —");
    QPushButton *btnConfirm = new QPushButton("Confirm Booking"),
                *btnCancel  = new QPushButton("Cancel Booking");
    btnConfirm->setStyleSheet("background-color:#27ae60;color:white;font-weight:bold;");
    btnCancel->setStyleSheet("background-color:#c0392b;color:white;font-weight:bold;");
    bookRow->addWidget(m_bookTotalLabel); bookRow->addWidget(m_bookRefLabel);
    bookRow->addStretch();
    bookRow->addWidget(btnConfirm); bookRow->addWidget(btnCancel);

    main->addWidget(selBox); main->addWidget(seatBox); main->addLayout(bookRow);

    connect(btnLoad,    &QPushButton::clicked, this, &MainWindow::onLoadSeats);
    connect(btnConfirm, &QPushButton::clicked, this, &MainWindow::onConfirmBooking);
    connect(btnCancel,  &QPushButton::clicked, this, &MainWindow::onCancelBooking);
    connect(m_seatTable, &QTableWidget::cellClicked, this, &MainWindow::onSeatSelected);
    return w;
}

QWidget* MainWindow::buildHistoryTab() {
    QWidget* w = new QWidget;
    QVBoxLayout* main = new QVBoxLayout(w);

    QHBoxLayout* topRow = new QHBoxLayout;
    QPushButton *btnRefresh = new QPushButton("Refresh"), *btnPrint = new QPushButton("Print/Save Ticket");
    btnPrint->setStyleSheet("background-color:#2980b9;color:white;font-weight:bold;");
    topRow->addWidget(btnRefresh); topRow->addWidget(btnPrint); topRow->addStretch();

    m_bookingTable = new QTableWidget(0,6);
    m_bookingTable->setHorizontalHeaderLabels({"ID","Reference","Customer","Screening","Total","Status"});
    m_bookingTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_bookingTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_bookingTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_histDetailLabel = new QLabel("Select a booking to see ticket details.");
    m_histDetailLabel->setWordWrap(true);
    m_histDetailLabel->setStyleSheet("padding:8px; background:#f9f9f9; border:1px solid #ccc;");

    main->addLayout(topRow);
    main->addWidget(m_bookingTable);
    main->addWidget(m_histDetailLabel);

    connect(btnRefresh, &QPushButton::clicked, this, &MainWindow::onLoadAllBookings);
    connect(btnPrint,   &QPushButton::clicked, this, &MainWindow::onPrintTicket);
    connect(m_bookingTable, &QTableWidget::cellClicked, this, &MainWindow::onBookingTableClicked);
    return w;
}

void MainWindow::refreshMovieTable() {
    auto movies = m_movieMgr.getAllMovies();
    m_movieTable->setRowCount(0);
    for (auto& m : movies) {
        int row = m_movieTable->rowCount();
        m_movieTable->insertRow(row);
        m_movieTable->setItem(row,0,new QTableWidgetItem(QString::number(m.getId())));
        m_movieTable->setItem(row,1,new QTableWidgetItem(m.getTitle()));
        m_movieTable->setItem(row,2,new QTableWidgetItem(m.getGenre()));
        m_movieTable->setItem(row,3,new QTableWidgetItem(QString::number(m.getDuration())+" min"));
        m_movieTable->setItem(row,4,new QTableWidgetItem(m.getRating()));
        m_movieTable->setItem(row,5,new QTableWidgetItem(m.getLanguage()));
        m_movieTable->setItem(row,6,new QTableWidgetItem(m.getStatusString()));
        m_movieTable->setItem(row,7,new QTableWidgetItem(m.getDescription()));
    }
    populateScreeningCombos();
}

void MainWindow::refreshHallTable() {
    auto halls = m_movieMgr.getAllHalls();
    m_hallTable->setRowCount(0);
    for (auto& h : halls) {
        int row = m_hallTable->rowCount();
        m_hallTable->insertRow(row);
        m_hallTable->setItem(row,0,new QTableWidgetItem(QString::number(h.getId())));
        m_hallTable->setItem(row,1,new QTableWidgetItem(h.getName()));
        m_hallTable->setItem(row,2,new QTableWidgetItem(QString::number(h.getRows())));
        m_hallTable->setItem(row,3,new QTableWidgetItem(QString::number(h.getColumns())));
    }
    populateScreeningCombos();
}

void MainWindow::refreshScreeningTable() {
    auto screenings = m_movieMgr.getAllScreenings();
    m_screeningTable->setRowCount(0);
    for (auto& s : screenings) {
        Movie* mv = m_movieMgr.findMovieById(s.getMovieId());
        ScreeningHall* hl = m_movieMgr.findHallById(s.getHallId());
        int row = m_screeningTable->rowCount();
        m_screeningTable->insertRow(row);
        m_screeningTable->setItem(row,0,new QTableWidgetItem(QString::number(s.getId())));
        m_screeningTable->setItem(row,1,new QTableWidgetItem(mv ? mv->getTitle() : "?"));
        m_screeningTable->setItem(row,2,new QTableWidgetItem(hl ? hl->getName() : "?"));
        m_screeningTable->setItem(row,3,new QTableWidgetItem(s.getDate().toString("yyyy-MM-dd")));
        m_screeningTable->setItem(row,4,new QTableWidgetItem(s.getShowtime().toString("HH:mm")));
        m_screeningTable->setItem(row,5,new QTableWidgetItem("$"+QString::number(s.getPriceStandard(),'f',2)));
        m_screeningTable->setItem(row,6,new QTableWidgetItem("$"+QString::number(s.getPriceVIP(),'f',2)));
        m_screeningTable->setItem(row,7,new QTableWidgetItem("$"+QString::number(s.getPriceChild(),'f',2)));
    }
    populateScreeningCombos();
}

void MainWindow::refreshCustomerTable() {
    auto custs = m_bookingMgr.getAllCustomers();
    m_customerTable->setRowCount(0);
    for (auto& c : custs) {
        int row = m_customerTable->rowCount();
        m_customerTable->insertRow(row);
        m_customerTable->setItem(row,0,new QTableWidgetItem(QString::number(c.getId())));
        m_customerTable->setItem(row,1,new QTableWidgetItem(c.getName()));
        m_customerTable->setItem(row,2,new QTableWidgetItem(c.getContact()));
        m_customerTable->setItem(row,3,new QTableWidgetItem(c.getEmail()));
    }
    populateScreeningCombos();
}

void MainWindow::refreshBookingTable() {
    auto bookings = m_bookingMgr.getAllBookings();
    m_bookingTable->setRowCount(0);
    for (auto* b : bookings) {
        if (!m_isAdmin && b->getCustomerId() != m_loggedInCustomerId) continue;

        Customer* c = m_bookingMgr.findCustomerById(b->getCustomerId());
        Screening* s = m_movieMgr.findScreeningById(b->getScreeningId());
        Movie* mv = s ? m_movieMgr.findMovieById(s->getMovieId()) : nullptr;
        int row = m_bookingTable->rowCount();
        m_bookingTable->insertRow(row);
        m_bookingTable->setItem(row,0,new QTableWidgetItem(QString::number(b->getId())));
        m_bookingTable->setItem(row,1,new QTableWidgetItem(b->getReference()));
        m_bookingTable->setItem(row,2,new QTableWidgetItem(c ? c->getName() : "?"));
        m_bookingTable->setItem(row,3,new QTableWidgetItem(mv ? mv->getTitle() : "?"));
        m_bookingTable->setItem(row,4,new QTableWidgetItem("$"+QString::number(b->getTotal(),'f',2)));
        m_bookingTable->setItem(row,5,new QTableWidgetItem(Booking::statusToString(b->getStatus())));
        if (b->getStatus() == BookingStatus::Cancelled)
            for (int col=0; col<6; ++col)
                if (m_bookingTable->item(row,col))
                    m_bookingTable->item(row,col)->setBackground(QColor(255,200,200));
    }
}

void MainWindow::refreshSeatMap(int screeningId) {
    Screening* scr = m_movieMgr.findScreeningById(screeningId);
    if (!scr) return;
    ScreeningHall* hall = m_movieMgr.findHallById(scr->getHallId());
    if (!hall) return;

    int rows = hall->getRows(), cols = hall->getColumns();
    m_seatTable->setRowCount(rows);
    m_seatTable->setColumnCount(cols);

    QStringList rowLabels, colLabels;
    for (int r=0; r<rows; ++r) rowLabels << QString(QChar('A'+r));
    for (int c=0; c<cols; ++c) colLabels << QString::number(c+1);
    m_seatTable->setVerticalHeaderLabels(rowLabels);
    m_seatTable->setHorizontalHeaderLabels(colLabels);

    auto seats = m_movieMgr.getSeatsForScreening(screeningId);
    for (auto& seat : seats) {
        QTableWidgetItem* item = new QTableWidgetItem(seat.getSeatLabel());
        item->setTextAlignment(Qt::AlignCenter);
        item->setData(Qt::UserRole, seat.getId());
        if (!seat.isAvailable())
            item->setBackground(QColor(231,76,60));
        else
            item->setBackground(QColor(39,174,96));
        m_seatTable->setItem(seat.getRow(), seat.getCol(), item);
    }
    for (int c=0; c<cols; ++c) m_seatTable->setColumnWidth(c, 45);
    for (int r=0; r<rows; ++r) m_seatTable->setRowHeight(r, 38);
}

void MainWindow::populateScreeningCombos() {
    m_scrMovieCombo->clear();
    for (auto& m : m_movieMgr.getAllMovies())
        m_scrMovieCombo->addItem(m.getTitle(), m.getId());

    m_scrHallCombo->clear();
    for (auto& h : m_movieMgr.getAllHalls())
        m_scrHallCombo->addItem(h.getName(), h.getId());

    m_bookScrCombo->clear();
    for (auto& s : m_movieMgr.getAllScreenings()) {
        Movie* mv = m_movieMgr.findMovieById(s.getMovieId());
        ScreeningHall* hl = m_movieMgr.findHallById(s.getHallId());
        int avail = m_movieMgr.availableSeatCount(s.getId());
        QString label = QString("[%1] %2 | %3 | %4 %5 | Avail:%6")
            .arg(s.getId())
            .arg(mv ? mv->getTitle() : "?")
            .arg(hl ? hl->getName() : "?")
            .arg(s.getDate().toString("dd MMM"))
            .arg(s.getShowtime().toString("HH:mm"))
            .arg(avail);
        m_bookScrCombo->addItem(label, s.getId());
    }

    m_bookCustCombo->clear();
    for (auto& c : m_bookingMgr.getAllCustomers())
        m_bookCustCombo->addItem(QString("[%1] %2").arg(c.getId()).arg(c.getName()), c.getId());
}

void MainWindow::clearMovieForm() {
    m_movieTitle->clear(); m_movieGenre->clear(); m_movieRating->clear();
    m_movieLang->clear(); m_movieDesc->clear(); m_movieDuration->setValue(90);
    m_movieStatus->setCurrentIndex(0); m_selectedMovieId = -1;
}
void MainWindow::clearHallForm() {
    m_hallName->clear(); m_hallRows->setValue(5); m_hallCols->setValue(10);
    m_selectedHallId = -1;
}
void MainWindow::clearCustomerForm() {
    m_custName->clear(); m_custContact->clear(); m_custEmail->clear();
    m_selectedCustomerId = -1;
}

void MainWindow::populateMovieForm(const Movie& m) {
    m_movieTitle->setText(m.getTitle());
    m_movieGenre->setText(m.getGenre());
    m_movieDuration->setValue(m.getDuration());
    m_movieRating->setText(m.getRating());
    m_movieLang->setText(m.getLanguage());
    m_movieDesc->setText(m.getDescription());
    m_movieStatus->setCurrentText(m.getStatusString());
    m_selectedMovieId = m.getId();
}
void MainWindow::populateHallForm(const ScreeningHall& h) {
    m_hallName->setText(h.getName());
    m_hallRows->setValue(h.getRows());
    m_hallCols->setValue(h.getColumns());
    m_selectedHallId = h.getId();
}
void MainWindow::populateCustomerForm(const Customer& c) {
    m_custName->setText(c.getName());
    m_custContact->setText(c.getContact());
    m_custEmail->setText(c.getEmail());
    m_selectedCustomerId = c.getId();
}

void MainWindow::onAddMovie() {
    if (m_movieTitle->text().trimmed().isEmpty()) {
        QMessageBox::warning(this,"Error","Title cannot be empty."); return;
    }
    Movie m(m_movieMgr.nextMovieId(), m_movieTitle->text().trimmed(),
            m_movieGenre->text().trimmed(), m_movieDuration->value(),
            m_movieRating->text().trimmed(), m_movieLang->text().trimmed(),
            m_movieDesc->toPlainText().trimmed(),
            Movie::statusFromString(m_movieStatus->currentText()));
    if (m_movieMgr.addMovie(m)) { refreshMovieTable(); clearMovieForm(); QMessageBox::information(this,"OK","Movie added."); }
    else QMessageBox::warning(this,"Error","Failed to add movie.");
}

void MainWindow::onUpdateMovie() {
    if (m_selectedMovieId < 0) { QMessageBox::warning(this,"Error","Select a movie first."); return; }
    Movie* existing = m_movieMgr.findMovieById(m_selectedMovieId);
    if (!existing) return;
    existing->setTitle(m_movieTitle->text().trimmed());
    existing->setGenre(m_movieGenre->text().trimmed());
    existing->setDuration(m_movieDuration->value());
    existing->setRating(m_movieRating->text().trimmed());
    existing->setLanguage(m_movieLang->text().trimmed());
    existing->setDescription(m_movieDesc->toPlainText().trimmed());
    existing->setStatus(Movie::statusFromString(m_movieStatus->currentText()));
    m_movieMgr.updateMovie(*existing);
    refreshMovieTable(); clearMovieForm();
    QMessageBox::information(this,"OK","Movie updated.");
}

void MainWindow::onRemoveMovie() {
    if (m_selectedMovieId < 0) { QMessageBox::warning(this,"Error","Select a movie first."); return; }
    if (QMessageBox::question(this,"Confirm","Remove this movie?") == QMessageBox::Yes) {
        m_movieMgr.removeMovie(m_selectedMovieId);
        refreshMovieTable(); clearMovieForm();
    }
}

void MainWindow::onSearchMovies() {
    auto tab = m_tabs->widget(0);
    QLineEdit* search = tab->findChild<QLineEdit*>();
    QString kw = search ? search->text() : "";
    auto movies = m_movieMgr.searchMovies(kw);
    m_movieTable->setRowCount(0);
    for (auto& m : movies) {
        int row = m_movieTable->rowCount(); m_movieTable->insertRow(row);
        m_movieTable->setItem(row,0,new QTableWidgetItem(QString::number(m.getId())));
        m_movieTable->setItem(row,1,new QTableWidgetItem(m.getTitle()));
        m_movieTable->setItem(row,2,new QTableWidgetItem(m.getGenre()));
        m_movieTable->setItem(row,3,new QTableWidgetItem(QString::number(m.getDuration())+" min"));
        m_movieTable->setItem(row,4,new QTableWidgetItem(m.getRating()));
        m_movieTable->setItem(row,5,new QTableWidgetItem(m.getLanguage()));
        m_movieTable->setItem(row,6,new QTableWidgetItem(m.getStatusString()));
        m_movieTable->setItem(row,7,new QTableWidgetItem(m.getDescription()));
    }
}

void MainWindow::onMovieTableClicked(int row, int) {
    auto* idItem = m_movieTable->item(row,0);
    if (!idItem) return;
    Movie* m = m_movieMgr.findMovieById(idItem->text().toInt());
    if (m) populateMovieForm(*m);
}

void MainWindow::onAddHall() {
    if (m_hallName->text().trimmed().isEmpty()) { QMessageBox::warning(this,"Error","Hall name required."); return; }
    ScreeningHall h(m_movieMgr.nextHallId(), m_hallName->text().trimmed(),
                    m_hallRows->value(), m_hallCols->value());
    if (m_movieMgr.addHall(h)) { refreshHallTable(); clearHallForm(); QMessageBox::information(this,"OK","Hall added."); }
}

void MainWindow::onRemoveHall() {
    if (m_selectedHallId < 0) { QMessageBox::warning(this,"Error","Select a hall first."); return; }
    if (QMessageBox::question(this,"Confirm","Remove this hall?") == QMessageBox::Yes) {
        m_movieMgr.removeHall(m_selectedHallId); refreshHallTable(); clearHallForm();
    }
}

void MainWindow::onHallTableClicked(int row, int) {
    auto* idItem = m_hallTable->item(row,0);
    if (!idItem) return;
    ScreeningHall* h = m_movieMgr.findHallById(idItem->text().toInt());
    if (h) populateHallForm(*h);
}

void MainWindow::onAddScreening() {
    if (m_scrMovieCombo->count()==0 || m_scrHallCombo->count()==0) {
        QMessageBox::warning(this,"Error","Add movies and halls first."); return;
    }
    int movieId = m_scrMovieCombo->currentData().toInt();
    int hallId  = m_scrHallCombo->currentData().toInt();
    Screening s(m_movieMgr.nextScreeningId(), movieId, hallId,
                m_scrDate->date(), m_scrTime->time(),
                m_scrPriceStd->value(), m_scrPriceVIP->value(), m_scrPriceChild->value());
    if (m_movieMgr.addScreening(s)) {
        refreshScreeningTable();
        QMessageBox::information(this,"OK","Screening added.");
    }
}

void MainWindow::onCancelScreening() {
    if (m_selectedScreeningId < 0) { QMessageBox::warning(this,"Error","Select a screening."); return; }
    if (QMessageBox::question(this,"Confirm","Cancel this screening?") == QMessageBox::Yes) {
        m_movieMgr.cancelScreening(m_selectedScreeningId);
        m_selectedScreeningId = -1; refreshScreeningTable();
    }
}

void MainWindow::onScreeningTableClicked(int row, int) {
    auto* idItem = m_screeningTable->item(row,0);
    if (idItem) m_selectedScreeningId = idItem->text().toInt();
}

void MainWindow::onAddCustomer() {
    if (m_custName->text().trimmed().isEmpty()) { QMessageBox::warning(this,"Error","Name required."); return; }
    Customer c(m_bookingMgr.nextCustomerId(), m_custName->text().trimmed(),
               m_custContact->text().trimmed(), m_custEmail->text().trimmed());
    if (m_bookingMgr.addCustomer(c)) { refreshCustomerTable(); clearCustomerForm(); QMessageBox::information(this,"OK","Customer added."); }
}

void MainWindow::onUpdateCustomer() {
    if (m_selectedCustomerId < 0) { QMessageBox::warning(this,"Error","Select a customer first."); return; }
    Customer* existing = m_bookingMgr.findCustomerById(m_selectedCustomerId);
    if (!existing) return;
    existing->setName(m_custName->text().trimmed());
    existing->setContact(m_custContact->text().trimmed());
    existing->setEmail(m_custEmail->text().trimmed());
    m_bookingMgr.updateCustomer(*existing); refreshCustomerTable(); clearCustomerForm();
    QMessageBox::information(this,"OK","Customer updated.");
}

void MainWindow::onDeleteCustomer() {
    if (m_selectedCustomerId < 0) { QMessageBox::warning(this,"Error","Select a customer."); return; }
    if (QMessageBox::question(this,"Confirm","Delete customer?") == QMessageBox::Yes) {
        m_bookingMgr.deleteCustomer(m_selectedCustomerId); refreshCustomerTable(); clearCustomerForm();
    }
}

void MainWindow::onSearchCustomers() {
    auto custs = m_bookingMgr.searchCustomers(m_custSearch->text());
    m_customerTable->setRowCount(0);
    for (auto& c : custs) {
        int row = m_customerTable->rowCount(); m_customerTable->insertRow(row);
        m_customerTable->setItem(row,0,new QTableWidgetItem(QString::number(c.getId())));
        m_customerTable->setItem(row,1,new QTableWidgetItem(c.getName()));
        m_customerTable->setItem(row,2,new QTableWidgetItem(c.getContact()));
        m_customerTable->setItem(row,3,new QTableWidgetItem(c.getEmail()));
    }
}

void MainWindow::onCustomerTableClicked(int row, int) {
    auto* idItem = m_customerTable->item(row,0);
    if (!idItem) return;
    Customer* c = m_bookingMgr.findCustomerById(idItem->text().toInt());
    if (c) populateCustomerForm(*c);
}

void MainWindow::onLoadSeats() {
    if (m_bookScrCombo->count() == 0) { QMessageBox::warning(this,"Error","No screenings available."); return; }
    m_selectedScreeningId = m_bookScrCombo->currentData().toInt();
    m_selectedSeatIds.clear();
    m_bookTotalLabel->setText("Total: $0.00");
    m_bookRefLabel->setText("Reference: —");
    m_currentBookingId = -1;
    refreshSeatMap(m_selectedScreeningId);
}

void MainWindow::onSeatSelected(int row, int col) {
    QTableWidgetItem* item = m_seatTable->item(row, col);
    if (!item) return;
    int seatId = item->data(Qt::UserRole).toInt();
    Seat* seat = m_movieMgr.findSeat(m_selectedScreeningId, seatId);
    if (!seat || !seat->isAvailable()) {
        if (m_selectedSeatIds.contains(seatId)) {
            m_selectedSeatIds.removeAll(seatId);
            item->setBackground(QColor(39,174,96));
        } else {
            QMessageBox::warning(this,"Seat Taken","This seat is already booked."); return;
        }
    } else {
        if (m_selectedSeatIds.contains(seatId)) {
            m_selectedSeatIds.removeAll(seatId);
            item->setBackground(QColor(39,174,96));
        } else {
            m_selectedSeatIds.append(seatId);
            item->setBackground(QColor(52,152,219));
        }
    }
    Screening* scr = m_movieMgr.findScreeningById(m_selectedScreeningId);
    double total = 0;
    if (scr) {
        QString catStr = m_seatCatCombo->currentText();
        double price = (catStr=="VIP") ? scr->getPriceVIP() :
                       (catStr=="Child") ? scr->getPriceChild() * 0.6 :
                       scr->getPriceStandard();
        total = price * m_selectedSeatIds.size();
    }
    m_bookTotalLabel->setText(QString("Total: $%1  (%2 seat(s) selected)").arg(total,0,'f',2).arg(m_selectedSeatIds.size()));
}

void MainWindow::onConfirmBooking() {
    if (m_selectedSeatIds.isEmpty()) { QMessageBox::warning(this,"Error","Select at least one seat."); return; }
    
    int custId = -1;
    if (m_isAdmin) {
        if (m_bookCustCombo->count() == 0) { QMessageBox::warning(this,"Error","No customers available."); return; }
        custId = m_bookCustCombo->currentData().toInt();
    } else {
        custId = m_loggedInCustomerId;
    }
    
    Booking* b = m_bookingMgr.createBooking(custId, m_selectedScreeningId);
    QString catStr = m_seatCatCombo->currentText();
    SeatCategory cat = (catStr=="VIP") ? SeatCategory::VIP :
                       (catStr=="Child") ? SeatCategory::Child : SeatCategory::Standard;
    for (int seatId : m_selectedSeatIds) {
        Seat* seat = m_movieMgr.findSeat(m_selectedScreeningId, seatId);
        if (seat) m_bookingMgr.addTicketToBooking(b->getId(), seat->getSeatLabel(), seatId, cat);
    }
    m_bookingMgr.finalizeBooking(b->getId());
    m_bookRefLabel->setText("Reference: " + b->getReference());
    m_currentBookingId = b->getId();
    m_selectedSeatIds.clear();
    refreshSeatMap(m_selectedScreeningId);
    refreshBookingTable();
    populateScreeningCombos();
    QMessageBox::information(this,"Booking Confirmed",
        QString("Booking confirmed!\nReference: %1\nTotal: $%2")
        .arg(b->getReference()).arg(b->getTotal(),0,'f',2));
}

void MainWindow::onCancelBooking() {
    if (m_currentBookingId < 0) { QMessageBox::warning(this,"Error","No active booking to cancel."); return; }
    if (QMessageBox::question(this,"Confirm","Cancel current booking and release seats?") == QMessageBox::Yes) {
        m_bookingMgr.cancelBooking(m_currentBookingId);
        m_currentBookingId = -1; m_selectedSeatIds.clear();
        refreshSeatMap(m_selectedScreeningId); refreshBookingTable(); populateScreeningCombos();
        QMessageBox::information(this,"OK","Booking cancelled.");
    }
}

void MainWindow::onBookingTableClicked(int row, int) {
    auto* idItem = m_bookingTable->item(row,0);
    if (!idItem) return;
    Booking* b = m_bookingMgr.findBookingById(idItem->text().toInt());
    if (!b) return;
    Customer* c = m_bookingMgr.findCustomerById(b->getCustomerId());
    Screening* s = m_movieMgr.findScreeningById(b->getScreeningId());
    Movie* mv = s ? m_movieMgr.findMovieById(s->getMovieId()) : nullptr;
    ScreeningHall* hl = s ? m_movieMgr.findHallById(s->getHallId()) : nullptr;

    QString detail = QString("Ref: %1  |  Status: %2\nCustomer: %3  |  Movie: %4  |  Hall: %5\nDate: %6  Time: %7\n\nTickets:\n")
        .arg(b->getReference())
        .arg(Booking::statusToString(b->getStatus()))
        .arg(c ? c->getName() : "?")
        .arg(mv ? mv->getTitle() : "?")
        .arg(hl ? hl->getName() : "?")
        .arg(s ? s->getDate().toString("dd MMM yyyy") : "?")
        .arg(s ? s->getShowtime().toString("hh:mm AP") : "?");
    for (auto* t : b->getTickets())
        detail += QString("  Seat %1  [%2]  $%3\n").arg(t->getSeatLabel()).arg(t->getTypeName()).arg(t->getPrice(),0,'f',2);
    detail += QString("\nTOTAL: $%1").arg(b->getTotal(),0,'f',2);
    m_histDetailLabel->setText(detail);
    m_currentBookingId = b->getId();
}

void MainWindow::onPrintTicket() {
    if (m_currentBookingId < 0) { QMessageBox::warning(this,"Error","Select a booking first."); return; }
    Booking* b = m_bookingMgr.findBookingById(m_currentBookingId);
    if (!b) return;
    Screening* s = m_movieMgr.findScreeningById(b->getScreeningId());
    Movie* mv = s ? m_movieMgr.findMovieById(s->getMovieId()) : nullptr;
    ScreeningHall* hl = s ? m_movieMgr.findHallById(s->getHallId()) : nullptr;
    QString dir = QFileDialog::getExistingDirectory(this,"Select Output Folder");
    if (dir.isEmpty()) return;
    bool ok = m_bookingMgr.generateTicketFile(m_currentBookingId,
        mv ? mv->getTitle() : "Unknown", hl ? hl->getName() : "Unknown", dir);
    if (ok) QMessageBox::information(this,"Saved","Ticket saved to:\n" + dir);
    else     QMessageBox::warning(this,"Error","Failed to save ticket.");
}

void MainWindow::onLoadAllBookings() {
    refreshBookingTable();
}
