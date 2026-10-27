#include "widget.h"
#include "ui_widget.h"
#include "MyQtSQL.h"
MemberData::MemberData()
{
    Water = 0;
    Elec = 0;
    Gas = 0;
    Money = 0;
}

//构造函数
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    //构造对象
    ui->setupUi(this);
    //启动后显示主界面
    ui->stackedWidget->setCurrentWidget(ui->MainPage);
    //初始化其他数据
    MyQtSQL_Other Tem_OtherData(this);
    Tem_OtherData.GetData(the_OtherData);
    //重置DataIn的Box的菜单（清空后设置选项）
    ui->DataInMonth1Box->clear();
    for(int i=0; i<=(the_OtherData.Month - 1) / 12; i++)
        ui->DataInMonth1Box->addItem(QString::number(2024 + i));

    connect(ui->DataInGasLineEdit, SIGNAL(returnPressed()), this, SLOT(on_DataInInputButton_clicked()));

}

//析构函数
Widget::~Widget()
{
    delete ui;
}

//主界面的“用户更新”
void Widget::on_MainUpdateUserButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->UpdateUserPage);//跳转到“用户更新”界面
}

void Widget::on_UpdateUserCreateButton_clicked()
{
    ui->CreatCheckLabel->setText("");
    ui->CreateIDLineEdit->clear();
    ui->CreateNameLineEdit->clear();
    ui->stackedWidget->setCurrentWidget(ui->CreateTitlePage);
}

//主界面的“数据录入”
void Widget::on_MainDataInButton_clicked()
{
    //清空“数据录入”界面的四条输入框
    ui->DataInIDLineEdit->clear();
    ui->DataInWaterLineEdit->clear();
    ui->DataInElecLineEdit->clear();
    ui->DataInGasLineEdit->clear();
    //清空“数据录入”界面的CheckLabel
    ui->DataInCheckLabel->setText("");
    //重置Box
    ui->DataInMonth1Box->clear();
    for(int i=0; i<=(the_OtherData.Month - 1) / 12; i++)
        ui->DataInMonth1Box->addItem(QString::number(2024 + i));
    ui->DataInMonth2Box->clear();
    for(int i=0; i<12; i++)
        ui->DataInMonth2Box->addItem(QString::number(1 + i));
    //有关时间的Box下标设置为0
    ui->DataInMonth1Box->setCurrentIndex(0);
    ui->DataInMonth2Box->setCurrentIndex(0);
    //跳转到“数据录入”界面
    ui->stackedWidget->setCurrentWidget(ui->DataInPage);
}

//主界面的“信息分析”
void Widget::on_MainAnalyseButton_clicked()
{
    //跳转到“信息分析”界面
    ui->stackedWidget->setCurrentWidget(ui->AnalysisPage);
}


//主界面的“选项设置”
void Widget::on_MainOptionButton_clicked()
{
    //计算当前时间并打印到屏幕
    QString Tem_year = QString::number(2024 + (the_OtherData.Month - 1) / 12);
    QString Tem_Month = QString::number((the_OtherData.Month - 1) % 12 + 1);
    ui->OptionMonthData1Label->setText(Tem_year);
    ui->OptionMonthData2Label->setText(Tem_Month);
    //第二行选项设置：Box为首项、输入框清空
    ui->OptionUPriceBox->setCurrentIndex(0);
    ui->OptionUPriceOriginalLabel->setText(QString::number(the_OtherData.UPrice_Water));
    ui->OptionUPriceLineEdit->clear();
    //跳转到“选项设置”界面
    ui->stackedWidget->setCurrentWidget(ui->OptionPage);
}

//“用户更新”的“返回”
void Widget::on_UpdateUserExitButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->MainPage);//跳转回主界面
}

//“用户更新”的“删除用户”
void Widget::on_UpdateUserDeleteButton_clicked()
{
    ui->DeleteCheckLabel->setText("");//清空CheckLabel
    ui->DeleteIDLineEdit->clear();//清空“删除用户”中的输入框
    ui->stackedWidget->setCurrentWidget(ui->DeletePage);//跳转到“删除用户”界面
}

