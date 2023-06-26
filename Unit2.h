#ifndef Unit2H
#define Unit2H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include "sqlite3.h"
using namespace std;
//---------------------------------------------------------------------------
class DBThreat : public TThread
{
private:
int id =0;
string sign;
 ULONG NumberCluster;
sqlite3 * DB;
sqlite3_stmt * State;
char * errmsg;
const char * showerr;
protected:
	void __fastcall Execute();
public:
    pairqueue MainQueue;
	__fastcall DBThreat(bool CreateSuspended,const char * sql_path);
    void __fastcall ProgressBarChange();
	void __fastcall UpdateVST();
    void __fastcall ShowError();
	__fastcall ~DBThreat();
};
//---------------------------------------------------------------------------
#endif
