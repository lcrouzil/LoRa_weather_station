#include "widget.h"
#include "ui_widget.h"
#include "QObject"
#include "QJsonDocument"
#include "QJsonObject"

QString hostname = "eu1.cloud.thethings.network";
int port = 1883;
QString username = "station-meteo-lolo@ttn";
QString password = "NNSXS.RY3TRZJDXTGXLOVDODECIGFECHYADX2VMBMISYQ.WL362BOA5EURSOV4SFNNN7QPNOURHSA75AZUVSNSKKJQMHOD2OTQ";
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_client = new QMqttClient(this);
    m_client->setHostname(hostname);
    m_client->setUsername(username);
    m_client->setPort(port);
    m_client->setPassword(password);

    connect(ui->pushButton,&QPushButton::pressed,this,&Widget::send);

    connect(m_client,&QMqttClient::connected,this,&Widget::connecter);

    connect(ui->pushButton_2,&QPushButton::pressed,this,&Widget::pres);

}

Widget::~Widget()
{
    delete ui;
}

void Widget::connection()
{
    qDebug() << "Connexion";
    m_client->connectToHost();
}

void Widget::connecter()
{
  qDebug() << "Connecter";
  auto subscription = m_client->subscribe(QMqttTopicFilter("#"),0);
  connect(subscription, &QMqttSubscription::messageReceived, this,&Widget::msgRecu);
  qDebug() << "====================================fin connecter==================================================\n";

}

void Widget::msgRecu(QMqttMessage msg)
{

    qDebug() << "======================================================================================\n";
    qDebug() << "msg recu";
    qDebug() << "======================================================================================\n";

    QByteArray b = msg.payload();
    QJsonParseError error;
    QString content = QDateTime::currentDateTime().toString()+ " " + b;

    qDebug() << content;

    QJsonDocument doc = QJsonDocument::fromJson(b,&error);

    qDebug() << error.errorString();

    QJsonObject obj = doc.object();


    QJsonObject uplink = obj.value("uplink_message").toObject();

    qDebug() << uplink;

    QJsonObject payload = uplink.value("decoded_payload").toObject();

    double temp = payload.value("Temperature").toDouble();
    ui->lcdTemperature->display(temp);
    int alt = payload.value("Altitude").toInt();
    ui->lcdAltitude->display(alt);
    int pression = payload.value("Pression").toInt();
    ui->lcdPressure->display(pression);
    int humSol = payload.value("Humidite_sol").toInt();
    ui->lcdSoilHumidity->display(humSol);
    int ultrason = payload.value("ultrason").toInt();
    ui->lcdWater->display(ultrason);

    if(payload.value("Luminosite").toInt() == 0){
       ui->lumi->setCheckState(Qt::Checked);
    }else{
        ui->lumi->setCheckState(Qt::Unchecked);
    }

    qDebug() << payload.value("pluie").toInt();
    if(payload.value("pluie").toInt() == 0){
        ui->rain->setCheckState(Qt::Checked);
    }else {
        ui->rain->setCheckState(Qt::Unchecked);
    }
}

void Widget::send()
{
    qDebug() << "Activation relai";
    QByteArray b = "{\"downlinks\":[{\"f_port\": 15,\"frm_payload\":\"AQ==\",\"decoded_payload\":{\"relais\":true},\"priority\": \"NORMAL\"}]}";
    m_client->publish(QMqttTopicName("v3/station-meteo-lolo@ttn/devices/eui-70b3d57ed00583d9/down/push"),b,0,false);
}

void Widget::pres()
{
    qDebug() << "Désactivation relai";
    QByteArray b = "{\"downlinks\":[{\"f_port\": 15,\"frm_payload\":\"AQ==\",\"decoded_payload\":{\"relais\":false},\"priority\": \"NORMAL\"}]}";
    m_client->publish(QMqttTopicName("v3/station-meteo-lolo@ttn/devices/eui-70b3d57ed00583d9/down/push"),b,0,false);
}

