# 水电煤气管理系统 · WEG System

一个基于 **Qt 6 + SQLite** 的桌面端水电煤气用量管理系统，面向宿舍/合租场景：按人按月登记用水、用电、用气量，自动按单价折算费用，并提供个人趋势预测与全体成员的数据统计。

界面用 `QStackedWidget` 组织成单窗口多页面结构，数据全部存放在与可执行文件同目录的 SQLite 数据库中。

## 功能

| 模块 | 说明 |
|---|---|
| 用户更新 | 按学号（ID）+ 姓名注册新成员；按 ID 删除成员 |
| 数据录入 | 选择年月后录入水/电/煤气用量；**留空的字段不会被覆盖**，只更新填了值的那几项 |
| 个人信息分析 | 查询某人某月的三项用量与折算费用，并给出下一期预测值 |
| 总体分析 | 统计全体成员某月某一指标的最大值、最小值、均值、方差（`0` 值不参与统计） |
| 选项设置 | 修改水/电/煤气单价；推进当前月份；查看系统说明 |

### 计算规则

费用按当前单价折算：

```
Money = Water × UPrice_Water + Elec × UPrice_Elec + Gas × UPrice_Gas
```

未来预测取**最近三个月内的非零值**求算术平均；若三个月全为零，则预测值为 `0`。

总体统计同样跳过 `0` 值（未录入的月份视为缺失而非零消耗），方差按总体方差 `Σ(x-μ)² / n` 计算。

## 界面结构

单窗口内通过 `stackedWidget` 切换的 10 个页面：

| 页面对象名 | 内容 |
|---|---|
| `MainPage` | 主菜单：用户更新 / 数据录入 / 信息分析 / 选项设置 |
| `UpdateUserPage` | 用户更新入口 |
| `CreateTitlePage` | 注册新用户 |
| `DeletePage` | 删除用户 |
| `DataInPage` | 数据录入 |
| `AnalysisPage` | 信息分析入口 |
| `AnalysisPersonalPage` | 个人分析 |
| `AnalysisTotalPage` | 总体分析 |
| `OptionPage` | 选项设置 |
| `IntroducePage` | 系统说明 |

## 技术栈

- **语言**：C++17
- **框架**：Qt 6（`core gui widgets sql`）
- **构建**：qmake，工程文件 `Project3.pro`
- **存储**：SQLite，通过 `QSqlDatabase` / `QSqlTableModel` 操作

## 项目结构

```
WEG_System_Qt6Verson/
├── main.cpp          # 入口，仅创建并显示 Widget
├── widget.h/.cpp     # 主界面逻辑，所有页面跳转与业务槽函数
├── widget.ui         # 界面布局（QStackedWidget 多页面）
├── MyQtSQL.h/.cpp    # 数据访问层，封装两张表的增删改查与统计
└── Project3.pro      # qmake 工程文件
```

数据访问层拆成两个类：

- `MyQtSQL_WEG` —— 管理 `WEGData` 表：注册/删除用户、录入某月数据、读取个人数据、预测、总体统计、费用计算。
- `MyQtSQL_Other` —— 管理 `OtherData` 表：读取与更新单价、当前月份。

## 数据库设计

数据库文件为可执行文件同目录下的 `UserData.db`。

### `OtherData`（全局配置，键值表）

| 列 | 类型 | 说明 |
|---|---|---|
| `Title` | TEXT | 配置项名称 |
| `Mess` | REAL | 配置值 |

**行顺序是硬约定**，代码按下标读取，必须严格保持：

| 行 | Title | 含义 |
|---|---|---|
| 1 | `UGas` | 煤气单价 |
| 2 | `UElec` | 电单价 |
| 3 | `UWater` | 水单价 |
| 4 | `Month` | 当前月份序号（从 `1` 起算，`1` 表示 2024 年 1 月） |

### `WEGData`（成员用量宽表）

| 列 | 类型 | 说明 |
|---|---|---|
| `ID` | TEXT | 主键，学号 |
| `Name` | TEXT | 姓名 |
| `MonthW{n}` | REAL | 第 n 月用水量 |
| `MonthE{n}` | REAL | 第 n 月用电量 |
| `MonthG{n}` | REAL | 第 n 月用气量 |

用量按「一月三列」横向展开，`n` 从 `1` 递增。点击「更新时间」推进月份时，程序会对该表执行 `ALTER TABLE ... ADD COLUMN` 动态补列，因此月份上限由实际使用决定，无需预先规划。

### 月份与日期的换算

`Month` 是自 2024 年 1 月起的连续序号，界面显示的年份与月份由它换算：

