//project2-CPSC-351//
//NIDHI and JANELLE//
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <algorithm>
//#include "process.h"

using namespace std;


//global constant
int TIME_MAX = 100000;
//variables to use
int memory_size =0; //should be multiple of 100,200,400
int page_size = 0; //should be from 100,200,400
int pagesizearray[3] = {100,200,400};
int page_size_index = 0; //index of array
int number_of_process =0;//how many process
int virtual_clock =0; //time clock
vector <int> arrivaltimeset;
struct Frame
{
  int lowerBound;
  int upperBound;
  int pageNumber;
  int pid;

  Frame();
  Frame(int x, int y);
};

Frame::Frame()
{
}

Frame::Frame(int x, int y)
{
  lowerBound = x;
  upperBound = y;
}

struct process
{
  int processid;
  int processarrival;
  int processtermination;
  int processrequirememory;

  //void processstore()

};
std::vector <process> processlist;
std::vector <process> processqueue;
std::vector <process> processInmemory;
std::vector <process> processcomplete;
//process processlist[7];


//////////////////////////////////////////////
////////////////get_user_input()//////////////
/////////memory size and pagesize//////////////
//////////////////////////////////////////////
void get_user_input()
{

      cout<< "memory size (multiple of 100,200,400): ";
      cin>> memory_size;


      cout<< " Select Page size:(type 1 for 100, 2 for 200, 3 for 400):  ";
      cin>> page_size_index;
      while(page_size_index <1 || page_size_index >3)
      {
        cout<< " not correct page size: "<<endl;
        cout<< " Select Page size:(type 1 for 100, 2 for 200, 3 for 400):  ";
        cin>> page_size_index;
      }

  ///get the page size from array according to the index ..index-1
  page_size = pagesizearray[page_size_index-1];

  if(memory_size % page_size !=0)
  {
    cout<<" the memory size should be multiple of page size:"<<endl;
    cout<< " select proper memory size and page_size"<<endl;
    get_user_input();
  }
}

////////////////////print memorymap///////////////
void printmemorymap(Frame *memoryMap,int pagesize,int memorysize,FILE *fp)
{
  int lbound,hbound;
  if (fprintf(fp, "\t\t\t  Memory map:") < 0)
  {
    perror("fprintf");
  }
  for (int i = 0; i < memorysize; i+= pagesize)
  {
    lbound = i;
    hbound = i + pagesize - 1;
    if (memoryMap[i].pid == -1)
    {
      if (fprintf(fp, "%d-%d: Free frame\n\t\t\t\t\t\t     ", lbound, hbound) < 0)
      {
        perror("fprintf");
      }
    }
    else
    {
      if (fprintf(fp, "%d-%d: Process %d, Page %d\n\t\t\t\t\t\t     ", lbound,
                  hbound, memoryMap[i].pid, memoryMap[i].pageNumber) < 0)
      {
        perror("fprintf");
      }
    }
  }
}
///////////////////////////////////////////////////
//////////collect_file_data()////////////////////
///store data in
void collect_file_data()
{
  int count =0;
  string line;

  ifstream file("in1.txt");   //input file
//  FILE* file = fopen("in1.txt","r");
//file.open();
 //check file is found or not
 if (!file)
 {
   cout << "cannot open the file. \n";      //message for file is not found
   return;
 }
 // while (file>>line)
 // {
 //   cout<< line;
 // }

   // if(count ==0)
   // {
     //getline(file,line);
     file >> line;
     number_of_process = stoi(line);
  //   cout<< "number of files: "<<number_of_process<<endl;
   //}
  //  cout<< "size of processlist: "<< processlist.size()<<endl;
   while(count<number_of_process)
   {
     //int numberOfPieces, pid, arrivalTime, lifetimeInMemory;
     int numberOfPieces;
     process eachprce;
  //   cout<< "counts : "<<count<<endl;
     file >> line;
      eachprce.processid = stoi(line);
    //  cout<< "pid: "<<  eachprce.processid<<endl;
      //getline(file,line);
      file >> line;
      eachprce.processarrival = stoi(line);
    //  cout<< "arrival time: "<<eachprce.processarrival<<endl;
      file >> line;
      eachprce.processtermination= stoi(line);
    //  cout<< "life of memory: "<< eachprce.processtermination<<endl;
      file>>line;
      numberOfPieces = stoi(line);
    //  cout<< "numnber of pieces"<< numberOfPieces<<endl;
      int f =0;
      eachprce.processrequirememory=0;
      while(f<numberOfPieces)
      {
        file>>line;
        eachprce.processrequirememory = eachprce.processrequirememory + stoi(line);
        f++;
      }
    //  cout<< "total memory need to process"<<eachprce.processid << " : "<< eachprce.processrequirememory<<endl;

      //p1.Process(pid,arrivalTime,lifetimeInMemory,totalmemoryofprocess);
      processlist.push_back(eachprce);
    //   cout<< "size of processlist: "<< processlist.size()<<endl;
     count++;
   }

  // cout<< "size of processlist: "<< processlist.size()<<endl;
  // cout<< "processlist process 1: "<<processlist[1].processrequirememory<<endl;

  //close the file
//fclose(file);
file.close();

}

