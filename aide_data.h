#ifndef AIDE_DATA_H
#define AIDE_DATA_H
	
#include <string>
#include <vector>
#include "aide_conf.h"

using namespace std;

// 负责进行数据检查以及数据库交互
class DataWorker
{
  
public:
    // 检查新项目的名称是否已经在数据库中存在
    static bool CheckProjNameCorrect(string name);
    // 检查新项目的信息是否过长
    static bool CheckProjInfoLong(string info);
    // 检查新项目的日期格式是否正确
    static bool CheckProjDateFormat(string date);
    // 根据名称找到ID号（已取消的不考虑）
    static int GetID(string name);
    // 将已经过检查的项目保存进入数据库中
    static void StoreNewProj(Project new_proj);
    // 对字符串进行切割
    static void StringSplit(const string &str, const string &split, vector<string> &res);
    // 检查ID号是否在数据库中并且处于未完成状态
    static bool CheckProjUndo(int id);
    // 根据ID号获取未完成的项目信息
    static Project GetUndoProj(int id, bool &res);
    // 根据修改信息来形成新的项目信息
    static Project GetRenewProj(Project ori_proj, ChangeInfo change_info);
    // 在数据库中更新对应ID号的数据
    static void RenewUndoProj(int id, Project ori_proj, Project new_proj, ChangeInfo change_info);
    // 修改文件中某一行的数据
    static void ModifyLineData(string file_path, int pos, string new_data);
    // 获取当前数据库中的数据量（项目行数）
    static int GetProjNum();
    // 获取当前数据库中的新建项目号
    static int GetIdPtr();
    // 从命名方式上检查是否为子项目
    static bool IfChildProj(string name);
    // 从子项目的名称解析出父项目的名称
    static string AnalyseParentProjName(string cname);
    // 将新项目数据写入到数据库文件的最后
    static void WriteDataTEnd(string data);
    // 根据新的ID_PTR和LINE_NUM更新数据库配置文件
    static void RenewConfigDataNumInfo(int new_idptr, int new_linenum); 
    // 根据父项目的新名称，递归更新后代项目的名称
    static void RenewCProjectName(int pid, string pname);
    // Cancel the undone project.
    static void EraseUndoProj(int id);
    // Finish the undone project.
    static void FinishUndoProj(int id);
    // Return the now date;
    static string GetNowDate();
    // 解析数据库文件中的所有未完成任务的数据成为特定
    // 格式的字符串，供返回给用户。
    static string AnalyseDeadlineInfo(bool &res);
    // 计算所有未完成项目的最长名称占用长度
    static int AnalyseLongestUProjNLen();
    // 计算未完成项目的名称的显示占用长度
    static int AlyseLgtUProjSLen();
    // 解析未完成项目的显示信息树数据结构
    static ShowInfo AnalyseUPTreeView(int &uproj_num);
    // 将从数据库读取的一行数据解析成显示信息结点
    static ShowInfo TranShowInfo(vector<string> li);
    // 比较项目的vector数据结构的sub-level，用于排序
    static bool CpProjVecSubLev(const vector<string> left, const vector<string> riht);
    // 插入结点到显示信息树数据结构中(用于未完成项目情形)
    static void InsertUPTreeView(ShowInfo &root, ShowInfo node);
    // 插入结点到显示信息树数据结构中(用于已完成项目情形)
    static void InsertDPTreeView(ShowInfo &root, ShowInfo node);
    // 在树数据结构中找到对应ID的结点
    static ShowInfo *FindIdInfo(ShowInfo *node, int id);
    // 将显示信息树数据结构解析为显示到屏幕上的字符串信息
    static void TranUPTreeView(string &data_info, ShowInfo *node, int &uproj_num, int width, int ihspce);
    // 检查该date是否距离今天不足7天
    static bool DeadlineWeek(string date);
    // 检查该date是否距离今天不足1个月
    static bool DeadlineMonth(string date);
    // 获取一个ID项目下面的所有不属于取消状态的子ID集
    static vector<int> GetCProjIDs(int id);
    // 解析数据库文件中已完成的任务为特定格式的字符串，
    // 供返回给用户
    static string AnalyseDoneInfo(bool &res);
    // 计算已完成项目的名称的显示占用长度
    static int AlyseLgtDProjSLen();
    // 解析已完成项目的显示信息树数据结构
    static ShowInfo AnalyseDPTreeView(int &dproj_num);
    // 将显示信息树数据结构解析为显示到屏幕上的字符串信息（已完成项目）
    static void TranDPTreeView(string &data_info, ShowInfo *node, int &dproj_num, int width, int ihspce);
    // 解析字符串的实际屏幕显示长度
    static int GetTerminalSLen(const char *str);
    // 根据宽度要求输出一段字符串
    static string OutputOneLineInfo(const char *info, int &index, bool &hitend, int width);
    
};

#endif