//“用户创建”的“注册”
void Widget::on_CreateRegisterButton_clicked()
{
    //提取文字
    QString Tem_ID = ui->CreateIDLineEdit->text();
    QString Tem_Name = ui->CreateNameLineEdit->text();
    //传入数据
    MyQtSQL_WEG Tem_WEG(this);
    bool Tem_Check = Tem_WEG.AddUsers(Tem_ID, Tem_Name);
    //界面打印打印CheckLable
    if(Tem_Check)
        ui->CreatCheckLabel->setText("注册成功！");
    else
        ui->CreatCheckLabel->setText("注册失败！");
    //清空输入框
    ui->CreateIDLineEdit->clear();
    ui->CreateNameLineEdit->clear();
}

//“用户创建”的“返回”
void Widget::on_CreateExitButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->UpdateUserPage);//返回到“用户更新”界面
}

//“删除用户”的“删除”
void Widget::on_DeleteButton_clicked()
{
    //获取输入框的ID
    QString Tem_ID = ui->DeleteIDLineEdit->text();
    //创建连接WEGData的对象并删除对应信息
    MyQtSQL_WEG Tem_WEG(this);
    bool Tem_Check = Tem_WEG.DeleteUsers(Tem_ID);
    //打印CheckLabel
    if(Tem_Check)
        ui->DeleteCheckLabel->setText("删除成功！");
    else
        ui->DeleteCheckLabel->setText("删除失败！");
    //清空输入框
    ui->DeleteIDLineEdit->clear();
}

//“删除用户”的“返回”
void Widget::on_DeleteExitButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->UpdateUserPage);//跳转回主界面
}


//“数据录入”的“录入”
void Widget::on_DataInInputButton_clicked()
{
    //通过Box获取需要录入的数据对应的时间
    int Tem_Year = ui->DataInMonth1Box->currentText().toInt();
    int Tem_Month = ui->DataInMonth2Box->currentText().toInt();
    int total_Month = (Tem_Year - 2024) * 12 + Tem_Month;
    //获取输入框的ID
    QString Tem_ID = ui->DataInIDLineEdit->text();
    //获取需要修改的信息（水/电/煤气）
    QString Tem_Str_Water = ui->DataInWaterLineEdit->text();
    QString Tem_Str_Elec = ui->DataInElecLineEdit->text();
    QString Tem_Str_Gas = ui->DataInGasLineEdit->text();
    //修改的信息（浮点格式）
    float Tem_Water = 0;
    float Tem_Elec = 0;
    float Tem_Gas = 0;
    bool Water_check = false;
    bool Elec_check = false;
    bool Gas_check = false;
    /*
        这一步的目的主要是判断输入框中修改的信息是否为空
        在之后传入的函数中若此处的Check为False则不修改SQL中的数据
    */
    if(Tem_Str_Water != "")
    {
        Tem_Water = Tem_Str_Water.toFloat();
        Water_check = true;
    }
    if(Tem_Str_Elec != "")
    {
        Tem_Elec = Tem_Str_Elec.toFloat();
        Elec_check = true;
    }
    if(Tem_Str_Gas != "")
    {
        Tem_Gas = Tem_Str_Gas.toFloat();
        Gas_check = true;
    }
    //构造连接WEGData的对象并修改数据
    MyQtSQL_WEG Tem_WEG(this);
    bool Success_Check = Tem_WEG.ChangeData(Tem_Water, Tem_Elec, Tem_Gas, Water_check, Elec_check, Gas_check, total_Month, Tem_ID);
    //打印CheckLabel
    if(Success_Check)
        ui->DataInCheckLabel->setText("修改成功！");
    else
        ui->DataInCheckLabel->setText("修改失败！");
    //清空四个输入框
    ui->DataInWaterLineEdit->clear();
    ui->DataInElecLineEdit->clear();
    ui->DataInGasLineEdit->clear();
}

