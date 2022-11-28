#include "aide_conf.h"

using namespace std;

void Project::Init(string in_name, string in_info, string in_date)
{
  name = in_name;
  info = in_info;
  date = in_date;
  status = DatabaseConfig::status_false;
}

void Project::ChangeName(string in_name)
{
  name = in_name;
}

void Project::ChangeDate(string in_date)
{
  date = in_date;
}

void Project::ChangeInfo(string in_info)
{
  info = in_info;
}

void Project::ChangeStatus(string in_status)
{
    status = in_status;
}

string Project::GetName()
{
  return this->name;
}

string Project::GetInfo()
{
  return this->info;
}

string Project::GetDate()
{
  return this->date;
}

string Project::GetStatus()
{
  return this->status;
}

void ChangeInfo::Init(bool cn, bool ci, bool cd, string nn, string ni, string nd)
{
    this->change_date = cd;
    this->change_info = ci;
    this->change_name = cn;
    this->new_date = nd;
    this->new_info = ni;
    this->new_name = nn;
}