```
year  = 2024 + (Month - 1) / 12
month = (Month - 1) % 12 + 1
```

## 构建与运行

### 依赖

- Qt 6（需包含 `Sql` 模块与 SQLite 驱动）
- 支持 C++17 的编译器（MinGW / MSVC 均可）

### 编译

用 Qt Creator 打开 `Project3.pro`，选择 Desktop Qt 6 套件后构建；或命令行：

```bash
qmake Project3.pro
mingw32-make       # MSVC 下为 nmake
```

### 首次运行必须手动建库

程序**不会自动创建表**，只做「打开已有数据库」的操作。若可执行文件同目录下没有 `UserData.db`，启动后会因读不到 `OtherData` 而无法正常工作。

用任意 SQLite 工具建库（或在 exe 同目录执行 `sqlite3 UserData.db` 后粘贴下列语句）：

```sql
CREATE TABLE OtherData (Title TEXT, Mess REAL);

INSERT INTO OtherData (Title, Mess) VALUES
    ('UGas',   9.0),   -- 煤气单价
    ('UElec',  0.3),   -- 电单价
    ('UWater', 1.6),   -- 水单价
    ('Month',  1.0);   -- 当前月份序号，1 = 2024 年 1 月

CREATE TABLE WEGData (
    ID      TEXT PRIMARY KEY UNIQUE NOT NULL,
    Name    TEXT NOT NULL,
    MonthW1 REAL DEFAULT (0), MonthE1 REAL DEFAULT (0), MonthG1 REAL DEFAULT (0),
    MonthW2 REAL DEFAULT (0), MonthE2 REAL DEFAULT (0), MonthG2 REAL DEFAULT (0),
    MonthW3 REAL DEFAULT (0), MonthE3 REAL DEFAULT (0), MonthG3 REAL DEFAULT (0),
    MonthW4 REAL DEFAULT (0), MonthE4 REAL DEFAULT (0), MonthG4 REAL DEFAULT (0),
    MonthW5 REAL DEFAULT (0), MonthE5 REAL DEFAULT (0), MonthG5 REAL DEFAULT (0),
    MonthW6 REAL DEFAULT (0), MonthE6 REAL DEFAULT (0), MonthG6 REAL DEFAULT (0),
    MonthW7 REAL DEFAULT (0), MonthE7 REAL DEFAULT (0), MonthG7 REAL DEFAULT (0),
    MonthW8 REAL DEFAULT (0), MonthE8 REAL DEFAULT (0), MonthG8 REAL DEFAULT (0),
    MonthW9 REAL DEFAULT (0), MonthE9 REAL DEFAULT (0), MonthG9 REAL DEFAULT (0),
    MonthW10 REAL DEFAULT (0), MonthE10 REAL DEFAULT (0), MonthG10 REAL DEFAULT (0),
    MonthW11 REAL DEFAULT (0), MonthE11 REAL DEFAULT (0), MonthG11 REAL DEFAULT (0),
    MonthW12 REAL DEFAULT (0), MonthE12 REAL DEFAULT (0), MonthG12 REAL DEFAULT (0)
);
```

建好后把 `UserData.db` 放到可执行文件所在目录，再启动程序。

## 使用流程

1. 「用户更新」→ 注册成员，填入 ID 与姓名
2. 「数据录入」→ 选择年月，录入该月的三项用量（只填要改的项）
3. 「信息分析」→ 个人分析查看某月用量与费用，总体分析查看全体统计
4. 「选项设置」→ 调整单价；每月月初点「更新时间」推进月份，系统自动为新月份补列

## 说明与已知限制

- `UserData.db` 含真实个人数据，已通过 `.gitignore` 排除，不纳入版本控制；换成新库请按上面的 SQL 重建。
- 数据库路径取自 `QCoreApplication::applicationDirPath()`，因此**换目录运行时会找不到数据**，需要把 `.db` 一起搬走。
- `DeleteUsers` 采用字符串拼接构造 SQL（`"DELETE FROM WEGData WHERE ID = " + ID`），ID 需为数值型；若将来要支持任意格式的 ID，应改为 `prepare()` + `bindValue()` 参数化写法。
- 预测模型为最近三个月非零值的简单均值，不含季节性、趋势项，仅作粗略参考。
- `TotalDataAnalysis` 统计非零值时依赖「首个非零元素」的下标 `NoZeroFirstIndex`；若该月全体成员此项均为 `0`，此变量不会被赋值，后续循环的起点未定义（可能越界）。已知缺陷，尚未修复。
