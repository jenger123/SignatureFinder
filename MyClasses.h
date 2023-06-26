#pragma once
#include <iostream>
#include <windows.h>
#include <string>
#include <iomanip>
#include <utility>
#include <queue>
#include <mutex>
using namespace std;

class FileSystem
{
    friend class FileSystemIterator;
public:
    string GetName()
    {
        return this->name;
    }

    FileSystem(BYTE(&BootRecord)[1024])
    {
        StartOffset.QuadPart = 0;
    }
protected:
    int ClusterSize = 4096;
    ULONG TotalClusters = 0;
    LARGE_INTEGER StartOffset;
    string name;


};
class NTFS : public FileSystem
{
public:
    NTFS(BYTE(&BootRecord)[1024]) :FileSystem(BootRecord) {
        this->name = "ntfs";
        BootNTFSRecord = (BOOT_NTFS*)BootRecord;
        TotalClusters = BootNTFSRecord->sec_count / BootNTFSRecord->secs_cluster;
        ClusterSize = BootNTFSRecord->secs_cluster * BootNTFSRecord->sec_size;
    };
private:
#pragma pack(push, 1)
    typedef struct _BOOT_NTFS
    {
        BYTE    jump[3];
        BYTE    name[8];
        UINT16  sec_size;
        BYTE    secs_cluster;
        BYTE    reserved_0[7];
        BYTE    media_desc;
        UINT16  reserved_2;
        UINT16  secs_track;
        UINT16  num_heads;
        BYTE    reserved_3[8];
        UINT16  reserved_4;
        UINT16  reserved_5;
        ULONGLONG sec_count;
        UINT32  mft_clus_lo;
        UINT32  mft_clus_hi;
        UINT32  mft2_clus_lo;
        UINT32  mft2_clus_hi;
        UINT32  mft_rec_size;
        UINT32  buf_size;
        UINT32  volume_id_lo;
        UINT32  volume_id_hi;
    } BOOT_NTFS;
    BOOT_NTFS* BootNTFSRecord;
#pragma pack(pop)

};
class Exfat :public FileSystem
{
public:
    Exfat(BYTE(&BootRecord)[1024]) :FileSystem(BootRecord) {
        this->name = "exfat";
        BOOT_exFAT* BootRecord_ex = (BOOT_exFAT*)BootRecord;
        SectorSize = pow(2, int(BootRecord_ex->SectorSize));
        ClusterMultiplier = pow(2, BootRecord_ex->ClusterMultiplier);
        ClusterSize = SectorSize * ClusterMultiplier;
        TotalClusters = BootRecord_ex->NumberCluster;

        StartOffset.QuadPart = BootRecord_ex->FirstBitmapSector * SectorSize;

    };
private:
    int SectorSize;
    int ClusterMultiplier;
#pragma pack(push, 1)
    typedef struct
    {
        BYTE jump[3];
        BYTE name[8];
        BYTE reserved[77];
        UINT32 FirstBitmapSector;
        UINT32 NumberCluster;
        BYTE reserved2[12];
        BYTE SectorSize;
        BYTE ClusterMultiplier;
    } BOOT_exFAT;
#pragma pack(pop)
    BOOT_exFAT* BootRecord_ex;
};

class FileSystemIterator
{
    friend class FileSystemDecorator;
public:
    FileSystemIterator(FileSystem* FS, HANDLE diskHandle)
    {
        TotalClusters = FS->TotalClusters;
        ClusterSize = FS->ClusterSize;
        disk = diskHandle;
        Offset = FS->StartOffset;
        Cluster = new BYTE[ClusterSize];
	};
	ULONG GetCurrentCluster(){return CurrentCluster;}
    ULONG GetTotalClusters(){return TotalClusters;}

    ~FileSystemIterator()
    {
        //delete[] Cluster;
        CloseHandle(disk);
    }

    BYTE* GetCurrent()
    {
        DWORD BytesRead;
        auto readresult = ReadFile(disk, Cluster, ClusterSize, &BytesRead, NULL);
        DWORD res = SetFilePointer(disk, -ClusterSize, NULL, FILE_CURRENT);
        if (!readresult || BytesRead != ClusterSize) return NULL;
        else return Cluster;
    }
    void Next()
    {

		auto result = SetFilePointer(disk, ClusterSize, NULL, FILE_CURRENT);
			CurrentCluster++;
	}
    void begin()
    {
		auto result = SetFilePointer(disk, Offset.LowPart, &Offset.HighPart, FILE_BEGIN);
        CurrentCluster = 0;
    }
    bool isDone()
    {
		if (CurrentCluster == TotalClusters - 1)
		return true;
        else return false;
    }



private:
    int ClusterSize;
    ULONG TotalClusters = 0, CurrentCluster = 0;
    LARGE_INTEGER Offset;
    HANDLE disk;
    BYTE* Cluster;
};


