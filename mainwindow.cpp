#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QMessageBox>
#include <QDateTime>
#include <QRegExp>
#include <QFile>
#include <QIODevice>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qDebug() << "Запуск";
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->authPage);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/home/noor/master_degree/master.db");
    db.open();

    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(sockReady()));
    connect(socket, SIGNAL(connected()), this, SLOT(onSockConnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(sockDisc()));
    ap.reset(new analysisPage(Q_NULLPTR));
    this->installEventFilter(retBut);
    connect(ap.data(), &analysisPage::report, [=]() {socket->write(reportData);});
    connect(ap.data(), SIGNAL(report()), this, SLOT(reportConvertion()));
    connect(ap.data(), SIGNAL(analysisPage::report), this, SLOT(sockReady()));
    connect(retBut, SIGNAL(entered()), ui->enterButton, SIGNAL(clicked()));
    // тут коннект сигнала нажатия на энтер


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sockReady() // ответ сервера
{   
    // ввод в бд юзера
    QSqlQuery userEntryFix;
    userEntryFix.prepare("INSERT INTO Visitors (Login, Entry) VALUES (?, ?)");
    userEntryFix.addBindValue(authLogin);
    userEntryFix.addBindValue(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss"));
    userEntryFix.exec();

    /*if (socket->waitForConnected(500)) {
        socket->waitForReadyRead(500);
        serverReply = socket->readAll();
        logFill(serverReply);
    }*/

    while (socket->canReadLine()) { // если сокет не пустой
        QString socketContent = QString::fromUtf8(socket->readLine()).trimmed(); // считываем строку с сокета
        QRegExp messageRE("^([^:]+):(.*)$"); // регулярка в формате юзернейм:сообщение
        QRegExp userRE("^/users:(.*)$");
        if(userRE.indexIn(socketContent) != -1) { // ищем соответствие регулярки в строке line, если не равно -1, значит, оно найдено
            QStringList users = userRE.cap(1).split(","); // захватываем из регулярки юзернейм
            ui->usersListWidget->clear();
            foreach(QString user, users)
                new QListWidgetItem(user, ui->usersListWidget); // icon, text, view
        }
        // Is this a normal chat message:
        else if(messageRE.indexIn(socketContent) != -1) { // захватываем обычное юзейрнейм:сообщение и выводим в чат {
            // If so, append this message to our chat box:
            QString user = messageRE.cap(1);
            QString message = messageRE.cap(2);
            qDebug() << "got message: " << message;
            logFill("<b>" + user + "</b>: " + message);
        }

        else if (messageRE.indexIn(socketContent) == -1) logFill("сообщение не получено");
    }
}

void MainWindow::sockDisc()
{
    socket->deleteLater();
    QMessageBox::information(this, "Уведомление", "Соединение потеряно", QMessageBox::Ok);
    logFill("Соединение потеряно");
}

void MainWindow::onSockConnected()
{
    socket->write(QString("/me: " + authLogin + "\n").toUtf8()); // высылаем серверу наш юзернейм
}

void MainWindow::on_enterButton_clicked()
{
    QString enteredLogin = ui->loginEdit->text();
    QString enteredPass = ui->passEdit->text();
    QString adminLogin;
    QString pass;
    QString adminPass;
    QSqlQuery adminDataQuery;
    adminDataQuery.prepare("SELECT User FROM Users WHERE User ='Admin'");
    adminDataQuery.exec();
    QSqlQuery conformityQuery;
    conformityQuery.prepare("SELECT Pass FROM Users WHERE User =?");
    conformityQuery.addBindValue(enteredLogin);
    conformityQuery.exec();
    QSqlQuery adminConformityQuery;
    adminConformityQuery.prepare("SELECT Pass FROM Users WHERE User = 'Admin'");
    adminConformityQuery.exec();
    while (adminConformityQuery.next()) {
        adminPass = adminConformityQuery.value(0).toString();
    }
    while (adminDataQuery.next())  { // проход по строчкам
        adminLogin = adminDataQuery.value(0).toString(); // value(0) -первое в селекте
    }
    while (conformityQuery.next()) {
        pass = conformityQuery.value(0).toString();
    }
    if ((enteredLogin == adminLogin) && (enteredPass == adminPass)) {
        ui->stackedWidget->setCurrentWidget(ui->adminPage);
        ui->headLabel->setText("Вы вошли как " + enteredLogin);
    }
    else if ((ui->loginEdit->text().isEmpty()) || (ui->passEdit->text().isEmpty())) {
        errorBox("Пустые поля ввода");
    }
    else if (pass != enteredPass) errorBox("Неверный пароль");

    else {
        disconnect(retBut, SIGNAL(entered()), ui->enterButton, SIGNAL(clicked()));
        connect(retBut, SIGNAL(entered()), ui->sendButton, SIGNAL(clicked()));
        ui->stackedWidget->setCurrentWidget(ui->adminPage);
        ui->headLabel->setText("Вы вошли как " + enteredLogin);
    }

    authLogin = enteredLogin;
    ui->plotTypeSelector->addItem("Оценка ситуации");
    QSqlQuery reportTheme;
    reportTheme.prepare("SELECT Name FROM Criterias");
    reportTheme.exec();
    while (reportTheme.next()) {
        ui->comboBox->addItem(reportTheme.value(0).toString());
    }
    // ui->comboBox->addItem("");
}

void MainWindow::on_exitButton_clicked()
{
    QSqlQuery userExitFix;
    userExitFix.prepare("UPDATE Visitors SET Exit=:time WHERE Login=:login AND Exit IS NULL");
    userExitFix.bindValue(":time", QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss"));
    userExitFix.bindValue(":login", authLogin);
    userExitFix.exec();
    close();
    exit(0);
}

void MainWindow::errorBox(QString errorText)
{
    QMessageBox::information(this, "Ошибка", errorText, QMessageBox::Ok);
}

void MainWindow::on_analysisButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->analysisPage);
}

void MainWindow::logFill(QString txt)
{
    ui->logBrowser->append(QString("[%1] %2").arg(QDateTime::currentDateTime().toString("hh:mm:ss")).arg(txt));
}

void MainWindow::on_returnButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->authPage);
    connect(retBut, SIGNAL(entered()), ui->enterButton, SIGNAL(clicked()));
}

