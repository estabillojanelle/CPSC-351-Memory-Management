/*AUTHORS
--------------------------------------
Nidhi Shah
CPSC 351-02
Janelle Estabillo
CPSC 351-02
 */
 //Simulator:an integer variable representing virtual clock with incrementing each cycle..
 //In each cycle, until all processes have completed,
 //making appropriate memory management decisions along the way as processes arrive and complete.
 // Each arriving process will be first put into the queue. Processes in the input queue will be ordered according to their arrival times(FCFS) ordering).
  //In case of a completion, MM will first adjust the memory map as the memory region(s) previously allocated to the completed process is/are now free.
  //At, completion of process or arrival of process,MM will take decision if any process waiting to execute.. will start execution or not..by checking mempry requirement
  // If it is, MM will allocate the memory to that process.. and upadate required vectors..
  //during the cycle,,, Will show the reuired information regranding memory managemet processes in output file.
  ///after all processes' execution simulation done,, will show the average turnaroudtime

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include "process.h"
using namespace std;


//global constant
int TIME_MAX = 100000;
//global variable to use
int memory_size =0; //should be multiple of 100,200,400
int page_size = 0; //should be from 100,200,400
int pagesizearray[3] = {100,200,400};//as pagesize..is fixed..
int page_size_index = 0; //index of array
int number_of_process =0;//how many process
int virtual_clock =0; //time clock
vector <int> arrivaltimeset; //set of arrived process time
vector <int> completetimeset;//set of comple process time
bool comp =false; //for display time on output file.. for function works

////////////////////////////////
/////////Frame struct////////////////
/////////////////////////////
struct Frame
{
  int lowerBound; //frame lower val in range of memory per page
  int upperBound; //frame higher val in range of memory per page
  int pageNumber; //per process, the page number for that process
  int pid;         //process id.....default set to -1 , setting done in main

///////////////functions to set frames with lowerboud and upperboud ////////////
  Frame();
  Frame(int x, int y);
};
/////////////////frame function details//////////
Frame::Frame()
{
}

Frame::Frame(int x, int y)
{
  lowerBound = x;
  upperBound = y;
}

//vectors to store the informations related to the processes status
std::vector <process> processlist;  //store all processes in this list
std::vector <process> processqueue;  //The set of processes who is input queue.....waiting to get memory to start execution
std::vector <process> processInmemory; //The set of processes who get the memory map space to use...start their execution
std::vector <process> processcomplete; //the set of processes who complete thier execution.. need to come out from memorymap
std::vector <process> processcompleteandremoved; //the process is done executing and removed from memory,,

//////////////////////////////////////////////
////////////////get_user_input()//////////////
/////////memory size and pagesize//////////////
//////////////////////////////////////////////
void get_user_input()
{
      //ask user the size memory map...should be multiple of the 100,200,400..cause those oagesize is fixed
      //for precaution
      cout<< "memory size (multiple of 100,200,400): ";
      cin>> memory_size;

      //ask user to choose the index number according to the page size user want
      cout<< " Select Page size:(type 1 for 100, 2 for 200, 3 for 400):  ";
      cin>> page_size_index;
      //checking the page size index is among 1,2,3..
      //to avoid user to enter the actual page size instead of index
      //if is incorrect ...ask again again... until it get proper index
      while(page_size_index <1 || page_size_index >3)
      {
            cout<< " not correct page size: "<<endl;
            cout<< " Select Page size:(type 1 for 100, 2 for 200, 3 for 400):  ";
            cin>> page_size_index;
      }

      ///get the page size from array according to the index ..index-1
      page_size = pagesizearray[page_size_index-1];
     //checking the error in selecting the memory size and page size
      if(memory_size % page_size !=0)
      {
            //found error. wronge number selection ..either memory size and page size
            cout<<" the memory size should be multiple of page size:"<<endl;
            cout<< " select proper memory size and page_size"<<endl;
            //ask user again to select proper page size and memory size
            get_user_input();
      }
}

