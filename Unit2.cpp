//---------------------------------------------------------------------------

#include <System.hpp>
#pragma hdrstop
#include "Unit1.h"
#include "Unit2.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall DBThreat::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall DBThreat::DBThreat(bool CreateSuspended,const char * sql_path)
	: TThread(CreateSuspended)
{
int dbopen = sqlite3_open(sql_path,&this->DB);
  int result= sqlite3_prepare_v2(this->DB,"CREATE TABLE IF NOT EXISTS\
   clusters(id integer primary key autoincrement,number_cluster\
	integer,type varchar(255));",-1,&this->State,NULL);
	if(result!=SQLITE_OK)
	{
	showerr ="Запрос создания таблицы не подготовлен";
    Synchronize(&ShowError);
	}
	result = sqlite3_step(State);
	if(result!=SQLITE_DONE)
	{
	showerr="Запрос создания таблицы не выполнен";
	Synchronize(&ShowError);
	}
	auto tableClear=sqlite3_exec(DB,"delete from clusters;",NULL,NULL,&errmsg);
auto incrementClear =sqlite3_exec(DB,"delete from sqlite_sequence where \
 name='clusters';",NULL,NULL,&errmsg);
 if (incrementClear!=SQLITE_OK || tableClear!=SQLITE_OK)
 {
 showerr = "Ошибка очистки таблицы!";
 Synchronize(&ShowError);
 }
}
//---------------------------------------------------------------------------
void __fastcall DBThreat::Execute()
{
id =0;
FreeOnTerminate =true;
int result = sqlite3_prepare_v2(DB,"insert into clusters(number_cluster,type) \
 values(?,?);",-1,&State,NULL);
 if(result!= SQLITE_OK)
 {
 showerr = "Не удалось подготовить запрос"    ;
 Synchronize(&ShowError);
 Terminate();
 }
 pair<ULONG,string> note;
 while (!Form1->diskread->Finished || !MainQueue.empty())
 {
 if(Terminated==true)
 break;
 note = MainQueue.frontpop();
 if(note.first!= 0 && note.second != "")
 {
 sign = note.second;
 NumberCluster = note.first;
 sqlite3_bind_int(State,1,NumberCluster);
 sqlite3_bind_text(State,2,sign.c_str(),strlen(sign.c_str()),NULL);
 sqlite3_step(State);
 sqlite3_reset(State);
 Synchronize(&UpdateVST);
  id++;
 }
 }
}
//---------------------------------------------------------------------------
void __fastcall DBThreat::UpdateVST()
{
 Form1->VirtualStringTree1->BeginUpdate();
 PVirtualNode Node = Form1->VirtualStringTree1->AddChild(NULL);
 TreeData* treeData = (TreeData*)Form1->VirtualStringTree1->GetNodeData(Node);
 treeData->id = id;
 treeData->NumberCluster  = NumberCluster;
 treeData->type	 = sign;
Form1->VirtualStringTree1->EndUpdate();

}
void __fastcall DBThreat::ShowError()
{
Form1->ErrorMessageLabel->Caption = showerr;
Form1->ErrorMessageLabel->Visible=true;
}
__fastcall DBThreat::~DBThreat()
{
sqlite3_finalize(State);
sqlite3_close(DB);
}