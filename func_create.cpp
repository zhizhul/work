#include <iostream>
#include "aide_itrc.h"
#include "aide_data.h"
#include "func_create.h"

using namespace std;

void FuncCreate::execute()
{
    // 首先交由Interactor类交互解析出用户输入的项目信息。
    bool res = false;
    this->proj = Interactor::AnalyseNewProjInfo(res);
    if (res == false) return ;
    // 然后由DataWorker类保存进入csv型数据库中，
    // 数据库的相关文件参数由aide_conf.h文件定义。
    DataWorker::StoreNewProj(this->proj);
    cout << "Create the project done." << endl;
}
