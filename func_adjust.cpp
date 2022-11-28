#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include "aide_data.h"
#include "aide_itrc.h"
#include "func_adjust.h"

using namespace std;

void FuncAdjust::execute(int argc, char *argv[])
{
    // adjust指令的结构如下：
    // 用户首先在--adjust或-a指令之后要输入拟调整项目的ID号，
    // 然后接着可选地使用--name/-n、--info/-i、--date/-d
    // 来分别调整项目的名称、信息、计划完成日期。
    // 首先检查输入的ID号是否正确，必须为数字，并且在数据库中
    // 必须处于未完成的状态。
    if (argc <= 2)
    {
	cout << "ID error: please input ID." << endl;
	return ;
    }
    int id = atoi(argv[2]);
    if (DataWorker::CheckProjUndo(id) == false)
    {
	cout << "ID error: not undone/not find, please check." << endl;
	return ;
    }
    // Translate parameters into proper format. For example, 
    // work -a 10 -n "proj name" -i "proj info"
    // Then need to change num of parameters, since "proj name"
    // or "proj info" should be one parameter.
    int new_argc = 0;
    vector<string> new_argv;
    for (int i = 0; i < argc; i++)
    {
	string p = argv[i];
	if (p[0] == '\"')
	{
	    for (int j = i + 1; j < argc; j++)
	    {
		string np = argv[j];
		if (np[np.length() - 1] == '\"' || j == argc - 1)
		{
		    p += " " + np;
		    new_argc++;
		    new_argv.push_back(p);
		    i = j;
		    break;
		}
		else
		{
		    p += " " + np;
		}
	    }
	}
	else
	{
	    new_argc++;
	    new_argv.push_back(p);
	}
    }
    for (int i = 0; i < new_argc; i++)
    {
	argv[i] = (char *)Interactor::Trim(new_argv[i]).c_str();
    }
    // 然后检查用户要调整的内容。
    // 拟修改的名称不能和其他项目的名称重复，拟修改的日期不能
    // 早于当天。
    bool res = false;
    ChangeInfo change_info;
    change_info = Interactor::AnalyseChangeInfo(argc, argv, id, res);
    if (res == false)
    {
	return ;
    }
    // 向用户报告修改前后的信息内容，确认无误后再进行修改。
    // 首先从数据库中提取出原有的信息
    Project ori_proj = DataWorker::GetUndoProj(id, res);
    // 然后根据修改内容来更新出新的信息
    Project new_proj = DataWorker::GetRenewProj(ori_proj, change_info);
    // 然后将两段信息显示出来供用户确认
    bool ensure = Interactor::EnsureAdjustInfo(ori_proj, new_proj);
    if (ensure == true)
    {
	// 执行更新
	DataWorker::RenewUndoProj(id, ori_proj, new_proj, change_info);
	cout << "Adjust the project done." << endl;
    }
}
