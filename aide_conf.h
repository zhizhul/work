#ifndef AIDE_CONF_H
#define AIDE_CONF_H
	
#include <string>
#include <vector>

using namespace std;

// 项目的数据结构
class Project
{

private:
    // 项目的名称
    string name;
    // 项目的信息
    string info;
    // 项目的计划完成日期，以8字符表示，例如20221118
    string date;
    // 状态
    string status;

public:
    // 初始化项目
    void Init(string in_name, string in_info, string in_date);
    // 修改项目名称
    void ChangeName(string in_name);
    // 修改计划完成日期
    void ChangeDate(string in_date);
    // 修改项目的信息
    void ChangeInfo(string in_info);
    // 修改项目的状态
    void ChangeStatus(string in_status);  
    // 返回项目的名称
    string GetName();
    // 返回项目的信息
    string GetInfo();
    // 返回项目的计划完成日期
    string GetDate();
    // 返回项目的状态
    string GetStatus();
  
};

// 修改信息的数据结构
class ChangeInfo
{

public:
    // 是否修改名称、信息、计划完成日期
    bool change_name = false;
    bool change_info = false;
    bool change_date = false;
    // 准备修改的名称、信息、计划完成日期
    string new_name = "";
    string new_info = "";
    string new_date = "";
    // 初始化修改信息
    void Init(bool cn, bool ci, bool cd, string nn, string ni, string nd);
    
};

// 项目显示信息的解析数据结构
typedef struct ShowInfo
{
    string fullname;
    string info;
    string date;
    string name_id;
    int id;
    int pid;
    int sub_level;
    vector<ShowInfo> sub_projs;   
} ShowInfo;

// 定义要实现的功能
enum Function
{
  // 创建项目
  create,
  // 调整项目内容
  adjust,
  // 删除项目
  erase,
  // 完成项目
  finish,
  // 列出所有未完成的项目，特别是显示日期要求
  date,
  // 列出所有已完成的项目
  list,
  // 显示所有指令的用法
  help
};

// 定义相关的配置
namespace DatabaseConfig
{
  // 数据文件的所在目录
  const string dir_path = "/home/zhizhul/data/work/";
  // 数据库文件的文件名
  const string db_name = "database";
  // 数据库配置文件的文件名
  const string config_name = "config";
  // 项目的状态：未完成、已完成、被取消
  const string status_false = "0";
  const string status_true = "1";
  const string status_cancel = "2"; 
};

#endif
