#include "qtincoming.h"
#include "ui_qtincoming.h"
#include "incomingmailmodel.h"

QtIncoming::QtIncoming(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QtIncoming)
{
    ui->setupUi(this);
    IncomingMailModel *mailModel = new IncomingMailModel(this);
    QTableView *view = ui->mailTable;
    view->setModel(mailModel);
}

QtIncoming::~QtIncoming()
{
    delete ui;
}

void QtIncoming::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