class FileSystemDecorator
{
public:
    FileSystemDecorator(FileSystemIterator* Iterator)
    {
        It = Iterator;
        CurrentCluster = It->CurrentCluster;
        Cluster = new BYTE[It->ClusterSize];
    }
    void begin()
    {
        for (It->begin(); !It->isDone(); It->Next())
        {
            Cluster = It->GetCurrent();
            if (isJPEG(Cluster))
            {
                CurrentCluster = It->CurrentCluster;
                signature = "jpg/jpeg";
                break;
            }
            if (isPNG(Cluster))
            {
                CurrentCluster = It->CurrentCluster;
                signature = "png";
                break;
            }
            if (isBMP(Cluster))
            {
                CurrentCluster = It->CurrentCluster;
                signature = "bmp";
                break;
            }

        }
    }
    void Next()
    {
        It->Next();
		for (; !It->isDone(); It->Next())
        {
			Cluster = It->GetCurrent();
			if (isJPEG(Cluster))
			{
				CurrentCluster = It->CurrentCluster;
				signature = "jpg/jpeg";
				break;
			}
			if (isPNG(Cluster))
			{
				CurrentCluster = It->CurrentCluster;
                signature = "png";
                break;
            }
            if (isBMP(Cluster))
            {
                CurrentCluster = It->CurrentCluster;
                signature = "bmp";
                break;
            }
        }
    }
    bool isDone()
    {
        return It->isDone();
    }
    string GetSign() { return signature; }
    ULONG GetNumberCluster() { return CurrentCluster; }
    ~FileSystemDecorator() {
        delete[] Cluster;
        delete It;
    }
private:
    FileSystemIterator* It;
    BYTE* Cluster;
    string signature;
    ULONG CurrentCluster;
    bool isJPEG(BYTE* Cluster)
    {
        BYTE jpg[10]{ 0xff,0xd8,0xff,0xe0,0x00,0x10,0x4a,0x46,0x49,0x46 };
        for (int i = 0; i < 10; i++)
            if (jpg[i] != Cluster[i]) return false;
        return true;
    }
    bool isPNG(BYTE* Cluster)
    {
        BYTE png[8]{ 0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a };
        for (int i = 0; i < 8; i++)
            if (png[i] != Cluster[i]) return false;
        return true;
    }
    bool isBMP(BYTE* Cluster)
    {
        BYTE png[6]{ 0x42,0x4d,0x36,0x5d,0xb0,0x01 };
        for (int i = 0; i < 6; i++)
            if (png[i] != Cluster[i]) return false;
        return true;
    }
};



int DetectFileSystem(BYTE(&name)[1024])
{   // 1- ntfs 2- exfat 0 - nothing
    BYTE exfat[8] = { 0xeb,0x76, 0x90, 0x45, 0x58, 0x46, 0x41, 0x54 };
    BYTE ntfs[8] = { 0xeb,0x52, 0x90, 0x4E, 0x54, 0x46, 0x53, 0x20 };
    bool ntfs_check = true, exfat_check = true;
    for (int i = 0; i < 8; ++i)
        if (name[i] != exfat[i])
        {
            exfat_check = false;
            break;
        }
    for (int i = 0; i < 8; ++i)
        if (name[i] != ntfs[i])
        {
            ntfs_check = false;
            break;
        }
    if (ntfs_check == true && exfat_check == false) return 1;
    else if (ntfs_check == false && exfat_check == true) return 2;
    else return 0;
}

FileSystem * GetFS(BYTE(&bootrecord)[1024])
{

    int fsmode = DetectFileSystem(bootrecord);
    switch (fsmode) {
    case 1:
        return new NTFS(bootrecord);
    case 2:
        return new Exfat(bootrecord);
    default:
        return NULL;
    }

}

class pairqueue
{
private:
mutex m;
typedef pair<ULONG,string> note;
queue<note> notequeue;
public:
note frontpop()
{
lock_guard<mutex> lock(m);
pair<ULONG,string> a(0,"");
if(!notequeue.empty())
{
a=notequeue.front();
notequeue.pop();
};
return a;
}
void push(note a)
{
lock_guard<mutex> lock(m);
notequeue.push(a);
};
bool empty()
{
if(notequeue.empty()) return true;
else return false;
}
};