/////////////////////////////////////////////////
////////////////////print memorymap///////////////
/////////////////////////////////////////////////
void printmemorymap(Frame *memoryMap,int pagesize,int memorysize,FILE *fp)
{
      int lbound,hbound;
      //showing the output data in output file with checking error
      if (fprintf(fp, "\t\t\t Memory map:\n") < 0)
      {
            perror("fprintf");
      }
      //showing the page range, process id and page number for that process in output file with checking error
      for (int i = 0; i < memorysize; i+= pagesize)
      {
            lbound = i;
            hbound = i + pagesize - 1;
            //if the pid in memory map is -1 ...means no process took that frame ...of page..so it is free frmaes
            if (memoryMap[i].pid == -1)
            {
                  //showing free frames with error checking
                  if (fprintf(fp, "\t\t\t\t\t\t      %d-%d: Free frame\n", lbound, hbound) < 0)
                  {
                      perror("fprintf");
                  }
            }
            ///if is not free frames,,, menas some process is using that frame.. show which process is using it
            else
            {
                  //showing frame,, with lower..upper range number of page, with page number and processid with error check
                  if (fprintf(fp, "\t\t\t\t\t\t      %d-%d: Process %d, Page %d\n", lbound,hbound, memoryMap[i].pid, memoryMap[i].pageNumber) < 0)
                  {
                      perror("fprintf");
                  }
            }
      }
}

