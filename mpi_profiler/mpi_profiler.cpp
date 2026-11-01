/*
 * :: Copyright 2026
 * :: Author    : Lucas Cruz
 * :: Title     : MySupervisor HPC Collector (MSHPCC)
 * :: Version   : 0.0.1
 * :: Maintainer: Lucas Cruz
 * :: Email     : lucruz@posgrad.lncc.br
 * :: Type      : Research
 * ::
 * :: Message from maintainer : 
 * ::     Please, if you have any questions about the implementation, don't send it to me!
 * ::     Just God knowns what I've tried to do in here. I don't remember anything anymore.
 * ::     ...
 * ::     I'm just kidding! Besides, just God knowns, you can send me your questions!
 *
 *
*/
#include <iostream>
#include <string>
// #include <mpi.h>


struct CommunicationEvent { 
    int main_rank;
    int sec_rank;
    long long int num_bytes;
    double duration;
    std::string operation;
    std::string communicator;
};


int main(){
    CommunicationEvent event;

    event.main_rank = 0;
    event.sec_rank = 1;
    event.num_bytes = 4;
    event.duration = 0.5;
    event.operation = "MPI_Send";
    event.communicator = "MPI_COMM_WORLD";

    std::cout << "Operation    : " << event.operation    << std::endl;
    std::cout << "Bytes        : " << event.num_bytes    << std::endl;
    std::cout << "Main rank    : " << event.main_rank    << std::endl;
    std::cout << "Sec. rank    : " << event.sec_rank     << std::endl;
    std::cout << "Duration     : " << event.duration     << std::endl;
    std::cout << "Communicator : " << event.communicator << std::endl;

    return 0;
}
