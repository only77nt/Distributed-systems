#include <stdio.h>
#include <mpi.h>

#define CONST_P 4
#define CONST_Q 4
#define CONST_M 4

int world_size = 1;
int world_rank = 0;

int send(int rank, char* what, int whither){
    int temp = MPI_Ssend(what, CONST_M, MPI_BYTE, whither, 0, MPI_COMM_WORLD);
    return temp;
}

int receive(int from, char* what, MPI_Status *status){
    int temp = MPI_Recv(what, CONST_M, MPI_BYTE, from, 0, MPI_COMM_WORLD, status);
    return temp;
}

void create_message(char *message, int count){
    for (int i=0; i<count; i++) message[i] = 49+i;
    return;
}

void print_info(char* message){
    int row = world_rank%CONST_P;
    int col = world_rank/CONST_Q;
    printf("Процесс - строка: %d, столбец: %d, получил сообщение: %s \n", row, col, message);
}

int main(int argc, char ** argv){
	//Инициализируем MPI
    int mpi_status = MPI_Init(&argc, &argv);
    if (mpi_status != MPI_SUCCESS){
    	return mpi_status;
    }
    
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    if (world_size != CONST_P*CONST_Q){
    	return -1;
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Status status;

	//Начинаем работать с транспьютерной матрицей (48 - ASCII код "0", 49 - "1")
    int row = world_rank%CONST_P;
    int col = world_rank/CONST_Q;
    char message[CONST_M+1];
    for(int i=0; i<CONST_M; i++)message[i] = 48;
    message[CONST_M] = 0;
    int count = CONST_M;
    switch (col)
    {
    case 0:
        switch (row)
        {
        case 0:
            create_message(message, count);
            send(world_rank, message, world_rank+1);
            send(world_rank, message, world_rank+CONST_P);
            break;
        case CONST_P-1:
            receive(world_rank-1, message, &status);
            send(world_rank, message, world_rank+CONST_P);
            break;
        default:
            receive(world_rank-1, message, &status);
            send(world_rank, message, world_rank+1);
            send(world_rank, message, world_rank+CONST_P);
            break;
        };
        break;
    case CONST_Q-1:
        receive(world_rank-CONST_P, message, &status);
        break;
    default:
        receive(world_rank-CONST_P, message, &status);
        send(world_rank, message, world_rank+CONST_P);
        break;
    }
    
    print_info(message);

    MPI_Finalize();
    return 0;
}
