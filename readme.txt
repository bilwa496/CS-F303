Execution Instructions:

Note- First server is to be executed and then followed by client. I have already produced input.txt [deliberately kept it as a file of big size] in order to see the real-time transmission between server and client.

Step0- Open two teminals.

Step1- In terminal 1 run
gcc server.c
./a.out

Step2- In terminal 2 run
gcc client.c
./a.out input.txt

Step-4
The full transmission and receival of packets will be reflected in both the terminals along with other packet detail atributes.
Client will automatically close once it receives the last packet sent by server. Server needs to be manually closed, once client receives final packet chunk and terminates the connection subsequently.


**********************************************************************
Methodology-
System calls like fork() are used to synchronize the dual clients when they are listening to the remote server. Server efficiently uses the file descriptors and system calls to handle multiple client connections without any error.
The contents of input.txt are seamlessly tranferred and written in a new file.

Note-
If in any case, you need to compile and run this code for more than 1 time, kindly delete the automatically created output file after previous run. Because the previous output file may not be accessed again, due to the immutable inode file descriptors.

For verification purpose, you may please match the file sizes of input and output files.

I have kept a sufficiently large input file.
