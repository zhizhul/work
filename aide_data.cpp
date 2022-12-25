#include <ctime>
#include <fstream>
#include <vector>
#include <map>
#include <set> 
#include <utility>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include "aide_data.h"
#include "aide_conf.h"

using namespace std;

bool DataWorker::CheckProjNameCorrect(string name)
{
    // 一方面会检查是否和已有的未取消项目重名，
    // 另一方面会检查如果是子项目，看父项目是否存在。
    if (name.length() == 0)
    {
	return false;
    }
    bool need_check_pid = false;
    bool find_pid = false;
    string pname = "";
    if (DataWorker::IfChildProj(name) == true)
    {
	need_check_pid = true;
	pname = DataWorker::AnalyseParentProjName(name);
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
	StringSplit(buff, ",", li);
	if (name == li[2] && li[5] != DatabaseConfig::status_cancel)
	{
	    // 和已有的未取消项目重名
	    db.close();
	    return false;
	}
	if (need_check_pid == true && li[2] == pname && li[5] != DatabaseConfig::status_cancel)
	{
	    find_pid = true;
	}
    }
    if (need_check_pid == true && find_pid == false)
    {
	// 需要检查父项目是否存在，并且没有找到父项目
	db.close();
	return false;
    }
    // 通过检查
    db.close();
    return true;
}

bool DataWorker::CheckProjInfoLong(string info)
{
  // 目前以信息的字符长度是否超过200为计。
  if (info.length() > 200)
  {
    return true;
  }
  return false;
}

bool DataWorker::CheckProjDateFormat(string date)
{
    // 必须全部是数字，并且长度为8。
    if (date.length() != 8)
    {
	return false;
    }
    for (int i = 0; i < 8; i++)
    {
	if (date[i] < '0' || date[i] > '9')
	{
	    return false;
	}
    }
    // 前4位是年份，必须大于等于今年。
    time_t now = time(0);
    struct tm *ptime = gmtime(&now);
    int now_year = ptime->tm_year + 1900;
    int year = (date[0] - '0') * 1000 + (date[1] - '0') * 100;
    year += (date[2] - '0') * 10 + (date[3] - '0');
    if (now_year > year)
    {
	return false;
    }
    // 中间2位是月份，不能超过12或者等于0，也不能小于这个月
    int now_month = ptime->tm_mon + 1;
    int month = (date[4] - '0') * 10 + (date[5] - '0');
    if (month == 0 || month > 12 || (now_month > month && now_year == year))
    {
	return false;
    }
    // 后面2位是日期，不能超过31，并且也不能使得日期小于今天。
    int now_day = ptime->tm_mday;
    int day =  (date[6] - '0') * 10 + (date[7] - '0');
    if (day == 0 || day > 31)
    {
	return false;
    }
    if (now_month == month && now_day > day && now_year == year)
    {
	return false;
    }
    // 通过检查。
    return true;
}

void DataWorker::StringSplit(const string &str, const string &split, vector<string> &res)
{
    // 根据从网上查到的代码来实现。
    res.clear();
    if (str == "") return ;
    string strs = str + split;
    size_t pos = strs.find(split);
    int step = split.size();
    while (pos != strs.npos)
    {
	string tmp = strs.substr(0, pos);
	res.push_back(tmp);
	strs = strs.substr(pos + step, strs.size());
	pos = strs.find(split);
    }
}

int DataWorker::GetID(string name)
{
    // 根据名称查找ID号，注意cancel状态的不管。
    int line_num = DataWorker::GetProjNum();
    ifstream in;
    in.open(DatabaseConfig::dir_path + DatabaseConfig::db_name, ios::out);
    string buff;
    getline(in, buff);
    for (int i = 0; i < line_num; i++)
    {
	getline(in, buff);
	vector<string> li;
	StringSplit(buff, ",", li);
	if (li[2] == name && li[5] != DatabaseConfig::status_cancel)
	{
	    int id = atoi(li[0].c_str());
	    in.close();
	    return id;
	}
    }
    in.close();
    return -1;
}

void DataWorker::StoreNewProj(Project proj)
{
    // 首先确认已有数据的行数和新数据的ID号
    int id_ptr = DataWorker::GetIdPtr();
    int line_num = DataWorker::GetProjNum();
    // 检查项目名称，如果属于子项目，还需要先找到父项目的ID以作为PID
    int pid = 0;
    if (DataWorker::IfChildProj(proj.GetName()) == true)
    {
	string pname = DataWorker::AnalyseParentProjName(proj.GetName());
	pid = DataWorker::GetID(pname);
    }
    // 然后在数据文件的对应位置写入数据，并更新数据库配置信息
    string write_data = "";
    write_data += to_string(id_ptr) + ",";
    write_data += to_string(pid) + ",";
    write_data += proj.GetName() + ",";
    write_data += proj.GetInfo() + ",";
    write_data += proj.GetDate() + ",";
    write_data += DatabaseConfig::status_false;
    DataWorker::WriteDataTEnd(write_data);
    DataWorker::RenewConfigDataNumInfo(id_ptr + 1, line_num + 1);    
}