///////////////////////////////////////////////////
//////////collect_file_data()////////////////////
///store data of each process..except process admit,/////
//complete time and turnaround time/////////////////////
////and store in processlist vector/////////////////
///////////////////////////////////////////////////
void collect_file_data()
{
      int count =0;
      string word; ///work as word by word..
      //input file
      ifstream file("in1.txt");
      //check file is found or not
      if (!file)
      {
           cout << "cannot open the file. \n";      //message for file is not found
           return;
      }
      //get the word(string)....for number of processes...
     file >> word;
     //store number of processes in variable
     number_of_process = stoi(word);
     //will read each process data until number of processes
     while(count<number_of_process)
     {
         //variables local... to store process data...
         int numberOfPieces;
         process eachprce;
         //get the process id..and store as processid
         file >> word;
         eachprce.processid = stoi(word);
         //get the process set arrival time..and store as processarrival
         file >> word;
         eachprce.processarrival = stoi(word);
         //get the process set termination time..and store as processtermination
         file >> word;
         eachprce.processtermination= stoi(word);
         //get the process numebr of memory pieces for process and store as numberOfPieces
         file>>word;
         numberOfPieces = stoi(word);
         ///until the number of pieces count... calculate total memory storage reuirement..and store as processrequirememory
         int f =0;
         eachprce.processrequirememory=0;
         while(f<numberOfPieces)
          {
                file>>word;
                eachprce.processrequirememory = eachprce.processrequirememory + stoi(word);
                f++;
          }
         //seach information needed for process to start is stored.. and store that object in processlist vector
         //prepare processlist vector
         processlist.push_back(eachprce);
         count++;
   }

        //close the file
        file.close();

}
/////////////////////////////////////////////////////////////////////////
//////////////display queue in output file at virtualclock//////////////
/////////////////at process arrival in queue as ready to execute stage../////
///////////////////////////////////////////////////////////////////////////
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
    //first display the time if time is not displayed before..with error check
      if(check == false)
      {
          arrivaltimeset.push_back(clock);

          if(fprintf(fp,"t = %d:\n",clock)<0)
          {
              perror("fprintf");
          }
      }
    //now display the process list of arrival process with error check
      if(fprintf(fp," \t\t   Process %d arrives\n\t     Input Queue: [",processqueue[processqueue.size()-1].processid)<0)
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
///////////////////////////////////////////////////////////////
/////////////Important function:    memorymanager////////////
//////////////manage memory map, processqueue, processInmemory, processcomplete vectors......./////////////
////decide to give memory pages to the prcess, decide to remove process, once process done..executing/////////
//////////and if any process waiting after getting free memory space.. decide to give meory to that process///////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void memorymanager(Frame *memoryMap,vector<process>&processqueue,vector<process> &processInmemory,int pagesize,int memorysize, vector<process>&processcomplete,FILE *fp,int virtclock)
{
      ////////////////////////////////////process complete,,, show the complition of process with new memory map//////////////////////
      bool check = false;
      //check completed time is already display in the output file or not
       if(completetimeset.size()!= 0)
       {
         for(int t=0; t<completetimeset.size();t++)
         {
           if(completetimeset[t]==virtclock)
           {
             check = true;
           }
         }
       }
      //now display the output
      //first display the time if time is not displayed before..
      if(check == false && comp == true)
      {
          completetimeset.push_back(virtclock);

          if(fprintf(fp,"t = %d:\n",virtclock)<0)
          {
              perror("fprintf");
          }
          comp =false;
      }

      int processId;
      size_t length;

      /* Traverse through a vector of the id's of completed processes */
      while (!processcomplete.empty())
      {
            //get the process id of the completed process
            processId = processcomplete.front().processid;
            //remove that process from list... for further completed process
            processcomplete.erase(processcomplete.begin());

            /* Free the memory previously occupied by the process with id processId */
            for(int i=0;i<memorysize;i++)
             {
                    if(memoryMap[i].pid ==processId)
                    {
                        memoryMap[i].pid = -1;   ///free memory means set pid to -1
                    }
             }
             //have to remvoe that process process from processInmemory vector and show the new memory map...
             //have to store that process details in the processcompleteandremoved vector... before erasing from processInmemory vec.
            /* Traverse through the vector of processes currently in memory */
            length = processInmemory.size();
            for (int i = 0; i < length; i++)
            {
                  /* If the pid of the completed process matches the pid of the process in
                  ** memory, then announce its completion, and print the current memory map.
                  ** Also, add it to the vector of completed processes, and erase it from
                  ** the vector of processes currently in memory.
                  */
                  if (processInmemory[i].processid == processId)
                  {
                        process temp = processInmemory[i];
                        if (fprintf(fp, " \t\t   Process %d completes\n",temp.processid) < 0)
                        {
                          perror("fprintf");
                        }
                        //print the memory map here....///////////////////////////
                        printmemorymap(memoryMap,pagesize,memorysize,fp);
                        ////////store that process as completed and removed process
                        processcompleteandremoved.push_back (temp);
                        ////////////////remove that process from the inmemory list
                        processInmemory.erase(processInmemory.begin() + i);
                  }
            }
      }
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      ////////////check if any proess waiting in processqueue and if any free memory available in memory map to provide that process
      ////////////check how much memory that process require... if any process required memory is lower or equal than free memory..
      ////////////////then give that free memory to that process and store that process in inmemory vector.
       /////////////////.... otherwise process have to wait until the running process freed the memory..
      //make list of frames according to page size and memorysize..
      list <Frame> freeframes;
      int freelbound, freehbound;
      ////////////////catch all free memory list in frameslist
      for(int f=0;f<memorysize;f+=pagesize)
      {
          //checking if memorymap.pid is empty or not.. if is then put those frame list in free frames
          if(memoryMap[f].pid == -1)
          {
            freelbound = f;
            freehbound = f + (pagesize -1);
            freeframes.push_back(Frame(freelbound,freehbound));
          }
      }
      ///////////////now checking the processqueue processes can have memory in
      /////////////////memory map as processrequirememory
      int pageneed;
      size_t size;
      bool cannotbesatisfied =false;
      /////////////checking any process is waiting then check it can get memory from memory map or not
      ///////////...if not need those process waiting in queue
      while(!processqueue.empty())
      {
            ///get the size of the processqueue vector...as process is in inmemory vector.. will be removed from the processqueue
            size = processqueue.size();
            //////////check for each process that waiting ,,if it can get memory...
            for(int d=0; d<size;d++)
            {
                  int dval = d;
                  //get value of pageneed according to the page size user choose, and memoryrequirement of each process in queue
                  if((processqueue[d].processrequirememory % pagesize)!=0)
                  {
                    pageneed = (processqueue[d].processrequirememory /pagesize) + 1;
                  }
                  else
                  {
                    pageneed = processqueue[d].processrequirememory /pagesize;
                  }

                  //now check the pageneed is less than or equal to available frames or not
                  //if yes,,, allocate the memory to that process
                  if(pageneed<= freeframes.size())
                  {
                            ///allocate the frames to the process
                            for(int j=0; j<pageneed;j++)
                            {
                                freelbound = freeframes.front().lowerBound;
                                freehbound = freeframes.front().upperBound;
                                ///so range from lower bound to upper bound......
                                ////and change pid for that range.. to process id which is using that frame.
                                ////and the page number too..related the process
                                for(;freelbound<=freehbound;freelbound++)
                                {
                                    memoryMap[freelbound].pid = processqueue[d].processid;
                                    memoryMap[freelbound].pageNumber = j+1;
                                }

                                //now remove the frames that used for process from the freeframes list
                                freeframes.pop_front();
                            }
                              //now store that process information in the processinmemory vector
                            process temp = processqueue[d];
                            //first...set the addmited time and turnaround time..
                            temp.setprocessadmittime(virtclock);
                            //set the actual completed time
                            temp.setturnaroudtime();
                            processInmemory.push_back(temp);
                            //show the prcoess is now in memory map...with error check
                            if(fprintf(fp,"\t\t\t MM moves Process %d to memory\n",processqueue[d].processid)<0)
                            {
                                  perror("fprintf");
                            }
                              //erasing the process object frome the processqueue vector.. as process in not in waitlist anymore../...
                           processqueue.erase(processqueue.begin() + dval);
                            //now display the process list of arrival process that are waiting in processqueue
                            ///if any process is waiting.... otherwise show... no process is waiting by showing empty brackets...[]
                            //////with error check for output file display
                            if(processqueue.size()!=0)
                            {
                                  if(fprintf(fp,"\t\t\t Input Queue: [")<0)
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
                              if(fprintf(fp,"\t\t\t Input Queue: [ ]\n")<0)
                              {
                                    perror("fprintf");
                              }
                            }

                            //print the memory map here....///////////////////////////
                            printmemorymap(memoryMap,pagesize,memorysize,fp);

                            break;
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

//////////////////////////////////////////////////////
///////////////main function//////////////////////
////////////////////////////////////////////////////
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
     FILE* fp = fopen("output","w");
     ///check error in file is open or not
     if(!fp)
     {
       cout<< "output file is not open"<<endl;
       exit(-1);
     }
      vector <process> temp;
      //Now untill the processcompleteandremoved size is equal to the number of prcoesses... means until all process executed..
      ////and until time run out....limit 100000
      while(processcompleteandremoved.size()!= number_of_process && virtual_clock < 100000)
      {
            //get the processes in the ready queue at time of arrival time
            if(processlist.size()!=0)
            {
              for(int j=0; j<processlist.size();j++)
              {

                //if the virtual clock is equal to the processarrival time
                if(processlist[j].processarrival == virtual_clock)
                {
                    //store the process in temp3 vector and push that vector in the processqueue vector
                    process temp3;
                    temp3 = processlist[j];
                    processqueue.push_back(temp3);
                    // display process arrival status in output file
                      displayarrival(virtual_clock,processqueue,fp);
                }
              }
            }

           ///get the information of memory map and decide to take it out the process from the queue and transfer to currently in memorymap
           memorymanager(memoryMap,processqueue,processInmemory,page_size,memory_size,processcomplete,fp,virtual_clock);

            //check if any complete process ....
            for(int g =0; g<processInmemory.size();g++)
            {
                  ///if the processcompletime is equal to the virtual clock..
                  if(processInmemory[g].processcompletime== virtual_clock)
                  {
                    processcomplete.push_back(processInmemory[g]);
                      comp =true;
                  }
            }
            ///get the information of memory map and remove the completed process from inmemory process list..
            // decide if any process is waiting in queue, so take it out the process from the queue and transfer to currently in memorymap if free memory condition satisfied
            memorymanager(memoryMap,processqueue,processInmemory,page_size,memory_size,processcomplete,fp,virtual_clock);

        virtual_clock++;
      }

      /* Calculate the average turnaround time by summing the total turnaround
      ** time from the processes completed vector and dividing it by the number of
      ** processes completed.
      */
      int processesCompletedSize = processcompleteandremoved.size();
      int totalTurnaroundTime = 0;
      float averageTurnaroundTime;

      for (int i = 0; i < processesCompletedSize; i++)
      {
          totalTurnaroundTime += processcompleteandremoved[i].processturnaroundtime;
      }
      averageTurnaroundTime = (float)totalTurnaroundTime / processesCompletedSize;

      //display the average turnaround time of the entire process..with error check
      if (fprintf(fp, "Average turnaround: %.2f \n", averageTurnaroundTime) < 0)
      {
        perror("fprintf");
      }

      /* Close the file with the error check */
      if (fclose(fp) != 0)
      {
        perror("fclose");
      }

      /* Deallocate the memory map */
      delete[] memoryMap;
      return(0);
}
