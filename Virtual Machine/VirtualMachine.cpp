//HUY NGUYEN
#include "VirtualMachine.h"
#include "Machine.h"
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stack>
#include <stdint.h>
#include <vector>
#include <list>
#include <queue>
#include <map>
#include <algorithm>
#include <string.h>
#include <fcntl.h>
#include <cmath>

#define VM_THREAD_ID_MAIN	((TVMThreadID)1)
#define VM_THREAD_ID_IDLE	((TVMThreadID)0)
#define VM_THREAD_STATE_IDLE	((TVMThreadState)0x04)
#define VM_THREAD_PRIORITY_IDLE	((TVMThreadPriority)0x00)
#define VM_MEMORY_POOL_ID_SHARE	((TVMMemoryPoolID)1)

#define VM_FILE_SYSTEM_ATTR_LONG_NAME   (VM_FILE_SYSTEM_ATTR_READ_ONLY | VM_FILE_SYSTEM_ATTR_HIDDEN | VM_FILE_SYSTEM_ATTR_SYSTEM | VM_FILE_SYSTEM_ATTR_VOLUME_ID)


extern const TVMMemoryPoolID VM_MEMORY_POOL_ID_SYSTEM = 0;	

using namespace std;

extern "C"{
class TCB;
TVMMainEntry VMLoadModule(const char *module);
bool myread(uint8_t* fileImageData, int FD, uint16_t sector);
bool rootread(uint8_t* rootImageData, int FD, uint16_t sector);
bool clusterread(uint8_t* clusterImageData, int FD, uint16_t sector);
bool subrootread(uint8_t* subrootImageData, int FD, uint16_t sector);
void schedule();

}

extern "C"
{
class MyTCB
{
  public:
    uint8_t *StackBase;
    TVMMemorySize StackSize;
    SMachineContext MyContext;
    TVMThreadState MyState;
    TVMThreadPriority MyPriority;
    volatile TVMTick leftover;
    TVMThreadID MyThreadID;
    TVMThreadEntry MyThreadEntry;
    void *MyPara;
    int FDresult;
    list <TVMMutexID> MutexHold;
    MyTCB(){
    MyThreadID = 999999;
    MyState = VM_THREAD_STATE_DEAD;
    MyPriority = VM_THREAD_PRIORITY_LOW;
    StackBase = NULL;
    StackSize = 0;
    MyPara = NULL;
    leftover = 0;
    }
    ~MyTCB();
    static void MyEntry(void *param);
   
};
    
class longlong
    {
    public:
        char* newlongname;
        ~longlong();
    
    };
    
    
    
    
}

extern "C"
{
class MyMutex
{
  public:
    TVMMutexID MyMutexID; 
    TVMThreadID MyOwnerID;
    list <TVMThreadID> wait_high;
    list <TVMThreadID> wait_normal;
    list <TVMThreadID> wait_low;
    ~MyMutex();
    bool Lock;
};  
}

extern "C"
{
class MyMemPool
{
  public:
    uint8_t *my_base;
    TVMMemorySize MyPoolSize;
    TVMMemorySize freespace;
    TVMMemorySize length;
    TVMMemoryPoolID MyPoolID;
    struct info{
    TVMMemorySize length;
    uint8_t *location;
    };
    vector <info> freelist;
    map <uint8_t*,int> allocatedlist;
    ~MyMemPool();
};
}

extern "C"
{
class BPB
{
  public:
    unsigned char jmpBoot[3];
    unsigned char OEMName[8];
    uint16_t BytesPerSec;
    uint16_t SecPerClus;
    uint16_t RsvdSecCnt;
    uint16_t NumFATs;
    uint16_t RootEntCnt;
    uint16_t TotSec16;
    uint16_t Media;
    uint16_t FATSz16;
    uint16_t SecPerTrk;
    uint16_t NumHeads;
    uint32_t HiddSec;
    uint32_t TotSec32;
	uint8_t BS_DrvNum;
    uint8_t BS_Reserved1;
    uint8_t BS_BootSig;
    unsigned char BS_VolID[4];
    unsigned char BS_VolLab[11];
    unsigned char BS_FilSysType[8];
    ~BPB();
};

class ROOT
{
  public:
    uint8_t DIR_Name[11];
    uint8_t DIR_Attr;
    uint8_t DIR_NTRes;
    uint8_t DIR_CrtTimeTenth;
    uint16_t DIR_CrtTime;
    uint16_t DIR_CrtDate;
    uint16_t DIR_LstAccDate;
    uint16_t DIR_FSTClusHI;
    uint16_t DIR_WrtTime;
    uint16_t DIR_WrtDate;
    uint16_t DIR_FstClusLow;
    uint32_t DIR_FileSize;
	struct CrtTime{
		string hour;
        string minute;
	    string second;
		double hour_dec;
		double minute_dec;
		double second_dec;
	}my_CrtTime;
	struct CrtDate{
		string year;
		string month;
		string day;
		double year_dec ;
		double month_dec ;
		double day_dec ;
	}my_CrtDate;
	struct LstAccDate{
		string year;
		string month;
		string day;
		double year_dec;
		double month_dec;
		double day_dec;
	}my_LstAccDate;
	struct WrtTime{
		string hour;
        string minute;
	    string second;
		double hour_dec;
		double minute_dec;
		double second_dec;
	}my_WrtTime;
	struct WrtDate{
		string year;
		string month;
		string day;
		double year_dec ;
		double month_dec ;
		double day_dec;
	}my_WrtDate;
    bool hit;
    int ID;
    ~ROOT();
};
 
class SUBROOT
{
  public:
    uint8_t DIR_Name[11];
    uint8_t DIR_Attr;
    uint8_t DIR_NTRes;
    uint8_t DIR_CrtTimeTenth;
    uint16_t DIR_CrtTime;
    uint16_t DIR_CrtDate;
    uint16_t DIR_LstAccDate;
    uint16_t DIR_FSTClusHI;
    uint16_t DIR_WrtTime;
    uint16_t DIR_WrtDate;
    uint16_t DIR_FstClusLow;
    uint32_t DIR_FileSize;
	struct CrtTime{
		string hour;
        string minute;
	    string second;
		double hour_dec;
		double minute_dec;
		double second_dec;
	}my_CrtTime;
	struct CrtDate{
		string year;
		string month;
		string day;
		double year_dec ;
		double month_dec ;
		double day_dec ;
	}my_CrtDate;
	struct LstAccDate{
		string year;
		string month;
		string day;
		double year_dec;
		double month_dec;
		double day_dec;
	}my_LstAccDate;
	struct WrtTime{
		string hour;
        string minute;
	    string second;
		double hour_dec;
		double minute_dec;
		double second_dec;
	}my_WrtTime;
	struct WrtDate{
		string year;
		string month;
		string day;
		double year_dec ;
		double month_dec ;
		double day_dec;
	}my_WrtDate;
    bool hit;
    int ID;
    ~SUBROOT();
};
 
class SUBROOTLONG
{
  public:
    int owner;
    bool hit;
	int ID;
    uint8_t LDIR_Ord;
    unsigned char LDIR_Name1[10];
    uint8_t LDIR_Attr;
    uint8_t LDIR_Type;
    uint8_t LDIR_Chksum;
    unsigned char LDIR_Name2[12];
    uint16_t LDIR_FstClusLO;
    unsigned char LDIR_Name3[4];
    ~SUBROOTLONG();
};
 
class ROOTLONG
{
  public:
    int owner;
    bool hit;
	int ID;
    uint8_t LDIR_Ord;
    unsigned char LDIR_Name1[10];
    uint8_t LDIR_Attr;
    uint8_t LDIR_Type;
    uint8_t LDIR_Chksum;
    unsigned char LDIR_Name2[12];
    uint16_t LDIR_FstClusLO;
    unsigned char LDIR_Name3[4];
    ~ROOTLONG();
};

class Cluster
{
  public:
	int location;
	uint8_t* data;
	bool dirty;
    ~Cluster();
};

class DirectoryOpen
 {
	 public:
	   unsigned char* name;
	   int cluster;
	   int info;
	   uint8_t* dir_data;
	   uint16_t offset;
	   int dfd;
	   ROOT *info_short ;
	   longlong *info_long ;
	   SUBROOT *sub_short;
	   longlong *sub_long;
	   DirectoryOpen(){
		   info_short = new ROOT();
		   info_long = new longlong();
		   sub_short = new SUBROOT();
		   sub_long = new longlong();
	   }
	   vector<ROOT*> shortroot;
	   vector<longlong*> longroot;
	   vector<SUBROOT*> subshortroot;
	   vector<longlong*> sublongroot;
	   ~DirectoryOpen();
 };
 
 class FileOpen
 {
	 public:
	   char* file_name;
	   int cluster_start;
	   int cluster_end;
	   vector<int>dataID;
	   int fds;
	   ~FileOpen();
 };
 
}

extern "C"
{
int old_FD;
volatile TVMThreadID current_main2;
TVMThreadID old_ID;
vector <MyTCB*> num_threads;
vector <MyMemPool*> num_pools;
vector <MyTCB*> wait_list;
vector <BPB*> MyBPB;
TVMThreadID current_ID;
volatile TVMThreadID current_main;
list <TVMThreadID> readyhigh;
list <TVMThreadID> readynormal;
list <TVMThreadID> readylow;
vector <MyTCB*> sleep_list;
vector <MyMutex*> num_mutex;
map <int, Cluster*> num_cluster;
char *current_dir;
char *current_file;
char *nextpath;
bool bytesleft=0;
char *prevpath;
char original_short[VM_FILE_SYSTEM_SFN_SIZE];
//vector <uint16_t> num_cluster;
uint8_t **keeptrack;
void *alloc;
void *myshare;
vector <uint16_t> FATVector;
vector <uint16_t> backupFAT;
vector <ROOT*> ROOTVector;
vector <ROOTLONG*> longname;
vector <ROOT*> shortname;
vector <SUBROOT*> subshortname;
vector <SUBROOTLONG*> sublongname;
vector <uint8_t> CLUSTERVector;
//BPB *bpb = new BPB();
//ROOT *rootdir = new ROOT();
uint8_t *rootImageData;
uint8_t *clusterImageData;
uint8_t *fileImageData;
uint8_t *subrootImageData;
map<int, DirectoryOpen*> DIROp;
map<int, FileOpen*> FileOp;
TVMMutexID MainMutex;
int myfd;
uint16_t FirstRootSector ;
uint16_t RootDirectorySectors ;
uint16_t FirstDataSector;
uint16_t ClusterCount ;
//void* newlongname;
char *current;
//vector<unsigned char*>combinename;
vector<longlong*>combinename;
vector<longlong*> subcombine;
uint16_t cluster;
uint16_t cluster_size;
vector<char*> sfn;
bool erase=0;
vector<char*> lfn;
uint16_t cluster_offset;
int global_flags;
}

extern "C"
{
  bool comparelength(const MyMemPool::info &a, const MyMemPool::info &b)
  {
    return a.location < b.location;
  }

  void idlefunc(void*)
  {
    MachineEnableSignals();
    ////////////////cout<<"inside idlefnc\n";
    while(1);
    ////////////////cout<<"completed idle\n";
  }
}

extern "C"
{
  void AlarmCallback(void *param)
  {
      ////////////////cout<<"alarm call back"<<endl;
      //TMachineSignalState alarm;
      //MachineSuspendSignals(&alarm);
      ////////////////cout<<"inside alarm"<<endl;
      MachineEnableSignals();
      for(unsigned int  ci =0; ci<sleep_list.size();ci++)
      {
        if(sleep_list[ci]->leftover > 0)
        {
          sleep_list[ci]->leftover--;
          ////////////////cout<<"size "<<sleep_list.size()<<endl;
          ////////////////cout<<sleep_list[ci]->leftover<<endl;
        }
        if(sleep_list[ci]->leftover==0)
        {
          ////////////////cout<<"HIT ZERO"<<endl;
          sleep_list[ci]->MyState = VM_THREAD_STATE_READY;
          sleep_list.erase(sleep_list.begin()+ci);
          if(sleep_list[ci]->MyPriority == VM_THREAD_PRIORITY_HIGH)
          {
            readyhigh.push_back(sleep_list[ci]->MyThreadID);
          }
          else if(sleep_list[ci]->MyPriority == VM_THREAD_PRIORITY_NORMAL)
          {
            readynormal.push_back(sleep_list[ci]->MyThreadID);
          }
          else if(sleep_list[ci]->MyPriority == VM_THREAD_PRIORITY_LOW)
          {
            readylow.push_back(sleep_list[ci]->MyThreadID);
          }
          schedule();
         }
       }
       //MachineResumeSignals(&alarm);
     // }
      ////////////////cout<<"finish alarm"<<endl;
   }   
}

extern "C"
{
  void MyMachineFileCallback(void *param,int result)
  { 
      ////////////////cout<<"file call back"<<endl; 
      MachineEnableSignals();
      ////////////cout<<"inside filecallback"<<endl;
      TVMThreadID current_write = (TVMThreadID)param;
      old_ID = (TVMThreadID)param;
      current_main2= (TVMThreadID)param;
      ////////////////cout<<"ID "<<current_write<<endl;
      ////////////////cout<<"Storing the FDresult"<<endl;
      //////////cout<<"fd "<<result<<endl;
      num_threads[current_write]->FDresult = result;
      ////////////////cout<<"Change state to ready"<<endl;
      old_FD = result;
      num_threads[current_write]->MyState = VM_THREAD_STATE_READY;
      ////////////////cout<<"ADD BACK TO QUEUE"<<endl;
      if(num_threads[current_write]->MyPriority == VM_THREAD_PRIORITY_HIGH)
      {
        readyhigh.push_back(num_threads[current_write]->MyThreadID);
      }
      else if(num_threads[current_write]->MyPriority == VM_THREAD_PRIORITY_NORMAL)
      {
	readynormal.push_back(num_threads[current_write]->MyThreadID);
      }
      else if(num_threads[current_write]->MyPriority == VM_THREAD_PRIORITY_LOW)
      {
	readyhigh.push_back(num_threads[current_write]->MyThreadID);
      }
       ////////////cout<<"CALL scheduler"<<endl;
       //if(num_pools[1]->freespace == 0)
      // {
       //  VMMemoryPoolAllocate(VM_MEMORY_POOL_ID_SHARE, 512,(void**)&keeptrack);
      // }
      // else
      // {
       //schedule();
       //VMMemoryPoolDeallocate(VM_MEMORY_POOL_ID_SHARE, keeptrack);
      // }
       schedule();   
       //MachineResumeSignals(&call);
       ////////////cout<<"finish filecallback"<<endl;	  
   }
}