//修改“数据录入”有关年份的Box后有关月的Box对应会产生修改
void Widget::on_DataInMonth1Box_currentIndexChanged(int index)
{
    //最后的年份的时候要考虑对应的月不满的条件
    if(index == ((the_OtherData.Month - 1) / 12))
    {
        ui->DataInMonth2Box->clear();
        for(int i=0; i<((the_OtherData.Month - 1) % 12 + 1); i++)
            ui->DataInMonth2Box->addItem(QString::number(i + 1));
    }
    //其他的时候的月份是1-12月
    else
    {
        ui->DataInMonth2Box->clear();
        for(int i=0; i<12; i++)
            ui->DataInMonth2Box->addItem(QString::number(i + 1));
    }
}

//“数据录入”的返回
void Widget::on_DataInExitButton_clicked()
{
    //跳转回主界面
    ui->stackedWidget->setCurrentWidget(ui->MainPage);
}

//“信息分析”界面的“个人分析”
void Widget::on_AnalysisPersonalButton_clicked()
{
    ui->AnalysisPersonalMonth1Box->clear();
    for(int i=0; i<=((the_OtherData.Month - 1) / 12); i++)
    {
        ui->AnalysisPersonalMonth1Box->addItem(QString::number(2024 + i));
    }
    ui->AnalysisPersonalMonth2Box->clear();
    for(int i=0; i<12; i++)
        ui->AnalysisPersonalMonth2Box->addItem(QString::number(1 + i));
    ui->AnalysisPersonalIDLineEdit->clear();

    ui->AnalysisPersonalMonth1Box->setCurrentIndex(0);
    ui->AnalysisPersonalMonth2Box->setCurrentIndex(0);

    ui->AnalysisPersonalWaterDataLabel->clear();
    ui->AnalysisPersonalElecDataLabel->clear();
    ui->AnalysisPersonalGasDataLabel->clear();
    ui->AnalysisPersonalMoneyDataLabel->clear();

    ui->AnalysisPersonalWaterText2Label->clear();
    ui->AnalysisPersonalElecText2Label->clear();
    ui->AnalysisPersonalGasText2Label->clear();
    ui->AnalysisPersonalMoneyText2Label->clear();

    ui->AnalysisPersonalPredictTextLabel->clear();

    ui->AnalysisPersonalPredictWaterDataLabel->clear();
    ui->AnalysisPersonalPredictElecDataLabel->clear();
    ui->AnalysisPersonalPredictGasDataLabel->clear();

    ui->AnalysisPersonalPredictWaterTextLabel->clear();
    ui->AnalysisPersonalPredictElecTextLabel->clear();
    ui->AnalysisPersonalPredictGasTextLabel->clear();

    //跳转到“个人分析”界面
    ui->stackedWidget->setCurrentWidget(ui->AnalysisPersonalPage);
}

//“信息分析”里的“总体分析”
void Widget::on_AnalysisTotalButton_clicked()
{
    //清空Box后重置
    ui->AnalysisTotalMonth1Box->clear();
    for(int i=0; i<=((the_OtherData.Month - 1) / 12); i++)
    {
        ui->AnalysisTotalMonth1Box->addItem(QString::number(2024 + i));
    }
    ui->AnalysisTotalMonth2Box->clear();
    for(int i=0; i<12; i++)
        ui->AnalysisTotalMonth2Box->addItem(QString::number(1 + i));
    //设定Box显示下标
    ui->AnalysisTotalMonth1Box->setCurrentIndex(0);
    ui->AnalysisTotalMonth2Box->setCurrentIndex(0);
    ui->AnalysisTotalAspectBox->setCurrentIndex(0);
    //清空数据部分的值
    ui->AnalysisTotalMaxDataLabel->clear();
    ui->AnalysisTotalMinDataLabel->clear();
    ui->AnalysisTotalAverDataLabel->clear();
    ui->AnalysisTotalVariDataLabel->clear();
    //清空结尾单位
    ui->AnalysisTotalMinText2Label->clear();
    ui->AnalysisTotalMaxText2Label->clear();
    ui->AnalysisTotalAverText2Label->clear();

    ui->stackedWidget->setCurrentWidget(ui->AnalysisTotalPage);
}

