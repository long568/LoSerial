#include "lotools.h"

LoTools::LoTools(){}

// Network
#if TOOL_NETWORK
QStringList LoTools::getLocalIPv4List()
{
    QStringList res;
    QList<QHostAddress> ip_list = QNetworkInterface::allAddresses();
    foreach (QHostAddress ip, ip_list) {
        if(QAbstractSocket::IPv4Protocol == ip.protocol())
            res.push_back(ip.toString());
    }
    return res;
}
#endif

// Data Conversion
QByteArray LoTools::Hex2BArray(const QString &h)
{
    if(h.isEmpty())
        return QByteArray();
    bool        ok;
    quint8      b;
    QByteArray  d;
    QStringList b_list;
    d.clear();
    b_list = h.split(' ');
    if(b_list.last().isEmpty())
        b_list.takeLast();
    foreach(QString b_str, b_list) {
        b = b_str.toUInt(&ok, 16);
        d.append(b);
    }
    return d;
}

QString LoTools::BArray2Hex(const QByteArray &a)
{
    QString s;
    s.clear();
    if(a.isEmpty()) return s;
    foreach(char b, a)
        s.append(QString("%1 ").arg((quint8)b, 2, 16, QLatin1Char('0')));
    s.remove(s.length()-1, 1);
    return s;
}

QString LoTools::Str2Hex(const QString &s)
{
    if(s.isEmpty()) return s;
    QByteArray a = s.toUtf8();
    return BArray2Hex(a);
}

QString LoTools::Hex2Str(const QString &h)
{
    if(h.isEmpty()) return h;
    return QString().fromUtf8(Hex2BArray(h));
}

// Qt Extend
void LoTools::go2TheEnd(QTextEdit *te)
{
    QTextCursor cursor = te->textCursor();
    cursor.movePosition(QTextCursor::End);
    te->setTextCursor(cursor);
}

void LoTools::append(QTextEdit *te, const QByteArray &d)
{
    te->append(d);
    go2TheEnd(te);
}

void LoTools::append(QTextEdit *te, const QString &s)
{
    te->append(s);
    go2TheEnd(te);
}
