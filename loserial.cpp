#include "loserial.h"
#include "ui_loserial.h"

LoSerial::LoSerial(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoSerial),
    m_Translator(0)
{
    ui->setupUi(this);
    m_appPath = QCoreApplication::applicationDirPath();
    QStringList sp_name_list;
    QList<QSerialPortInfo> sp_list = QSerialPortInfo::availablePorts();
    foreach (QSerialPortInfo sp_ifo, sp_list)
        sp_name_list.append(sp_ifo.portName());
    ui->COMs->addItems(sp_name_list);
    m_isOpened = false;
    m_timAutoTx = new QTimer(this);
    auto ag = new QActionGroup(ui->menuLanguage);
    ag->addAction(ui->actionChinese);
    ag->addAction(ui->actionEnglish);
    ag->setExclusive(true);
    loadConf();
    onLanguageChanged();
    connect(m_timAutoTx, SIGNAL(timeout()), this, SLOT(onAutoTx()));
    connect(ag, SIGNAL(triggered(QAction*)), this, SLOT(onLanguageChanged()));
    connect(ui->Open,   SIGNAL(clicked(bool)), this, SLOT(onOpenClicked()));
    connect(ui->HexRx,  SIGNAL(toggled(bool)), this, SLOT(onHexRxChecked(bool)));
    connect(ui->HexTx,  SIGNAL(toggled(bool)), this, SLOT(onHexTxChecked(bool)));
    connect(ui->AutoTx, SIGNAL(toggled(bool)), this, SLOT(onAutoTxChecked(bool)));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(onShowAbout()));
}

LoSerial::~LoSerial()
{
    if(m_isOpened)
        closeSerialPort();
    saveConf();
    delete ui;
}

void LoSerial::keyPressEvent(QKeyEvent *event)
{
    switch ((Qt::Key)event->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if(m_isOpened && ui->TxData->hasFocus()) {
            sendData();
            if(ui->AutoClear->isChecked())
                ui->TxData->clear();
        }
        break;
    default: break;
    }
}

void LoSerial::saveConf()
{
    QSettings settings(CONF_FILE, CONF_FORMAT);
    if(ui->actionEnglish->isChecked())
        settings.setValue("Language", QVariant("enUS"));
    else
        settings.setValue("Language", QVariant("zhCN"));
    settings.setValue("Com_Index", QVariant(ui->COMs->currentText()));
    settings.setValue("Baud_Rate", QVariant(ui->BaudRate->currentText()));
    settings.setValue("Data_Bits", QVariant(ui->DataBits->currentText()));
    settings.setValue("Stop_Bits", QVariant(ui->StopBits->currentIndex()));
    settings.setValue("Parity_Md", QVariant(ui->ParityMode->currentIndex()));
    settings.setValue("Flow_Ctrl", QVariant(ui->FlowControl->currentIndex()));
    settings.setValue("Hex_Rx",    QVariant(ui->HexRx->isChecked()));
    settings.setValue("Hex_Tx",    QVariant(ui->HexTx->isChecked()));
    settings.setValue("Echo",      QVariant(ui->Echo->isChecked()));
    settings.setValue("Attach_CR", QVariant(ui->AttachCR->isChecked()));
    settings.setValue("Aut_Clear", QVariant(ui->AutoClear->isChecked()));
    settings.setValue("Aut_Tx_Ir", QVariant(ui->AutoTxInterval->value()));
    settings.setValue("Tx_Data",   QVariant(ui->TxData->text()));
    settings.setValue("Rx_Data",   QVariant(ui->RxData->toPlainText()));
}

void LoSerial::loadConf()
{
    QFile f(CONF_FILE);
    if(f.exists()) {
        QSettings settings(CONF_FILE, CONF_FORMAT);
        if(settings.value("Language").isValid()) {
            QString lan = settings.value("Language").toString();
            if(lan == "enUS") ui->actionEnglish->setChecked(true);
            else              ui->actionChinese->setChecked(true);
        }
        if(settings.value("Com_Index").isValid())
            ui->COMs->setCurrentText(settings.value("Com_Index").toString());
        if(settings.value("Baud_Rate").isValid())
            ui->BaudRate->setCurrentText(settings.value("Baud_Rate").toString());
        if(settings.value("Data_Bits").isValid())
            ui->DataBits->setCurrentText(settings.value("Data_Bits").toString());
        if(settings.value("Stop_Bits").isValid())
            ui->StopBits->setCurrentIndex(settings.value("Stop_Bits").toInt());
        if(settings.value("Parity_Md").isValid())
            ui->ParityMode->setCurrentIndex(settings.value("Parity_Md").toInt());
        if(settings.value("Flow_Ctrl").isValid())
            ui->FlowControl->setCurrentIndex(settings.value("Flow_Ctrl").toInt());
        if(settings.value("Hex_Rx").isValid())
            ui->HexRx->setChecked(settings.value("Hex_Rx").toBool());
        if(settings.value("Hex_Tx").isValid())
            ui->HexTx->setChecked(settings.value("Hex_Tx").toBool());
        if(settings.value("Echo").isValid())
            ui->Echo->setChecked(settings.value("Echo").toBool());
        if(settings.value("Attach_CR").isValid())
            ui->AttachCR->setChecked(settings.value("Attach_CR").toBool());
        if(settings.value("Aut_Clear").isValid())
            ui->AutoClear->setChecked(settings.value("Aut_Clear").toBool());
        if(settings.value("Aut_Tx_Ir").isValid())
            ui->AutoTxInterval->setValue(settings.value("Aut_Tx_Ir").toInt());
        if(settings.value("Tx_Data").isValid())
            ui->TxData->setText(settings.value("Tx_Data").toString());
        if(settings.value("Rx_Data").isValid())
            ui->RxData->setPlainText(settings.value("Rx_Data").toString());
    }
}