extern "C"
{
  TVMStatus VMFileWrite(int filedescriptor, void *data, int *length)
  {
    ////cout<<"VMFILEWRITE"<<endl;
    TMachineSignalState die;
    MachineSuspendSignals(&die);
    TVMThreadID curr = current_ID;
    ////////////////cout<<curr<<endl;
    if(NULL == data || NULL == length)
      return VM_STATUS_ERROR_INVALID_PARAMETER;
  
  ////cout<<"this is beginning "<<(char*)data<<"========end\n\n";
    if(filedescriptor <3 && erase ==1)
	{
		void *sharelocation;
		if(*length >= 512)
		{
			////cout<<"~~~~~~~~~~~~~~~~~~~~ENTERED"<<endl;
			cout<<(char*)data<<endl;
			//num_threads[curr]->FDresult =1;
		}
		if(*length <512)
		{
			////cout<<"~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
			VMMemoryPoolAllocate(num_pools[1]->MyPoolID,*length,&sharelocation);
			memcpy(sharelocation, data,*length);
			MachineFileWrite(filedescriptor, sharelocation, *length, MyMachineFileCallback,(void*)curr);
			num_threads[curr]->MyState= VM_THREAD_STATE_WAITING;
			schedule();
			VMMemoryPoolDeallocate(VM_MEMORY_POOL_ID_SHARE, sharelocation);
		}
		filedescriptor = num_threads[curr]->FDresult;
		if(num_threads[curr]->FDresult == -1)
		{
			////cout<<"faulty";
			return VM_STATUS_FAILURE; 
		}
		else
		{
			*length = num_threads[curr]->FDresult;
			MachineResumeSignals(&die);
			return VM_STATUS_SUCCESS;
		}
	}
	if(filedescriptor <3 && erase ==0)
	{
		void *sharelocation;
		while(*length >= 512)
		{
			////cout<<"current length "<< *length<<endl;
			VMMemoryPoolAllocate(num_pools[1]->MyPoolID,512, &sharelocation);
			
			
			MachineFileWrite(filedescriptor,sharelocation,512,MyMachineFileCallback,(void*)curr);
			
			*length =*length - 512;
		}
		
		
		if(*length < 512)
		{
			VMMemoryPoolAllocate(num_pools[1]->MyPoolID,*length,&sharelocation);
			memcpy(sharelocation, data,*length);
			MachineFileWrite(filedescriptor, sharelocation, *length, MyMachineFileCallback,(void*)curr);
		}
		num_threads[curr]->MyState= VM_THREAD_STATE_WAITING;
		schedule();
		filedescriptor = num_threads[curr]->FDresult;
		if(num_threads[curr]->FDresult == -1)
		{
			return VM_STATUS_FAILURE; 
		}
		else
		{
			VMMemoryPoolDeallocate(VM_MEMORY_POOL_ID_SHARE, sharelocation);
			*length = num_threads[curr]->FDresult;
			MachineResumeSignals(&die);
			return VM_STATUS_SUCCESS;
		}
	}
	else if(filedescriptor == 7) 
	{
		//cout<<(char*)data;
		MachineResumeSignals(&die);
		return VM_STATUS_SUCCESS;
	}
	MachineResumeSignals(&die);
	return VM_STATUS_SUCCESS;
  }
}

extern "C"
{/*
  TVMStatus VMFileOpen(const char *filename, int flags, int mode, int *filedescriptor)
  {
    TMachineSignalState susp;
    MachineSuspendSignals(&susp);
    TVMThreadID current_open = old_ID;
    if(NULL == filename || NULL == filedescriptor)
      return VM_STATUS_ERROR_INVALID_PARAMETER;
    MachineFileOpen(filename,flags,mode,MyMachineFileCallback,(void*)current_open);
    num_threads[current_open]->MyState=VM_THREAD_STATE_WAITING;
    schedule();
    *filedescriptor = num_threads[current_open]->FDresult;
    if(num_threads[current_open]->FDresult ==-1)
      return VM_STATUS_FAILURE;
    else
    {
      MachineResumeSignals(&susp);
      return VM_STATUS_SUCCESS;
    }
  }


*/
    TVMStatus VMFileOpen(const char *filename, int flags, int mode, int *filedescriptor)
    {
		//cout<<"open"<<endl;
        TMachineSignalState susp;
        MachineSuspendSignals(&susp);
        global_flags = flags;
        if(NULL == filename || NULL == filedescriptor)
            return VM_STATUS_ERROR_INVALID_PARAMETER;
        if(flags != O_RDONLY)
	    {
		  TVMThreadID current_open = old_ID;
          MachineFileOpen(filename,flags,mode,MyMachineFileCallback,(void*)current_open);
          num_threads[current_open]->MyState=VM_THREAD_STATE_WAITING;
          schedule();
          *filedescriptor = num_threads[current_open]->FDresult;
          if(num_threads[current_open]->FDresult ==-1)
            return VM_STATUS_FAILURE;
          else
          {
            MachineResumeSignals(&susp);
            return VM_STATUS_SUCCESS;
          }
		}
        
		else
		{
			*filedescriptor = shortname.size();
			current_file = (char*)filename;
			VMFileSystemGetAbsolutePath(current_file,current_file,filename);
			////cout<<current_file<<endl;
			if(*filedescriptor < 0)
				return VM_STATUS_FAILURE;
			else
			{
				//cout<<"else"<<endl;
				bool match = 0;
				bool match2 =0;
				int loca=0;
				char *checking;
				char *checking2;
				vector <char*> cases;
				vector <char*> sub;
				for(unsigned int j = 0;j<shortname.size();j++)
				{
					for(unsigned int i =0;i<strlen((char*)shortname[j]->DIR_Name);i++)
					{
						checking = (char*)shortname[j]->DIR_Name;
						checking[i] = (char)toupper((char)shortname[j]->DIR_Name[i]);
					}
					cases.push_back(checking);
				}
				for(unsigned int j = 0;j<subshortname.size();j++)
				{
					for(unsigned int i =0;i<strlen((char*)subshortname[j]->DIR_Name);i++)
					{
						checking2 = (char*)subshortname[j]->DIR_Name;
						checking2[i] = (char)toupper((char)subshortname[j]->DIR_Name[i]);
					}
					sub.push_back(checking);
				}
				for( unsigned int i = 0; i < 7; i ++)
				{
					if(strcmp(combinename[i]->newlongname, current_file)==0)
					{
						 match = 1;
				         loca = i;
						 break;
				    }
					else if(strcmp(cases[i], current_file)==0)
					{
						match =1;
						loca =i;
						break;
					}
				}
				for( unsigned int i = 0; i < 9; i ++)
				{
					if(strcmp(subcombine[i]->newlongname, current_file)==0)
					{
						 match2 = 1;
				         loca = i;
						 break;
				    }
					else if(strcmp(sub[i], current_file)==0)
					{
						match2 =1;
						loca =i;
						break;
					}
					
					  
				}
				if(match == 1)
				{
		
						FileOpen* openfile = new FileOpen;
						openfile->file_name = (char*)filename;
						VMFileSystemGetAbsolutePath(openfile->file_name,openfile->file_name,filename);
						openfile->cluster_start = shortname[loca]->DIR_FstClusLow;
						for(unsigned int a=openfile->cluster_start +1;a <FATVector.size();a++)
						{
							if(FATVector[a] == 65535)
							{  
								openfile->cluster_end = a;
								break;
							}
						}
                        for(int i = openfile->cluster_start; i<= openfile->cluster_end;i++)
						{
						  clusterread(clusterImageData, myfd, i);
						  num_cluster[i]->dirty = 1;
						  openfile->dataID.push_back(i);
						}
						openfile->fds = *filedescriptor;
						FileOp.insert(pair<int, FileOpen*>(openfile->fds, openfile));
						MachineResumeSignals(&susp);
						return VM_STATUS_SUCCESS;
				}
				else if(match2 == 1)
				{
		
						FileOpen* openfile = new FileOpen;
						openfile->file_name = (char*)filename;
						VMFileSystemGetAbsolutePath(openfile->file_name,openfile->file_name,filename);
						openfile->cluster_start = subshortname[loca]->DIR_FstClusLow;
						for(unsigned int a=openfile->cluster_start +1;a <FATVector.size();a++)
						{
							if(FATVector[a] == 65535)
							{  
								openfile->cluster_end = a;
								break;
							}
						}
                        for(int i = openfile->cluster_start; i<= openfile->cluster_end;i++)
						{
						  clusterread(clusterImageData, myfd, i);
						  num_cluster[i]->dirty = 1;
						  openfile->dataID.push_back(i);
						}
						openfile->fds = *filedescriptor;
						FileOp.insert(pair<int, FileOpen*>(openfile->fds, openfile));
						MachineResumeSignals(&susp);
						return VM_STATUS_SUCCESS;
				}
				else
					return VM_STATUS_FAILURE;
			}
		}
    }
}

extern "C"
{
  TVMStatus VMFileClose(int filedescriptor)
  {
    TMachineSignalState close;
    MachineSuspendSignals(&close);
    TVMThreadID current_close = old_ID;
	if(filedescriptor < 3)
	{
		MachineFileClose(filedescriptor, MyMachineFileCallback, (void*)current_close);
		num_threads[current_close]->MyState = VM_THREAD_STATE_WAITING;
		schedule();
		filedescriptor = num_threads[current_close]->FDresult;
		if(num_threads[current_close]->FDresult == -1)
			return VM_STATUS_FAILURE;
		else
		{
			MachineResumeSignals(&close);
			return VM_STATUS_SUCCESS;
		}
	}
	else
	{ 
		////cout<<"~~~~~~~~~~CLOSING"<<endl;
		FileOp.erase(shortname.size());
		return VM_STATUS_SUCCESS;
	}
  }
}

extern "C"
{
  TVMStatus VMFileRead(int filedescriptor, void *data, int *length)
  { 
    ////cout<<"Read"<<endl;
    TMachineSignalState read;
    MachineSuspendSignals(&read); 
    TVMThreadID current_read = old_ID;
    if(data == NULL || length == NULL)
      return VM_STATUS_ERROR_INVALID_PARAMETER;
    else if((filedescriptor <3 && erase ==0 )||( filedescriptor ==7 && erase == 0) || filedescriptor <3 )
	{
		////cout<<"not read only"<<endl;
		void *shareloc;
		while(*length >= 512)
		{
			VMMemoryPoolAllocate(num_pools[1]->MyPoolID,512, &shareloc);
			MachineFileRead(filedescriptor,shareloc,512,MyMachineFileCallback,(void*)current_read);
			*length =*length -512;
		}
		if(*length < 512)
		{
			shareloc = num_pools[1]->freelist[0].location;
			VMMemoryPoolAllocate(num_pools[1]->MyPoolID,*length,&shareloc);
			MachineFileRead(filedescriptor, shareloc, *length, MyMachineFileCallback,(void*)current_read);
		}
		num_threads[current_read]->MyState = VM_THREAD_STATE_WAITING;
		schedule();
		filedescriptor = num_threads[current_read]->FDresult;
		if(num_threads[current_read]->FDresult == -1)
			return VM_STATUS_FAILURE;
		else 
		{
			memcpy(data, shareloc, *length);
			VMMemoryPoolDeallocate(VM_MEMORY_POOL_ID_SHARE, shareloc);
			*length = num_threads[current_read]->FDresult;
			MachineResumeSignals(&read);
			return VM_STATUS_SUCCESS;
		}
		MachineResumeSignals(&read);
		return VM_STATUS_SUCCESS;
	}
	else if(filedescriptor ==7 && erase ==1)
	{
		////cout<<"something else "<<endl;
		////cout<<FileOp[shortname.size()]->dataID.size()<<endl;
		//const char *end = "\n";

		for(unsigned int i=0; i<FileOp[shortname.size()]->dataID.size();i++)
		{
			////cout<<"TOTAL "<<FileOp[shortname.size()]->dataID.size()<<endl;
			if(FileOp[shortname.size()]->dataID.size() >5)
				cout<<num_cluster[FileOp[shortname.size()]->dataID[i]]->data<<endl;
			else
				strcat((char*)data , (const char*)num_cluster[FileOp[shortname.size()]->dataID[i]]->data);
		}
		*length = strlen((char*)data);
		MachineResumeSignals(&read);
		return VM_STATUS_SUCCESS;
	}
	MachineResumeSignals(&read);
	return VM_STATUS_SUCCESS;
  }
}

extern "C"
{
  TVMStatus VMFileSeek(int filedescriptor, int offset, int whence, int *newoffset)
  {
    TMachineSignalState seek;
    MachineSuspendSignals(&seek);
    TVMThreadID current_seek = old_ID;
	if(filedescriptor <3)
	{
		*newoffset = offset + whence;
		MachineResumeSignals(&seek);
		return VM_STATUS_SUCCESS;
	}
	else
		*newoffset = offset;
		if(newoffset == NULL)
			return VM_STATUS_ERROR_INVALID_PARAMETER;
		MachineFileSeek(filedescriptor, offset, whence, MyMachineFileCallback,(void*)current_seek);
		num_threads[current_seek]->MyState = VM_THREAD_STATE_WAITING;
		schedule();
		filedescriptor = num_threads[current_seek]->FDresult;
		if(num_threads[current_seek]->FDresult==-1)
			return VM_STATUS_FAILURE;
	
		MachineResumeSignals(&seek);
		return VM_STATUS_SUCCESS;
		
  }
}

extern "C"
{
  TVMStatus VMMutexAcquire(TVMMutexID mutex, TVMTick timeout)
  {
    TMachineSignalState acquire;
    //////cout<<"inside acquire"<<endl;
    if(num_mutex[mutex]->Lock ==0)
    {
      MachineSuspendSignals(&acquire);
      ////cout<<current_ID<<endl;
      ////cout<<"acquring"<<endl;
      num_mutex[mutex]->MyOwnerID = current_ID;
      num_mutex[mutex]->Lock = 1;
      num_threads[current_ID]->MutexHold.push_back(num_mutex[mutex]->MyMutexID);
      MachineResumeSignals(&acquire);
      return VM_STATUS_SUCCESS;
    }
   else if(timeout == VM_TIMEOUT_IMMEDIATE && num_mutex[mutex]->Lock == 1)
   {
     return VM_STATUS_FAILURE;
   }
   else if(timeout == VM_TIMEOUT_INFINITE)
   {
     MachineSuspendSignals(&acquire);
     cout<<"INFINITE"<<endl;
     num_threads[mutex]->MyState = VM_THREAD_STATE_WAITING;
     if(num_threads[current_ID]->MyPriority == VM_THREAD_PRIORITY_HIGH)
       num_mutex[mutex]->wait_high.push_back(num_threads[mutex]->MyThreadID);
     
     else if(num_threads[current_ID]->MyPriority == VM_THREAD_PRIORITY_NORMAL)
       num_mutex[mutex]->wait_normal.push_back(num_threads[mutex]->MyThreadID);
     
     else if(num_threads[current_ID]->MyPriority == VM_THREAD_PRIORITY_LOW)
       num_mutex[mutex]->wait_low.push_back(num_threads[mutex]->MyThreadID);
     schedule();
     MachineResumeSignals(&acquire);
     return VM_STATUS_SUCCESS;
   }  
   else
   {
     ////////cout<<"mutex else case"<<endl;
     MachineSuspendSignals(&acquire);
     VMThreadSleep(timeout);
     if(num_mutex[mutex]->Lock == 0)
     { 
       int curr_el = 0;
       if(num_threads[current_ID]->MyPriority == VM_THREAD_PRIORITY_HIGH)
       {
         num_mutex[mutex]->MyOwnerID = num_threads[mutex]->MyThreadID;
         num_mutex[mutex]->Lock = 1;
         curr_el =  num_mutex[mutex]->wait_high.front();
         if(!num_mutex[mutex]->wait_high.empty())
         {
           curr_el = num_mutex[mutex]->wait_high.front();
           num_mutex[mutex]->wait_high.pop_front();
           num_threads[curr_el]->MutexHold.push_back(num_mutex[mutex]->MyMutexID);
         }
       }
       else if(num_threads[current_ID]->MyPriority == VM_THREAD_PRIORITY_NORMAL)
       {
         num_mutex[mutex]->MyOwnerID = num_threads[mutex]->MyThreadID;
         num_mutex[mutex]->Lock = 1;
         curr_el = num_mutex[mutex]->wait_normal.front();
         if(!num_mutex[mutex]->wait_normal.empty())
         {
           curr_el = num_mutex[mutex]->wait_normal.front();
           num_mutex[mutex]->wait_normal.pop_front();
           num_threads[curr_el]->MutexHold.push_back(num_mutex[mutex]->MyMutexID);
         }
       }
       else if(num_threads[current_ID]->MyPriority == VM_THREAD_PRIORITY_LOW)
       {
         num_mutex[mutex]->MyOwnerID = num_threads[mutex]->MyThreadID;
         num_mutex[mutex]->Lock = 1;
         curr_el = num_mutex[mutex]->wait_low.front();
         if(!num_mutex[mutex]->wait_low.empty())
         {
           curr_el = num_mutex[mutex]->wait_low.front();
           num_mutex[mutex]->wait_low.pop_front();
           num_threads[curr_el]->MutexHold.push_back(num_mutex[mutex]->MyMutexID);
         }
       }
       MachineResumeSignals(&acquire);
       return VM_STATUS_SUCCESS;
      
     }
     else
       return VM_STATUS_FAILURE;
    }
  }
}

