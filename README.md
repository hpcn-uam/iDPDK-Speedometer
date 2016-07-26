High Speed Speedometer using Intel DPDK
=================

Intel Data Plane Development Kit (DPDK) Speedometer (Bandwidth consumed monitor) 

This program allows to measure the bandwith consumed on an interface and counts the number of packets per second.


Compilation
=================
The program can be easly compiled using the makefile provided.
It needs (as anyother DPDK app) the DPDK env. variables such as *RTE_SDK* to be defined first.

The script *setup.sh* can be used to compile everything. Feel free to modify the script and modify those variables.


Execution
=================
There are many test.sh for different (but quite similar) scenarios. All of them wrote the information on the screen using stdout.
If we want to save that into an ordered log, we can use the "record.sh" scripts.
There are no limit on how many interfaces can be measured at a time. The program just have to be configured/lunched as any DPDK app.
