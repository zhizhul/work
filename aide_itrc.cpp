#include <iostream>
#include <string>
#include <fstream>
#include "aide_conf.h"
#include "aide_itrc.h"
#include "aide_data.h"

using namespace std;

int Interactor::GetFunctionType(int argc, char * argv[])
{
  if (argc <= 1)
  {
    // 参数数量不足
    return -1;
  }
  string type = argv[1];
  if (type == "--create" || type == "-c")
  {
    return Function::create;
  }
  if (type == "--adjust" || type == "-a")
  {
    return Function::adjust;
  }
  if (type == "--erase" || type == "-e")
  {
    return Function::erase;
  }
  if (type == "--finish" || type == "-f")
  {
    return Function::finish;
  }
  if (type == "--date" || type == "-d")
  {
    return Function::date;
  }
  if (type == "--list" || type == "-l")
  {
    return Function::list;
  }
  if (type == "--help" || type == "-h")
  {
    return Function::help;
  }
  return -1;  
}

Project Interactor::AnalyseNewProjInfo(bool &res)
{
    // 在交互的过程中，还由DataWorker类进行检查，确认项目名称不重复。
    Project new_proj;
    cout << "Please input the project name: ";
    string name;
    getline(cin, name);
    name = Interactor::Trim(name);
    if (DataWorker::CheckProjNameCorrect(name) == false)
    {
	cout << "Error name, please input a different name. " << endl;
	res = false;
	return new_proj;
    }
    cout << "Please input the information: ";
    string info;
    getline(cin, info);
    info = Interactor::Trim(info);
    if (DataWorker::CheckProjInfoLong(info) == true)
    {
	cout << "Too long, please input in simply. " << endl;
	res = false;
	return new_proj;
    }
    cout << "Please input the deadline: ";
    string date;
    getline(cin, date);
    if (DataWorker::CheckProjDateFormat(date) == false)
    {
	cout << "Date format error, please input again, such as 20221120. " << endl;
	res = false;
	return new_proj;
    }
    // 解析成功，接下来开始初始化项目，然后返回。
    new_proj.Init(name, info, date);
    res = true;
    return new_proj;
}

