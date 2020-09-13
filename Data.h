#ifndef DATA_H
#define DATA_H

#include <QObject>
#include <QtWidgets>
struct DataPart
{
    QString name;
    QColor color;
    QImage icon;
    float value;
};
struct ColumnPart
{
    QString columname;
    QVector<DataPart> data;

};

class Data : public QObject
{
    Q_OBJECT
public:
    explicit Data(QObject *parent = nullptr);
    static QVector<ColumnPart> makeFramesFromCSV(const QString & path, char separator = ';');
    static QVector<ColumnPart> sliceFrameData(const QVector<ColumnPart> &data, unsigned value);

signals:

};

#endif // DATA_H
