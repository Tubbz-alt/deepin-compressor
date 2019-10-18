#include "myfileitem.h"
#include <QDateTime>
#include <QDebug>


MyFileItem::MyFileItem()
{

}

MyFileItem::MyFileItem(const QString &text)
   :QStandardItem(text)
{
}

MyFileItem::MyFileItem(const QIcon &icon, const QString &text)
   : QStandardItem(icon, text)
{
}


bool MyFileItem::operator<(const QStandardItem &other) const
{
    const QVariant l = data(Qt::DisplayRole), r = other.data(Qt::DisplayRole);
    switch (column()) {
    case 3:
    {
        if (column() == other.column())
        {
            qint64 lint = Utils::humanReadableToSize(l.toString());
            qint64 rint = Utils::humanReadableToSize(r.toString());
            return lint < rint;
        }
        break;
    }
    case 1:
    {
        if (column() == other.column())
        {
            QDateTime ldate = QDateTime::fromString(l.toString(), "yyyy/MM/dd hh:mm:ss");
            QDateTime rdate = QDateTime::fromString(r.toString(), "yyyy/MM/dd hh:mm:ss");
            return !rdate.operator<(ldate);
        }
        break;
    }
    default:
        break;


    }

    return QStandardItem::operator<(other);
}