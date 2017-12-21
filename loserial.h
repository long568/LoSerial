#ifndef LOSERIAL_H
#define LOSERIAL_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QDebug>
#include <QKeyEvent>
#include <QFile>
#include <QTranslator>
#include <QTimer>
#include <QSettings>
#include <QVariant>

#include "lotools.h"

#define CONF_FILE "Config.ini"

namespace Ui {
class LoSerial;
}

class LoSerial : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoSerial(QWidget *parent = 0);
    ~LoSerial();

    virtual void keyPressEvent(QKeyEvent *event);

private:
    void saveConf();
    void loadConf();
    void openSerialPort();
    void closeSerialPort();
    void sendData();

private slots:
    void onLanguageChanged();
    void onOpenClicked();
    void onReadyRead();
    void onHexRxChecked(bool isChecked);
    void onHexTxChecked(bool isChecked);
    void onError(QSerialPort::SerialPortError error);
    void onAutoTxChecked(bool isChecked);
    void onAutoTx();

private:
    Ui::LoSerial *ui;
    bool          m_isOpened;
    QSerialPort  *m_SerialPort;
    QTranslator  *m_Translator;
    QTimer       *m_timAutoTx;
};

#endif // LOSERIAL_H