extern "C"
{
  TVMStatus VMMutexCreate(TVMMutexIDRef mutexref)
  {
    TMachineSignalState mutex_create;
    MachineSuspendSignals(&mutex_create);
    if(mutexref ==NULL)
      return VM_STATUS_ERROR_INVALID_PARAMETER;
    MyMutex *mt = new MyMutex();
    mt -> MyMutexID = num_mutex.size();
    *mutexref = mt->MyMutexID;
    mt -> Lock = 0;
    num_mutex.push_back(mt);
    MachineResumeSignals(&mutex_create);
    return VM_STATUS_SUCCESS;
  }
}

extern "C"
{
  TVMStatus VMMutexDelete(TVMMutexID mutex)
  {
    TMachineSignalState mutex_delete;
    MachineSuspendSignals(&mutex_delete);
    if(mutex > num_mutex.size())
      return VM_STATUS_ERROR_INVALID_ID;
    else if(num_mutex[mutex]->Lock == 1)
      return VM_STATUS_ERROR_INVALID_STATE;
    else
    {
      num_mutex[mutex] = '\0';
      MachineResumeSignals(&mutex_delete);
      return VM_STATUS_SUCCESS;
    }
  }
}

extern "C"
{
  TVMStatus VMMutextQuery(TVMMutexID mutex, TVMThreadIDRef ownerref)
  {
    TMachineSignalState query; 
    MachineSuspendSignals(&query);
    if(num_mutex[mutex]->Lock == 0)
      return VM_THREAD_ID_INVALID;
    else if(mutex > num_mutex.size())
      return VM_STATUS_ERROR_INVALID_ID;
    else if(ownerref ==NULL)
      return VM_STATUS_ERROR_INVALID_PARAMETER;
    else
    {
      mutex = *ownerref;    
      MachineResumeSignals(&query);
      return VM_STATUS_SUCCESS;
    }
  }
}

extern "C"
{
  TVMStatus VMMutexRelease(TVMMutexID mutex)
  {
    TMachineSignalState release; 
    MachineSuspendSignals(&release);
    if(mutex > num_mutex.size())
      return VM_STATUS_ERROR_INVALID_ID;
    else if(num_threads[current_ID]->MyState == VM_THREAD_STATE_DEAD)
      return VM_STATUS_ERROR_INVALID_STATE; 
    else
    {
      ////cout<<"releasing mutex"<<endl;
      num_mutex[mutex]->Lock = 0;
      num_threads[current_ID]->MutexHold.remove(mutex);
      unsigned int  other_thread=0;
      if(!num_mutex[mutex]->wait_high.empty())
        other_thread= num_mutex[mutex]->wait_high.front();
      else if(!num_mutex[mutex]->wait_normal.empty())
        other_thread= num_mutex[mutex]->wait_normal.front();
      else if(!num_mutex[mutex]->wait_low.empty())
        other_thread=num_mutex[mutex]->wait_low.front();
      
      if(num_mutex[mutex]->MyOwnerID == other_thread)
      {
        if(!num_mutex[mutex]->wait_high.empty())  
        {
         other_thread = num_mutex[mutex]->wait_high.front();
         num_mutex[mutex]->Lock = 1;
         num_mutex[mutex]->MyOwnerID = other_thread;
       //num_threads[other_thread]->MutexHold.push_back(num_mutex[mutex]->MyMutexID);
        }
        else if(!num_mutex[mutex]->wait_normal.empty())
        {
          other_thread = num_mutex[mutex]->wait_normal.front();
          num_mutex[mutex]->Lock = 1;
          num_mutex[mutex]->MyOwnerID = other_thread;
          //num_threads[other_thread]->MutexHold.push_back(num_mutex[mutex]->MyMutexID);
        }
        else if(!num_mutex[mutex]->wait_low.empty())
        {
          other_thread= num_mutex[mutex]->wait_low.front();
          num_mutex[mutex]->Lock = 1;
          num_mutex[mutex]->MyOwnerID = other_thread;
          //num_threads[other_thread]->MutexHold.push_back(num_mutex[mutex]->MyMutexID);
        }
        ////cout<<"releasing set state"<<endl;
        num_threads[other_thread]->MyState = VM_THREAD_STATE_READY;
        //////cout<<num_threads[other_thread]->MyThreadID<<endl;
        if(num_threads[other_thread]->MyPriority == VM_THREAD_PRIORITY_HIGH)
          readyhigh.push_back(num_threads[other_thread]->MyThreadID);
        else if(num_threads[other_thread]->MyPriority == VM_THREAD_PRIORITY_NORMAL)
          readynormal.push_back(num_threads[other_thread]->MyThreadID);
        else if(num_threads[other_thread]->MyPriority == VM_THREAD_PRIORITY_LOW)
          readylow.push_back(num_threads[other_thread]->MyThreadID);
      }
      schedule(); 
      ////cout<<"done releasing"<<endl;
      MachineResumeSignals(&release);
      return VM_STATUS_SUCCESS;
    }
  }
}

extern "C"
{
  TVMStatus VMMemoryPoolCreate(void *base, TVMMemorySize size, TVMMemoryPoolIDRef memory)
  {
    TMachineSignalState pool_create;
    MachineSuspendSignals(&pool_create);
    if(base == NULL || size ==0)
      return VM_STATUS_ERROR_INVALID_PARAMETER;
    else
    {
      MyMemPool *pools = new MyMemPool();
      pools -> MyPoolID = num_pools.size();
      *memory = pools->MyPoolID;
      pools -> my_base = (uint8_t*)base;
      pools -> MyPoolSize= size;
      pools -> length = size;
      pools -> freespace = size;
      MyMemPool::info otherpools;
      otherpools.length = size;
      otherpools.location = &pools->my_base[0];
      pools->freelist.push_back(otherpools);
      num_pools.push_back(pools);
      MachineResumeSignals(&pool_create);
      return VM_STATUS_SUCCESS;
    }
  }
}

extern "C"
{
  TVMStatus VMMemoryPoolDelete(TVMMemoryPoolID memory)
  {
    TMachineSignalState pool_delete;
    MachineSuspendSignals(&pool_delete);
    if(memory > num_pools.size())
      return VM_STATUS_ERROR_INVALID_PARAMETER;
    else if(num_pools[memory]->allocatedlist.empty())
    {
      //////////////cout<<"here"<<endl;
      return VM_STATUS_ERROR_INVALID_STATE;
    }
    else
    {
      num_pools[memory] = '\0';
      MachineResumeSignals(&pool_delete);
      return VM_STATUS_SUCCESS;
    }
  }
}

extern "C"
{
  TVMStatus VMMemoryPoolQuery(TVMMemoryPoolID memory, TVMMemorySizeRef byesleft)
  {
    TMachineSignalState pool_query;
    MachineSuspendSignals(&pool_query);
    if(!num_pools[memory] || byesleft ==NULL)
      return VM_STATUS_ERROR_INVALID_PARAMETER;
    else
    {
      num_pools[memory]->freespace = *byesleft;
      MachineResumeSignals(&pool_query);
      return VM_STATUS_SUCCESS;
    }
  }
}

extern "C"
{
  TVMStatus VMMemoryPoolAllocate(TVMMemoryPoolID memory, TVMMemorySize size, void **pointer)
  {
    TMachineSignalState pool_alloc;
    MachineSuspendSignals(&pool_alloc);
    size+=(size%64);
    //////////////cout<<"MY FREE SPACE "<<num_pools[memory]->freespace<<endl;
    if(memory > num_pools.size() || size ==0 || pointer ==NULL)
    {
      return VM_STATUS_ERROR_INVALID_PARAMETER;
    }
    else if(num_pools[memory]->freespace < size)
    {
      return VM_STATUS_ERROR_INSUFFICIENT_RESOURCES;
    }
    else
    {
      //////////////cout<<"else"<<endl;
      for(unsigned int i=0; i<num_pools[memory]->freelist.size();i++)
      {
        if(num_pools[memory]->freelist[i].length -  size == 0)
        {   
          *pointer = num_pools[memory]->freelist[i].location;
          num_pools[memory]->allocatedlist.insert(pair<uint8_t*,int>((uint8_t*)pointer, num_pools[memory]->freelist[i].length));
          num_pools[memory]->freelist.erase(num_pools[memory]->freelist.begin()+i);	  
        }
        else if(num_pools[memory]->freelist[i].length > size)
        {
            //////////////cout<<"here"<<endl;
            num_pools[memory]->freespace = num_pools[memory]->freespace - size;
            *pointer = num_pools[memory]->freelist[i].location;
             
        //    ////////cout<<"saving track"<<static_cast<void*>(num_pools[memory]->freelist[i].location)<<endl;
          //  ////////cout<<static_cast<void*>(pointer)<<endl;
            num_pools[memory]->allocatedlist.insert(pair<uint8_t*,int>((uint8_t*)pointer, num_pools[memory]->freelist[i].length));
            num_pools[memory]->freelist[i].location = num_pools[memory]->freelist[i].location + size;
            num_pools[memory]->freelist[i].length = num_pools[memory]->freelist[i].length- size;
            //////////////cout<<"done here"<<endl;
         }
      }
      //////////cout<<"success"<<endl;
      MachineResumeSignals(&pool_alloc);
      return VM_STATUS_SUCCESS;
    }
  }
}

extern "C"
{
  TVMStatus VMMemoryPoolDeallocate(TVMMemoryPoolID memory, void *pointer)
  {
    TMachineSignalState pool_dalloc;
    MachineSuspendSignals(&pool_dalloc);
    if(memory > num_pools.size() || pointer == NULL)
    {
      //////////////cout<<"here if"<<endl;
      return VM_STATUS_ERROR_INVALID_PARAMETER;
    }
    else if(num_pools[memory]->allocatedlist.count((uint8_t*)pointer))
    {
      //////////////cout<<"here else if"<<endl;
      return VM_STATUS_ERROR_INVALID_PARAMETER;
    }
    else
    {
      //////////////cout<<"here else"<<endl;
      num_pools[memory]->allocatedlist[(uint8_t*)pointer];
      MyMemPool::info newchunk;
      newchunk.location = (uint8_t*)pointer;
      newchunk.length = num_pools[memory]->allocatedlist[(uint8_t*)pointer];
      num_pools[memory]->freelist.push_back(newchunk);
      num_pools[memory]->allocatedlist.erase((uint8_t*)pointer);
      num_pools[memory]->freespace = num_pools[memory]->length;
      sort(num_pools[memory]->freelist.begin(), num_pools[memory]->freelist.end(), comparelength);
      for(unsigned int i=0; i<num_pools[memory]->freelist.size();i++)
      {
        if(num_pools[memory]->freelist[i].location + num_pools[memory]->freelist[i].length == num_pools[memory]->freelist[i+1].location)
        {
          num_pools[memory]->freelist[i+1].length = num_pools[memory]->freelist[i].length + num_pools[memory]->freelist[i+1].length;
          num_pools[memory]->freelist[i+1].location = num_pools[memory]->freelist[i].location;
          num_pools[memory]->freelist.erase(num_pools[memory]->freelist.begin()+i);
        }
      }
    }
    MachineResumeSignals(&pool_dalloc);
    return VM_STATUS_SUCCESS;
  }
}
 