bool DataWorker::CheckProjUndo(int id)
{
    // 首先读取项目行数
    int line_num = DataWorker::GetProjNum();
    // 然后读取数据库，看一看对应ID的项目有没有，并且是不是处于未完成状态
    ifstream db;
    db.open(DatabaseConfig::dir_path + DatabaseConfig::db_name, ios::out);
    string buff;
    getline(db, buff);
    for (int i = 0; i < line_num && i < id; i++)
    {
	getline(db, buff);
	vector<string> li;
	StringSplit(buff, ",", li);
	int fid = atoi(li[0].c_str());
	string fstatus = li[5];
	if ((fid == id && fstatus == DatabaseConfig::status_false))
	{
	    db.close();
	    return true;
	}
    }
    db.close();
    return false;
}

Project DataWorker::GetUndoProj(int id, bool &res)
{
    // 根据id号获取未完成项目的信息。
    int line_num = DataWorker::GetProjNum();
    ifstream db;
    db.open(DatabaseConfig::dir_path + DatabaseConfig::db_name, ios::out);
    string buff;
    getline(db, buff);
    for (int i = 0; i < line_num; i++)
    {
	getline(db, buff);
	vector<string> li;
	StringSplit(buff, ",", li);
	if (atoi(li[0].c_str()) == id)
	{
	    string name = li[2];
	    string info = li[3];
	    string date = li[4];
	    Project proj;
	    proj.Init(name, info, date);
	    db.close();
	    res = true;
	    return proj;
	}
    }
    db.close();
    Project proj;
    res = false;
    return proj;
}

Project DataWorker::GetRenewProj(Project ori_proj, ChangeInfo change_info)
{
    Project new_proj = ori_proj;
    if (change_info.change_name == true)
    {
	new_proj.ChangeName(change_info.new_name);
    }
    if (change_info.change_info == true)
    {
	new_proj.ChangeInfo(change_info.new_info);
    }
    if (change_info.change_date == true)
    {
	new_proj.ChangeDate(change_info.new_date);
    }
    return new_proj;
}

void DataWorker::RenewUndoProj(int id, Project ori_proj, Project new_proj, ChangeInfo change_info)
{
    // 在数据库中更新项目的信息。
    // 1. 如果该ID项目存在子项目并且该项目要改名字，那么依次更新子项目的名称；
    if (change_info.change_name == true)
    {
	DataWorker::RenewCProjectName(id, new_proj.GetName());    
    }
    // 2. 如果该项目的名称修改后是子项目型，那么找到其父项目的ID以用作PID，
    //    否则从文件中读取该项目的PID；
    int pid = 0;
    if (DataWorker::IfChildProj(new_proj.GetName()))
    {
	pid = DataWorker::GetID(DataWorker::AnalyseParentProjName(new_proj.GetName()));
    }
    // 3. 根据ID号从数据库对应位置更新项目信息。
    string write_data = to_string(id) + "," + to_string(pid) + "," + new_proj.GetName() + "," + new_proj.GetInfo() + "," + new_proj.GetDate() + "," + DatabaseConfig::status_false;
    // 读取数据库文件找到位置并实施调整，主要基于ModifyLineData函数，区别在于
    // 前者基于行号，这里基于ID号。
    ifstream in;
    in.open(DatabaseConfig::dir_path + DatabaseConfig::db_name, ios::out);
    string file_data = "";
    string buff;
    getline(in, buff);
    file_data += buff + "\n";
    while (getline(in, buff))
    {
	vector<string> li;
	StringSplit(buff, ",", li);
	if (atoi(li[0].c_str()) == id)
	{
	    file_data += write_data + "\n";	    
	}
	else
	{
	    file_data += buff + "\n";
	}
    }
    in.close();
    ofstream out;
    out.open(DatabaseConfig::dir_path + DatabaseConfig::db_name);
    out << file_data.c_str();
    out.close();
}

void DataWorker::ModifyLineData(string file_path, int pos, string new_data)
{
    // 主要根据网上查到的代码复现。
    ifstream in;
    in.open(file_path, ios::out);
    string file_data = "";
    int line = 1;
    string buff;
    while (getline(in, buff))
    {
	if (line == pos)
	{
	    file_data += new_data + "\n";
	}
	else
	{
	    file_data += buff + "\n";
	}
	line++;
    }
    in.close();
    ofstream out;
    out.open(file_path);
    out << file_data.c_str();
    out.close();
}

int DataWorker::GetProjNum()
{
    string config_path = DatabaseConfig::dir_path + DatabaseConfig::config_name;
    ifstream in;
    in.open(config_path, ios::out);
    string buff;
    getline(in, buff);
    getline(in, buff);
    vector<string> li;
    StringSplit(buff, ":", li);
    int proj_num = atoi(li[1].c_str());
    in.close();
    return proj_num;
}

int DataWorker::GetIdPtr()
{
    string config_path = DatabaseConfig::dir_path + DatabaseConfig::config_name;
    ifstream in;
    in.open(config_path, ios::out);
    string buff;
    getline(in, buff);
    vector<string> li;
    StringSplit(buff, ":", li);
    int id_ptr = atoi(li[1].c_str());
    in.close();
    return id_ptr;
}

bool DataWorker::IfChildProj(string name)
{
    if (name.rfind('.') != string::npos) return true;
    return false;
}