//“信息分析”界面的“返回”
void Widget::on_AnalysisExitButton_clicked()
{
    //返回主界面
    ui->stackedWidget->setCurrentWidget(ui->MainPage);
}

//“个人分析”的有关年的Box
void Widget::on_AnalysisPersonalMonth1Box_currentIndexChanged(int index)
{
    //最后的年份的时候要考虑对应的月不满的条件
    if(index == ((the_OtherData.Month - 1) / 12))
    {
        ui->AnalysisPersonalMonth2Box->clear();
        for(int i=0; i<((the_OtherData.Month - 1) % 12 + 1); i++)
            ui->AnalysisPersonalMonth2Box->addItem(QString::number(i + 1));
    }
    //其他的时候的月份是1-12月
    else
    {
        ui->AnalysisPersonalMonth2Box->clear();
        for(int i=0; i<12; i++)
            ui->AnalysisPersonalMonth2Box->addItem(QString::number(i + 1));
    }
}

//“个人分析”的“查询”
void Widget::on_AnalysisPersonalFindButton_clicked()
{
    //获取时间
    QString Tem_Str_ID = ui->AnalysisPersonalIDLineEdit->text();
    int Tem_Year = ui->AnalysisPersonalMonth1Box->currentText().toInt();
    int Tem_Month = ui->AnalysisPersonalMonth2Box->currentText().toInt();
    int Total_Month = (Tem_Year - 2024) * 12 + Tem_Month;
    //获取信息
    MemberData Tem_MemberData;
    MyQtSQL_WEG Tem_WEG(this);
    bool Tem_Check_Id = Tem_WEG.ShowData(&Tem_MemberData, Total_Month, Tem_Str_ID, the_OtherData);
    //ID不存在的情况：清空选项
    if(!Tem_Check_Id)
    {
        //清空查询的数据的值
        ui->AnalysisPersonalWaterDataLabel->clear();
        ui->AnalysisPersonalElecDataLabel->clear();
        ui->AnalysisPersonalGasDataLabel->clear();
        ui->AnalysisPersonalMoneyDataLabel->clear();
        //清空查询的数据的单位
        ui->AnalysisPersonalWaterText2Label->clear();
        ui->AnalysisPersonalElecText2Label->clear();
        ui->AnalysisPersonalGasText2Label->clear();
        ui->AnalysisPersonalMoneyText2Label->clear();
        //清空“未来预测”
        ui->AnalysisPersonalPredictTextLabel->clear();
        //清空“未来预测”的数据
        ui->AnalysisPersonalPredictWaterDataLabel->clear();
        ui->AnalysisPersonalPredictElecDataLabel->clear();
        ui->AnalysisPersonalPredictGasDataLabel->clear();
        //清空“未来预测”的数据的单位
        ui->AnalysisPersonalPredictWaterTextLabel->clear();
        ui->AnalysisPersonalPredictElecTextLabel->clear();
        ui->AnalysisPersonalPredictGasTextLabel->clear();

        return;
    }
    //ID存在的情况，直接输入
    ui->AnalysisPersonalWaterDataLabel->setText(QString::number(Tem_MemberData.Water));
    ui->AnalysisPersonalElecDataLabel->setText(QString::number(Tem_MemberData.Elec));
    ui->AnalysisPersonalGasDataLabel->setText(QString::number(Tem_MemberData.Gas));
    ui->AnalysisPersonalMoneyDataLabel->setText(QString::number(Tem_MemberData.Money));

    ui->AnalysisPersonalWaterText2Label->setText("吨");
    ui->AnalysisPersonalElecText2Label->setText("千瓦时");
    ui->AnalysisPersonalGasText2Label->setText("立方米");
    ui->AnalysisPersonalMoneyText2Label->setText("元");
    //设置并计算预测数据，并且打印
    MemberData Predict_MemberData;
    Tem_WEG.PredictData(&Predict_MemberData, the_OtherData.Month, Tem_Str_ID, the_OtherData);
    ui->AnalysisPersonalPredictTextLabel->setText("未来预测：");
    ui->AnalysisPersonalPredictWaterDataLabel->setText(QString::number(Predict_MemberData.Water, 'g', 3));
    ui->AnalysisPersonalPredictElecDataLabel->setText(QString::number(Predict_MemberData.Elec, 'g', 3));
    ui->AnalysisPersonalPredictGasDataLabel->setText(QString::number(Predict_MemberData.Gas, 'g', 3));
    ui->AnalysisPersonalPredictWaterTextLabel->setText("吨");
    ui->AnalysisPersonalPredictElecTextLabel->setText("千瓦时");
    ui->AnalysisPersonalPredictGasTextLabel->setText("立方米");
}

