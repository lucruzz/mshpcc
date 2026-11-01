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
#include <fstream>
#include <mutex>
// #include <mpi.h>

// Para os dados de um evento
struct CommunicationEvent { 
    int main_rank;
    int sec_rank;
    long long int num_bytes;
    double duration;
    std::string timestamp;
    std::string operation;
    std::string communicator;
};

// Para executar as operações de registro
class CommunicationLogger {
private:
    std::ofstream outputfile;
    std::mutex mutex;

public:
    CommunicationLogger(const std::string& filepath){
        outputfile.open(filepath, std::ios::out);

        if(outputfile.is_open()){
            outputfile 
                << "timestamp,"
                << "operation,"
                << "main_rank,"
                << "second_rank,"
                << "num_bytes,"
                << "duration,"
                << "communicator\n";
        }
    }
    
    ~CommunicationLogger(){
        if(outputfile.is_open()){
            outputfile.close();
        }
    }

    void log(const CommunicationEvent& event){
        std::lock_guard<std::mutex> lock(mutex);

        if ( !outputfile.is_open() ) { return; }

        outputfile 
            << event.timestamp << ","
            << event.operation << ","
            << event.main_rank << ","
            << event.sec_rank << ","
            << event.num_bytes << ","
            << event.duration << ","
            << event.communicator << "\n";

        outputfile.flush();
    }
};


int main(){
    
    CommunicationLogger logger("test.csv");

    CommunicationEvent event;

    event.main_rank = 0;
    event.sec_rank = 1;
    event.num_bytes = 4;
    event.duration = 0.5;
    event.operation = "MPI_Send";
    event.communicator = "MPI_COMM_WORLD";
    event.timestamp = "00:10:05";

    logger.log(event);

    // std::cout << "Operation    : " << event.operation    << std::endl;
    // std::cout << "Bytes        : " << event.num_bytes    << std::endl;
    // std::cout << "Main rank    : " << event.main_rank    << std::endl;
    // std::cout << "Sec. rank    : " << event.sec_rank     << std::endl;
    // std::cout << "Duration     : " << event.duration     << std::endl;
    // std::cout << "Communicator : " << event.communicator << std::endl;

    return 0;
}
