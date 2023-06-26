//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "Unit2.h"
#include "Unit3.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "VirtualTrees"
#pragma resource "*.dfm"
TForm1 *Form1;


//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
VirtualStringTree1->NodeDataSize = sizeof(TreeData);
auto mask = GetLogicalDrives();
	int o = 0;
    for (int x = 0; x < 26; x++) // прогоняем по битам
    {
        auto n = ((mask >> x) & 1); // определяем значение текущего бита
        if (n) // если единица - диск с номером x есть
        {
			o++;
			ComboBox1->Items->Add((char)(65 + x));
        }
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ComboBox1Select(TObject *Sender)
{
Path=L"\\\\.\\"+ ComboBox1->Items->Strings[ComboBox1->ItemIndex] +L":";
VolumeHandle = CreateFileW(Path.c_str(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
if (VolumeHandle == INVALID_HANDLE_VALUE)
{
 ErrorMessageLabel->Visible = true;
ErrorMessageLabel->Caption = "Ошибка открытия тома";
}
DWORD BytesRead, BytesToRead = 1024;

	auto readvolume = ReadFile(VolumeHandle, BOOTRECORD, BytesToRead, &BytesRead, NULL);
	if (!readvolume || BytesRead != BytesToRead)
	{
    ErrorMessageLabel->Visible = true;
	ErrorMessageLabel->Caption = "Ошибка чтения загрузочной записи тома";
	}


}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormDestroy(TObject *Sender)
{
CloseHandle(VolumeHandle);
//delete dbthreat;
//delete diskread;
}
//---------------------------------------------------------------------------



void __fastcall TForm1::StartButtonClick(TObject *Sender)
{
Flag = false;
VirtualStringTree1->Clear();
MyFS = GetFS(BOOTRECORD);
dbthreat = new DBThreat(false,"clusters.db");
diskread = new DiskReader(false);
InfoLabel->Caption ="Выполняется чтение диска";
ProgressBar1->Position = 0;
StartButton->Enabled=false;
StopButton->Enabled = true;
 InfoLabel->Visible=true;
 Timer1->Interval =500;
  Timer1->Enabled=true;

}
//---------------------------------------------------------------------------


void __fastcall TForm1::VirtualStringTree1GetText(TBaseVirtualTree *Sender, PVirtualNode Node,
          TColumnIndex Column, TVSTTextType TextType, UnicodeString &CellText)

{
TreeData* treeData = (TreeData*)Sender->GetNodeData(Node);
	switch (Column) {
		case 0:
			CellText = treeData->id;
			break;
		case 1:
			CellText = treeData->NumberCluster;
			break;
		case 2:
            CellText = treeData->type;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
ProgressBar1->Position=int(diskread->It->GetCurrentCluster() * 100 / diskread->It->GetTotalClusters() );
}
//---------------------------------------------------------------------------

void __fastcall TForm1::StopButtonClick(TObject *Sender)
{
StopButton->Enabled=false;
StartButton->Enabled=true;
if(!dbthreat->Finished||!diskread->Finished)
{
Flag = true;
dbthreat->Terminate();
diskread->Terminate();
}





}
//---------------------------------------------------------------------------