//“个人分析”界面的“返回”
void Widget::on_AnalysisPersonalExitButton_clicked()
{
    //返回到“信息分析”界面
    ui->stackedWidget->setCurrentWidget(ui->AnalysisPage);
}

//“总体分析”的“返回”
void Widget::on_AnalysisTotalExitButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->AnalysisPage);
}

//“总体分析”里的“查询”
void Widget::on_AnalysisTotalFindButton_clicked()
{
    //获取时间
    int Tem_Year = ui->AnalysisTotalMonth1Box->currentText().toInt();
    int Tem_Month = ui->AnalysisTotalMonth2Box->currentText().toInt();
    int Total_Month = (Tem_Year - 2024) * 12 + Tem_Month;
    int Aspect_Index = ui->AnalysisTotalAspectBox->currentIndex();

    /*
        利用标准模板库中的动态数组（vector）
        获取（某一项）全部数据后通过函数计算最值、均值、方差
    */
    std::vector<float> AllData;
    TotalData Tem_TotalData;
    MyQtSQL_WEG Tem_WEG(this);
    AllData = Tem_WEG.TotalShowData(AllData, Total_Month, Aspect_Index);
    Tem_WEG.TotalDataAnalysis(AllData, &Tem_TotalData);
    //通过label打印数据
    switch (Aspect_Index) {
    case 0:
        ui->AnalysisTotalMinText2Label->setText("吨");
        ui->AnalysisTotalMaxText2Label->setText("吨");
        ui->AnalysisTotalAverText2Label->setText("吨");
        break;
    case 1:
        ui->AnalysisTotalMinText2Label->setText("千瓦时");
        ui->AnalysisTotalMaxText2Label->setText("千瓦时");
        ui->AnalysisTotalAverText2Label->setText("千瓦时");
        break;
    case 2:
        ui->AnalysisTotalMinText2Label->setText("立方米");
        ui->AnalysisTotalMaxText2Label->setText("立方米");
        ui->AnalysisTotalAverText2Label->setText("立方米");
        break;
    }

    ui->AnalysisTotalMaxDataLabel->setText(QString::number(Tem_TotalData.MaxData));
    ui->AnalysisTotalMinDataLabel->setText(QString::number(Tem_TotalData.MinData));
    ui->AnalysisTotalAverDataLabel->setText(QString::number(Tem_TotalData.AverData, 'g', 3));
    ui->AnalysisTotalVariDataLabel->setText(QString::number(Tem_TotalData.VariData, 'g', 3));
}