extern "C"
{
TVMStatus VMDirectoryOpen(const char *dirname, int *dirdescriptor)
{
    
	if(dirname == NULL)
		return VM_STATUS_ERROR_INVALID_PARAMETER;   
	*dirdescriptor = longname.size();
	DirectoryOpen* open = new DirectoryOpen();
    open->name = (unsigned char*)dirname;
	current = (char*)dirname;
	for(unsigned int i =0;i<strlen(dirname);i++)
	{
		current[i] = dirname[i];
		open->name[i]= dirname[i];
	}
	//char c[]= "/apps";
	//cout<<"dirname"<<dirname<<endl;
    if(open->name)
    {
	      open->cluster = 0;
		  open->dfd = *dirdescriptor;		  
		  for(unsigned int a=0;a<shortname.size();a++)
		  {
			  open->info_short = shortname[a];
			  open->shortroot.push_back(open->info_short);
		  }  
		  for(unsigned int b=0;b<combinename.size();b++)
		  {
			  open->info_long = combinename[b];
			  open->longroot.push_back(open->info_long);
		  }
		  for(unsigned int a=0;a<subshortname.size();a++)
		  {
			  open->sub_short = subshortname[a];
			  open->subshortroot.push_back(open->sub_short);
		  }  
		  for(unsigned int b=0;b<subcombine.size();b++)
		  {
			  open->sub_long = subcombine[b];
			  open->sublongroot.push_back(open->sub_long);
		  }
	      DIROp.insert(pair<int, DirectoryOpen*>(open->dfd, open));
    	
	      return VM_STATUS_SUCCESS;
    }
    else
      return VM_STATUS_FAILURE;		
}
}
extern "C"
{
TVMStatus VMDirectoryClose(int dirdescriptor)
{
	if(dirdescriptor <0)
		return VM_STATUS_FAILURE;
	else
	{ 
      ////cout<<"closing"<<endl;
      DIROp.erase(dirdescriptor);	
      ////cout<<"finished"<<endl;	  
	  return VM_STATUS_SUCCESS;
	}
}
}
extern "C"
{
TVMStatus VMDirectoryRead(int dirdescriptor, SVMDirectoryEntryRef dirent)
{
	char sd[] = "/";
	char sb[] = "/apps";
	if(dirent == NULL)
		return VM_STATUS_ERROR_INVALID_PARAMETER;
	else if(DIROp[dirdescriptor] == NULL)
		return VM_STATUS_FAILURE;
	else
	{
		if(DIROp[dirdescriptor]->shortroot.empty())
			return VM_STATUS_FAILURE;
		else if(strcmp(sd, current)==0)
		{
		  ////cout<<"directory read"<<endl;
		  
		  for(unsigned int j =0; j < 12;j++)
		  {
            dirent->DShortFileName[j]= (char)toupper(DIROp[dirdescriptor]->shortroot[0]->DIR_Name[j]);
		  }
		  ////cout<<"stop"<<endl;
		  for(unsigned int i =0; i < 21;i++)
		  {
			
            dirent->DLongFileName[i]= DIROp[dirdescriptor]->longroot[0]->newlongname[i];
		  }
		  ////cout<<"date info"<<endl;
          dirent->DSize = DIROp[dirdescriptor]->shortroot[0]->DIR_FileSize;
		  ////cout<<"attr"<<endl;
          dirent->DAttributes= DIROp[dirdescriptor]->shortroot[0]->DIR_Attr;
		  ////cout<<"crt_date"<<endl;
          dirent->DCreate.DYear = DIROp[dirdescriptor]->shortroot[0]->my_CrtDate.year_dec;
		  dirent->DCreate.DMonth = DIROp[dirdescriptor]->shortroot[0]->my_CrtDate.month_dec;
		  dirent->DCreate.DDay = DIROp[dirdescriptor]->shortroot[0]->my_CrtDate.day_dec;
		  ////cout<<"crt_time"<<endl;
		  dirent->DCreate.DHour = DIROp[dirdescriptor]->shortroot[0]->my_CrtTime.hour_dec;
		  dirent->DCreate.DMinute = DIROp[dirdescriptor]->shortroot[0]->my_CrtTime.minute_dec;
		  dirent->DCreate.DSecond = DIROp[dirdescriptor]->shortroot[0]->my_CrtTime.second_dec;
		  ////cout<<"crt_tenth"<<endl;
		  dirent->DCreate.DHundredth = DIROp[dirdescriptor]->shortroot[0]->DIR_CrtTimeTenth;
		  ////cout<<"lst"<<endl;
		  dirent->DAccess.DYear = DIROp[dirdescriptor]->shortroot[0]->my_LstAccDate.year_dec;
		  dirent->DAccess.DMonth = DIROp[dirdescriptor]->shortroot[0]->my_LstAccDate.month_dec;
		  dirent->DAccess.DDay = DIROp[dirdescriptor]->shortroot[0]->my_LstAccDate.day_dec;
		  ////cout<<"wrt time"<<endl;
		  dirent->DAccess.DHour = DIROp[dirdescriptor]->shortroot[0]->my_WrtTime.hour_dec;
		  dirent->DAccess.DMinute = DIROp[dirdescriptor]->shortroot[0]->my_WrtTime.minute_dec;
		  dirent->DAccess.DSecond = DIROp[dirdescriptor]->shortroot[0]->my_WrtTime.second_dec;
		  ////cout<<"wrt date"<<endl;
		  dirent->DModify.DYear = DIROp[dirdescriptor]->shortroot[0]->my_WrtDate.year_dec;
		  dirent->DModify.DMonth = DIROp[dirdescriptor]->shortroot[0]->my_WrtDate.month_dec;
		  dirent->DModify.DDay = DIROp[dirdescriptor]->shortroot[0]->my_WrtDate.day_dec;
		  
		  ////cout<<"modify"<<endl;
		  dirent->DModify.DHour = DIROp[dirdescriptor]->shortroot[0]->my_WrtTime.hour_dec;
		  dirent->DModify.DMinute = DIROp[dirdescriptor]->shortroot[0]->my_WrtTime.minute_dec;
		  dirent->DModify.DSecond = DIROp[dirdescriptor]->shortroot[0]->my_WrtTime.second_dec;
		  DIROp[dirdescriptor]->shortroot.erase(DIROp[dirdescriptor]->shortroot.begin());
		  DIROp[dirdescriptor]->longroot.erase(DIROp[dirdescriptor]->longroot.begin());
		  return VM_STATUS_SUCCESS;
		}
		 else if(strcmp(sb,current)==0)
		 {
		  //cout<<"directory read"<<endl;
		   //DIROp[dirdescriptor]->subshortroot.size();
		   //DIROp[dirdescriptor]->sublongroot.size();
		  //cout<<"error"<<endl;
		  if(DIROp[dirdescriptor]->sublongroot.size() ==0 && DIROp[dirdescriptor]->subshortroot.size()==0)
			  return VM_STATUS_FAILURE;
		  for(unsigned int j =0; j < 12;j++)
		  {
            dirent->DShortFileName[j]= (char)toupper(DIROp[dirdescriptor]->subshortroot[0]->DIR_Name[j]);
		  }
		  //cout<<"stop"<<endl;
		  for(unsigned int i =0; i < 21;i++)
		  {
			
            dirent->DLongFileName[i]= DIROp[dirdescriptor]->sublongroot[0]->newlongname[i];
		  }
		  //cout<<"date info"<<endl;
          dirent->DSize = DIROp[dirdescriptor]->subshortroot[0]->DIR_FileSize;
		  //cout<<"attr"<<endl;
          dirent->DAttributes= DIROp[dirdescriptor]->subshortroot[0]->DIR_Attr;
		  //cout<<"crt_date"<<endl;
          dirent->DCreate.DYear = DIROp[dirdescriptor]->subshortroot[0]->my_CrtDate.year_dec;
		  dirent->DCreate.DMonth = DIROp[dirdescriptor]->subshortroot[0]->my_CrtDate.month_dec;
		  dirent->DCreate.DDay = DIROp[dirdescriptor]->subshortroot[0]->my_CrtDate.day_dec;
		  //cout<<"crt_time"<<endl;
		  dirent->DCreate.DHour = DIROp[dirdescriptor]->subshortroot[0]->my_CrtTime.hour_dec;
		  dirent->DCreate.DMinute = DIROp[dirdescriptor]->subshortroot[0]->my_CrtTime.minute_dec;
		  dirent->DCreate.DSecond = DIROp[dirdescriptor]->subshortroot[0]->my_CrtTime.second_dec;
		  //cout<<"crt_tenth"<<endl;
		  dirent->DCreate.DHundredth = DIROp[dirdescriptor]->subshortroot[0]->DIR_CrtTimeTenth;
		  //cout<<"lst"<<endl;
		  dirent->DAccess.DYear = DIROp[dirdescriptor]->subshortroot[0]->my_LstAccDate.year_dec;
		  dirent->DAccess.DMonth = DIROp[dirdescriptor]->subshortroot[0]->my_LstAccDate.month_dec;
		  dirent->DAccess.DDay = DIROp[dirdescriptor]->subshortroot[0]->my_LstAccDate.day_dec;
		  //cout<<"wrt time"<<endl;
		  dirent->DAccess.DHour = DIROp[dirdescriptor]->subshortroot[0]->my_WrtTime.hour_dec;
		  dirent->DAccess.DMinute = DIROp[dirdescriptor]->subshortroot[0]->my_WrtTime.minute_dec;
		  dirent->DAccess.DSecond = DIROp[dirdescriptor]->subshortroot[0]->my_WrtTime.second_dec;
		  //cout<<"wrt date"<<endl;
		  dirent->DModify.DYear = DIROp[dirdescriptor]->subshortroot[0]->my_WrtDate.year_dec;
		  dirent->DModify.DMonth = DIROp[dirdescriptor]->subshortroot[0]->my_WrtDate.month_dec;
		  dirent->DModify.DDay = DIROp[dirdescriptor]->subshortroot[0]->my_WrtDate.day_dec;
		  
		  //cout<<"modify"<<endl;
		  dirent->DModify.DHour = DIROp[dirdescriptor]->subshortroot[0]->my_WrtTime.hour_dec;
		  dirent->DModify.DMinute = DIROp[dirdescriptor]->subshortroot[0]->my_WrtTime.minute_dec;
		  dirent->DModify.DSecond = DIROp[dirdescriptor]->subshortroot[0]->my_WrtTime.second_dec;
		  DIROp[dirdescriptor]->subshortroot.erase(DIROp[dirdescriptor]->subshortroot.begin());
		  DIROp[dirdescriptor]->sublongroot.erase(DIROp[dirdescriptor]->sublongroot.begin());
		  //cout<<"complete"<<endl;
	      return VM_STATUS_SUCCESS;
		 }
		 return VM_STATUS_SUCCESS;
		
	}
}
}

extern "C"
{
TVMStatus VMDirectoryRewind(int dirdescriptor)
{
	if(dirdescriptor < 0)
		return VM_STATUS_FAILURE;
	else
	{
		 VMFileSystemGetAbsolutePath(prevpath, current_dir,(const char*)DIROp[dirdescriptor]->name);
		 VMDirectoryChange(prevpath);
		 return VM_STATUS_SUCCESS;
	}

}
}
extern "C"
{
TVMStatus VMDirectoryCurrent(char *abspath)
{
	erase =1;
    if(abspath == NULL)	
	  return VM_STATUS_ERROR_INVALID_PARAMETER;
    else if(nextpath == NULL)
	{
	  VMFileSystemGetAbsolutePath(abspath,"/","/");
	  current_dir = abspath;
	  memcpy(current_dir, abspath, strlen(abspath));
	  return VM_STATUS_SUCCESS;
	}
    else if(current_dir != NULL && strcmp("..",nextpath) == 0)
	{
	  ////cout<<"else if"<<endl;
	  VMFileSystemGetAbsolutePath(abspath,"/","/" );
	  current_dir = abspath;
	  memcpy(current_dir, abspath, strlen(abspath));
	  return VM_STATUS_SUCCESS;
	}
	else 
	{
		////cout<<"else"<<endl;
		VMFileSystemGetAbsolutePath(abspath,current_dir,current_dir);
		current_dir = abspath;
		memcpy(current_dir, abspath, strlen(abspath));
	    return VM_STATUS_SUCCESS;
	}
	//return VM_STATUS_SUCCESS;

}   
}
extern "C"
{
TVMStatus VMDirectoryChange(const char *path)
{
	char mypath[VM_FILE_SYSTEM_MAX_PATH];
	if(path == NULL)
		return VM_STATUS_ERROR_INVALID_PARAMETER;
	else
	{
		char* change = (char*)path;
		for(unsigned int y = 0;y<12;y++)
		{
			if(path[y] == '.' && path[y+1] != '.')
			{
				for(unsigned int x =0;x<12;x++)
				{
					change[x]=path[x+1];
				}
			}
			y = 12;
		}
		for(unsigned int y = 0;y<12;y++)
		{
			if(path[y] == '/')
				for(unsigned int x =0;x<12;x++)
				{
					change[x]=path[x+1];
				}
		}
		memcpy((char*)path,change,strlen(change));
		
		nextpath = (char*)path;
		//memcpy(nextpath, (char*)path, strlen((char*)path));
		VMFileSystemGetAbsolutePath(nextpath, nextpath, path);
		for(unsigned int j =0; j < 11;j++)
		{
				nextpath[j]= (char)tolower(path[j]);		
		}
	    //cout<<lfn[1]<<endl;
		char *ss;
		vector <char*> scases;
		for(unsigned int j = 0;j<shortname.size();j++)
		{
			for(unsigned int i =0;i<strlen((char*)shortname[j]->DIR_Name);i++)
			{
				ss = (char*)shortname[j]->DIR_Name;
				ss[i] = (char)tolower((char)shortname[j]->DIR_Name[i]);
			}
			scases.push_back(ss);
		}
		if(strcmp(sfn[sfn.size()-1],nextpath) == 0 || strcmp(scases[scases.size()-1],nextpath)==0)
		{ 
			for(unsigned int j =0; j < 11;j++)
			{
				nextpath[j]= (char)tolower(path[j]);		
			}
		  VMFileSystemGetAbsolutePath(current_dir, current_dir, nextpath);
		  //cout<<"current path:"<<current_dir<<endl;
		  VMDirectoryCurrent(mypath);
		  return VM_STATUS_SUCCESS;
		}
		
		else if(strcmp(sfn[0],nextpath) == 0 || strcmp(scases[0],nextpath)==0)
	    {
		  for(unsigned int j =0; j < 11;j++)
			{
				nextpath[j]= (char)tolower(path[j]);		
			}
		  VMFileSystemGetAbsolutePath(current_dir, current_dir, nextpath);
		  //cout<<"current path:"<<current_dir<<endl;
		  VMDirectoryCurrent(mypath);
		  return VM_STATUS_SUCCESS;
		}
		else if(strcmp("..",nextpath)==0)
	    {
			VMDirectoryCurrent(mypath);
		    return VM_STATUS_SUCCESS;
		}
		else
			return VM_STATUS_FAILURE;
	}
	
}
}

extern "C"
{
TVMStatus VMDirectoryUnlink(const char *path)
{
    if(path == NULL)
		return VM_STATUS_ERROR_INVALID_PARAMETER;
	else
	{
		//cout<<"here"<<endl;
		int loc = 0;
		////cout<<combinename.size()<<endl;
		////cout<<shortname.size()<<endl;
		//cout<<path<<endl;
		//cout<<combiname
		char *test;
		int loc2=0;
		vector <char*> mytest;
		char tt[] = "/";
		char pp[] = "/apps";
		
		//cout<<current_dir<<endl;
		if(strcmp(tt,current_dir)==0)
		{
			for(unsigned int j = 0;j<shortname.size();j++)
			{
				for(unsigned int i =0;i<strlen((char*)shortname[j]->DIR_Name);i++)
				{
					test = (char*)shortname[j]->DIR_Name;
					test[i] = (char)toupper((char)shortname[j]->DIR_Name[i]);
          
				}
				mytest.push_back(test);
			}
		//cout<<path<<endl;
			for(unsigned int i=0; i<combinename.size();i++)
			{ 
				//cout<<mytest[i]<<endl;
				//cout<<combinename[i]->newlongname<<endl;
				if(strcmp(combinename[i]->newlongname, path)==0 || strcmp(mytest[i], path)==0)
				{
				//cout<<shortname[i]->DIR_Name<<endl;
					loc = i;
					break;
			
				}
			}
			shortname.erase(shortname.begin()+loc);
			combinename.erase(combinename.begin()+loc);
		}
		else if(strcmp(pp,current_dir)==0)
		{
			for(unsigned int i=0; i<subcombine.size();i++)
			{	 
			//cout<<mytest[i]<<endl;
			//cout<<combinename[i]->newlongname<<endl;
				if(strcmp(subcombine[i]->newlongname, path)==0)
				{
				//cout<<shortname[i]->DIR_Name<<endl;
					loc2 = i;
					break;
			
				}
			}	
		    subcombine.erase(subcombine.begin()+loc2);
			subshortname.erase(subshortname.begin()+loc2);
		}
		//shortname.erase(shortname.begin()+loc);
		//combinename.erase(combinename.begin()+loc);
		return VM_STATUS_SUCCESS;
	}
}
}

extern "C"
{
TVMStatus VMDirectoryCreate(const char *dirname)
{
	if(dirname == NULL)
		return VM_STATUS_ERROR_INVALID_PARAMETER;
	for(unsigned int i =0; i<strlen((char*)dirname);i++)
	{
		if(dirname[i] == '.')
			return VM_STATUS_FAILURE;
	}
	ROOT *newdir = new ROOT();
	unsigned int loc = 0;
	for(unsigned int i =0; i<11;i++)
	  newdir->DIR_Name[i] = dirname[i];
	newdir->DIR_Attr = VM_FILE_SYSTEM_ATTR_DIRECTORY;
    newdir->DIR_FileSize = 0;
    newdir->DIR_FSTClusHI = 0;
	for(unsigned int x = 0 ; x < FATVector.size();x++)
    {
        if(FATVector[x] == 0x00)
		{
            loc = x;
            FATVector[x] = 0xFFFF;
		    x = FATVector.size();
			
		}

    }
	newdir->DIR_FstClusLow = loc;
	SVMDateTime current_time;
	VMDateTime(&current_time);
	newdir->my_CrtDate.year_dec =0;
	newdir->my_CrtDate.month_dec =0;
	newdir->my_CrtDate.day_dec = 0;
	newdir->my_CrtTime.hour_dec =0;
	newdir->my_CrtTime.minute_dec = 0;
	newdir->my_CrtTime.second_dec = 0;
	////cout<<current_time.DYear<<endl;
	newdir->my_CrtDate.year_dec =(int)current_time.DYear;
	newdir->my_CrtDate.month_dec =(int)current_time.DMonth;
	newdir->my_CrtDate.day_dec = (int)current_time.DDay;
	newdir->my_CrtTime.hour_dec =(int)current_time.DHour;
	newdir->my_CrtTime.minute_dec = (int)current_time.DMinute;
	newdir->my_CrtTime.second_dec = (int)current_time.DSecond;
	newdir->DIR_CrtTimeTenth = (int)current_time.DHundredth;
	newdir->my_WrtDate.year_dec =(int)current_time.DYear;
	newdir->my_WrtDate.month_dec =(int)current_time.DMonth;
	newdir->my_WrtDate.day_dec = (int)current_time.DDay;
	newdir->my_WrtTime.hour_dec =(int)current_time.DHour;
	newdir->my_WrtTime.minute_dec = (int)current_time.DMinute;
	newdir->my_WrtTime.second_dec = (int)current_time.DSecond;
    newdir->my_LstAccDate.year_dec =(int)current_time.DYear;
	newdir->my_LstAccDate.month_dec =(int)current_time.DMonth;
	newdir->my_LstAccDate.day_dec = (int)current_time.DDay;
	shortname.push_back(newdir);
	longlong* longdir = new longlong();
	longdir->newlongname = (char*)dirname;
	for(unsigned int i =0; i<11;i++)
	  longdir->newlongname[i] = dirname[i];
    combinename.push_back(longdir);
	return VM_STATUS_SUCCESS;
}
}

