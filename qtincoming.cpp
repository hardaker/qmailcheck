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
    connect(mailModel, SIGNAL(mailUpdated()),
            view, SLOT(resizeRowsToContents()));
    connect(mailModel, SIGNAL(mailUpdated()),
            view, SLOT(resizeColumnsToContents()));
    connect(mailModel, SIGNAL(updateCount(int, int)),
            view, SLOT(rowCountChanged(int, int)));
    view->resizeColumnsToContents();
    view->resizeRowsToContents();

    connect(mailModel, SIGNAL(statusMessage(const QString &, int)),
            ui->statusBar, SLOT(showMessage(const QString &, int)));
    connect(ui->checkMail, SIGNAL(clicked()),
            mailModel, SLOT(checkMail()));
    connect(mailModel, SIGNAL(mailUpdated()),
            this, SLOT(maybeRaise()));
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

void QtIncoming::maybeRaise()
{
    raise();
}

#include "moc_qtincoming.cpp"