string DataWorker::AnalyseParentProjName(string cname)
{
    size_t pos = cname.rfind('.');
    string pname = cname.substr(0, pos);
    return pname;
}

void DataWorker::WriteDataTEnd(string data)
{
    ifstream in;
    in.open(DatabaseConfig::dir_path + DatabaseConfig::db_name, ios::out);
    string file_data = "";
    string buff;
    while (getline(in, buff))
    {
	file_data += buff + "\n";
    }
    in.close();
    file_data += data + "\n";
    ofstream out;
    out.open(DatabaseConfig::dir_path + DatabaseConfig::db_name);
    out << file_data.c_str();
    out.close();
}

void DataWorker::RenewConfigDataNumInfo(int new_idptr, int new_linenum)
{
    ifstream in;
    in.open(DatabaseConfig::dir_path + DatabaseConfig::config_name, ios::out);
    string other_conf = "";
    string buff;
    getline(in, buff);
    getline(in, buff);
    while (getline(in, buff))
    {
	other_conf += buff + "\n";
    }
    in.close();
    ofstream out;
    out.open(DatabaseConfig::dir_path + DatabaseConfig::config_name);
    string write_data = "PTR:" + to_string(new_idptr) + "\nNUM:" + to_string(new_linenum) + "\n" + other_conf;
    out << write_data.c_str();
    out.close();
}

void DataWorker::RenewCProjectName(int pid, string pname)
{
    // 采用广度优先递归。
    // 首先找出所有pid的子项目，将其修改名称后的新数据保存。
    int line_num = DataWorker::GetProjNum();
    ifstream in;
    in.open(DatabaseConfig::dir_path + DatabaseConfig::db_name, ios::out);
    string buff;
    vector<int> clns;
    vector<int> cids;
    vector<string> cns;
    vector<string> cprojs;
    getline(in, buff);
    for (int i = 0; i < line_num; i++)
    {
	getline(in, buff);
	vector<string> li;
	StringSplit(buff, ",", li);
	if (atoi(li[1].c_str()) == pid)
	{
	    clns.push_back(i + 2);
	    cids.push_back(atoi(li[0].c_str()));
	    vector<string> ns;
	    StringSplit(li[2], ".", ns);
	    string new_name = pname + "." + ns.at(ns.size() - 1);
	    cns.push_back(new_name);
	    cprojs.push_back(li[0] + "," + li[1] + "," + new_name + "," + li[3] + "," + li[4] + "," + li[5]);
	}
    }
    in.close();
    // 依次从文件中更新对应子项目的名称数据
    for (int i = 0; i < clns.size(); i++)
    {
	DataWorker::ModifyLineData(DatabaseConfig::dir_path + DatabaseConfig::db_name, clns.at(i), cprojs.at(i));
    }
    // 然后依次遍历子项目，检查子项目是否还有要检查的
    for (int i = 0; i < clns.size(); i++)
    {
	DataWorker::RenewCProjectName(cids.at(i), cns.at(i));
    }
}

void DataWorker::EraseUndoProj(int id)
{
    // 首先将该ID对应的项目状态设置为取消。
    int line_num = DataWorker::GetProjNum();
    ifstream in;
    in.open(DatabaseConfig::dir_path + DatabaseConfig::db_name, ios::out);
    string file_data = "";
    string buff;
    getline(in, buff);
    file_data += buff + "\n";
    for (int i = 0; i < line_num; i++)
    {
	getline(in, buff);
	vector<string> li;
	StringSplit(buff, ",", li);
	if (atoi(li[0].c_str()) == id)
	{
	    file_data += li[0] + "," + li[1] + "," + li[2] + "," + li[3] + "," + li[4] + "," + DatabaseConfig::status_cancel + "\n";
	}
	else
	{
	    file_data += buff + "\n";
	}
    }
    in.close();
    ofstream out;
    out.open(DatabaseConfig::dir_path + DatabaseConfig::db_name);
    out << file_data.c_str();
    out.close();
    // 然后查找出该ID对应的所有子项目ID，然后依次设置取消。
    vector<int> cids = DataWorker::GetCProjIDs(id);
    for (int i = 0; i < cids.size(); i++)
    {
	DataWorker::EraseUndoProj(cids.at(i));
    }
}

void DataWorker::FinishUndoProj(int id)
{
    // 首先将该ID对应的项目状态设置为已完成。
    int line_num = DataWorker::GetProjNum();
    ifstream in;
    in.open(DatabaseConfig::dir_path + DatabaseConfig::db_name, ios::out);
    string file_data = "";
    string buff;
    getline(in, buff);
    file_data += buff + "\n";
    for (int i = 0; i < line_num; i++)
    {
	getline(in, buff);
	vector<string> li;
	StringSplit(buff, ",", li);
	if (atoi(li[0].c_str()) == id)
	{
	    file_data += li[0] + "," + li[1] + "," + li[2] + "," + li[3] + "," + DataWorker::GetNowDate() + "," + DatabaseConfig::status_true + "\n";
	}
	else
	{
	    file_data += buff + "\n";
	}
    }
    in.close();
    ofstream out;
    out.open(DatabaseConfig::dir_path + DatabaseConfig::db_name);
    out << file_data.c_str();
    out.close();
    // 然后查找出该ID对应的所有子项目ID，然后依次设置完成。
    vector<int> cids = DataWorker::GetCProjIDs(id);
    for (int i = 0; i < cids.size(); i++)
    {
	DataWorker::FinishUndoProj(cids.at(i));
    }
}

