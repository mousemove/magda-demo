#include "Data.h"

Data::Data(QObject *parent) : QObject(parent)
{

}

QVector<ColumnPart> Data::makeFramesFromCSV(const QString & path,char separator)
{
    QVector<ColumnPart> result;
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {


        QStringList wordList;
        unsigned lineNum = 0;
        while (!file.atEnd()) {
            QByteArray line = file.readLine();
            auto row = line.split(separator);
            if(lineNum == 0)
            {
                if (row.size() <5 ) {file.close();return result;}
                for(unsigned int i = 3; i < row.size();i++)
                {
                    ColumnPart c;
                    c.columname = row.at(i);
                    result.push_back(c);
                }
            }
            else
            {
                DataPart d;

                if (row.size() < 3) {QVector<ColumnPart> empty;file.close();return empty;}//Для демки выдаем пустой вектор при ошибке
                d.name = row.at(0);
                d.icon = QImage(row.at(1));
                d.color = row.at(2).size() == 0 ? QColor(rand()%255,rand()%255,rand()%255) : QColor(QString::fromStdString(row.at(2).toStdString()));//некрасиво ;(

                //d.icon = row.ar
                int value = 3;
                for(auto & r : result)
                {
                    d.value = (value < row.size())? row.at(value).toFloat() : 0;
                    r.data.push_back(d);
                    value++;
                }

            }
            lineNum++;
        }
    }
    file.close();
    return result;

}
//Функция привязана к одинаковой нумерации в векторах DataPart, нужно переделать зависимости после релиза демки
QVector<ColumnPart> Data::sliceFrameData(const QVector<ColumnPart> &data, unsigned value)
{

    if (data.size() < 2) return data;
    QVector<ColumnPart> output;
    for(unsigned int i = 0; i < data.size()-1;i++)
    {
        output.push_back(data.at(i));
        ColumnPart m;
        m.columname = data.at(i).columname;
        m.data = data.at(i).data;

        for(unsigned int j= 0; j < value-1;j++)
        {
            for(unsigned int k = 0;k< m.data.size();k++)
            {
                m.data[k].value += ( (data.at(i+1).data.at(k).value - data.at(i).data.at(k).value)/value );
            }
            output.push_back(m);


        }
    }
    output.push_back(data.at(data.size()-1));
    return output;
}