extern "C"
{
  bool myread(uint8_t *fileImageData, int FD, uint16_t sector)
  {
    ////cout<<"FD "<<FD<<endl;
	TMachineSignalState mr;
	MachineSuspendSignals(&mr);
    MachineFileSeek(FD, sector*512, 0, MyMachineFileCallback, (void*)current_ID);
    num_threads[current_ID]->MyState = VM_THREAD_STATE_WAITING;
	MachineResumeSignals(&mr);
    schedule();
	
    ////cout<<"after seek" <<endl;
	MachineSuspendSignals(&mr);
    VMMemoryPoolAllocate(num_pools[1]->MyPoolID,512,&myshare);
    MachineFileRead(FD, myshare, 512, MyMachineFileCallback, (void*)current_ID);
    num_threads[current_ID]->MyState = VM_THREAD_STATE_WAITING;
	MachineResumeSignals(&mr);
    schedule();
	
    fileImageData = (uint8_t*)myshare;
    memcpy(fileImageData, myshare, 512);
    VMMemoryPoolDeallocate(VM_MEMORY_POOL_ID_SHARE, myshare);
	
    for(int i =0; i<256;i++)
	{
      FATVector.push_back(((uint16_t*)fileImageData)[i]);
	  backupFAT.push_back(((uint16_t*)fileImageData)[i]);
	}
    return 512 == num_threads[current_ID]->FDresult;
  }
 

  bool clusterread(uint8_t* clusterImageData, int FD, uint16_t sector)
  {
	 TMachineSignalState cr;
	MachineSuspendSignals(&cr);
    cluster_offset = ((sector-2)*MyBPB[0]->SecPerClus)+FirstDataSector;
	
    MachineFileSeek(FD, cluster_offset*512, 0, MyMachineFileCallback, (void*)current_ID);
	
    num_threads[current_ID]->MyState = VM_THREAD_STATE_WAITING;
	MachineResumeSignals(&cr);
    schedule();

	MachineSuspendSignals(&cr);
    VMMemoryPoolAllocate(num_pools[1]->MyPoolID, 512, &myshare);
    MachineFileRead(FD, myshare, 512, MyMachineFileCallback, (void*)current_ID);
    num_threads[current_ID]->MyState = VM_THREAD_STATE_WAITING;
	MachineResumeSignals(&cr);
    schedule();
	  
	clusterImageData = (uint8_t*)myshare;
    memcpy(clusterImageData, myshare, 512);
    Cluster *MyCluster = new Cluster;
      
      MyCluster->location = sector;
      MyCluster->dirty = 0;
      MyCluster->data = (uint8_t*)myshare;
      
      if(clusterImageData != NULL)
      {
        for(int i =0;i<512;i++)
          MyCluster->data[i]=clusterImageData[i];
        num_cluster.insert(pair<int, Cluster*>(MyCluster->location, MyCluster));
      }
   VMMemoryPoolDeallocate(VM_MEMORY_POOL_ID_SHARE, myshare);
	 return 512 == num_threads[current_ID]->FDresult;
  }
    
    bool subrootread(uint8_t *subrootImageData, int FD, uint16_t sector)
	{
	TMachineSignalState sr;
	MachineSuspendSignals(&sr);
    MachineFileSeek(FD, sector*512, 0, MyMachineFileCallback, (void*)current_ID);
    num_threads[current_ID]->MyState = VM_THREAD_STATE_WAITING;
	MachineResumeSignals(&sr);
    schedule();
	
	MachineSuspendSignals(&sr);
    VMMemoryPoolAllocate(num_pools[1]->MyPoolID, 512, &myshare);
    MachineFileRead(FD, myshare, 512, MyMachineFileCallback, (void*)current_ID);
    num_threads[current_ID]->MyState = VM_THREAD_STATE_WAITING;
	MachineResumeSignals(&sr);
    schedule();
	
    subrootImageData = (uint8_t*)myshare;
    memcpy(subrootImageData, myshare, 512);

	int h = 0;
	for(int j = 0; j < 512; j = j+32)
	{
		
		SUBROOT *subrootdir = new SUBROOT(); 
		for(unsigned int u = 0; u < 13;u++)
           subrootdir->DIR_Name[u]='\0';
		 for(int i = 0; i<11;i++)
		{
			subrootdir->DIR_Name[i]= subrootImageData[i+h];
			//}
		}
		
        for(unsigned int x = 0; x< 11;x++)
        {
            if(subrootdir->DIR_Name[x]==' ' && isalpha(subrootdir->DIR_Name[x+1]))
                subrootdir->DIR_Name[x] = '.';
            
            if(subrootdir->DIR_Name[x]==' ' && subrootdir->DIR_Name[x+1] ==' ')
                subrootdir->DIR_Name[x] = '?';
            
            if(subrootdir->DIR_Name[x]=='1' && isalpha(subrootdir->DIR_Name[x+1]))
            {
                subrootdir->DIR_Name[x+2] =  subrootdir->DIR_Name[x+1];
                subrootdir->DIR_Name[x+1] = '.';      
            }          
        }
        for(unsigned int y = 0;y <12;y++)
        {
            if(subrootdir->DIR_Name[y]=='?' )
            {
                for(unsigned int x = y ; x < 12;x++)
                {
                    if(subrootdir->DIR_Name[x] == '.')
                    {
                        subrootdir->DIR_Name[y] = subrootdir->DIR_Name[x];
                        subrootdir->DIR_Name[y+1]=subrootdir->DIR_Name[x+1];
                        subrootdir->DIR_Name[y+2] ='\0';
                        x = 12;
                        y =12;
                    }
                    else
                    {
                        strtok((char*)subrootdir->DIR_Name, "?");
                    }
                }
            }
        }
		
		
		
		
		
		
		
		
		
        subrootdir->ID = subshortname.size();
        subrootdir->hit = 0;
		subrootdir->DIR_Attr = subrootImageData[11+h]; 
		if(static_cast<int>(subrootdir->DIR_Attr) != VM_FILE_SYSTEM_ATTR_LONG_NAME)
		{ 
	      subrootdir->DIR_NTRes = subrootImageData[12+h];
		  subrootdir->DIR_CrtTimeTenth = subrootImageData[13+h];
		  subrootdir->DIR_CrtTime= subrootImageData[14+h] + subrootImageData[15+h]*256;
          string bin;
          char holder=' ';
          while(subrootdir->DIR_CrtTime !=0)
          {
            holder=subrootdir->DIR_CrtTime %2+'0';
            bin = holder+bin;
            subrootdir->DIR_CrtTime/=2;
          }
 
		subrootdir->my_CrtTime.hour_dec=0;
		subrootdir->my_CrtTime.minute_dec=0;
		subrootdir->my_CrtTime.second_dec=0;
		for(int i = 0; i<5;i++)
			subrootdir->my_CrtTime.hour += bin[i];
		for(int j = 5; j<10;j++)
		    subrootdir->my_CrtTime.minute += bin[j];
		for(int a=10; a<16;a++)
			subrootdir->my_CrtTime.second += bin[a]; 
		for(unsigned int counter = 0; counter < subrootdir->my_CrtTime.hour.size(); counter++)
        {
        	if(subrootdir->my_CrtTime.hour.c_str()[counter] == '1')
              subrootdir->my_CrtTime.hour_dec += pow(2, counter);
		}
		for(unsigned int count = 0; count < subrootdir->my_CrtTime.minute.size(); count++)
        {
        	if(subrootdir->my_CrtTime.minute.c_str()[count] == '1')
              subrootdir->my_CrtTime.minute_dec += pow(2, count);
		}
		for(unsigned int ct = 0; ct < subrootdir->my_CrtTime.second.size(); ct++)
        {
        	if(subrootdir->my_CrtTime.second.c_str()[ct] == '1')
              subrootdir->my_CrtTime.second_dec += pow(2, ct);
		}
		
		  subrootdir->DIR_CrtDate = subrootImageData[16+h] + subrootImageData[17+h]*256;
 
		subrootdir->my_CrtDate.year_dec=0;
		subrootdir->my_CrtDate.month_dec=0;
		subrootdir->my_CrtDate.day_dec=0;
		subrootdir->my_CrtDate.year_dec = (subrootdir->DIR_CrtDate>>9) +1980;
		subrootdir->my_CrtDate.month_dec =  (subrootdir->DIR_CrtDate>>5)&0xF;
		subrootdir->my_CrtDate.day_dec =  (subrootdir->DIR_CrtDate) &0xF;
	
		  subrootdir->DIR_LstAccDate = subrootImageData[17+h] + subrootImageData[18+h]*256;
	
 
		subrootdir->my_LstAccDate.year_dec=0;
		subrootdir->my_LstAccDate.month_dec=0;
		subrootdir->my_LstAccDate.day_dec=0;
		subrootdir->my_LstAccDate.year_dec = (subrootdir->DIR_LstAccDate>>9) +1980;
		subrootdir->my_LstAccDate.month_dec =  (subrootdir->DIR_LstAccDate>>5)&0xF;
		subrootdir->my_LstAccDate.day_dec =  (subrootdir->DIR_LstAccDate) &0xF;
		
		  subrootdir->DIR_FSTClusHI = subrootImageData[20+h] + subrootImageData[21+h]*256;
		  subrootdir->DIR_WrtTime = subrootImageData[22+h] + subrootImageData[23+h]*256;
		  string bin4;
          char holder4=' ';
          while(subrootdir->DIR_WrtTime !=0)
          {
            holder4=subrootdir->DIR_WrtTime %2+'0';
            bin4 = holder4+bin4;
            subrootdir->DIR_WrtTime/=2;
          }
 
		subrootdir->my_WrtTime.hour_dec=0;
		subrootdir->my_WrtTime.minute_dec=0;
		subrootdir->my_WrtTime.second_dec=0;
		for(int i = 0; i<5;i++)
			subrootdir->my_WrtTime.hour += bin[i];
		for(int j = 5; j<10;j++)
		    subrootdir->my_WrtTime.minute += bin[j];
		for(int a=10; a<16;a++)
			subrootdir->my_WrtTime.second += bin[a]; 
		for(unsigned int counter = 0; counter < subrootdir->my_WrtTime.hour.size(); counter++)
        {
        	if(subrootdir->my_WrtTime.hour.c_str()[counter] == '1')
              subrootdir->my_WrtTime.hour_dec += pow(2, counter);
		}
		for(unsigned int count = 0; count < subrootdir->my_WrtTime.minute.size(); count++)
        {
        	if(subrootdir->my_WrtTime.minute.c_str()[count] == '1')
              subrootdir->my_WrtTime.minute_dec += pow(2, count);
		}
		for(unsigned int ct = 0; ct < subrootdir->my_WrtTime.second.size(); ct++)
        {
        	if(subrootdir->my_WrtTime.second.c_str()[ct] == '1')
              subrootdir->my_WrtTime.second_dec += pow(2, ct);
		}
		  subrootdir->DIR_WrtDate = subrootImageData[24+h] + subrootImageData[25+h]*256;
	

		subrootdir->my_WrtDate.year_dec=0;
		subrootdir->my_WrtDate.month_dec=0;
		subrootdir->my_WrtDate.day_dec=0;
		subrootdir->my_WrtDate.year_dec = (subrootdir->DIR_WrtDate>>9) +1980;
		subrootdir->my_WrtDate.month_dec =  (subrootdir->DIR_WrtDate>>5)&0xF;
		subrootdir->my_WrtDate.day_dec =  (subrootdir->DIR_WrtDate) &0xF;
		
		  subrootdir->DIR_FstClusLow = subrootImageData[26+h] + subrootImageData[27+h]*256;
		  subrootdir->DIR_FileSize = (((uint32_t)subrootImageData[31+h])<<24) + (((uint32_t)subrootImageData[30+h])<<16) + (((uint32_t)subrootImageData[29+h])<<8) + subrootImageData[28+h]; 
		  subshortname.push_back(subrootdir);
		//ROOTVector.push_back(rootdir);
		}
		//else if(static_cast<int>(subrootdir->DIR_Attr) == VM_FILE_SYSTEM_ATTR_LONG_NAME)
		//{
                        SUBROOTLONG *sublongroot = new SUBROOTLONG();
                        sublongroot->LDIR_Ord = subrootImageData[0];
                        int cnt = 0;
                        //memcpy(longroot->LDIR_Name1, rootImageData+1,10);
                        for(int a=1; a<10;a++)
                        {
                          if(isprint(subrootImageData[a+h]))
                          {
                            sublongroot->LDIR_Name1[cnt]=subrootImageData[a+h];
                            ////cout<<longroot->LDIR_Name1[cnt]<<"!!!"<<endl;
                            cnt++;
                          }
		          ////cout<<longroot->LDIR_Name1[cnt]<<"!!!!"<<endl;
                        }
                        sublongroot->hit = 0;
						sublongroot->ID = longname.size();
                        sublongroot->LDIR_Attr = subrootImageData[11+h];
                        sublongroot->LDIR_Type = subrootImageData[12+h];
                        sublongroot->LDIR_Chksum = subrootImageData[13+h]; 
                        int ct=0;
                        for(int b=14;b<26;b++)
                        {
                          if(isprint(subrootImageData[b+h]))
                          {
                            sublongroot->LDIR_Name2[ct]=subrootImageData[b+h];
                            ct++;
                          }
                        }
                        sublongroot->LDIR_FstClusLO = subrootImageData[26+h] + subrootImageData[27+h]*256;
                        int c=0;
                        for(int z=28;z<32;z++)
                        {
                          if(isprint(subrootImageData[z+h]))
                          {
                            sublongroot->LDIR_Name3[c]=subrootImageData[z+h];
                            c++;
                          }
                        }
			sublongname.push_back(sublongroot);
		//}
		h = h + 32;
	}
    VMMemoryPoolDeallocate(VM_MEMORY_POOL_ID_SHARE, myshare);
    return 512 == num_threads[current_ID]->FDresult;
  }

    
    
    
    
  bool rootread(uint8_t *rootImageData, int FD, uint16_t sector)
  {
	TMachineSignalState rr;
	MachineSuspendSignals(&rr);
    MachineFileSeek(FD, sector*512, 0, MyMachineFileCallback, (void*)current_ID);
    num_threads[current_ID]->MyState = VM_THREAD_STATE_WAITING;
	MachineResumeSignals(&rr);
    schedule();
	
	MachineSuspendSignals(&rr);
    VMMemoryPoolAllocate(num_pools[1]->MyPoolID, 512, &myshare);
    MachineFileRead(FD, myshare, 512, MyMachineFileCallback, (void*)current_ID);
    num_threads[current_ID]->MyState = VM_THREAD_STATE_WAITING;
	MachineResumeSignals(&rr);
    schedule();
	
    rootImageData = (uint8_t*)myshare;
    memcpy(rootImageData, myshare, 512);

	int h = 0;
	for(int j = 0; j < 512; j = j+32)
	{
		
		ROOT *rootdir = new ROOT(); 
		for(unsigned int u = 0; u < 13;u++)
           rootdir->DIR_Name[u]='\0';
		 for(int i = 0; i<11;i++)
		{
			rootdir->DIR_Name[i]= rootImageData[i+h];
			//}
		}
		
        for(unsigned int x = 0; x< 11;x++)
        {
            if(rootdir->DIR_Name[x]==' ' && isalpha(rootdir->DIR_Name[x+1]))
                rootdir->DIR_Name[x] = '.';
            
            if(rootdir->DIR_Name[x]==' ' && rootdir->DIR_Name[x+1] ==' ')
                rootdir->DIR_Name[x] = '?';
            
            if(rootdir->DIR_Name[x]=='1' && isalpha(rootdir->DIR_Name[x+1]))
            {
                rootdir->DIR_Name[x+2] =  rootdir->DIR_Name[x+1];
                rootdir->DIR_Name[x+1] = '.';
                
                
            }
            
            
        }
        for(unsigned int y = 0;y <12;y++)
        {
            if(rootdir->DIR_Name[y]=='?' )
            {
                for(unsigned int x = y ; x < 12;x++)
                {
                    if(rootdir->DIR_Name[x] == '.')
                    {
                        rootdir->DIR_Name[y] = rootdir->DIR_Name[x];
                        rootdir->DIR_Name[y+1]=rootdir->DIR_Name[x+1];
                        rootdir->DIR_Name[y+2] ='\0';
                        x = 12;
                        y =12;
                    }
                    else
                    {
                        strtok((char*)rootdir->DIR_Name, "?");
                    }
                }
            }
        }
		
		
		
		
		
		
		
		
		
        rootdir->ID = shortname.size();
        rootdir->hit = 0;
		rootdir->DIR_Attr = rootImageData[11+h]; 
		if(static_cast<int>(rootdir->DIR_Attr) != VM_FILE_SYSTEM_ATTR_LONG_NAME)
		{ 
	      rootdir->DIR_NTRes = rootImageData[12+h];
		  rootdir->DIR_CrtTimeTenth = rootImageData[13+h];
		  rootdir->DIR_CrtTime= rootImageData[14+h] + rootImageData[15+h]*256;
          string bin;
          char holder=' ';
          while(rootdir->DIR_CrtTime !=0)
          {
            holder=rootdir->DIR_CrtTime %2+'0';
            bin = holder+bin;
            rootdir->DIR_CrtTime/=2;
          }
 
		rootdir->my_CrtTime.hour_dec=0;
		rootdir->my_CrtTime.minute_dec=0;
		rootdir->my_CrtTime.second_dec=0;
		for(int i = 0; i<5;i++)
			rootdir->my_CrtTime.hour += bin[i];
		for(int j = 5; j<10;j++)
		    rootdir->my_CrtTime.minute += bin[j];
		for(int a=10; a<16;a++)
			rootdir->my_CrtTime.second += bin[a]; 
		for(unsigned int counter = 0; counter < rootdir->my_CrtTime.hour.size(); counter++)
        {
        	if(rootdir->my_CrtTime.hour.c_str()[counter] == '1')
              rootdir->my_CrtTime.hour_dec += pow(2, counter);
		}
		for(unsigned int count = 0; count < rootdir->my_CrtTime.minute.size(); count++)
        {
        	if(rootdir->my_CrtTime.minute.c_str()[count] == '1')
              rootdir->my_CrtTime.minute_dec += pow(2, count);
		}
		for(unsigned int ct = 0; ct < rootdir->my_CrtTime.second.size(); ct++)
        {
        	if(rootdir->my_CrtTime.second.c_str()[ct] == '1')
              rootdir->my_CrtTime.second_dec += pow(2, ct);
		}
		
		  rootdir->DIR_CrtDate = rootImageData[16+h] + rootImageData[17+h]*256;
 
		rootdir->my_CrtDate.year_dec=0;
		rootdir->my_CrtDate.month_dec=0;
		rootdir->my_CrtDate.day_dec=0;
		rootdir->my_CrtDate.year_dec = (rootdir->DIR_CrtDate>>9) +1980;
		rootdir->my_CrtDate.month_dec =  (rootdir->DIR_CrtDate>>5)&0xF;
		rootdir->my_CrtDate.day_dec =  (rootdir->DIR_CrtDate) &0xF;
	
		  rootdir->DIR_LstAccDate = rootImageData[17+h] + rootImageData[18+h]*256;
	
 
		rootdir->my_LstAccDate.year_dec=0;
		rootdir->my_LstAccDate.month_dec=0;
		rootdir->my_LstAccDate.day_dec=0;
		rootdir->my_LstAccDate.year_dec = (rootdir->DIR_LstAccDate>>9) +1980;
		rootdir->my_LstAccDate.month_dec =  (rootdir->DIR_LstAccDate>>5)&0xF;
		rootdir->my_LstAccDate.day_dec =  (rootdir->DIR_LstAccDate) &0xF;
		
		  rootdir->DIR_FSTClusHI = rootImageData[20+h] + rootImageData[21+h]*256;
		  rootdir->DIR_WrtTime = rootImageData[22+h] + rootImageData[23+h]*256;
		  string bin4;
          char holder4=' ';
          while(rootdir->DIR_WrtTime !=0)
          {
            holder4=rootdir->DIR_WrtTime %2+'0';
            bin4 = holder4+bin4;
            rootdir->DIR_WrtTime/=2;
          }
 
		rootdir->my_WrtTime.hour_dec=0;
		rootdir->my_WrtTime.minute_dec=0;
		rootdir->my_WrtTime.second_dec=0;
		for(int i = 0; i<5;i++)
			rootdir->my_WrtTime.hour += bin[i];
		for(int j = 5; j<10;j++)
		    rootdir->my_WrtTime.minute += bin[j];
		for(int a=10; a<16;a++)
			rootdir->my_WrtTime.second += bin[a]; 
		for(unsigned int counter = 0; counter < rootdir->my_WrtTime.hour.size(); counter++)
        {
        	if(rootdir->my_WrtTime.hour.c_str()[counter] == '1')
              rootdir->my_WrtTime.hour_dec += pow(2, counter);
		}
		for(unsigned int count = 0; count < rootdir->my_WrtTime.minute.size(); count++)
        {
        	if(rootdir->my_WrtTime.minute.c_str()[count] == '1')
              rootdir->my_WrtTime.minute_dec += pow(2, count);
		}
		for(unsigned int ct = 0; ct < rootdir->my_WrtTime.second.size(); ct++)
        {
        	if(rootdir->my_WrtTime.second.c_str()[ct] == '1')
              rootdir->my_WrtTime.second_dec += pow(2, ct);
		}
		  rootdir->DIR_WrtDate = rootImageData[24+h] + rootImageData[25+h]*256;
	

		rootdir->my_WrtDate.year_dec=0;
		rootdir->my_WrtDate.month_dec=0;
		rootdir->my_WrtDate.day_dec=0;
		rootdir->my_WrtDate.year_dec = (rootdir->DIR_WrtDate>>9) +1980;
		rootdir->my_WrtDate.month_dec =  (rootdir->DIR_WrtDate>>5)&0xF;
		rootdir->my_WrtDate.day_dec =  (rootdir->DIR_WrtDate) &0xF;
		
		  rootdir->DIR_FstClusLow = rootImageData[26+h] + rootImageData[27+h]*256;
		  rootdir->DIR_FileSize = (((uint32_t)rootImageData[31+h])<<24) + (((uint32_t)rootImageData[30+h])<<16) + (((uint32_t)rootImageData[29+h])<<8) + rootImageData[28+h]; 
		  shortname.push_back(rootdir);
		//ROOTVector.push_back(rootdir);
		}
		else if(static_cast<int>(rootdir->DIR_Attr) == VM_FILE_SYSTEM_ATTR_LONG_NAME)
		{
                        ROOTLONG *longroot = new ROOTLONG();
                        longroot->LDIR_Ord = rootImageData[0];
                        int cnt = 0;
                        //memcpy(longroot->LDIR_Name1, rootImageData+1,10);
                        for(int a=1; a<10;a++)
                        {
                          if(isprint(rootImageData[a+h]))
                          {
                            longroot->LDIR_Name1[cnt]=rootImageData[a+h];
                            ////cout<<longroot->LDIR_Name1[cnt]<<"!!!"<<endl;
                            cnt++;
                          }
		          ////cout<<longroot->LDIR_Name1[cnt]<<"!!!!"<<endl;
                        }
                        longroot->hit = 0;
						longroot->ID = longname.size();
                        longroot->LDIR_Attr = rootImageData[11+h];
                        longroot->LDIR_Type = rootImageData[12+h];
                        longroot->LDIR_Chksum = rootImageData[13+h]; 
                        int ct=0;
                        for(int b=14;b<26;b++)
                        {
                          if(isprint(rootImageData[b+h]))
                          {
                            longroot->LDIR_Name2[ct]=rootImageData[b+h];
                            ct++;
                          }
                        }
                        longroot->LDIR_FstClusLO = rootImageData[26+h] + rootImageData[27+h]*256;
                        int c=0;
                        for(int z=28;z<32;z++)
                        {
                          if(isprint(rootImageData[z+h]))
                          {
                            longroot->LDIR_Name3[c]=rootImageData[z+h];
                            c++;
                          }
                        }
			longname.push_back(longroot);
		}
		h = h + 32;
	}
    VMMemoryPoolDeallocate(VM_MEMORY_POOL_ID_SHARE, myshare);
    return 512 == num_threads[current_ID]->FDresult;
  }
}