string DataWorker::GetNowDate()
{
    time_t now = time(0);
    struct tm *ptime = gmtime(&now);
    int now_year = ptime->tm_year + 1900;
    int now_month = ptime->tm_mon + 1;
    int now_day = ptime->tm_mday;
    string date = to_string(now_year);
    if (now_month < 10) date += "0";
    date += to_string(now_month);
    if (now_day < 10) date += "0";
    date += to_string(now_day);
    return date;
}

string DataWorker::AnalyseDeadlineInfo(bool &res)
{
    string output = "";
    // 1. 首先生成一串和屏幕相等的等号加上去
    struct winsize window;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &window);
    int width = window.ws_col;
    if (width < 45)
    {
	cout << "Narrow terminal, please adjust. " << endl;
	res = false;
	return output;
    }
    for (int i = 0; i < width; i++) output += "=";
    output += "\n";
    // 2. 计算所有未完成项目的最长占用长度
    // 该长度和NAME(ID)中的最大值加7之后是INFO数据应该起始的位置
    int proj_llen = DataWorker::AlysejLgtUProjSLen();
    if (proj_llen + 30 >= width)
    {	
	cout << "Narrow terminal, please adjust. " << endl;
	res = false;
	return output;
    }
    // 如果返回-1，说明没有未完成项目，向用户提示
    if (proj_llen == -1)
    {
	cout << "Undone project not be found." << endl;
	res = false;
	return output;
    }
    // 3. 分析得出INFO表头应处于的位置，然后输出表头信息和第二行等号
    output += "   NAME(ID)";
    int ihspce = proj_llen > 8 ? proj_llen - 5 : 3;
    for (int i = 0; i < ihspce; i++) output += " ";
    output += "INFO";
    for (int i = 0; i < width - ihspce - 26; i++) output += " ";
    output += "  DATE     \n";
    for (int i = 0; i < width; i++) output += "=";
    output += "\n";
    // 4. 解析出所有未完成项目的显示数据，并进行排序
    int uproj_num = 0;
    ShowInfo root = DataWorker::AnalyseUPTreeView(uproj_num);
    // 5. 将数据信息输入到output字符串中
    string data_info = "";
    DataWorker::TranUPTreeView(data_info, &root, uproj_num, width, ihspce);
    output += data_info;
    // 6. 最后再输出一行等号
    for (int i = 0; i < width; i++) output += "=";
    output += "\n";
    res = true;
    return output;
}

int DataWorker::AlyseLgtUProjSLen()
{
    // 依次从文件中读取未完成项目，计算显示长度最大值
    // 没有找到就返回-1
    int max_len = -1;
    int line_num = DataWorker::GetProjNum();
    ifstream db;
    db.open(DatabaseConfig::dir_path + DatabaseConfig::db_name, ios::out);
    string buff;
    getline(db, buff);
    for (int i = 0; i < line_num; i++)
    {
	getline(db, buff);
	vector<string> li;
	StringSplit(buff, ",", li);
	if (li[5] == DatabaseConfig::status_false)
	{
	    string name = li[2];
	    int name_len;
	    if (name.rfind('.') != string::npos)
	    {
		string cname = name.substr(name.rfind('.') + 1, name.length() - name.rfind('.') - 1);
		name_len = DataWorker::GetTerminalSLen(cname.c_str());
		name_len += li[0].length() + 2;
		for (int j = 0; j < name.length(); j++)
		{
		    if (name[j] == '.') name_len++;
		}
	    }
	    else
	    {
		name_len = DataWorker::GetTerminalSLen(name.c_str());
		name_len += li[0].length() + 2;	    
	    }
	    if (max_len < name_len) max_len = name_len;
	}
    }
    db.close();
    return max_len;
}

ShowInfo DataWorker::AnalyseUPTreeView(int &uproj_num)
{
    // 依次读取文件，找到未完成项目后解析其信息
    // 并加入到vector列表中
    vector<vector<string>> uprojs;
    int line_num = DataWorker::GetProjNum();
    ifstream db;
    db.open(DatabaseConfig::dir_path + DatabaseConfig::db_name, ios::out);
    string buff;
    getline(db, buff);
    for (int i = 0; i < line_num; i++)
    {
	getline(db, buff);
	vector<string> li;
	StringSplit(buff, ",", li);
	if (li[5] == DatabaseConfig::status_false)
	{
	    uprojs.push_back(li);
	}
    }
    db.close();
    // 对vector列表基于sub_level进行排序
    // sub_level等于全名中'.'的个数
    sort(uprojs.begin(), uprojs.end(), DataWorker::CpProjVecSubLev);
    // 遍历vector列表，生成显示信息树数据结构
    ShowInfo root;
    root.sub_level = -1;
    root.id = 0;
    root.pid = -1;
    for (int i = 0; i < uprojs.size(); i++)
    {
	ShowInfo node = DataWorker::TranShowInfo(uprojs.at(i));
	DataWorker::InsertTreeView(root, node);
    }
    uproj_num = uprojs.size();
    return root;
}

