#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "qmqttclient.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    QMqttClient* m_client;
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void connection();
private:
    Ui::Widget *ui;

private slots:
    void connecter();
    void msgRecu(QMqttMessage msg);
    void send();
    void pres();

};
#endif // WIDGET_H
