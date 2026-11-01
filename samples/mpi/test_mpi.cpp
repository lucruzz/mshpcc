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
#include <mpi.h>

int main(int argc, char** argv){

    // inicializa MPI
    MPI_Init(&argc, &argv);

    // atribui um ID a cada tarefa MPI
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if (rank == 0){

        // Pega o número de processos MPI (tarefas) que foram iniciados
        int num_tasks;
        MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);

        // atribui valor a variável. Valor que será enviado às outras tarefas
        int value = 2048;
        // envia um inteiro para cada tarefa
        for (int i = 1; i < num_tasks; i++){
            MPI_Send(&value, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        
        std::cout << "Rank " << rank << " enviou: " 
                  << value << std::endl;
    }else{
        int recv;
        MPI_Recv(&recv, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        std::cout << "Rank " << rank << " recebeu: "
                  << recv << std::endl;
    }

    MPI_Finalize();
    
    return 0;
}