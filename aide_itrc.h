#ifndef AIDE_ITRC_H
#define AIDE_ITRC_H

#include "aide_conf.h"

// 负责与命令行参数的交互
class Interactor
{

public:
    // 分析指令的类型
    static int GetFunctionType(int argc, char *argv[]);
    // 交互解析用户输入的项目信息
    static Project AnalyseNewProjInfo(bool &res);
    // 解析用户输入的修改信息内容
    static ChangeInfo AnalyseChangeInfo(int argc, char *argv[], int id, bool &res);
    // 弹出用于用户确认的项目信息修改情况，并由用户决定，返回决定结果
    static bool EnsureAdjustInfo(Project ori_proj, Project new_proj);
    // Output the project information for ensuring erase.
    static bool EnsureEraseInfo(Project proj);
    // Output the project information for ensuring finish.
    static bool EnsureFinishInfo(Project proj);
    // 去除头尾空格
    static string &Trim(string &ori_str);
    
};

#endif