bool DataWorker::CpProjVecSubLev(const vector<string> left, const vector<string> riht)
{
    int llev = 0;
    int rlev = 0;
    string lstr = left[2];
    string rstr = riht[2];
    for (int i = 0; i < lstr.length(); i++)
    {
	if (lstr[i] == '.') llev++;
    }
    for (int i = 0; i < rstr.length(); i++)
    {
	if (rstr[i] == '.') rlev++;
    }
    if (llev > rlev) return false;
    return true;
}

ShowInfo DataWorker::TranShowInfo(vector<string> li)
{
    ShowInfo info;
    info.fullname = li[2];
    info.info = li[3];
    info.date = li[4];
    if (li[2].rfind('.') != string::npos)
    {
	info.name_id = li[2].substr(li[2].rfind('.') + 1, li[2].length() - li[2].rfind('.') - 1) + "(" + li[0] + ")";
    }
    else
    {
	info.name_id = li[2] + "(" + li[0] + ")";
    }
    info.id = atoi(li[0].c_str());
    info.pid = atoi(li[1].c_str());
    int slev = 0;
    for (int i = 0; i < li[2].length(); i++)
    {
	if (li[2][i] == '.') slev++;
    }
    info.status = li[5];
    info.sub_level = slev;
    info.sub_projs.clear();
    return info;
}

void DataWorker::InsertTreeView(ShowInfo &root, ShowInfo node)
{
    // 从根结点开始，找到合适的位置之后将信息插入
    // 首先根据pid找到对应的父结点
    // 也有可能找不到父结点，比如说list功能里只完成了子项目的
    // 情况，那么直接插到根结点上。    
    int pid = node.pid;
    ShowInfo *fnode = DataWorker::FindIdInfo(&root, pid);
    // 然后在父结点下插入相应结点
    if (fnode->sub_projs.size() == 0)
    {
	fnode->sub_projs.push_back(node);
    }
    else
    {
	int pos = 0;
	for (; pos < fnode->sub_projs.size(); pos++)
	{
	    string node_date = node.date;
	    string relt_date = fnode->sub_projs.at(pos).date;
	    if (node_date == relt_date)
	    {
		int node_id = node.id;
		int relt_id = fnode->sub_projs.at(pos).id;
		if (node_id < relt_id) break;
		else continue;
	    }
	    bool earlier = false;
	    for (int j = 0; j < 8; j++)
	    {
		if (node_date[j] < relt_date[j])
		{
		    earlier = true;
		    break;
		}
		else if (node_date[j] > relt_date[j])
		{
		    earlier = false;
		    break;
		}
	    }
	    if (earlier == true) break;
	}
	fnode->sub_projs.insert(fnode->sub_projs.begin() + pos, node);
    }
}

ShowInfo *DataWorker::FindIdInfo(ShowInfo *node, int id)
{
    if (node->id == id) return node;
    for (int i = 0; i < node->sub_projs.size(); i++)
    {
	ShowInfo *find = DataWorker::FindIdInfo(&(node->sub_projs.at(i)), id);
	if (find != nullptr) return find;
    }
    // 没有找到，那么还是返回根结点作为其父结点
    if (node->id == 0) return node;
    else return nullptr;
}

void DataWorker::TranUPTreeView(string &data_info, ShowInfo *node, int &uproj_num, int width, int ihspce)
{
    if (node->pid != -1)
    {
	// 输入当前结点的信息
	uproj_num--;
	string info = "   ";
	for (int i = 0; i < node->sub_level; i++)
	{
	    info += " ";
	}
	info += node->name_id;
	int name_id_tslen = DataWorker::GetTerminalSLen(node->name_id.c_str());
	for (int i = 0; i < (ihspce + 8 - name_id_tslen - node->sub_level); i++)
	{
	    info += " ";
	}
      	int info_width = width - ihspce - 25;	
	int info_len = DataWorker::GetTerminalSLen(node->info.c_str());
	bool multiline_info = false;
	int info_index = 0;
	bool info_hitend = false;
	if (info_len <= info_width)
	{
	    info += node->info;
	    for (int i = 0; i < (info_width - info_len); i++)
	    {
		info += " ";
	    }
	}
	else
	{
	    multiline_info = true;
	    info += DataWorker::OutputOneLineInfo(node->info.c_str(), info_index, info_hitend, info_width);
	}
	info += "   ";
	if (DataWorker::DeadlineWeek(node->date))
	{
	    info += "\033[31m" + node->date + "\033[0m";
	}
	else if (DataWorker::DeadlineMonth(node->date))
	{
	    info += "\033[33m" + node->date + "\033[0m";
	}
	else
	{
	    info += "\033[32m" + node->date + "\033[0m";	
	}
	info += "   \n";
	if (multiline_info == true)
	{
	    while (info_hitend == false)
	    {
		for (int i = 0; i < ihspce + 11; i++) info += " ";
		info += DataWorker::OutputOneLineInfo(node->info.c_str(), info_index, info_hitend, info_width);
		info += "\n";
	    }
	}
	if (uproj_num != 0)
	{
	    info += "   ";
	    for (int i = 0; i < node->sub_level; i++) info += " ";
	    for (int i = 0; i < width - node->sub_level - 6; i++)
	    {
		info += "-";
	    }
	    info += "   \n";
	}
	data_info += info;
    }
    for (int i = 0; i < node->sub_projs.size(); i++)
    {
	// 遍历子结点，依次输出
	DataWorker::TranUPTreeView(data_info, &(node->sub_projs.at(i)), uproj_num, width, ihspce);
    }
}