void LoSerial::openSerialPort()
{
    m_SerialPort = new QSerialPort(ui->COMs->currentText());
    if(NULL != m_SerialPort) {
        m_SerialPort->setBaudRate(ui->BaudRate->currentText().toInt());
        m_SerialPort->setDataBits((QSerialPort::DataBits)ui->DataBits->currentText().toInt());
        m_SerialPort->setStopBits((QSerialPort::StopBits)ui->StopBits->currentIndex());
        m_SerialPort->setParity((QSerialPort::Parity)ui->ParityMode->currentIndex());
        m_SerialPort->setFlowControl((QSerialPort::FlowControl)ui->FlowControl->currentIndex());
        if(m_SerialPort->open(QIODevice::ReadWrite)) {
            connect(m_SerialPort, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
            connect(m_SerialPort, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(onError(QSerialPort::SerialPortError)));
            ui->COMSetting->setEnabled(false);
            ui->Open->setText(tr("Close"));
            m_isOpened = true;
        } else {
            QString msg = tr("Open failed ... ");
            msg += QString("%1").arg((int)m_SerialPort->error());
            QMessageBox::critical(this, tr("Error"), msg);
            m_SerialPort->deleteLater();
            m_SerialPort = NULL;
        }
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Create failed !"));
    }
}

void LoSerial::closeSerialPort()
{
    disconnect(m_SerialPort, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    disconnect(m_SerialPort, SIGNAL(error(QSerialPort::SerialPortError)),
               this, SLOT(onError(QSerialPort::SerialPortError)));
    m_SerialPort->close();
    m_SerialPort->deleteLater();
    m_SerialPort = NULL;
    ui->COMSetting->setEnabled(true);
    ui->Open->setText(tr("Open"));
    m_isOpened = false;
}

void LoSerial::sendData()
{
    QByteArray a;
    QString s = ui->TxData->text();
    if(ui->HexTx->isChecked()) {
        a = LoTools::Hex2BArray(s);
        if(!ui->HexRx->isChecked())
            s = LoTools::Hex2Str(s);
    } else {
        a = s.toUtf8();
        if(ui->HexRx->isChecked())
            s = LoTools::Str2Hex(s);
    }
    if(ui->Echo->isChecked())
        ui->RxData->append(s);
    if(ui->AttachCR->isChecked())
        a.append('\r');
    if(a.isEmpty())
        return;
    m_SerialPort->write(a);
}

void LoSerial::onLanguageChanged()
{
    QString lan;
    if(ui->actionChinese->isChecked())
        lan = ":/zhCN";
    else
        lan = ":/enUS";

    if(m_Translator == 0) {
        m_Translator = new QTranslator(this);
        m_Translator->load(lan);
        qApp->installTranslator(m_Translator);
        ui->retranslateUi(this);
    } else {
        if(m_Translator->load(lan)) {
            ui->retranslateUi(this);
            if(!ui->COMSetting->isEnabled())
                ui->Open->setText(tr("Close"));
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Load new language failed !"));
        }
    }
}

void LoSerial::onOpenClicked()
{
    if(m_isOpened)
        closeSerialPort();
    else
        openSerialPort();
}

void LoSerial::onReadyRead()
{
    QString s;
    QByteArray a = m_SerialPort->readAll();
    if(ui->HexRx->isChecked()) {
        s = LoTools::BArray2Hex(a);
    } else {
        s = QString().fromUtf8(a);
    }
    s = ui->RxData->toPlainText() + s;
    ui->RxData->setPlainText(s);
}

void LoSerial::onHexRxChecked(bool isChecked)
{
    QString s = ui->RxData->toPlainText();
    if(isChecked) {
        s = LoTools::Str2Hex(s);
    } else {
        s = LoTools::Hex2Str(s);
    }
    ui->RxData->setPlainText(s);
}

void LoSerial::onHexTxChecked(bool isChecked)
{
    QString s = ui->TxData->text();
    if(isChecked) {
        s = LoTools::Str2Hex(s);
    } else {
        s = LoTools::Hex2Str(s);
    }
    ui->TxData->setText(s);
}

void LoSerial::onError(QSerialPort::SerialPortError error)
{
    switch (error) {
    case QSerialPort::ResourceError: // Lost connection
        closeSerialPort();
        break;
    default: return;
    }
    QString msg = tr("Serial port error ... ");
    msg += QString("%1").arg((int)error);
    QMessageBox::critical(this, tr("Error"), msg);
}

void LoSerial::onAutoTxChecked(bool isChecked)
{
    if(isChecked) {
        m_timAutoTx->start(ui->AutoTxInterval->value());
    } else {
        m_timAutoTx->stop();
    }
}

void LoSerial::onAutoTx()
{
    if(m_isOpened)
        sendData();
}

void LoSerial::onShowAbout()
{
    About *p = new About(this);
    p->exec();
}