ChangeInfo Interactor::AnalyseChangeInfo(int argc, char *argv[], int id, bool &res)
{
    ChangeInfo cinfo;
    // 如果argc为3，那么说明没有输入调整项，提示用户。
    if (argc == 3)
    {
	cout << "No change item be input." << endl;
	res = false;
	return cinfo;
    }
    // argc只能为5、7、9,否则不正确
    if (!(argc == 5 || argc == 7 || argc == 9))
    {
	cout << "Error format." << endl;
	res = false;
	return cinfo;
    }
    bool cn = false;
    bool ci = false;
    bool cd = false;
    string nn = "";
    string ni = "";
    string nd = "";
    // 首先解析第一对参数
    string left = argv[3];
    string riht = argv[4];
    if (left == "--name" || left == "-n")
    {
	cn = true;
	nn = riht;
    }
    else if (left == "--info" || left == "-i")
    {
	ci = true;
	ni = riht;
    }
    else if (left == "--date" || left == "-d")
    {
	cd = true;
	nd = riht;
    }
    else
    {
	cout << "Error format." << endl;
	res = false;
	return cinfo;
    }
    // 然后如果有第二对参数，解析第二对
    if (argc == 7 || argc == 9)
    {
	left = argv[5];
	riht = argv[6];
	if (left == "--name" || left == "-n")
	{
	    cn = true;
	    nn = riht;
	}
	else if (left == "--info" || left == "-i")
	{
	    ci = true;
	    ni = riht;
	}
	else if (left == "--date" || left == "-d")
	{
	    cd = true;
	    nd = riht;
	}
	else
	{
	    cout << "Error format." << endl;
	    res = false;
	    return cinfo;
	}
    }
    // 如果有第三对参数，解析第三对
    if (argc == 9)
    {
	left = argv[7];
	riht = argv[8];
	if (left == "--name" || left == "-n")
	{
	    cn = true;
	    nn = riht;
	}
	else if (left == "--info" || left == "-i")
	{
	    ci = true;
	    ni = riht;
	}
	else if (left == "--date" || left == "-d")
	{
	    cd = true;
	    nd = riht;
	}
	else
	{
	    cout << "Error format." << endl;
	    res = false;
	    return cinfo;
	}
    }
    // 然后在数据库内进行检查。
    if (cn == true)
    {
	// 1. 拟修改的名称不能和其他未完成或已完成项目的名称重复；
	// 2. 如果要改为子项目，那么要检查对应的父项目是否存在，不存在则不行, but if it's itself, it's also wrong.
	if (nn.length() == 0)
	{
	    cout << "Error name: null name." << endl;
	    res = false;
	    return cinfo;
	}
	bool need_check_pid = false;
	bool find_pid = false;
	string pname = "";
	if (DataWorker::IfChildProj(nn) == true)
	{
	    need_check_pid = true;
	    pname = DataWorker::AnalyseParentProjName(nn);
	}
	int line_num = DataWorker::GetProjNum();
	ifstream db;
	db.open(DatabaseConfig::dir_path + DatabaseConfig::db_name, ios::out);
	string buff;
	getline(db, buff);
	for (int i = 0; i < line_num; i++)
	{
	    getline(db, buff);
	    vector<string> li;
	    DataWorker::StringSplit(buff, ",", li);
	    if (nn == li[2] && li[5] != DatabaseConfig::status_cancel)
	    {
		// 项目重名
		db.close();
		cout << "Error name: repetitious." << endl;
		res = false;
		return cinfo;
	    }
	    if (need_check_pid == true && li[2] == pname && li[5] != DatabaseConfig::status_cancel && id != atoi(li[0].c_str()))
	    {
		find_pid = true;
	    }
	}
	if (need_check_pid == true && find_pid == false)
	{
	    // 没有找到对应的父项目
	    db.close();
	    cout << "Error name: no corresponding parent project." << endl;
	    res = false;
	    return cinfo;

	}
	db.close();
    }
    // 还要检查日期格式是否正确
    if (cd == true && DataWorker::CheckProjDateFormat(nd) == false)
    {
	cout << "Error date: format error." << endl;
	res = false;
	return cinfo;
    }
    // 基本通过了检查，设置cinfo变量并返回
    cinfo.Init(cn, ci, cd, nn, ni, nd);
    res = true;
    return cinfo;
}

bool Interactor::EnsureAdjustInfo(Project ori_proj, Project new_proj)
{
    cout << " -- ori project detail -- " << endl;
    cout << "NAME: " << ori_proj.GetName() << endl;
    cout << "INFO: " << ori_proj.GetInfo() << endl;
    cout << "DATE: " << ori_proj.GetDate() << endl;
    cout << " -- new project detail -- " << endl;
    cout << "NAME: " << new_proj.GetName() << endl;
    cout << "INFO: " << new_proj.GetInfo() << endl;
    cout << "DATE: " << new_proj.GetDate() << endl;
    cout << "Ensure to adjust? (yes/no): ";
    string res = "n";
    cin >> res;
    if (res == "yes" || res == "y" || res == "Y") return true;
    return false;
}

bool Interactor::EnsureEraseInfo(Project proj)
{
    cout << " -- project detail -- " << endl;
    cout << "NAME: " << proj.GetName() << endl;
    cout << "INFO: " << proj.GetInfo() << endl;
    cout << "DATE: " << proj.GetDate() << endl;
    cout << "Ensure to cancel? (yes/no) ";
    string res = "n";
    cin >> res;
    if (res == "yes" || res == "y" || res == "Y") return true;
    return false;    
}

bool Interactor::EnsureFinishInfo(Project proj)
{
    cout << " -- project detail -- " << endl;
    cout << "NAME: " << proj.GetName() << endl;
    cout << "INFO: " << proj.GetInfo() << endl;
    cout << "DATE: " << proj.GetDate() << endl;
    cout << "Ensure to finish? (yes/no) ";
    string res = "n";
    cin >> res;
    if (res == "yes" || res == "y" || res == "Y") return true;
    return false;    
}

string &Interactor::Trim(string &ori_str)
{
    if (ori_str.empty() == true) return ori_str;
    ori_str.erase(0, ori_str.find_first_not_of(" "));
    ori_str.erase(ori_str.find_last_not_of(" ") + 1);
    return ori_str;
}