//////////////display queue in output file at virtualclock
void displayarrival(int clock, vector<process> &processqueue,FILE *fp)
{
      bool check = false;
      //check time is already display in the output file or not
       if(arrivaltimeset.size()!= 0)
       {
         for(int t=0; t<arrivaltimeset.size();t++)
         {
           if(arrivaltimeset[t]==clock)
           {
             check = true;
           }
         }
       }
    //now display the output
    //first display the time if time is not displayed before..
      if(check == false)
      {
          arrivaltimeset.push_back(clock);

          if(fprintf(fp,"t = %d:",clock)<0)
          {
              perror("fprintf");
          }
      }
    //now display the process list of arrival process
      if(fprintf(fp," Process %d arrives\n\t      Input Queue: [",processqueue[processqueue.size()-1].processid)<0)
      {
            perror("fprintf");
      }
      for(int g=0;g<processqueue.size();g++)
      {
          if(fprintf(fp,"%d ",processqueue[g].processid)<0)
            {
                perror("fprintf");
            }
      }
      if(fprintf(fp,"]\n\t\t\t\t")<0)
      {
          perror("fprintf");
      }

}

/////////////memorymanager////////////
void memorymanager(Frame *memoryMap,vector<process>&processqueue,vector<process> &processInmemory,int pagesize,int memorysize,FILE *fp)
{
  //make list of frames according to page size and memorysize..
  list <Frame> freeframes;
  int freelbound, freehbound;
  ////////////////catch all free memory list in frameslist
  for(int f=0;f<memorysize;f+=pagesize)
  {
    cout<<"FFFFFFFFFFFFFFFFFFFFFFFFF"<<endl;
    //checking if memorymap.pid is empty or not.. if is then put those frame list in free frames
    if(memoryMap[f].pid == -1)
    {
      freelbound = f;
      freehbound = f + (pagesize -1);
      freeframes.push_back(Frame(freelbound,freehbound));
    }
  }
  cout<<"doennnnnnnnnnnnnnnnnnnnnnnnnnn...free frames ready"<<endl;
  ///////////////now checking the processqueue processes can have memory in
  /////////////////memory map as processrequirememory
  int pageneed;
  size_t size;
  bool cannotbesatisfied =false;
/////////////checking queue is empty or not ...if not need those process waiting in quueue in memorymap
  while(processqueue.size() !=0)
  {
    cout<<"process....queue ....check....pageneed count..begin"<<endl;
    size = processqueue.size();
    for(int d=0; d<size;d++)
    {
      //get value of pageneed according to the page size user choose, and memoryrequirement of each process in queue
      if((processqueue[d].processrequirememory % pagesize)!=0)
      {
        pageneed = (processqueue[d].processrequirememory /pagesize) + 1;
      }
      else
      {
        pageneed = processqueue[d].processrequirememory /pagesize;
      }

      //now check the pageneed is less than available frames or not
      //if yes,,, allocate the memory to that process
      cout<<"process....queue ....check....pageneed count..done.. get process freememory"<<endl;
      if(pageneed<= freeframes.size())
      {
                ///allocate the frames to the process
                for(int j=0; j<pageneed;j++)
                {
                  freelbound = freeframes.front().lowerBound;
                  freehbound = freeframes.front().upperBound;
                  ///so change from lower bound to upper bound
                  for(;freelbound<=freehbound;freelbound++)
                  {
                    memoryMap[freelbound].pid = processqueue[d].processid;
                    memoryMap[freelbound].pageNumber = j+1;
                  }

                  //now remove the frames that used for process from the freeframes list
                  freeframes.pop_front();
                }
                  cout<<"process....queue ....check... get process freememory...done"<<endl;
                  //now store that process information in the processinmemory vector
                process temp = processqueue[d];
                processInmemory.push_back(temp);

                cout<<"process....queue ....check... showing output as process moved.."<<endl;
                //show the prcoess is now in memory map
                if(fprintf(fp,"\tMM moves Process %d to memory\n",processqueue[d].processid)<0)
                {
                      perror("fprintf");
                }
                  cout<<"process....queue ....check... showing output as process moved..done.. erasing process from processque"<<endl;
                  //erasing the vector/...
               processqueue.erase(processqueue.begin());

                //now display the process list of arrival process
                if(processqueue.size()!=0)
                {
                  cout<<"process....queue ....check... showing output as process moved..done.. erasing process from processque....done..showing process queue..not empty"<<endl;
                      if(fprintf(fp,"\t\t\t\t Input Queue: [")<0)
                      {
                            perror("fprintf");
                      }
                      for(int g=0;g<processqueue.size();g++)
                      {
                          if(fprintf(fp,"%d ",processqueue[g].processid)<0)
                            {
                                perror("fprintf");
                            }
                      }
                      if(fprintf(fp,"]\n")<0)
                      {
                          perror("fprintf");
                      }
                }
                else
                {
                  cout<<"process....queue ....check... showing output as process moved..done.. erasing process from processque....done..showing process queue..empty"<<endl;
                  if(fprintf(fp,"\t\t\t\t Input Queue: [ ]\n")<0)
                  {
                        perror("fprintf");
                  }
                }

        //print the memory map here....///////////////////////////
        printmemorymap(memoryMap,pagesize,memorysize,fp);
      }
      ///the process in queue did not find space in memory map according to its memoryrequirement
      else if(d == size-1)
      {
        cannotbesatisfied = true;
      }
    }

    //if no more proces can be satisfy... then break from while loop
    if(cannotbesatisfied)
    {
      break;
    }
  }

  ///clear the list of freeframes..
  freeframes.clear();
}

