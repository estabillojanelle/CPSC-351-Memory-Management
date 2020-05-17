
#include "process.h"
#include <iostream>


/* Set the admitted time of a process */
void process::setprocessadmittime(int clock)
{
  processaddmittime = clock;
}

/* Set the turnaround time of a process after calculating the time of its
** completion.
*/
void process::setturnaroudtime()
{
  processcompletime = processaddmittime + processtermination;
  processturnaroundtime = processcompletime - processarrival;
}