extern "C"
{
  TVMStatus VMStart(int tickms, TVMMemorySize heapsize, int machinetickms, TVMMemorySize sharedsize, const char *mount,  int argc, char *argv[])
  {
    MyTCB *main= new MyTCB();
    MyTCB *idle= new MyTCB();
    MyMemPool *mainpool = new MyMemPool();
    TMachineSignalState idling;
    MachineSuspendSignals(&idling);
    num_pools.push_back(mainpool);
    idle -> MyThreadID = VM_THREAD_ID_IDLE;
    idle -> MyPriority = VM_THREAD_PRIORITY_IDLE;
    idle -> MyState = VM_THREAD_STATE_IDLE;
    idle -> leftover = 0;
    idle -> StackSize = 0x1000000;
    idle -> StackBase = new uint8_t[idle->StackSize];
    num_threads.push_back(idle);
    MachineContextCreate(&num_threads[0]->MyContext,idlefunc,NULL,num_threads[0]->StackBase,num_threads[0]->StackSize); 
    MachineResumeSignals(&idling);
    main -> MyThreadID = VM_THREAD_ID_MAIN;
    main -> MyPriority =VM_THREAD_PRIORITY_NORMAL;
    main -> MyState = VM_THREAD_STATE_RUNNING;
    main -> leftover = 0;
    main -> FDresult =0;
    num_threads.push_back(main);
    current_ID = num_threads[1]->MyThreadID;
    sharedsize += (sharedsize%4096);
    mainpool -> MyPoolID = VM_MEMORY_POOL_ID_SYSTEM;
    mainpool -> freespace = heapsize;
    mainpool -> MyPoolSize = heapsize;
    mainpool -> my_base = new uint8_t[heapsize];
    MyMemPool::info myinfo;
    myinfo.location = mainpool->my_base;
    myinfo.length = heapsize;
    mainpool->freelist.push_back(myinfo);
    void *shared = MachineInitialize(machinetickms, sharedsize);
    MyMemPool *sharemem = new MyMemPool();
    sharemem -> MyPoolID = VM_MEMORY_POOL_ID_SHARE;
    sharemem -> freespace = sharedsize;
    sharemem -> MyPoolSize = sharedsize;
    sharemem -> length = sharedsize;
    sharemem -> my_base = (uint8_t*)shared;
    MyMemPool::info shareinfo;
    shareinfo.location = sharemem->my_base;
    shareinfo.length = (TVMMemorySize)shared;
    sharemem->freelist.push_back(shareinfo);
    num_pools.push_back(sharemem); 
    MachineRequestAlarm(1000*tickms,AlarmCallback,NULL);
    MachineEnableSignals();
    TVMMainEntry MyEntry;
    MyEntry = VMLoadModule(argv[0]);
    MachineFileOpen(mount, 0644, O_RDWR, MyMachineFileCallback, (void*)current_ID);
    num_threads[current_ID]->MyState = VM_THREAD_STATE_WAITING;
    schedule();
    myfd = num_threads[current_ID]->FDresult;
    uint8_t* buffer;
    VMMemoryPoolAllocate(num_pools[1]->MyPoolID, 512, &myshare);
    MachineFileRead(myfd, myshare, 512, MyMachineFileCallback, (void*)current_ID);
    num_threads[current_ID]->MyState = VM_THREAD_STATE_WAITING;
    schedule();
	
	////cout <<endl<<buffer<<endl;
	// put image to BPB
    buffer = (uint8_t*)myshare;
    memcpy(buffer, myshare, 512);
    BPB *bpb = new BPB();
    memcpy(bpb->OEMName, buffer+3, 8);
    bpb->BytesPerSec = (buffer[11])+ (((uint16_t)buffer[12])<<8);
    bpb->SecPerClus = buffer[13];
    bpb->RsvdSecCnt = buffer[14] + buffer[15]*256;
    bpb->NumFATs = buffer[16];
    bpb->RootEntCnt = buffer[18]*256 + buffer[17];
    bpb->TotSec16 = buffer[20]*256 + buffer[19];
    bpb->Media = buffer[21];
    bpb->FATSz16 = buffer[23]*256 + buffer[22];
    bpb->SecPerTrk = buffer[25]*256 + buffer[24];
    bpb->NumHeads = buffer[27]*256 + buffer[26];
    bpb->HiddSec = (((uint32_t)buffer[31])<<24) + (((uint32_t)buffer[30])<<16) + (((uint32_t)buffer[29])<<8) + buffer[28];
    bpb->TotSec32 =(((uint32_t)buffer[35])<<24) + (((uint32_t)buffer[34])<<16) + (((uint32_t) buffer[33])<<8) +  buffer[32];
    memcpy(buffer, myshare, 512);
    bpb->BS_DrvNum = buffer[36];
    bpb->BS_Reserved1 = buffer[37];
    bpb->BS_BootSig = buffer[38];
    memcpy(bpb->BS_VolID, buffer+39,4);
    memcpy(bpb->BS_VolLab, buffer+43,11);
    memcpy(bpb->BS_FilSysType, buffer+54,8);
	
	
	
    MyBPB.push_back(bpb);
    VMMemoryPoolDeallocate(VM_MEMORY_POOL_ID_SHARE, myshare);
	
	
 
    FirstRootSector = bpb->RsvdSecCnt + bpb->NumFATs * bpb->FATSz16;
    RootDirectorySectors = (bpb->RootEntCnt *32) / 512;
    FirstDataSector = FirstRootSector +  RootDirectorySectors;
    ClusterCount = (bpb->TotSec32 - FirstDataSector) / bpb->SecPerClus;
	
    myread(fileImageData, myfd, bpb->RsvdSecCnt);
	
    rootread(rootImageData, myfd, FirstRootSector);
	subrootread(subrootImageData, myfd, FirstDataSector);
	/*
    //cout<<"OEMNAME: "<<bpb->OEMName<<endl;
    //cout<<"Bytes Per Sector: "<<bpb->BytesPerSec<<endl;
    //cout<<"Sectors Per Cluster: "<<bpb->SecPerClus<<endl;
    //cout<<"Reserved Sectors: "<<bpb->RsvdSecCnt<<endl;
    //cout<<"FAT Count: "<<bpb->NumFATs<<endl;
    //cout<<"Root Entry: "<<bpb->RootEntCnt<<endl;
    //cout<<"Sector Count 16: "<<bpb->TotSec16<<endl;
    //cout<<"Media: "<<bpb->Media<<endl;
    //cout<<"FAT Size 16: "<<bpb->FATSz16<<endl;
    //cout<<"Sectors Per Track: "<<bpb->SecPerTrk<<endl;
    //cout<<"Head Count: "<<bpb->NumHeads<<endl;
    //cout<<"Hidden Sector Count: "<<bpb->HiddSec<<endl;
    //cout<<"Sector Count 32: "<<bpb->TotSec32<<endl;
    //cout<<"Drive Number: "<<static_cast<int>(bpb->BS_DrvNum)<<endl; 
    //cout<<"Boot Signature: "<<static_cast<int>(bpb->BS_BootSig)<<endl;
    //cout<<"Volume ID: "<<bpb->BS_VolID<<endl;
    //cout<<"Volume Label: "<<bpb->BS_VolLab<<endl;
    //cout<<"File System Type: "<<bpb->BS_FilSysType<<endl;
    //cout<<"First Root Sector: "<<FirstRootSector<<endl;
    //cout<<"Root Directory Sectors: "<<RootDirectorySectors<<endl;
    //cout<<"First Data Sector: "<<FirstDataSector<<endl;
    //cout<<"Cluster Count: "<<ClusterCount<<endl;
    //cout<<endl;
	*/
	//for(unsigned int i=0; i<subshortname.size();i++)
     // printf("Directory Name: %s\n",subshortname[i]->DIR_Name);
    
    //cout<<"Directory Name: Short File"<<endl;
	//cout<<"Directory Attribute: "<<static_cast<int>(subshortname[0]->DIR_Attr)<<endl;
    ////cout<<"Directory Entries: "<<static_cast<int>(shortname[0]->DIR_NTRes)<<endl;
    ////cout<<"Directory Time Tenth: "<<static_cast<int>(shortname[0]->DIR_CrtTimeTenth)<<endl;
	//printf("Create Time: %s\n",shortname[1]->DIR_CrtTime);
	//printf("Create Date: %s\n",shortname[1]->DIR_CrtDate);
	////cout<<"Create Time: "<<shortname[1]->my_CrtTime.hour_dec<<":"<<shortname[1]->my_CrtTime.minute_dec<<":"<<shortname[1]->my_CrtTime.second_dec<<endl;
	////cout<<"Create Date: "<<shortname[1]->my_CrtDate.year_dec<<"/"<<shortname[1]->my_CrtDate.month_dec<<"/"<<shortname[1]->my_CrtDate.day_dec<<endl;
	////cout<<"Last Access Date: "<<shortname[1]->my_LstAccDate.year_dec<<"/"<<shortname[1]->my_LstAccDate.month_dec<<"/"<<shortname[1]->my_LstAccDate.day_dec<<endl;
	////cout<<"Create Date: "<<static_cast<int>(shortname[1]->DIR_CrtDate)<<endl;
   // //cout<<"Last Access Date: "<<static_cast<int>(shortname[1]->DIR_LstAccDate)<<endl;
    ////cout<<"FSTClusHi: "<<shortname[0]->DIR_FSTClusHI<<endl;
	////cout<<"Write Time: "<<shortname[1]->my_WrtTime.hour_dec<<":"<<shortname[1]->my_WrtTime.minute_dec<<":"<<shortname[1]->my_WrtTime.second_dec<<endl;
	////cout<<"Write Date: "<<shortname[1]->my_WrtDate.year_dec<<"/"<<shortname[1]->my_WrtDate.month_dec<<"/"<<shortname[1]->my_WrtDate.day_dec<<endl;
    ////cout<<"WrtTime: "<<static_cast<int>(shortname[1]->DIR_WrtTime)<<endl;
    ////cout<<"WrtDate: "<<static_cast<int>(shortname[1]->DIR_WrtDate)<<endl;
    //printf("FstClusLow: %02X\n",shortname[0]->DIR_FstClusLow);
    ////cout<<"File Size: "<<shortname[0]->DIR_FileSize<<endl;
	/*
    //cout<<endl;
    //cout<<endl;
    //cout<<"LDIR_Ord: "<<longname[1]->LDIR_Ord<<endl;
    //cout<<"Long Directory Name: "<<longname[1]->LDIR_Name1<<endl;
    //cout<<"Directory Name: Long File"<<endl;
    //cout<<"Long Directory Attribute: "<<static_cast<int>(longname[1]->LDIR_Attr)<<endl;
    //cout<<"Directory Type: "<<static_cast<int>(longname[1]->LDIR_Type)<<endl;
    //cout<<"Directory Check Sum: "<<static_cast<int>(longname[1]->LDIR_Chksum)<<endl;
    //cout<<"Long Directory Name 2: "<<longname[1]->LDIR_Name2<<endl;
    printf("FstClusLO: %02X\n",longname[1]->LDIR_FstClusLO);
    //cout<<"Long Directory Name 3: "<<longname[1]->LDIR_Name3<<endl;
    //cout<<endl;
	*/
    
    ////cout<<endl;
    ////cout<<"SHORT SIZE: "<<shortname.size()<<endl;
    ////cout<<"LONG SIZE: "<<longname.size()<<endl;
      
      
      
      for(unsigned int i = 0; i < longname.size(); i++)
      {
          longlong* mylong = new longlong();
          VMMemoryPoolAllocate(VM_MEMORY_POOL_ID_SYSTEM,512, (void**)&mylong->newlongname);
          strcat((char*)mylong->newlongname,(char*)longname[i]->LDIR_Name1);
          strcat((char*)mylong->newlongname,(char*)longname[i]->LDIR_Name2);
          strcat((char*)mylong->newlongname,(char*)longname[i]->LDIR_Name3);
          combinename.push_back(mylong);
          VMMemoryPoolDeallocate(VM_MEMORY_POOL_ID_SHARE, (void*)mylong->newlongname);
      }
      
	  for(unsigned int i = 0; i < sublongname.size(); i++)
      {
          longlong* submylong = new longlong();
          VMMemoryPoolAllocate(VM_MEMORY_POOL_ID_SYSTEM,512, (void**)&submylong->newlongname);
          strcat((char*)submylong->newlongname,(char*)sublongname[i]->LDIR_Name1);
          strcat((char*)submylong->newlongname,(char*)sublongname[i]->LDIR_Name2);
          strcat((char*)submylong->newlongname,(char*)sublongname[i]->LDIR_Name3);
          subcombine.push_back(submylong);
          VMMemoryPoolDeallocate(VM_MEMORY_POOL_ID_SHARE, (void*)submylong->newlongname);
      }
	
		for(int i =3;i<10;i++)
			subcombine.erase(subcombine.begin()+i);
		subcombine[0]->newlongname[0] = '\0';
		//subcombine.erase(subcombine.begin()+4);
		 /*for(unsigned int u= 0; u < subcombine.size(); u++)
		 {
			 cout<<subcombine[u]->newlongname<<endl;
		 }*/
		 //cout<<subcombine.size()<<endl;
      char *add;


      for(unsigned int j = 0;j<shortname.size();j++)
      {
          for(unsigned int i =0;i<strlen((char*)shortname[j]->DIR_Name);i++)
          {
              add = (char*)shortname[j]->DIR_Name;
              add[i] = (char)tolower((char)shortname[j]->DIR_Name[i]);
          
          }
          sfn.push_back(add);
      }
      
      
      for(unsigned int j = 0;j<combinename.size();j++)
      {
          /*for(unsigned int i =0;i<strlen((char*)combinename[j]->newlongname);i++)
          {
              add = (char*)combinename[j]->newlongname;
              add[i] = (char)tolower((char)combinename[j]->newlongname[i]);
              
          }*/
          lfn.push_back(add);
      }
	  
	  strcat(combinename[5]->newlongname,combinename[4]->newlongname);
      strcat(combinename[7]->newlongname,combinename[6]->newlongname);
      combinename.erase(combinename.begin()+4);
      combinename.erase(combinename.begin()+5);
	  
	  
	   //clusteread(clusterImageData, myfd, 21);
	   ////cout<<num_cluster[21]->data<<endl;
	  //for(unsigned int i=0;i<combinename.size();i++)
       // //cout<<combinename[i]->newlongname<<endl;
      /*for(unsigned int i =0; i<shortname.size();i++)
	  {
		printf("FSTCLUSLOW: %02X\n",shortname[i]->DIR_FstClusLow);
	    clusterread(clusterImageData,myfd, shortname[i]->DIR_FstClusLow);
        //cout<<(char*)num_cluster[shortname[i]->DIR_FstClusLow]->data<<endl;	  
	  }*/
	  //clusterread(clusterImageData, myfd , shortname[0]->DIR_FstClusLow);
	  //for(int i =0; i<512;i++)
	   // //cout<<(char)num_cluster[shortname[0]->DIR_FstClusLow]->data[i];
	  ////cout<<endl;
     
      
      /*
      
      for(unsigned int i = 0; i< shortname.size();i++)
          for (unsigned int j = 0; j < combinename.size(); j++)
          {
              for(unsigned int a = 1; a<strlen((char* )shortname[i]->DIR_Name);a++)
              {
                 strcat(add, (char*)tolower((char)shortname[i]->DIR_Name[a]));
                  
               }
              
              //cout<<add<<" \n";
                if(add == (char*)combinename[j]->newlongname )
                    //cout<<"match"<<endl;
                else
                  //cout<<"not match"<<endl;
              
     
              add = NULL;
              ////cout<<shortname[i]->DIR_Name << " " <<combinename[j]<<endl;
              
          }
      
      
      
      
      
    //cluster = bpb->NumFATs * bpb->FATSz16 * bpb->BytesPerSec + bpb->RsvdSecCnt;
    //cluster_size = bpb->SecPerClus * bpb->BytesPerSec;
    */
    /*for(int i =44; i<48;i++)
    {
      //cout<<"Cluster No."<<i<<endl;
      ////cout<<endl;
      clusterread(clusterImageData, myfd, i);
	  //cout<<(char*)num_cluster[i]->data<<endl;
    }*/	
     
    MyEntry(argc,argv);
    if(MyEntry != NULL)
    { 
	  MachineFileWrite(myfd,(void*)mount,sizeof(mount),MyMachineFileCallback, NULL);
      MachineTerminate();
      return VM_STATUS_SUCCESS;
    }
    else
    {
      return VM_STATUS_FAILURE;
    }
    //////////////////cout<<"THE END"<<endl;
  }
}


