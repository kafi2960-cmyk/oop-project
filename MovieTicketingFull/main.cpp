#include <QApplication>
#include "mainwindow.h"
#include "loginwindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Movie Ticketing Management System");
    app.setStyle("Fusion");
    
    MainWindow window;
    
    LoginWindow login(window.getBookingManager());
    if (login.exec() == QDialog::Accepted) {
        window.setUserRole(login.isAdmin(), login.getLoggedInCustomerId());
        window.show();
        return app.exec();
    }
    
    return 0;
}