void MainWindow::waitForConnection()
{

}

void MainWindow::on_connectionButton_clicked()
{
    //  QByteArray username = authLogin.toLocal8Bit();
    socket->connectToHost("127.0.0.1", 5555);
    // socket->write(username.data());
    logFill("Установка соединения...");
}

void MainWindow::on_returnButton_2_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->adminPage);
    historyUpdate("/home/noor/master_degree/history.txt", QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")
                  + " session: \n" + ui->logBrowser->toPlainText());
}

void MainWindow::on_sendButton_clicked()
{
    // logFill(authLogin + ": " + ui->typeLine->text());
    QString message = ui->typeLine->text().trimmed();
    if(!message.isEmpty()) socket->write(QString (message + "\n").toUtf8());
    ui->typeLine->clear();
    ui->typeLine->setFocus();
    QVector <QString> vect;
    QStringList list = message.split(" ");
    qDebug() << "size equals " << list.length();
    for (int i = 0; i <= list.length()-1; i++) {
       vect.push_back(list[i]);
    }
    qDebug() << "vector = " << vect;
    QSqlQuery reportFill;
    reportFill.prepare("INSERT INTO NatLangReport (user, report, strategy, result)"
                        "VALUES (?, ?, ?, ?)");
    reportFill.addBindValue(authLogin);
    reportFill.addBindValue(message);
    reportFill.addBindValue(ui->comboBox->currentText());
    reportFill.addBindValue("успех");
    reportFill.exec();
}

void MainWindow::on_logLoadButton_clicked()
{
    QFile his("/home/noor/master_degree/history.txt");
    if (!his.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    if (!his.exists()) QMessageBox::information(this, "error", "nope", QMessageBox::Ok);
    else {
        logFill("Лог найден");
        // his.open(stderr, QIODevice::ReadWrite);
        QTextStream in(&his);
        while (!his.atEnd()) {
            QString content = in.readAll();
            ui->logBrowser->append(content);
        }
        his.close();
    }
    
}

void MainWindow::historyUpdate(QString path, QString content)
{
    QFile his(path);
    if (his.open(QIODevice::ReadWrite)) {
        QTextStream stream(&his);
        // stream << "content" << endl;
        stream << content << endl;
    }
    his.close();
}

void MainWindow::on_analysisButton_2_clicked()
{
    // ap->connectedDB
    ap->show();
}

void MainWindow::reportConvertion()
{
    reportData = ap.data()->reportText.toLocal8Bit();
}

void MainWindow::on_fastEnterButton_clicked()
{
    ui->loginEdit->setText("user");
    ui->passEdit->setText("555");
}



void MainWindow::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->analysisPage);
}

void MainWindow::on_analysisButton_3_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->plotPage);
}

