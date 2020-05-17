struct process
{
  int processid;
  int processarrival;
  int processtermination;
  int processrequirememory;
  int processaddmittime;
  int processcompletime;
  int processturnaroundtime;

  void setprocessadmitandcompletetime(int clock);
  void setturnaroudtime();

};
