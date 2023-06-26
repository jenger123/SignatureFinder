//---------------------------------------------------------------------------

#include <System.hpp>
#pragma hdrstop
#include "Unit1.h"
#include "Unit2.h"
#include "Unit3.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall DiskReader::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall DiskReader::DiskReader(bool CreateSuspended)
	: TThread(CreateSuspended)
{

}
//---------------------------------------------------------------------------
void __fastcall DiskReader::Execute()
{
FreeOnTerminate=true;
string sign;
ULONG Cluster;
pair<ULONG,string>note;
	It = new FileSystemIterator(Form1->MyFS, Form1->VolumeHandle);
	FileSystemDecorator * Decorator = new FileSystemDecorator(It);
	for (Decorator->begin(); !Decorator->isDone() && !Terminated; Decorator->Next())
	{
	 Cluster = Decorator->GetNumberCluster();
	 sign = Decorator->GetSign();
	 note={Cluster,sign};
	 Form1 ->dbthreat->MainQueue.push(note);
	}
 delete It;
 Synchronize(&Final);
 delete Decorator;
}

//---------------------------------------------------------------------------
 void __fastcall DiskReader::Final()
 {
 Form1->Timer1->Enabled=false;
 Form1->ProgressBar1->Position=100;
 Form1->StopButton->Enabled = false;
 Form1->StartButton->Enabled = true;
 Form1->InfoLabel->Caption = "Чтение выполнено!";
 Form1->ErrorMessageLabel->Visible = false;
 }
