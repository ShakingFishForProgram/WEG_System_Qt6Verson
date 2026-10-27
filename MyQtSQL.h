#pragma once
#ifndef MYQTSQL_H
#define MYQTSQL_H

#include <QWidget>
#include <QtSql>
#include "widget.h"

class MyQtSQL_WEG{
    public:
    MyQtSQL_WEG(Widget* WidPtr);
    ~MyQtSQL_WEG();

    bool AddUsers(QString, QString);
    bool DeleteUsers(QString);
    void AddMonth(int Month);
    bool ChangeData(float, float, float, bool, bool, bool, int, QString);
    bool ShowData(MemberData* Tem_MemberData, int Month, QString Stu_ID, OtherData Ot_Data);
    void PredictData(MemberData* Pre_Data, int Current_Month, QString Stu_ID, OtherData Ot_Data);
    float ComputeData(MemberData The_MemberData, OtherData The_Data);
    std::vector<float> TotalShowData(std::vector<float> Tem_MemberDataS, int Month, int Aspect_Index);
    void TotalDataAnalysis(std::vector<float> Tem_MemberDataS, TotalData* the_TotalData);

    private:
    QSqlDatabase MyDB;
    QSqlTableModel* WegTabModel;

};

class MyQtSQL_Other{
    public:
        MyQtSQL_Other(Widget* WidPtr);
        ~MyQtSQL_Other();

        bool UpdateMonth();
        void GetData(OtherData & Tem_OtherDat);
        void SendData(float Data, int index);


    private:
        QSqlDatabase MyDB;
        QSqlTableModel* OtherTabModel;

};

#endif // MYQTSQL_H