extern "C"
{
  TVMStatus VMThreadCreate(TVMThreadEntry entry, void *param, TVMMemorySize memsize, TVMThreadPriority prio, TVMThreadIDRef tid)   
  {
    TMachineSignalState mystate;
    MachineSuspendSignals(&mystate);
    if(entry == NULL || tid == NULL)
      return VM_STATUS_ERROR_INVALID_PARAMETER;
    else 
    {
      ////////////////cout<<"creating thread"<<endl;
      MyTCB *TCB = new MyTCB();
      TCB -> MyThreadID = num_threads.size();
      *tid = TCB -> MyThreadID;
      TCB -> MyThreadEntry = entry;
      TCB -> MyPara = param;
      TCB -> StackSize = memsize;
      //void *alloc;
      VMMemoryPoolAllocate(VM_MEMORY_POOL_ID_SYSTEM,memsize, &alloc);
      TCB -> StackBase = (uint8_t*)alloc;
      TCB -> MyPriority = prio;
      TCB -> MyState = VM_THREAD_STATE_DEAD;
      TCB -> leftover = 0;
      TCB -> FDresult =0;
      num_threads.push_back(TCB);
      ////////////////cout<<"finish creating thread"<<endl;
      MachineResumeSignals(&mystate);
      return VM_STATUS_SUCCESS;
    }
  }
}

extern "C"
{
  TVMStatus VMThreadState(TVMThreadID thread, TVMThreadStateRef stateref)
  {
    ////////////////cout<<"inside thread state"<<endl;
    TMachineSignalState exit;
    MachineSuspendSignals(&exit);
    if(num_threads[thread]->MyThreadID >= num_threads.size())
      return VM_STATUS_ERROR_INVALID_ID;
    else if(num_threads[thread]->MyState )
      return VM_STATUS_ERROR_INVALID_PARAMETER;
    else
    {
      *stateref = num_threads[thread]->MyState;
      ////////////////cout<<"finish thread state"<<endl;
      MachineResumeSignals(&exit);
      return VM_STATUS_SUCCESS;
    }
  }
}

extern "C"
{
  TVMStatus VMThreadActivate(TVMThreadID thread)
  {
    ////cout<<"inside thread activation"<<endl;
    TMachineSignalState curr_state;
    MachineSuspendSignals(&curr_state);
    if(num_threads[thread]->MyThreadID >= num_threads.size())
      return VM_STATUS_ERROR_INVALID_ID;
    else if(num_threads[thread]->MyState != VM_THREAD_STATE_DEAD)
      return VM_STATUS_ERROR_INVALID_STATE;
    else
    {
      MachineEnableSignals();
      MachineContextCreate(&num_threads[thread]->MyContext,MyTCB::MyEntry,num_threads[thread]->MyPara,num_threads[thread]->StackBase,num_threads[thread]->StackSize);
      ////////////////cout<<"finish create context"<<endl; 
      num_threads[thread]->MyState = VM_THREAD_STATE_READY;
      //current_main = num_threads[thread]->MyThreadID;
      //current_ID = num_threads[thread]->MyThreadID;
      ////cout<<thread<<endl;
      if(num_threads[thread]->MyState == VM_THREAD_STATE_READY)
      {
        if(num_threads[thread]->MyPriority == VM_THREAD_PRIORITY_HIGH)
        {
            readyhigh.push_back(num_threads[thread]->MyThreadID);
	}
        else if(num_threads[thread]->MyPriority == VM_THREAD_PRIORITY_NORMAL)
	{
	    readynormal.push_back(num_threads[thread]->MyThreadID);
	}
        else if(num_threads[thread]->MyPriority == VM_THREAD_PRIORITY_LOW)
	{
	    readylow.push_back(num_threads[thread]->MyThreadID);  
        }
      }
    }   
      schedule(); 
      ////cout<<"finish activation"<<endl; 
      MachineResumeSignals(&curr_state);
      return VM_STATUS_SUCCESS; 
   // }
  }
}

extern "C"
{
  void MyTCB::MyEntry(void *param)
  {
    MachineEnableSignals();
    ////////////////cout<<"inside SKELETON function"<<endl;
    ////////////////cout<<current_ID<<endl;
    num_threads[current_ID]->MyThreadEntry(num_threads[current_ID]->MyPara);
    ////////////////cout<<"Terminating from Skeleton"<<endl;
    VMThreadTerminate(num_threads[current_ID]->MyThreadID);
    ////////////////cout<<"end of SKELETON function"<<endl;
  }
}