///////////////main function//////////////////////
int main()
{
  //create memory map
  Frame *memoryMap;
  //get memory_size value and page_size value from user
  get_user_input();

  //create the  memory map with -1 as input of array showing is free to use
  memoryMap = new Frame[memory_size];
  for(int i=0; i<memory_size;i++)
  {
    memoryMap[i].pid =-1;
  }

  //get the detail from the file..we know the file so,, not getting filename from user
  collect_file_data();

//now open the output file..
//ofstream fp("output");
 FILE* fp = fopen("output","w");
 if(!fp)
 {
  cout<< "output file is not open"<<endl;
   exit(-1);
 }
 vector <process> temp;
  //Now until the processlist is empty..
  while(processlist.size()!=0 && virtual_clock < 1)
  {
        //get the processes in the ready queue at time of arrival time
        if(processlist.size()!=0)
        {
          for(int j=0; j<processlist.size();j++)
          {

            //cout<< "virtual_clock tiem: "<<virtual_clock<<endl;
            if(processlist[j].processarrival == virtual_clock)
            {
            //  cout<<"processlist arrivaltime: "<<processlist[j].processarrival<<endl;
              processqueue.push_back(processlist[j]);
            // display process arrival status in output file
              displayarrival(virtual_clock,processqueue,fp);

               //processlist.erase(processlist.begin()+j);
              //cout<< "size of processlist: "<< processlist.size() <<" and first process in list now: "<< processlist.at(0).processid<<endl;

            }
          }
        }
          ///get the information of memory map and decide to take it out the process from the queue and transfer to currently in memorymap
       memorymanager(memoryMap,processqueue,processInmemory,page_size,memory_size,fp);



    virtual_clock++;
  }

  fclose(fp);
  // for(int k=0; k<processqueue.size();k++)
  // {
  //      cout<< "processqueue: " << processqueue[k].processid<<endl;
  // }

  return(0);
}
