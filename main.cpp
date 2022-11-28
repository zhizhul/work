#include <iostream>
#include "aide_conf.h"
#include "aide_itrc.h"
#include "aide_data.h"
#include "func_create.h"
#include "func_adjust.h"
#include "func_erase.h"
#include "func_finish.h"
#include "func_date.h"
#include "func_list.h"
#include "func_help.h"

using namespace std;

// 主程序入口
int main(int argc, char *argv[])
{
    // 首先由Interactor类解析命令行，分析出是什么指令，
    // 然后根据指令来展开操作。
    int type = Interactor::GetFunctionType(argc, argv);
    if (type == Function::create)
    {
        // 创建项目功能，不另外接收参数，会和用户交互并要求输入
        // 项目的名称、信息、计划日期。
        FuncCreate create;
        create.execute();
    }
    else if (type == Function::adjust)
    {
        // 调整项目功能，要另外接收2个参数，一个是项目ID，另
        // 一个是要修改的项。
	FuncAdjust adjust;
	adjust.execute(argc, argv);
    }
    else if (type == Function::erase)
    {
        // 删除项目功能，要接收项目名称ID，并且要确认。
	FuncErase erase;
	erase.execute(argc, argv);
    }
    else if (type == Function::finish)
    {
        // 完成项目功能，要接收项目名称ID，并且要确认。
        FuncFinish finish;
	finish.execute(argc, argv);
    }
    else if (type == Function::date)
    {
        // 显示未完成项目功能，不另外接收参数。
        FuncDate date;
	date.execute();
    }
    else if (type == Function::list)
    {
        // 显示已完成项目功能，不另外接收参数。
        FuncList list;
	list.execute();
    }
    else if (type == Function::help)
    {
        // 帮助功能，不另外接收参数，用于弹出使用说明。
        FuncHelp help;
	help.execute();
    }
    else
    {
        cout << "False instructions, you can input --help or -h for help." << endl;
    }
    return 0;   
}
