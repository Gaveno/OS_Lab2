# OS_Lab2
Lab 2 for Operating Systems CSC3453 - CPU Scheduling Simulation.

Author: Gavin Atkin
Lab2 - CSCI 3453
Date: 10/20/2017

README:
* From CSE Grid terminal:
	-Extract the atk8013.tar.gz file.
	-Build source with: make makefile all
	-Run program with: ./myscheduler inputfilename outputfilename mode [params]
	Where inputfilename and outputfilename are text files.
	Mode is: 0=FCFS, 1=SRTF, 2=RR, 3=PP.
	And params is used for RR as the time quantum value.

* Output will be written to the outputfilename as well as to the console in table format.
** NOTE: The algorithms all work correctly with the input4 dataset. No way to be sure if they are 100% correct with any other set of data due to multiple locations of deciding which happens first out of two things theoretically happening at the exact same time without specifications for what should occur first.