void MainWindow::on_updateButton_clicked()
{
    if (ui->plotTypeSelector->currentText() == "Оценка ситуации") {
        ui->customPlot->clearGraphs();
        QLinearGradient gradient(0, 0, 0, 400);
        gradient.setColorAt(0, QColor(90, 90, 90));
        gradient.setColorAt(0.38, QColor(105, 105, 105));
        gradient.setColorAt(1, QColor(70, 70, 70));
        ui->customPlot->setBackground(QBrush(gradient));

        // create empty bar chart objects:

        QCPBars *reports = new QCPBars(ui->customPlot->xAxis, ui->customPlot->yAxis);

        reports->setAntialiased(false);
        reports->setStackingGap(1);
        // set names and colors:
        reports->setName("visitors");
        reports->setPen(QPen(QColor(111, 9, 176).lighter(170)));
        reports->setBrush(QColor(111, 9, 176));

        // prepare x axis with labels:
        QVector<double> ticks;
        QVector<QString> labels;
        ticks << 1 << 2 << 3 << 4 << 5 << 6 << 7;
        QString reportName;
        QSqlQuery getReportNames;
        getReportNames.exec("SELECT user FROM Reports");
        while (getReportNames.next()){
            reportName = getReportNames.value(0).toString();
            labels << reportName;
        }
        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
        textTicker->addTicks(ticks, labels);
        ui->customPlot->xAxis->setTicker(textTicker);
        ui->customPlot->xAxis->setTickLabelRotation(60);// - поворот
        ui->customPlot->xAxis->setSubTicks(false);
        ui->customPlot->xAxis->setTickLength(0, 4);
        ui->customPlot->xAxis->setRange(0, 8);
        ui->customPlot->xAxis->setBasePen(QPen(Qt::white));
        ui->customPlot->xAxis->setTickPen(QPen(Qt::white));
        ui->customPlot->xAxis->grid()->setVisible(true);
        ui->customPlot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
        ui->customPlot->xAxis->setTickLabelColor(Qt::white);
        ui->customPlot->xAxis->setLabelColor(Qt::white);

        // prepare y axis:
        ui->customPlot->yAxis->setRange(0, 100);
        ui->customPlot->yAxis->setPadding(1); // a bit more space to the left border
        ui->customPlot->yAxis->setLabel("Оценка ситуации");
        ui->customPlot->yAxis->setBasePen(QPen(Qt::white));
        ui->customPlot->yAxis->setTickPen(QPen(Qt::white));
        ui->customPlot->yAxis->setSubTickPen(QPen(Qt::white));
        ui->customPlot->yAxis->grid()->setSubGridVisible(true);
        ui->customPlot->yAxis->setTickLabelColor(Qt::white);
        ui->customPlot->yAxis->setLabelColor(Qt::white);
        ui->customPlot->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
        ui->customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));

        // Add data:
        QVector<double> reportData;

        // берем из таблицы ивентов айди
        // смотрим эти айди в UsersOnEvents
        // делаем запрос, что если айди совпадает, то количетсво = +1
        // выводим количество в визиторсДата

        // берем из таблицы юзеров юзернейм, смотрим его

        QSqlQuery setValues;
        int reportValue;
        setValues.exec("SELECT value FROM Reports");
        int average = 0;
        int reportCounter = 0;
        while (setValues.next()) {
            reportValue = setValues.value(0).toInt();
            average += setValues.value(0).toInt();
            reportCounter += 1;
            reportData << reportValue;
        }
        qDebug() << "среднее - " << average / reportCounter;

        // reportData  << 1 << 2 << 5 << 4 << 4 << 2 << 1;

        reports->setData(ticks, reportData);

        // setup legend:
        ui->customPlot->legend->setVisible(false);
        ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
        ui->customPlot->legend->setBrush(QColor(255, 255, 255, 100));
        ui->customPlot->legend->setBorderPen(Qt::NoPen);
        QFont legendFont = font();
        legendFont.setPointSize(10);
        ui->customPlot->legend->setFont(legendFont);
        ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    }
}

void MainWindow::on_reportCheckButton_clicked()
{
    QSqlQuery check;
    check.prepare("SELECT report FROM NatLangReport");
    check.exec();
    QString message = ui->typeLine->text().trimmed();
    QStringList baseList;
    QStringList currList;
    int baseLen;
    int matches = 0;
    currList = message.split(" ");
    while (check.next()) {
        QString curr = check.value(0).toString();
        qDebug() << "в базе " << curr;
        baseList = curr.split(" ");
        for (int i = 0; i <= baseList.length() - 1; i++) {
            for (int j = 0; j <= currList.length() - 1; j++) {
                if (currList[j] == baseList[i]) matches += 1;
            }
        }
        QMessageBox::information(this, "Уведомление",
                                 QStringLiteral("Совпадений %1").arg(matches), QMessageBox::Ok);
        matches = 0;
    }
    qDebug() << "база - " << baseList;
    qDebug() << "сообщение - " << currList;
    QVector<QString> vector;
}