bool DataWorker::DeadlineWeek(string date)
{
    string now_date = DataWorker::GetNowDate();
    int days_now = atoi(now_date.substr(0, 4).c_str()) * 365 + atoi(now_date.substr(4, 2).c_str()) * 30 + atoi(now_date.substr(6, 2).c_str());
    int days = atoi(date.substr(0, 4).c_str()) * 365 + atoi(date.substr(4, 2).c_str()) * 30 + atoi(date.substr(6, 2).c_str());
    if (days - days_now <= 7) return true;
    return false;
}

bool DataWorker::DeadlineMonth(string date)
{
    string now_date = DataWorker::GetNowDate();
    int days_now = atoi(now_date.substr(0, 4).c_str()) * 365 + atoi(now_date.substr(4, 2).c_str()) * 30 + atoi(now_date.substr(6, 2).c_str());
    int days = atoi(date.substr(0, 4).c_str()) * 365 + atoi(date.substr(4, 2).c_str()) * 30 + atoi(date.substr(6, 2).c_str());
    if (days - days_now <= 30) return true;
    return false;
}

vector<int> DataWorker::GetCProjIDs(int id)
{
    vector<int> res;
    int line_num = DataWorker::GetProjNum();
    ifstream in;
    in.open(DatabaseConfig::dir_path + DatabaseConfig::db_name, ios::out);
    string buff;
    getline(in, buff);
    for (int i = 0; i < line_num; i++)
    {
	getline(in, buff);
	vector<string> li;
	StringSplit(buff, ",", li);
	if (atoi(li[1].c_str()) == id && li[5] != DatabaseConfig::status_cancel)
	{
	    res.push_back(atoi(li[0].c_str()));
	}
    }
    in.close();
    return res;
}

string DataWorker::AnalyseDoneInfo(bool &res)
{
    // 相较于显示未完成项目信息，显示已完成项目信息需要
    // 做一个调整，那就是如果子项目已完成，但是父项目还
    // 没完成，那么要把相关的父项目都显示出来，但是不显
    // 示未完成父项目的日期。
    string output = "";
    // 1. 首先生成一串和屏幕相等的等号加上去
    struct winsize window;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &window);
    int width = window.ws_col;
    if (width < 45)
    {
	cout << "Narrow terminal, please adjust. " << endl;
	res = false;
	return output;
    }
    for (int i = 0; i < width; i++) output += "=";
    output += "\n";
    // 2. 计算所有已完成项目的最长占用长度
    // 该长度和NAME(ID)中的最大值加7之后是INFO数据应该起始的位置
    int proj_llen = DataWorker::AlyseLgtDProjSLen();
    if (proj_llen + 30 >= width)
    {	
	cout << "Narrow terminal, please adjust. " << endl;
	res = false;
	return output;
    }
    // 如果返回-1，说明没有，向用户提示
    if (proj_llen == -1)
    {
	cout << "Done project not be found." << endl;
	res = false;
	return output;
    }
    // 3. 分析得出INFO表头应处于的位置，然后输出表头信息和第二行等号
    output += "   NAME(ID)";
    int ihspce = proj_llen > 8 ? proj_llen - 5 : 3;
    for (int i = 0; i < ihspce; i++) output += " ";
    output += "INFO";
    for (int i = 0; i < width - ihspce - 26; i++) output += " ";
    output += "  DATE     \n";
    for (int i = 0; i < width; i++) output += "=";
    output += "\n";
    // 4. 解析出所有要显示的项目的显示数据，并进行排序
    int dproj_num = 0;
    ShowInfo root = DataWorker::AnalyseDPTreeView(dproj_num);
    // 5. 将数据信息输入到output字符串中
    string data_info = "";
    DataWorker::TranDPTreeView(data_info, &root, dproj_num, width, ihspce);
    output += data_info;
    // 6. 最后再输出一行等号
    for (int i = 0; i < width; i++) output += "=";
    output += "\n";
    res = true;
    return output;
}

// 用于AlyseLgtDProjSLen等函数用于分析的数据结构
typedef struct ProjInfo
{
    int pid;
    string status;
    int show_len;
    vector<string> total_info;
} ProjInfo;

