//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "VirtualTrees.hpp"
#include <Vcl.CheckLst.hpp>
#include <Vcl.ComCtrls.hpp>
#include <windows.h>
#include <Vcl.ExtCtrls.hpp>
#include "MyClasses.h"
#include <string>
#include <Vcl.ExtCtrls.hpp>
#include "Unit2.h"
#include "Unit3.h"


typedef struct{
 int id;
 int NumberCluster;
 UnicodeString type;
}TreeData;
wstring Path;

//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TLabel *InfoLabel;
	TVirtualStringTree *VirtualStringTree1;
	TComboBox *ComboBox1;
	TCheckListBox *CheckListBox1;
	TProgressBar *ProgressBar1;
	TLabel *Label2;
	TButton *StartButton;
	TButton *StopButton;
	TLabel *ErrorMessageLabel;
	TTimer *Timer1;
	void __fastcall ComboBox1Select(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall StartButtonClick(TObject *Sender);
	void __fastcall VirtualStringTree1GetText(TBaseVirtualTree *Sender, PVirtualNode Node,
          TColumnIndex Column, TVSTTextType TextType, UnicodeString &CellText);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall StopButtonClick(TObject *Sender);

private:	// User declarations

public:		// User declarations
 FileSystem * MyFS;
	BYTE BOOTRECORD[1024];
    HANDLE VolumeHandle;
	bool Flag;
	DBThreat * dbthreat;
    DiskReader * diskread;
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;

//---------------------------------------------------------------------------
#endif
