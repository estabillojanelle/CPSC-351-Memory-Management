/*AUTHORS
--------------------------------------
Nidhi Shah
CPSC 351-02
Janelle Estabillo
CPSC 351-02
 */
struct process
{
  int processid;            //store process id
  int processarrival;        //store process starting time..as arrival time ..
  int processtermination;    //store process termination time... as official completing time
  int processrequirememory;  //total memory require to the process
  int processaddmittime;     //actual addmited time of process in process queue to wait for turn to execute
  int processcompletime;     //actual completion time of process... who was in memory ..executing.. is actual addmited time + official termination time
  int processturnaroundtime; //waiting time + in memory( execution time).... actual completion time- official arrival time

// two functions to calculate the actual processcompletion time and turnaroudtime
//set the addmited time to calcuate all these
  void setprocessadmittime(int clock);
  void setturnaroudtime();

};