int DataWorker::AlyseLgtDProjSLen()
{
    // 依次从文件中读取已完成项目，计算显示长度最大值
    // 没有找到就返回-1。
    // 这里需要注意的是，如果一个父项目虽然整个是没有
    // 完成的，但是里面有后代项目已经完成了，那么仍然
    // 要计算这个父项目的显示长度。
    int max_len = -1;
    int line_num = DataWorker::GetProjNum();
    ifstream db;
    db.open(DatabaseConfig::dir_path + DatabaseConfig::db_name, ios::out);
    string buff;
    getline(db, buff);
    // 这里采用与AlyseLgtUProjSLen不同的实现方式，
    // 首先取出所有<ID, PID, STATUS>来组成一个列表，
    // 这里只取STATUS是非已取消状态的，然后遍历这个列
    // 表，每找到一个已完成项目，就把它放到一个集合里，
    // 并从这个已完成项目开始往上追溯，追溯到的所有项
    // 目都放到集合里，最后这个集合就是所有要显示的项
    // 目，那么遍历该集合就可以计算出最大显示长度了。
    map<int, ProjInfo> ap_list;
    vector<int> id_list;
    for (int i = 0; i < line_num; i++)
    {
        getline(db, buff);
        vector<string> li;
        StringSplit(buff, ",", li);
        if (li[5] != DatabaseConfig::status_cancel)
        {
            int id = atoi(li[0].c_str());
            ProjInfo pi;
            pi.pid = atoi(li[1].c_str());
            pi.status = li[5];
	    string name = li[2];
	    int name_len;
	    if (name.rfind('.') != string::npos)
	    {
		string cname = name.substr(name.rfind('.') + 1, name.length() - name.rfind('.') - 1);
		name_len = DataWorker::GetTerminalSLen(cname.c_str());
		name_len += li[0].length() + 2;
		for (int j = 0; j < name.length(); j++)
		{
		    if (name[j] == '.') name_len++;
		}
	    }
	    else
	    {
		name_len = DataWorker::GetTerminalSLen(name.c_str());
		name_len += li[0].length() + 2;	    
	    }
	    pi.show_len = name_len;	    
	    ap_list.insert(make_pair(id, pi));
	    id_list.push_back(id);
        }
    }
    set<int> dp_list;
    for (int i = 0; i < id_list.size(); i++)
    {
        int id = id_list.at(i);
	if (ap_list[id].status == DatabaseConfig::status_true)
        {
            dp_list.insert(id);
            while (ap_list[id].pid != 0)
	    {
		id = ap_list[id].pid;
		dp_list.insert(id);
	    }
        }
    }
    if (dp_list.size() == 0)
    {
	db.close();
	return -1;
    }
    for (set<int>::iterator i = dp_list.begin(); i != dp_list.end(); i++)
    {
	if (max_len < ap_list[*i].show_len)
	{
	    max_len = ap_list[*i].show_len;
	}
    }
    db.close();
    return max_len;
}

ShowInfo DataWorker::AnalyseDPTreeView(int &dproj_num)
{
    // 这里先把所有非取消项目找出来，然后再解
    // 析出所有需要显示的项目，再生成显示信息
    // 树数据结构
    int line_num = DataWorker::GetProjNum();
    ifstream db;
    db.open(DatabaseConfig::dir_path + DatabaseConfig::db_name, ios::out);
    string buff;
    getline(db, buff);
    map<int, ProjInfo> ap_list;
    vector<int> id_list;
    for (int i = 0; i < line_num; i++)
    {
	getline(db, buff);
	vector<string> li;
	StringSplit(buff, ",", li);
	if (li[5] != DatabaseConfig::status_cancel)
	{
	    int id = atoi(li[0].c_str());
            ProjInfo pi;
            pi.pid = atoi(li[1].c_str());
            pi.status = li[5];
	    pi.show_len = -1;
	    pi.total_info = li;
	    ap_list.insert(make_pair(id, pi));
	    id_list.push_back(id);
	}
    }
    set<int> dp_list;
    for (int i = 0; i < id_list.size(); i++)
    {
        int id = id_list.at(i);
	if (ap_list[id].status == DatabaseConfig::status_true)
        {
            dp_list.insert(id);
            while (ap_list[id].pid != 0)
	    {
		id = ap_list[id].pid;
		dp_list.insert(id);
	    }
        }
    }
    vector<vector<string>> dprojs;
    for (set<int>::iterator i = dp_list.begin(); i != dp_list.end(); i++)
    {
	dprojs.push_back(ap_list[*i].total_info);
    }
    db.close();
    // 对vector列表基于sub_level进行排序
    // sub_level等于全名中'.'的个数
    sort(dprojs.begin(), dprojs.end(), DataWorker::CpProjVecSubLev);
    // 遍历vector列表，生成显示信息树数据结构
    ShowInfo root;
    root.sub_level = -1;
    root.id = 0;
    root.pid = -1;
    for (int i = 0; i < dprojs.size(); i++)
    {
	ShowInfo node = DataWorker::TranShowInfo(dprojs.at(i));
	DataWorker::InsertTreeView(root, node);
    }
    dproj_num = dprojs.size();
    return root;
}

