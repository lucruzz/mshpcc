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
#include <cstdlib>
#include <fstream>
#include <mutex>
#include <mpi.h>
#include <unistd.h> // getpid, getenv

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



static CommunicationLogger* logger = nullptr;


/**
 * @file mpi_profiler.cpp
 * @brief Função para inicializar o logger após a função MPI_Init.
 * @details Essa função criará um logger para cada processo, 
 *          o que significa que cada processo terá seu próprio arquivo para escrever.
 *
**
*/
static void initialize_profiler(){
    int rank = -1;
    PMPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const char* log_dir = std::getenv("MSHPCC_MPI_LOG_DIR");
    const char* jobid = std::getenv("SLURM_JOB_ID");

    if (!log_dir) { log_dir = "."; }

    if (!jobid) { jobid = "local"; }

    char hostname[100];

    if (gethostname(hostname, sizeof(hostname)) != 0){
        std::cerr << "Could not get hostname." << std::endl;
        return;
    }

    std::string filepath = 
        std::string(log_dir) +
        "/mshpcc-" + 
        std::string(jobid) +
        "-" +
        hostname +
        "-rank-" +
        std::to_string(rank) +
        "-pid-" +
        std::to_string(getpid()) +
        ".csv";

    try{
        
        logger = new CommunicationLogger(filepath);

    }catch(std::exception& e){
        
        std::cout << "An error occurred: " << e.what() << std::endl;
        logger = nullptr;

    }

    return;
}


/**
 * @file mpi_profiler.cpp
 * @brief Função para registrar um evento de comunicação.
 * @details ...
 *
**
*/
static void record_event(
    int main_rank,
    int sec_rank,
    long long int num_bytes,
    double duration,
    const std::string& timestamp,
    const std::string& operation,
    const std::string& communicator
){
    try{
        
        CommunicationEvent event;

        event.main_rank = main_rank;
        event.sec_rank = sec_rank;
        event.num_bytes = num_bytes;
        event.duration = duration; // TODO 1: Implement how to catch the communication duration 
        event.operation = operation;
        event.communicator = communicator;
        event.timestamp = timestamp; // TODO 2: Implement how to catch the timestamp (chronos/syscall?) 

        logger->log(event);

    }catch(std::exception& e){

        std::cout << "An error occurred: " << e.what() << std::endl;

    }
    
    return;
}

/**
 * @file mpi_profiler.cpp
 * @brief Função para interceptar chamada MPI_Init.
 * @details Esta interceptação precisa inicializar o profiler personalizado.
 * $ cat /usr/lib/x86_64-linux-gnu/openmpi/include/mpi.h | grep "MPI_Init("
 *       OMPI_DECLSPEC  int MPI_Init(int *argc, char ***argv);
 *       OMPI_DECLSPEC  int PMPI_Init(int *argc, char ***argv);
 *
 */
extern "C" int MPI_Init(int* argc, char*** argv){
    int result = PMPI_Init(argc, argv);

    if (result == MPI_SUCCESS){
        initialize_profiler();
    }

    return result;
}

extern "C" int MPI_Finalize(){
    delete logger;
    logger = nullptr;

    return PMPI_Finalize();
}

/**
 *
 *
 * OMPI_DECLSPEC  int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
 * OMPI_DECLSPEC  int PMPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
 */
extern "C" int MPI_Send(
    const void *buf, 
    int count, 
    MPI_Datatype datatype, 
    int dest, 
    int tag, 
    MPI_Comm comm
){
    int result = PMPI_Send(buf, count, datatype, dest, tag, comm);

    double duration = 0.0;
    std::string timestamp = "00:00:00";

    // para descobrir a ID da task com que estamos trabalhando
    int rank;
    PMPI_Comm_rank(comm, &rank);

    // para descobrir o tamanho do tipo utilizado
    int type_size;
    PMPI_Type_size(datatype, &type_size);
    long long int bytes = count * type_size;

    // para descobrir o nome do comunicador
    char communicator[MPI_MAX_OBJECT_NAME];
    int name_len;
    PMPI_Comm_get_name(comm, communicator, &name_len);
    
    if ( result == MPI_SUCCESS ){
        record_event(
            rank,
            dest,
            bytes,
            duration,
            timestamp,
            "MPI_Send",
            communicator
        );
    }

    return result;
}


/**
 *
 *
 * OMPI_DECLSPEC  int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status);
 * OMPI_DECLSPEC  int PMPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status);
 */
extern "C" int MPI_Recv(
    void *buf, 
    int count, 
    MPI_Datatype datatype, 
    int source, 
    int tag, 
    MPI_Comm comm, 
    MPI_Status *status
){
    int result = PMPI_Recv(buf, count, datatype, source, tag, comm, status);

    double duration = 0.0;
    std::string timestamp = "00:00:00";

    // para descobrir a ID da task com que estamos trabalhando
    int rank;
    PMPI_Comm_rank(comm, &rank);

    // para descobrir o tamanho do tipo utilizado
    int type_size;
    PMPI_Type_size(datatype, &type_size);
    long long int bytes = count * type_size;

    // para descobrir o nome do comunicador
    char communicator[MPI_MAX_OBJECT_NAME];
    int name_len;
    PMPI_Comm_get_name(comm, communicator, &name_len);
    
    if ( result == MPI_SUCCESS ){
        record_event(
            rank,
            source,
            bytes,
            duration,
            timestamp,
            "MPI_Recv",
            communicator
        );
    }

    return result;
}


// int main(){

//     initialize_profiler();
//     record_event(
//         0,
//         1,
//         4,
//         0.005,
//         "00:01:15",
//         "MPI_Send",
//         "MPI_COMM_WORLD"
//     );

//     record_event(
//         1,
//         0,
//         4,
//         0.008,
//         "00:01:16",
//         "MPI_Recv",
//         "MPI_COMM_WORLD"
//     );

//     return 0;
// }
