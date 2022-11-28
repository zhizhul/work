#ifndef FUNC_CREATE_H
#define FUNC_CREATE_H

#include "aide_conf.h"
	
// 创建项目功能类
class FuncCreate
{

private:
  // 新项目数据
  Project proj;

public:
  // 与用户交互并获得新项目的信息，保存项目数据
  void execute();

};

#endif
