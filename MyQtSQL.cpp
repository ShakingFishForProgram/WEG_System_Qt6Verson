#include "MyQtSQL.h"
#include <vector>

//WEGData对应Class的构造函数
MyQtSQL_WEG::MyQtSQL_WEG(Widget* WidPtr)
{
    //连接DataBase
    QString exeDir = QCoreApplication::applicationDirPath();
    QString dbPath = exeDir + "/UserData.db";
    MyDB = QSqlDatabase::addDatabase("QSQLITE");
    MyDB.setDatabaseName(dbPath);
    MyDB.open();

    //构建TableModel（有的函数需要使用TableModel）
    WegTabModel = new QSqlTableModel(WidPtr, MyDB);
    WegTabModel->setTable("WEGData");
    WegTabModel->select();
    WegTabModel->setEditStrategy(QSqlTableModel::OnFieldChange);
}

//WEGData对应Class的析构函数
MyQtSQL_WEG::~MyQtSQL_WEG()
{
    //删除TableModel并关闭DataBase
    delete WegTabModel;
    MyDB.close();
    QSqlDatabase::removeDatabase(MyDB.connectionName());

}

//WEGData中添加Users的函数（添加行，仅录入ID和Name）
bool MyQtSQL_WEG::AddUsers(QString Tem_ID, QString Tem_Name)
{
    //ID和Name必须非空才可以录入，否则跳出函数
    if((Tem_ID == "") || (Tem_Name == ""))
        return false;

    //用QSqlQuery的指令直接添加对象
    QSqlQuery query;
    query.prepare("INSERT INTO WEGData (ID, Name) VALUES (?, ?)");
    query.addBindValue(Tem_ID);
    query.addBindValue(Tem_Name);
    if (query.exec())
    {
        return true;
    }
    else
    {
        return false;
    }
}