void DataWorker::TranDPTreeView(string &data_info, ShowInfo *node, int &dproj_num, int width, int ihspce)
{
    if (node->pid != -1)
    {
	// 输入当前结点的信息
	dproj_num--;
	string info = "   ";
	for (int i = 0; i < node->sub_level; i++)
	{
	    info += " ";
	}
	info += node->name_id;
	int name_id_tslen = DataWorker::GetTerminalSLen(node->name_id.c_str());
	for (int i = 0; i < (ihspce + 8 - name_id_tslen - node->sub_level); i++)
	{
	    info += " ";
	}
      	int info_width = width - ihspce - 25;	
	int info_len = DataWorker::GetTerminalSLen(node->info.c_str());
	bool multiline_info = false;
	int info_index = 0;
	bool info_hitend = false;
	if (info_len <= info_width)
	{
	    info += node->info;
	    for (int i = 0; i < (info_width - info_len); i++)
	    {
		info += " ";
	    }
	}
	else
	{
	    multiline_info = true;
	    info += DataWorker::OutputOneLineInfo(node->info.c_str(), info_index, info_hitend, info_width);
	}
	if (node->status == DatabaseConfig::status_true)
	{
	    info += "   \033[32m" + node->date + "\033[0m";
	}
	info += "\n";	
	if (multiline_info == true)
	{
	    while (info_hitend == false)
	    {
		for (int i = 0; i < ihspce + 11; i++) info += " ";
		info += DataWorker::OutputOneLineInfo(node->info.c_str(), info_index, info_hitend, info_width);
		info += "\n";
	    }
	}
	if (dproj_num != 0)
	{
	    info += "   ";
	    for (int i = 0; i < node->sub_level; i++) info += " ";
	    for (int i = 0; i < width - node->sub_level - 6; i++)
	    {
		info += "-";
	    }
	    info += "   \n";
	}
	data_info += info;
    }
    for (int i = 0; i < node->sub_projs.size(); i++)
    {
	// 遍历子结点，依次输出
	DataWorker::TranDPTreeView(data_info, &(node->sub_projs.at(i)), dproj_num, width, ihspce);
    }
}

int DataWorker::GetTerminalSLen(const char *str)
{
    // 这里由于采用的是UTF-8编码集和Unicode字符集，所以
    // 做一个判断，看看是1字节字符还是3字节字符，1字节字符
    // 就是普通的ASCII编码，3字节字符则是汉字字符。暂时就
    // 默认只有这两种情形。
    //  经上网查阅，在Unicode字符集里面，汉字的编码范围在
    // 0x4e00-0x9fa5, 0x3400-0x4db5之间。
    //  而UTF-8的编码方式是：0b0xxxxxxx是1字节字符，
    // 0b110x...x是2字节字符，0b1110x...x是3字节字符。
    // 所以可以就按这个规律，来进行处理。对于1字节字符认为
    // 其占1个屏幕字符宽度，对于2字节字符认为其占2个屏幕字
    // 符宽度，对于3字节字符认为其还是占2个屏幕字符宽度。
    //  在Xterm和FBterm终端下，中文字体都是占2个字符宽度
    // 的，但是在Emacs shell下不是，暂时先这样吧，够了。
    //  另外还值得一提的是，将Unicode中文编码转换为UTF编
    // 码的方式是：0b1110xxxx 10xxxxxx 10xxxxxx,比如
    // “阅”字的Unicode编码是0x9605，即：
    // 0b1001011000000101，那么转换成UTF-8就是：
    // 0b1110 1001 10 011000 10 000101，即：
    // 0xe99885
    int index = 0;
    int slen = 0;
    while (true)
    {
	int ch = (int)str[index] & 0xff;
	if (ch == '\0') return slen;
	if ((ch >> 7) == 0b0)
	{
	    slen++;
	    index++;
	    continue;
	}
	if ((ch >> 5) == 0b110)
	{
	    slen += 2;
	    index += 2;
	    continue;
	}
	if ((ch >> 4) == 0b1110)
	{
	    slen += 2;
	    index += 3;
	    continue;
	}
	// 不符合上述3种情况，这我就不知道是怎么回事了，
	// 按一般方式处理。
	slen++;
	index++;
    }
    return slen;
}

string DataWorker::OutputOneLineInfo(const char *info, int &index, bool &hitend, int width)
{
    // 这里假设限定输入最多占width个屏幕字符宽度的字符，
    // 并且是从info[index]开始输入的，那么输入之后要
    // 更新index，并且如果找到头了还要设置hitend = true。
    char res[120];
    int rindex = 0;
    int now_width = 0;
    while (true)
    {
	int ch = (int)info[index] & 0xff;
	if ((ch >> 4) == 0b1110)
	{
	    if (now_width + 2 > width)
	    {
		res[rindex] = '\0';
		break;
	    }
	    else if (now_width + 2 == width)
	    {
		res[rindex++] = info[index++];
		res[rindex++] = info[index++];
		res[rindex++] = info[index++];
		res[rindex] = '\0';
		if (info[index] == '\0') hitend = true;
		break;
	    }
	    else
	    {	
	    	res[rindex++] = info[index++];
		res[rindex++] = info[index++];
		res[rindex++] = info[index++];
		now_width += 2;
		if (info[index] == '\0')
		{
		    hitend = true;
		    res[rindex] = '\0';
		    break;
		}
		continue;
	    }
	}
	if (now_width + 1 == width)
	{
	    res[rindex++] = info[index++];
	    res[rindex] = '\0';
	    if (info[index] == '\0') hitend = true;
	    break;
	}
	else
	{
	    res[rindex++] = info[index++];
	    now_width++;
	    if (info[index] == '\0')
	    {
		hitend = true;
		res[rindex] = '\0';
		break;
	    }
	}
    }
    string strres(res);
    return strres;
}
