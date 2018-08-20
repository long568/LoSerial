#ifndef MYTOOLS_H
#define MYTOOLS_H

#include <QString>
#include <QStringList>
#include <QtNetwork>
#include <QTextEdit>

#define LoProperty0(type, name) \
    private: type lp_##name;           \
    public:  void set##name(type v)    \
                  { lp_##name = v; }   \
    public:  type name(void) const     \
                  { return lp_##name; }

#define LoProperty1(type, name) \
    private: type lp_##name;                \
    public:  void set##name(const type & v) \
                  { lp_##name = v; }        \
    public:  type name(void) const          \
                  { return lp_##name; }
/*
#define LoPropertyObj(type, name) \
    private: type lp_##name;                 \
    signals: void sig##name##Changed(type v) \
    public:  void set##name(type v)          \
                  { lp_##name = v;           \
                    sig##name##Changed(v); } \
    public:  type name(void) const           \
                  { return lp_##name; }
*/
class LoTools
{
protected:
    LoTools();

public: // Network
    static QStringList getLocalIPv4List();

public: // Data Conversion
    static QByteArray Hex2BArray(const QString    &h);
    static QString    BArray2Hex(const QByteArray &a);
    static QString    Str2Hex   (const QString    &s);
    static QString    Hex2Str   (const QString    &h);
    static void       go2TheEnd (QTextEdit *te);

public:    // Qt Extend
    static void append(QTextEdit *te, const QByteArray &d);
    static void append(QTextEdit *te, const QString &s);
};

#endif // MYTOOLS_H