//“总体分析”有关年的Box
void Widget::on_AnalysisTotalMonth1Box_activated(int index)
{
    //最后的年份的时候要考虑对应的月不满的条件
    if(index == ((the_OtherData.Month - 1) / 12))
    {
        ui->AnalysisTotalMonth2Box->clear();
        for(int i=0; i<((the_OtherData.Month - 1) % 12 + 1); i++)
            ui->AnalysisTotalMonth2Box->addItem(QString::number(i + 1));
    }
    //其他的时候的月份是1-12月
    else
    {
        ui->AnalysisTotalMonth2Box->clear();
        for(int i=0; i<12; i++)
            ui->AnalysisTotalMonth2Box->addItem(QString::number(i + 1));
    }
}

//“选项设置”的“更新时间”
void Widget::on_OptionUpdateTimeButton_clicked()
{
    //时间自增
    the_OtherData.Month ++;
    //重新计算当前时间并打印
    QString Tem_year = QString::number(2024 + (the_OtherData.Month - 1) / 12);
    QString Tem_Month = QString::number((the_OtherData.Month - 1) % 12 + 1);
    ui->OptionMonthData1Label->setText(Tem_year);
    ui->OptionMonthData2Label->setText(Tem_Month);
    //创建连接WEGData的对象并添加新的列
    MyQtSQL_WEG Tem_WEG(this);
    Tem_WEG.AddMonth(the_OtherData.Month);
    //创建连接OtherData的对象并修改数据
    MyQtSQL_Other Tem_OtherData(this);
    Tem_OtherData.SendData((float)the_OtherData.Month, 4);
    //存疑，好像可以删除
    if(the_OtherData.Month%12 == 1)
        ui->DataInMonth1Box->addItem(Tem_year);
}

//“选项设置”第二行的Box发生改变对应的槽函数
void Widget::on_OptionUPriceBox_currentIndexChanged(int index)
{
    //根据Box的改变改变显示的单价
    switch(index)
    {
        case 0:
            ui->OptionUPriceOriginalLabel->setText(QString::number(the_OtherData.UPrice_Water)); break;
        case 1:
            ui->OptionUPriceOriginalLabel->setText(QString::number(the_OtherData.UPrice_Elec)); break;
        case 2:
            ui->OptionUPriceOriginalLabel->setText(QString::number(the_OtherData.UPrice_Gas)); break;
    }
}

//“选项设置”的“返回”
void Widget::on_OptionExitButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->MainPage);
}

//“选项设置”第二行的“确定”
void Widget::on_OptionUPriceButton_clicked()
{
    //获取修改后的参数
    QString Tem_UPrice_Str = ui->OptionUPriceLineEdit->text();
    //不输入则跳出函数
    if(Tem_UPrice_Str == "")
        return;
    //获取需要修改的数据的下标
    int Box_Index = ui->OptionUPriceBox->currentIndex();
    //创建连接OtherData的对象并修改Table中的值
    MyQtSQL_Other Tem_OtherData(this);
    switch(Box_Index)
    {
        case 0:
            the_OtherData.UPrice_Water = Tem_UPrice_Str.toFloat();
            Tem_OtherData.SendData(the_OtherData.UPrice_Water, 3);
            break;
        case 1:
            the_OtherData.UPrice_Elec = Tem_UPrice_Str.toFloat();
            Tem_OtherData.SendData(the_OtherData.UPrice_Elec, 2);
            break;
        case 2:
            the_OtherData.UPrice_Gas = Tem_UPrice_Str.toFloat();
            Tem_OtherData.SendData(the_OtherData.UPrice_Gas, 1);
            break;
    }
    //清空该行输入框
    ui->OptionUPriceLineEdit->clear();
    //更新该行标签显示的单价的数据
    ui->OptionUPriceOriginalLabel->setText(Tem_UPrice_Str);
}

//“选项设置”中的超长按钮
void Widget::on_OptionIntroduceButton_clicked()
{
    //跳转超长按钮对应界面
    ui->stackedWidget->setCurrentWidget(ui->IntroducePage);
}

//超长按钮对应界面的“返回按钮”
void Widget::on_IntroduceExitButton_clicked()
{
    //返回到“选项设置”界面
    ui->stackedWidget->setCurrentWidget(ui->OptionPage);
}
