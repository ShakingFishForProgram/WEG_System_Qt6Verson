#pragma once
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <vector>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

struct OtherData{
    int Month;
    float UPrice_Water;
    float UPrice_Elec;
    float UPrice_Gas;
}; 

struct TotalData{
    float MaxData;
    float MinData;
    float AverData;
    float VariData;
};

struct MemberData{
    float Water;
    float Elec;
    float Gas;
    float Money;

    MemberData();
};


class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_UpdateUserCreateButton_clicked();

    void on_CreateExitButton_clicked();

    void on_CreateRegisterButton_clicked();

    void on_MainOptionButton_clicked();

    void on_MainUpdateUserButton_clicked();

    void on_UpdateUserExitButton_clicked();

    void on_UpdateUserDeleteButton_clicked();

    void on_DeleteExitButton_clicked();

    void on_DeleteButton_clicked();

    void on_OptionUpdateTimeButton_clicked();

    void on_OptionUPriceBox_currentIndexChanged(int index);

    void on_OptionExitButton_clicked();

    void on_OptionUPriceButton_clicked();

    void on_MainDataInButton_clicked();

    void on_DataInExitButton_clicked();

    void on_DataInInputButton_clicked();

    void on_OptionIntroduceButton_clicked();

    void on_IntroduceExitButton_clicked();

    void on_MainAnalyseButton_clicked();

    void on_AnalysisExitButton_clicked();

    void on_AnalysisPersonalButton_clicked();

    void on_AnalysisPersonalExitButton_clicked();

    void on_DataInMonth1Box_currentIndexChanged(int index);

    void on_AnalysisPersonalMonth1Box_currentIndexChanged(int index);

    void on_AnalysisPersonalFindButton_clicked();

    void on_AnalysisTotalButton_clicked();

    void on_AnalysisTotalExitButton_clicked();

    void on_AnalysisTotalFindButton_clicked();

    void on_AnalysisTotalMonth1Box_activated(int index);

private:
    Ui::Widget *ui;
    OtherData the_OtherData;
};
#endif // WIDGET_H
