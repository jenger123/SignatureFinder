//---------------------------------------------------------------------------

#ifndef Unit3H
#define Unit3H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include "MyClasses.h"

//---------------------------------------------------------------------------
class DiskReader : public TThread
{
private:
protected:
	void __fastcall Execute();
public:
	__fastcall DiskReader(bool CreateSuspended);
	void __fastcall Final();

	FileSystemIterator * It;
};
//---------------------------------------------------------------------------
#endif
