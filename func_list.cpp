#include <iostream>
#include "aide_data.h"	
#include "func_list.h"

using namespace std;

void FuncList::execute()
{
    // 获取所有已完成项目的信息，然后根据完成时间和从属关系
    // 来显示出所有项目的信息情况。
    bool res = true;
    string output = DataWorker::AnalyseDoneInfo(res);
    if (res == true)
    {
	cout << output;
    }
}
