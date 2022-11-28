#include <iostream>
#include "aide_conf.h"
#include "aide_data.h"
#include "aide_itrc.h"
#include "func_finish.h"

using namespace std;

void FuncFinish::execute(int argc, char *argv[])
{
    // Parameter num should == 3.
    if (argc != 3)
    {
	cout << "Parameter num error." << endl;
    }
    // find if the project exists.
    int id = atoi(argv[2]);
    if (DataWorker::CheckProjUndo(id) == false)
    {
	cout << "Not find undone project, please check." << endl;
	return ;
    }
    // output the project information, let the user ensures.
    bool res;
    Project proj = DataWorker::GetUndoProj(id, res);
    bool ensure = Interactor::EnsureFinishInfo(proj);
    if (ensure == true)
    {
	// perform erase, change the status into done.
	DataWorker::FinishUndoProj(id);
	cout << "Finish the project done." << endl;    	
    }
}
