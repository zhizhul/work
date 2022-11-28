#include <iostream>
#include "aide_data.h"
#include "func_date.h"

using namespace std;

void FuncDate::execute()
{
    // 获取所有的未完成的项目的信息，然后根据项目ddl和从属关系
    // 来显示出所有项目的信息情况。
    bool res = true;
    string output = DataWorker::AnalyseDeadlineInfo(res);
    if (res == true)
    {
	cout << output;
    }
}