extern "C"
{
  TVMStatus VMThreadSleep(TVMTick tick)
  {
    TMachineSignalState sleepstate;
    MachineSuspendSignals(&sleepstate);
    ////////////////cout<<"current ID in threadsleep "<<current_ID<<endl;
    if(tick == VM_TIMEOUT_INFINITE) 
      return VM_STATUS_ERROR_INVALID_PARAMETER;
    else
    {
      ////////////////cout<<"Current tick "<<tick<<endl;
      num_threads[current_ID]->leftover = tick;
      num_threads[current_ID]->MyState = VM_THREAD_STATE_WAITING;
      sleep_list.push_back(num_threads[current_ID]);   
      schedule();
      MachineResumeSignals(&sleepstate);
      return VM_STATUS_SUCCESS;
    }
  }
}


extern "C"
{
  TVMStatus VMThreadID(TVMThreadIDRef threadref)
  {
    ////////////////cout<<"inside thread ID"<<endl;
    TMachineSignalState id;
    MachineSuspendSignals(&id);
    if(threadref == NULL)
      return VM_STATUS_ERROR_INVALID_PARAMETER;
    else
    {
      current_ID = *threadref;
      ////////////////cout<<"finish thread ID"<<endl;
      MachineResumeSignals(&id);
      return VM_STATUS_SUCCESS;
    }
  }
}

extern "C"
{
  TVMStatus VMThreadTerminate(TVMThreadID thread)
  {
    ////cout<<"inside termination"<<endl;
    TMachineSignalState terminate;
    MachineSuspendSignals(&terminate);
    if(num_threads[thread]->MyThreadID >=num_threads.size())
      return VM_STATUS_ERROR_INVALID_ID;
    else if(num_threads[thread]->MyState == VM_THREAD_STATE_DEAD)
      return VM_STATUS_ERROR_INVALID_PARAMETER;
    else
    {
      num_threads[thread]->MyState = VM_THREAD_STATE_DEAD;
      //current_ID=num_threads[thread]->MyThreadID;
      ////cout<<current_ID<<endl; 
      num_threads[thread]->MutexHold.clear();
      if(num_threads[thread]->MyPriority == VM_THREAD_PRIORITY_HIGH)
        {
            readyhigh.remove(num_threads[thread]->MyThreadID);
        }
        else if(num_threads[thread]->MyPriority == VM_THREAD_PRIORITY_NORMAL)
        {
            readynormal.remove(num_threads[thread]->MyThreadID);
        }
        else if(num_threads[thread]->MyPriority == VM_THREAD_PRIORITY_LOW)
        {
            readylow.remove(num_threads[thread]->MyThreadID);
        }
      schedule();
      ////cout<<"finish termination"<<endl;
      MachineResumeSignals(&terminate);
      return VM_STATUS_SUCCESS;
    }
  }
}

extern "C"
{
  TVMStatus VMThreadDelete(TVMThreadID thread)
  {
    ////////////////cout<<"inside thread delete"<<endl;
    TMachineSignalState del;
    MachineSuspendSignals(&del);
    if(!num_threads[thread]->MyThreadID)
      return VM_STATUS_ERROR_INVALID_ID;
    else if(num_threads[thread]->MyState != VM_THREAD_STATE_DEAD)
      return VM_STATUS_ERROR_INVALID_STATE;
    else
    {
      if(num_threads[thread]->MyState == VM_THREAD_STATE_DEAD)
      {
        VMMemoryPoolDeallocate(num_pools[0]->MyPoolID, alloc);
        num_threads[thread] = '\0';
      }
      ////////////////cout<<"finish thread delete"<<endl;
      MachineResumeSignals(&del);
      return VM_STATUS_SUCCESS; 
    }
  }
}

extern "C"
{
  void schedule()
  {
    MachineEnableSignals();
    TVMThreadID current_schedule = current_ID; 
    ////cout<<"current ID in schedule "<< current_schedule<<endl;
    ////////cout<<"inside schedule"<<endl;
    for(unsigned int i=0; i<num_threads.size();++i)
    {
      ////////////////cout<<"ID now "<<current_schedule<<endl;
      if(num_threads[current_schedule]->MyState == VM_THREAD_STATE_RUNNING)
      {
        ////cout<<"Schedule: Running case"<<endl;
        if(!readyhigh.empty())
        {
          if(num_threads[i]->MyPriority > num_threads[current_schedule]->MyPriority)
          {
            int popthread = readyhigh.back();
            readyhigh.pop_front();
            num_threads[popthread]-> MyState = VM_THREAD_STATE_RUNNING;
            num_threads[current_schedule]->MyState = VM_THREAD_STATE_READY;
            if(num_threads[current_schedule]->MyPriority == VM_THREAD_PRIORITY_NORMAL)
              readynormal.push_back(num_threads[current_schedule]->MyThreadID);
            else if(num_threads[current_schedule]->MyPriority == VM_THREAD_PRIORITY_LOW)
              readylow.push_back(num_threads[current_schedule]->MyThreadID);
            current_ID= num_threads[popthread]->MyThreadID;
            current_main = num_threads[current_schedule]->MyThreadID;
            MachineContextSwitch(&num_threads[current_schedule]->MyContext,&num_threads[popthread]->MyContext);

          }
        }
        else if(!readynormal.empty())
        {
          if(num_threads[i]->MyPriority > num_threads[current_ID]->MyPriority)
          {
            int popthreadnormal = readynormal.front();
            readynormal.pop_front();
            num_threads[popthreadnormal]-> MyState = VM_THREAD_STATE_RUNNING;
            num_threads[current_schedule]->MyState = VM_THREAD_STATE_READY;
            if(num_threads[current_schedule]->MyPriority == VM_THREAD_PRIORITY_LOW)
              readylow.push_back(num_threads[current_schedule]->MyThreadID);
            current_ID= num_threads[popthreadnormal]->MyThreadID;
            current_main = num_threads[current_schedule]->MyThreadID;
            MachineContextSwitch(&num_threads[current_schedule]->MyContext, &num_threads[popthreadnormal]->MyContext);
          }
        }
        else if(!readylow.empty())
        {
          if(num_threads[i]->MyPriority > num_threads[current_schedule]->MyPriority)
          {
            int popthreadlow = readylow.front();
            readylow.pop_front();
            num_threads[popthreadlow] -> MyState = VM_THREAD_STATE_RUNNING;
            current_ID= num_threads[popthreadlow]->MyThreadID;
            current_main = num_threads[current_schedule]->MyThreadID;
            MachineContextSwitch(&num_threads[current_schedule]->MyContext, &num_threads[popthreadlow]->MyContext);
          }
        } 
      }
       
      else if(num_threads[current_schedule]->MyState == VM_THREAD_STATE_WAITING)
      {
        ////cout<<"Schedule: Waiting case\n";
        if(!readyhigh.empty())
        {
          int popthread2 = readyhigh.front();
          readyhigh.pop_front();
          num_threads[popthread2]-> MyState = VM_THREAD_STATE_RUNNING;
          current_ID= num_threads[popthread2]->MyThreadID;
          current_main = num_threads[current_schedule]->MyThreadID;
          MachineContextSwitch(&num_threads[current_schedule]->MyContext,&num_threads[popthread2]->MyContext);
        }
        
        else if(!readynormal.empty())
        {
          int popthreadnormal2 = readynormal.front();
          readynormal.pop_front();
          num_threads[popthreadnormal2]-> MyState = VM_THREAD_STATE_RUNNING;
          current_ID= num_threads[popthreadnormal2]->MyThreadID;
          current_main = num_threads[current_schedule]->MyThreadID;
          MachineContextSwitch(&num_threads[current_schedule]->MyContext, &num_threads[popthreadnormal2]->MyContext);
        }
      
        else if(!readylow.empty())
        {
          int popthreadlow2 = readylow.front();
          readylow.pop_front();
          num_threads[popthreadlow2] -> MyState = VM_THREAD_STATE_RUNNING;
          current_ID= num_threads[popthreadlow2]->MyThreadID;
          current_main=num_threads[current_schedule]->MyThreadID;
          MachineContextSwitch(&num_threads[current_schedule]->MyContext, &num_threads[popthreadlow2]->MyContext);
        }
        
        else if(readyhigh.empty() && readynormal.empty() && readylow.empty())
	{
           ////////////////cout<<"nothing in queues"<<endl;
           current_ID=num_threads[0]->MyThreadID;
           current_main= num_threads[current_schedule]->MyThreadID;
           //current_main2=num_threads[current_schedule]->MyThreadID;
           ////////////////cout<<"switching context"<<endl;
           MachineContextSwitch(&num_threads[current_schedule]->MyContext,&num_threads[0]->MyContext);
        }
      }
       
      else if(num_threads[current_schedule]->MyState == VM_THREAD_STATE_READY)
      {
        ////cout<<"Schedule: READY case\n";
        if(!readyhigh.empty())
        {
          if(num_threads[current_schedule]->MyPriority < num_threads[i]->MyPriority)
          {
            int popthread3 = readyhigh.front();
            readyhigh.pop_front();
            num_threads[popthread3]-> MyState = VM_THREAD_STATE_RUNNING; 
            readyhigh.push_back(num_threads[current_schedule]->MyThreadID);
            current_ID= num_threads[popthread3]->MyThreadID;
            current_main = num_threads[current_schedule]->MyThreadID;
            MachineContextSwitch(&num_threads[current_schedule]->MyContext,&num_threads[popthread3]->MyContext);

          }
        }
         
        else if(!readynormal.empty())
        {
          if(num_threads[current_ID]->MyPriority < num_threads[i]->MyPriority)
          {
            int popthreadnormal3 = readyhigh.front();
            readyhigh.pop_front();
            num_threads[popthreadnormal3]-> MyState = VM_THREAD_STATE_RUNNING; 
            readynormal.push_back(num_threads[current_schedule]->MyThreadID);
            current_ID= num_threads[popthreadnormal3]->MyThreadID;
            current_main = num_threads[current_schedule]->MyThreadID;
            MachineContextSwitch(&num_threads[current_schedule]->MyContext,&num_threads[popthreadnormal3]->MyContext);
          }
        }
        
        else if(!readylow.empty())
        {
          if(num_threads[current_schedule]->MyPriority < num_threads[i]->MyPriority)
          {
            int popthreadlow3 = readyhigh.front();
            readyhigh.pop_front();
            num_threads[popthreadlow3]-> MyState = VM_THREAD_STATE_RUNNING; 
            readylow.push_back(num_threads[current_schedule]->MyThreadID);
            current_ID= num_threads[popthreadlow3]->MyThreadID;
            current_main = num_threads[current_schedule]->MyThreadID;
            MachineContextSwitch(&num_threads[current_schedule]->MyContext,&num_threads[popthreadlow3]->MyContext);
          }
        }
        else if(readyhigh.empty() && readynormal.empty() && readylow.empty())
        {
          num_threads[current_schedule]->MyState = VM_THREAD_STATE_RUNNING;
          current_ID=num_threads[current_schedule]->MyThreadID;
          current_main = num_threads[current_schedule]->MyThreadID;
        } 
      } 
      else if(num_threads[current_schedule]->MyState == VM_THREAD_STATE_DEAD)
      {
        ////cout<<"Schedule: Dead Case\n";
        if(!readyhigh.empty())
        {
          int popthread4 = readyhigh.front();
          readyhigh.pop_front();
          num_threads[popthread4]-> MyState = VM_THREAD_STATE_RUNNING;
          current_ID= num_threads[popthread4]->MyThreadID;
          current_ID = num_threads[current_schedule]->MyThreadID;
          MachineContextSwitch(&num_threads[current_schedule]->MyContext,&num_threads[popthread4]->MyContext);
        }
        
        else if(!readynormal.empty())
        {
          int popthreadnormal4 = readynormal.front();
          readynormal.pop_front();
          num_threads[popthreadnormal4]-> MyState = VM_THREAD_STATE_RUNNING;
          current_ID= num_threads[popthreadnormal4]->MyThreadID;
          current_main = num_threads[current_schedule]->MyThreadID;
          MachineContextSwitch(&num_threads[current_schedule]->MyContext, &num_threads[popthreadnormal4]->MyContext);
        }
      
        else if(!readylow.empty())
        {
          int popthreadlow4 = readylow.front();
          readylow.pop_front();
          num_threads[popthreadlow4] -> MyState = VM_THREAD_STATE_RUNNING;
          current_ID= num_threads[popthreadlow4]->MyThreadID;
          current_main = num_threads[current_schedule]->MyThreadID;
          MachineContextSwitch(&num_threads[current_schedule]->MyContext, &num_threads[popthreadlow4]->MyContext);
        }
        else if(readyhigh.empty() && readynormal.empty() && readylow.empty())
        {
          ////cout<<"nothing in queue"<<endl;
          ////////////////cout<<current_schedule<<endl;
          current_ID=num_threads[0]->MyThreadID;
          current_main = num_threads[current_schedule]->MyThreadID;
          MachineContextSwitch(&num_threads[current_schedule]->MyContext, &num_threads[0]->MyContext);
        }
      }
      else if(num_threads[current_schedule]->MyState == VM_THREAD_STATE_IDLE)
      {
        ////cout<<"IDLING"<<endl;
        if(!readyhigh.empty())
        {
          int popthread5 = readyhigh.front();
          readyhigh.pop_front();
          num_threads[popthread5]->MyState = VM_THREAD_STATE_RUNNING;
          current_ID= num_threads[popthread5]->MyThreadID;
          current_main=num_threads[current_schedule]->MyThreadID;
          MachineContextSwitch(&num_threads[current_schedule]->MyContext, &num_threads[popthread5]->MyContext);
        }
        else if(!readynormal.empty())
        {
          int popthreadnormal5 = readynormal.front();
          readynormal.pop_front();
          num_threads[popthreadnormal5]->MyState = VM_THREAD_STATE_RUNNING;
          current_ID = num_threads[popthreadnormal5]->MyThreadID;
          current_main = num_threads[current_schedule]->MyThreadID;
          MachineContextSwitch(&num_threads[current_schedule]->MyContext, &num_threads[popthreadnormal5]->MyContext);
        }
        else if(!readylow.empty())
        {
          int popthreadlow5 = readylow.front();
          readylow.pop_front();
          num_threads[popthreadlow5]->MyState = VM_THREAD_STATE_RUNNING;
          current_ID = num_threads[popthreadlow5]->MyThreadID;
          current_main = num_threads[current_schedule]->MyThreadID;
          MachineContextSwitch(&num_threads[current_schedule]->MyContext, &num_threads[popthreadlow5]->MyContext);
        }
        else if(readyhigh.empty() && readynormal.empty() && readylow.empty() && num_mutex.size()>0)
        {
          //////////////////cout<<"inside else"<<endl;
           ////////cout<<"IDLE RIGHT NOW WITH ID "<<current_schedule<<endl;
          //////////////////cout<<current_ID<<endl;
            ////////////////cout<<"CURRRENT ID IN IDLE: "<< current_ID<<endl;
           ////cout<<"CURRENT MAIN"<<current_main<<endl;
         // if(num_threads[current_main]->MyState == VM_THREAD_STATE_WAITING)
         // {
            current_ID = num_threads[1]->MyThreadID;
            num_threads[1]->MyState = VM_THREAD_STATE_RUNNING;
            MachineContextSwitch(&num_threads[current_schedule]->MyContext, &num_threads[1]->MyContext); 
         // }
        }
      }
    }
  }
}