//WEGData中删除Users的函数（删除行）
bool MyQtSQL_WEG::DeleteUsers(QString Tem_ID)
{
    //ID非空时才会录入，否则跳出函数
    if(Tem_ID == "")
        return false;
    //用QSqlQuery的指令直接删除对象
    QSqlQuery query;
    if (query.exec("DELETE FROM WEGData WHERE ID = " + Tem_ID))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//WEGData中添加Month对应的数据的函数（一次性添加三列）
void MyQtSQL_WEG::AddMonth(int Month)
{
    for(int i=0; i<3; i++)
    {
        //用QSqlQuery的指令直接添加对象
        QSqlQuery query;
        //制作ColName
        QString ColName = " Month";
        switch(i)
        {
            case 0:
                ColName += "W"; break;
            case 1:
                ColName += "E"; break;
            case 2:
                ColName += "G"; break;
        }
        ColName += QString::number(Month);
        ColName += " ";
        //生成ColType
        QString ColType = " REAL DEFAULT (0)";
        //添加列
        if (query.exec("ALTER TABLE WEGData ADD COLUMN " + ColName + ColType))
        {
            qDebug() << "添加列成功！";
        }
        else
        {
            qDebug() << "添加列失败：" << query.lastError().text();
        }
    }
}

//WEGData中修改特定Month对应数据的函数（一次性修改三个数据）
bool MyQtSQL_WEG::ChangeData(float Water, float Elec, float Gas, bool Water_check, bool Elec_check, bool Gas_check, int Month, QString ID)
{
    //寻找对应ID
    for(int row=0; row<WegTabModel->rowCount(); row++)
    {
        QSqlRecord Tem_Record = WegTabModel->record(row);
        if(Tem_Record.value("ID") == ID)
        {
            //只要在界面输入的数据非空，则修改数据
            if(Water_check)
                WegTabModel->setData(WegTabModel->index(row, WegTabModel->fieldIndex("Month"+("W"+QString::number(Month)))), Water);
            if(Elec_check)
                WegTabModel->setData(WegTabModel->index(row, WegTabModel->fieldIndex("Month"+("E"+QString::number(Month)))), Elec);
            if(Gas_check)
                WegTabModel->setData(WegTabModel->index(row, WegTabModel->fieldIndex("Month"+("G"+QString::number(Month)))), Gas);
            return true;
        }
    }
    return false;
}

//获取某个成员某个月的数据
bool MyQtSQL_WEG::ShowData(MemberData* Tem_MemberData, int Month, QString Stu_ID, OtherData Ot_Data)
{
    //寻找对应ID的成员
    for(int row=0; row<WegTabModel->rowCount(); row++)
    {
        QSqlRecord Tem_Record = WegTabModel->record(row);
        if(Tem_Record.value("ID") == Stu_ID)
        {
            //读取信息
            QString Str_Month = QString::number(Month);
            Tem_MemberData->Water = Tem_Record.value("MonthW"+Str_Month).toFloat();
            Tem_MemberData->Elec  = Tem_Record.value("MonthE"+Str_Month).toFloat();
            Tem_MemberData->Gas   = Tem_Record.value("MonthG"+Str_Month).toFloat();
            Tem_MemberData->Money = ComputeData(*Tem_MemberData, Ot_Data);
            return true;
        }
    }
    return false;
}

//预测函数
/*
    预测数据基于最新三个月的非零数据做均值运算
    若最新的三个月的数据均为零则预测数据为零
*/
void MyQtSQL_WEG::PredictData(MemberData* Pre_Data, int Current_Month, QString Stu_ID, OtherData Ot_Data)
{
    //初始化水/电/煤气的预测数据
    Pre_Data->Water = 0;
    Pre_Data->Elec = 0;
    Pre_Data->Gas = 0;
    Pre_Data->Money = 0;
    int Count[3] = {0, 0 ,0};//记录预测基于的数据的个数

    for(int i = Current_Month - 2; i <= Current_Month; i++)
    {
        //取出最新三个月的数据
        MemberData Tem_Predict_Data;
        ShowData(&Tem_Predict_Data, i, Stu_ID, Ot_Data);
        //统计数据
        if(Tem_Predict_Data.Water != 0)
        {
            Pre_Data->Water += Tem_Predict_Data.Water;
            Count[0] ++;
            qDebug() << "预测数据基数据：" << Tem_Predict_Data.Water << " 计次：" << Count[0];
        }
        if(Tem_Predict_Data.Elec != 0)
        {
            Pre_Data->Elec += Tem_Predict_Data.Elec;
            Count[1] ++;
            qDebug() << "预测数据基数据：" << Tem_Predict_Data.Elec << " 计次：" << Count[1];
        }
        if(Tem_Predict_Data.Gas != 0)
        {
            Pre_Data->Gas += Tem_Predict_Data.Gas;
            Count[2] ++;
            qDebug() << "预测数据基数据：" << Tem_Predict_Data.Gas << " 计次：" << Count[2];

        }
    }
    //若最新三个月的数据均为零，修改Count为1（避免后续运算分母为0）
    for(int i=0; i<3; i++)
        if(Count[i] == 0)
        {
            Count[i] = 1;
        }
    //做均值运算
    Pre_Data->Water /= Count[0];
    Pre_Data->Elec  /= Count[1];
    Pre_Data->Gas   /= Count[2];


}

//获取全体成员某个月某项的数据
std::vector<float> MyQtSQL_WEG::TotalShowData(std::vector<float> Tem_MemberDataS, int Month, int Aspect_Index)
{
    //获取全体成员的数据
    for(int row=0; row<WegTabModel->rowCount(); row++)
    {
        QSqlRecord Tem_Record = WegTabModel->record(row);
        QString Str_Month = QString::number(Month);
        switch (Aspect_Index)
        {
        case 0:
            Tem_MemberDataS.push_back(Tem_Record.value("MonthW"+Str_Month).toFloat());
            break;
        case 1:
            Tem_MemberDataS.push_back(Tem_Record.value("MonthE"+Str_Month).toFloat());
            break;
        case 2:
            Tem_MemberDataS.push_back(Tem_Record.value("MonthG"+Str_Month).toFloat());
            break;
        }
    }
    return Tem_MemberDataS;
}

//分析全体成员某个月某项的数据
void MyQtSQL_WEG::TotalDataAnalysis(std::vector<float> Tem_MemberDataS, TotalData* the_TotalData)
{
    //临时统计数据的变量
    float max = 0;
    float min = 0;
    float Total = 0;
    float Total_vari;
    int Count = 1;
    int NoZeroFirstIndex;
    //查找首个不为零的数据并进行统计（为零的数据不进入统计，除非全部为0）
    for(int i=0; i<Tem_MemberDataS.size(); i++)
    {
        if(Tem_MemberDataS[i] != 0)
        {
            max = Tem_MemberDataS[i];
            min = Tem_MemberDataS[i];
            Total = Tem_MemberDataS[i];

            NoZeroFirstIndex = i;
            break;
        }
    }
    //统计剩下的数据
    for(int i=NoZeroFirstIndex + 1; i<Tem_MemberDataS.size(); i++)
    {
        if(Tem_MemberDataS[i] != 0)
        {
            Count ++;
            if(max < Tem_MemberDataS[i])
                max = Tem_MemberDataS[i];
            if(min > Tem_MemberDataS[i])
                min = Tem_MemberDataS[i];
            Total += Tem_MemberDataS[i];
        }

    }
    //计算得到最值和均值
    the_TotalData->MaxData = max;
    the_TotalData->MinData = min;
    the_TotalData->AverData = Total / Count;
    //若均值为零，则方差为零（数据全为非负数）
    if(the_TotalData->AverData == 0)
        the_TotalData->VariData = 0;
    //均值不为零时，利用公式计算方差
    else
    {
        Total_vari = (Tem_MemberDataS[NoZeroFirstIndex] - the_TotalData->AverData) * (Tem_MemberDataS[NoZeroFirstIndex] - the_TotalData->AverData);
        qDebug() << (Tem_MemberDataS[NoZeroFirstIndex] - the_TotalData->AverData) * (Tem_MemberDataS[NoZeroFirstIndex] - the_TotalData->AverData);
        for(int i=NoZeroFirstIndex + 1; i<Tem_MemberDataS.size(); i++)
        {
            if(Tem_MemberDataS[i] != 0)
            {
                Total_vari += (Tem_MemberDataS[i] - the_TotalData->AverData) * (Tem_MemberDataS[i] - the_TotalData->AverData);
                qDebug() << (Tem_MemberDataS[i] - the_TotalData->AverData) * (Tem_MemberDataS[i] - the_TotalData->AverData);
            }

        }
        the_TotalData->VariData = Total_vari / Count;
    }
}

//计算金额的公式
float MyQtSQL_WEG::ComputeData(MemberData The_MemberData, OtherData The_Data)
{
    float Tem_Money;
    Tem_Money =\
        (The_MemberData.Water * The_Data.UPrice_Water)+\
        (The_MemberData.Elec * The_Data.UPrice_Elec)+\
        (The_MemberData.Gas * The_Data.UPrice_Gas);
    return Tem_Money;
}

//OtherData的构造函数
MyQtSQL_Other::MyQtSQL_Other(Widget* WidPtr)
{
    //连接DataBase
    QString exeDir = QCoreApplication::applicationDirPath();
    QString dbPath = exeDir + "/UserData.db";
    MyDB = QSqlDatabase::addDatabase("QSQLITE");
    MyDB.setDatabaseName(dbPath);
    MyDB.open();

    //生成OtherData的Table对应的TableModel
    OtherTabModel = new QSqlTableModel(WidPtr, MyDB);
    OtherTabModel->setTable("OtherData");
    if(OtherTabModel->select())
        qDebug() << "成功打开Table\n";
    else
        qDebug() << "失败打开Table" << OtherTabModel->lastError().text() << "\n";
    OtherTabModel->setEditStrategy(QSqlTableModel::OnFieldChange);
}

//OtherData的析构函数
MyQtSQL_Other::~MyQtSQL_Other()
{
    delete OtherTabModel;
    MyDB.close();
    QSqlDatabase::removeDatabase(MyDB.connectionName());
}

//获取OtherData里的数据并传到结构体中的函数
void MyQtSQL_Other::GetData(OtherData & Tem_OtherData)
{
    //从SQL中获取数据（使用STL容器的动态数组）
    std::vector<QSqlRecord> TemRecord;
    for(int i=3; i>=0; i--)
        TemRecord.push_back(OtherTabModel->record(i));

    //将数据传导到结构体中
    Tem_OtherData.Month = TemRecord[0].value("Mess").toInt();
    Tem_OtherData.UPrice_Water = TemRecord[1].value("Mess").toFloat();
    Tem_OtherData.UPrice_Elec = TemRecord[2].value("Mess").toFloat();
    Tem_OtherData.UPrice_Gas = TemRecord[3].value("Mess").toFloat();
}

//修改OtherData里的数据的函数
void MyQtSQL_Other::SendData(float Data, int index)
{
    //通过传入的下标修改Mess行（位于Table中）的数据
    QSqlRecord Month_record = OtherTabModel->record(index - 1);
    OtherTabModel->setData(OtherTabModel->index(index - 1, OtherTabModel->fieldIndex("Mess")), Data);
    if (OtherTabModel->submitAll())
        qDebug() << "修改成功！";
    else
        qDebug() << "修改失败：" << OtherTabModel->lastError().text();
}
