/**
    Copyright (C) 2018 Technische Universitaet Muenchen
                       Chair of Computer Architecture and
                                Parallel Systems
    Author: Bengisu Elis

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
   USA */

#include <time.h>

#include "qmpi.h"
#include <assert.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "counter.h"
#include "interceptions.h"

void send_data_record(int count, MPI_Datatype datatype) {
    switch (datatype) {
    case MPI_UINT8_T:
    case MPI_INT8_T:
    case MPI_CHAR:
    case MPI_UNSIGNED_CHAR:
    case MPI_SIGNED_CHAR:
    case MPI_BYTE:
        I8_SEND_COUNT++;
        break;

    case MPI_UINT16_T:
    case MPI_INT16_T:
        I16_SEND_COUNT++;
        break;

    case MPI_UINT32_T:
    case MPI_INT32_T:
        I32_SEND_COUNT++;
        break;

    case MPI_UINT64_T:
    case MPI_INT64_T:
        I64_SEND_COUNT++;
        break;

    case MPI_INT:
    case MPI_UNSIGNED:
        INT_SEND_COUNT++;
        break;

    case MPI_FLOAT:
        FLOAT_SEND_COUNT++;
        break;

    case MPI_DOUBLE:
        DOUBLE_SEND_COUNT++;
        break;
    }
}
void recv_data_record(int count, MPI_Datatype datatype) {
    switch (datatype) {
    case MPI_UINT8_T:
    case MPI_INT8_T:
    case MPI_CHAR:
    case MPI_UNSIGNED_CHAR:
    case MPI_SIGNED_CHAR:
    case MPI_BYTE:
        I8_RECV_COUNT++;
        break;

    case MPI_UINT16_T:
    case MPI_INT16_T:
        I16_RECV_COUNT++;
        break;

    case MPI_UINT32_T:
    case MPI_INT32_T:
        I32_RECV_COUNT++;
        break;

    case MPI_UINT64_T:
    case MPI_INT64_T:
        I64_RECV_COUNT++;
        break;

    case MPI_INT:
    case MPI_UNSIGNED:
        INT_RECV_COUNT++;
        break;

    case MPI_FLOAT:
        FLOAT_RECV_COUNT++;
        break;

    case MPI_DOUBLE:
        DOUBLE_RECV_COUNT++;
        break;
    }
}
void reduce_data_record(int count, MPI_Datatype datatype) {
    switch (datatype) {
    case MPI_UINT8_T:
    case MPI_INT8_T:
    case MPI_CHAR:
    case MPI_UNSIGNED_CHAR:
    case MPI_SIGNED_CHAR:
    case MPI_BYTE:
        I8_RDC_COUNT++;
        break;

    case MPI_UINT16_T:
    case MPI_INT16_T:
        I16_RDC_COUNT++;
        break;

    case MPI_UINT32_T:
    case MPI_INT32_T:
        I32_RDC_COUNT++;
        break;

    case MPI_UINT64_T:
    case MPI_INT64_T:
        I64_RDC_COUNT++;
        break;

    case MPI_INT:
    case MPI_UNSIGNED:
        INT_RDC_COUNT++;
        break;

    case MPI_FLOAT:
        FLOAT_RDC_COUNT++;
        break;

    case MPI_DOUBLE:
        DOUBLE_RDC_COUNT++;
        break;
    }
}

/* ================== C Wrappers for MPI_Finalize ================== */

int E_Finalize(int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(138, &f_dl, (*VECTOR_GET(v, i)).table);

    int counters_copy[COUNTERS_LEN];
    for (size_t i = 0; i < COUNTERS_LEN; i++) {
        counters_copy[i] = counters[i];
    }

    int smmd_counters[COUNTERS_LEN];
    int red_scc = MPI_Reduce(counters_copy, smmd_counters, COUNTERS_LEN,
                             MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (red_scc != MPI_SUCCESS) {
        return red_scc;
    }

    int rank;
    int cr_scc = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (cr_scc != MPI_SUCCESS) {
        return red_scc;
    }

    char *fin_debug = getenv("FINALIZE_DEBUG_CONFIRM");
    if (rank == 0 && (fin_debug == NULL || strcmp(fin_debug, "1") == 0)) {
        for (size_t counter_id = 0; counter_id < COUNTERS_LEN; counter_id++) {
            int count = smmd_counters[counter_id];
            if (count != 0) {
                printf("%s: %u\n", COUNTER_NAMES[counter_id], (unsigned)count);
            }
        }
    }

    int ret = EXEC_FUNC(f_dl, i, 138, v);
    return ret;
}

/* ================== C Wrappers for MPI_Send ================== */

int E_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag,
           MPI_Comm comm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(258, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 258, v, buf, count, datatype, dest, tag, comm);

    if (ret == MPI_SUCCESS){
        send_data_record(count, datatype);
    }

    return ret;
}
/* ================== C Wrappers for MPI_Bsend ================== 18*/

int E_Bsend(const void *buf, int count, MPI_Datatype datatype, int dest,
            int tag, MPI_Comm comm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(18, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 18, v, buf, count, datatype, dest, tag, comm);

    if (ret == MPI_SUCCESS){
        send_data_record(count, datatype);
    }

    return ret;
}
/* ================== C Wrappers for MPI_Rsend ================== */

int E_Rsend(const void *ibuf, int count, MPI_Datatype datatype, int dest,
            int tag, MPI_Comm comm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(253, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 253, v, ibuf, count, datatype, dest, tag, comm);

    if (ret == MPI_SUCCESS){
        send_data_record(count, datatype);
    }

    return ret;
}
/* ================== C Wrappers for MPI_Ssend ================== */

int E_Ssend(const void *buf, int count, MPI_Datatype datatype, int dest,
            int tag, MPI_Comm comm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(262, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 262, v, buf, count, datatype, dest, tag, comm);

    if (ret == MPI_SUCCESS){
        send_data_record(count, datatype);
    }

    return ret;
}
/* ================== C Wrappers for MPI_Recv ================== */

int E_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag,
           MPI_Comm comm, MPI_Status *status, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(241, &f_dl, (*VECTOR_GET(v, i)).table);

    MPI_Status local_status;
    int ret = EXEC_FUNC(f_dl, i, 241, v, buf, count, datatype, source, tag,
                        comm, &local_status);
    if (status != MPI_STATUS_IGNORE) {
        *status = local_status;
    }

    if (ret == MPI_SUCCESS){
        ret = PMPI_Get_count(&local_status, datatype, &count);
        if (ret == MPI_SUCCESS) {
            recv_data_record(count, datatype);
        }
    }

    return ret;
}
/* ================== C Wrappers for MPI_Sendrecv ================== */

int E_Sendrecv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
               int dest, int sendtag, void *recvbuf, int recvcount,
               MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm,
               MPI_Status *status, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(260, &f_dl, (*VECTOR_GET(v, i)).table);

    MPI_Status local_status;
    int ret =
        EXEC_FUNC(f_dl, i, 260, v, sendbuf, sendcount, sendtype, dest, sendtag,
                  recvbuf, recvcount, recvtype, source, recvtag, comm, &local_status);
    if (status != MPI_STATUS_IGNORE) {
      *status = local_status;
    }

    if (ret == MPI_SUCCESS){
        ret = PMPI_Get_count(&local_status, recvtype, &recvcount);
        if (ret == MPI_SUCCESS) {
            recv_data_record(recvcount, recvtype);
        }
    }

    return ret;
}
/* ================== C Wrappers for MPI_Bcast ================== 17*/

int E_Bcast(void *buffer, int count, MPI_Datatype datatype, int root,
            MPI_Comm comm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(17, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 17, v, buffer, count, datatype, root, comm);

    if (ret == MPI_SUCCESS){
        int curr_rank;
        ret = PMPI_Comm_rank(comm, &curr_rank);
        if (ret == MPI_SUCCESS) {
            if (curr_rank == root) {
                int comm_size;
                ret = PMPI_Comm_size(comm, &comm_size);
                if (ret == MPI_SUCCESS) {
                    send_data_record(count * (comm_size - 1), datatype);
                }
            } else {
                recv_data_record(count, datatype);
            }
        }
    }

    return ret;
}
/* ================== C Wrappers for MPI_Allreduce ================== 9*/

int E_Allreduce(const void *sendbuf, void *recvbuf, int count,
                MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, int i,
                vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(9, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 9, v, sendbuf, recvbuf, count, datatype, op, comm);

    if (ret == MPI_SUCCESS){
        reduce_data_record(count, datatype);
    }

    return ret;
}

// --- Empty wrappers ---

/* ================== C Wrappers for MPI_Init ================== */

int E_Init(int *argc, char ***argv, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(200, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 200, v, argc, argv);
    return ret;
}
/* ================== C Wrappers for MPI_Abort ================== 0*/

int E_Abort(MPI_Comm comm, int errorcode, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(0, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 0, v, comm, errorcode);
    return ret;
}
/* ================== C Wrappers for MPI_Accumulate ================== 1*/

int E_Accumulate(const void *origin_addr, int origin_count,
                 MPI_Datatype origin_datatype, int target_rank,
                 MPI_Aint target_disp, int target_count,
                 MPI_Datatype target_datatype, MPI_Op op, MPI_Win win, int i,
                 vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(1, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 1, v, origin_addr, origin_count,
                        origin_datatype, target_rank, target_disp, target_count,
                        target_datatype, op, win);
    return ret;
}
/* ================== C Wrappers for MPI_Add_error_class ================== 2*/

int E_Add_error_class(int *errorclass, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(2, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 2, v, errorclass);
    return ret;
}
/* ================== C Wrappers for MPI_Add_error_code ==================3 */

int E_Add_error_code(int errorclass, int *errorcode, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(3, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 3, v, errorclass, errorcode);
    return ret;
}
/* ================== C Wrappers for MPI_Add_error_string ================== 4*/

int E_Add_error_string(int errorcode, const char *string, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(4, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 4, v, errorcode, string);
    return ret;
}
/* ================== C Wrappers for MPI_Address ================== 5*/

int E_Address(void *location, MPI_Aint *address, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(5, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 5, v, location, address);
    return ret;
}
/* ================== C Wrappers for MPI_Allgather ================== 6*/

int E_Allgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                void *recvbuf, int recvcount, MPI_Datatype recvtype,
                MPI_Comm comm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(6, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 6, v, sendbuf, sendcount, sendtype, recvbuf,
                        recvcount, recvtype, comm);
    return ret;
}
/* ================== C Wrappers for MPI_Allgatherv ================== 7*/

int E_Allgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                 void *recvbuf, const int recvcounts[], const int displs[],
                 MPI_Datatype recvtype, MPI_Comm comm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(7, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 7, v, sendbuf, sendcount, sendtype, recvbuf,
                        recvcounts, displs, recvtype, comm);
    return ret;
}
/* ================== C Wrappers for MPI_Alloc_mem ================== 8*/

int E_Alloc_mem(MPI_Aint size, MPI_Info info, void *baseptr, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(8, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 8, v, size, info, baseptr);
    return ret;
}
/* ================== C Wrappers for MPI_Alltoall ================== 10*/

int E_Alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
               void *recvbuf, int recvcount, MPI_Datatype recvtype,
               MPI_Comm comm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(10, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 10, v, sendbuf, sendcount, sendtype, recvbuf,
                        recvcount, recvtype, comm);
    return ret;
}
/* ================== C Wrappers for MPI_Alltoallv ================== 11*/

int E_Alltoallv(const void *sendbuf, const int sendcounts[],
                const int sdispls[], MPI_Datatype sendtype, void *recvbuf,
                const int recvcounts[], const int rdispls[],
                MPI_Datatype recvtype, MPI_Comm comm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(11, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 11, v, sendbuf, sendcounts, sdispls, sendtype,
                        recvbuf, recvcounts, rdispls, recvtype, comm);
    return ret;
}
/* ================== C Wrappers for MPI_Alltoallw ================== 12*/

int E_Alltoallw(const void *sendbuf, const int sendcounts[],
                const int sdispls[], const MPI_Datatype sendtypes[],
                void *recvbuf, const int recvcounts[], const int rdispls[],
                const MPI_Datatype recvtypes[], MPI_Comm comm, int i,
                vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(12, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 12, v, sendbuf, sendcounts, sdispls, sendtypes,
                        recvbuf, recvcounts, rdispls, recvtypes, comm);
    return ret;
}
/* ================== C Wrappers for MPI_Attr_delete ================== 13*/

int E_Attr_delete(MPI_Comm comm, int keyval, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(13, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 13, v, comm, keyval);
    return ret;
}
/* ================== C Wrappers for MPI_Attr_get ================== 14*/

int E_Attr_get(MPI_Comm comm, int keyval, void *attribute_val, int *flag, int i,
               vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(14, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 14, v, comm, keyval, attribute_val, flag);
    return ret;
}
/* ================== C Wrappers for MPI_Attr_put ================== 15*/

int E_Attr_put(MPI_Comm comm, int keyval, void *attribute_val, int i,
               vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(15, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 15, v, comm, keyval, attribute_val);
    return ret;
}
/* ================== C Wrappers for MPI_Barrier ==================16 */

int E_Barrier(MPI_Comm comm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(16, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 16, v, comm);
    return ret;
}
/* ================== C Wrappers for MPI_Bsend_init ==================19 */

int E_Bsend_init(const void *buf, int count, MPI_Datatype datatype, int dest,
                 int tag, MPI_Comm comm, MPI_Request *request, int i,
                 vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(19, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 19, v, buf, count, datatype, dest, tag, comm,
                        request);
    return ret;
}
/* ================== C Wrappers for MPI_Buffer_attach ==================20 */

int E_Buffer_attach(void *buffer, int size, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(20, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 20, v, buffer, size);
    return ret;
}
/* ================== C Wrappers for MPI_Buffer_detach ================== 21*/

int E_Buffer_detach(void *buffer, int *size, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(21, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 21, v, buffer, size);
    return ret;
}
/* ================== C Wrappers for MPI_Cancel ================== 22*/

int E_Cancel(MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(22, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 22, v, request);
    return ret;
}
/* ================== C Wrappers for MPI_Cart_coords ================== 23*/

int E_Cart_coords(MPI_Comm comm, int rank, int maxdims, int coords[], int i,
                  vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(23, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 23, v, comm, rank, maxdims, coords);
    return ret;
}
/* ================== C Wrappers for MPI_Cart_create ================== 24*/

int E_Cart_create(MPI_Comm old_comm, int ndims, const int dims[],
                  const int periods[], int reorder, MPI_Comm *comm_cart, int i,
                  vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(24, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 24, v, old_comm, ndims, dims, periods, reorder,
                        comm_cart);
    return ret;
}
/* ================== C Wrappers for MPI_Cart_get ==================25 */

int E_Cart_get(MPI_Comm comm, int maxdims, int dims[], int periods[],
               int coords[], int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(25, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 25, v, comm, maxdims, dims, periods, coords);
    return ret;
}
/* ================== C Wrappers for MPI_Cart_map ================== 26*/

int E_Cart_map(MPI_Comm comm, int ndims, const int dims[], const int periods[],
               int *newrank, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(26, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 26, v, comm, ndims, dims, periods, newrank);
    return ret;
}
/* ================== C Wrappers for MPI_Cart_rank ================== 27*/

int E_Cart_rank(MPI_Comm comm, const int coords[], int *rank, int i,
                vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(27, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 27, v, comm, coords, rank);
    return ret;
}
/* ================== C Wrappers for MPI_Cart_shift ================== 28*/

int E_Cart_shift(MPI_Comm comm, int direction, int disp, int *rank_source,
                 int *rank_dest, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(28, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 28, v, comm, direction, disp, rank_source,
                        rank_dest);
    return ret;
}
/* ================== C Wrappers for MPI_Cart_sub ================== 29*/

int E_Cart_sub(MPI_Comm comm, const int remain_dims[], MPI_Comm *new_comm,
               int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(29, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 29, v, comm, remain_dims, new_comm);
    return ret;
}

/* ================== C Wrappers for MPI_Cartdim_get ================== 30*/

int E_Cartdim_get(MPI_Comm comm, int *ndims, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(30, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 30, v, comm, ndims);
    return ret;
}
/* ================== C Wrappers for MPI_Close_port ================== 31*/

int E_Close_port(const char *port_name, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(31, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 31, v, port_name);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_accept ================== 32*/

int E_Comm_accept(const char *port_name, MPI_Info info, int root, MPI_Comm comm,
                  MPI_Comm *newcomm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(32, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 32, v, port_name, info, root, comm, newcomm);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_call_errhandler ==================
 * 33*/

int E_Comm_call_errhandler(MPI_Comm comm, int errorcode, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(33, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 33, v, comm, errorcode);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_compare ================== 34*/

int E_Comm_compare(MPI_Comm comm1, MPI_Comm comm2, int *result, int i,
                   vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(34, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 34, v, comm1, comm2, result);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_connect ================== 35*/

int E_Comm_connect(const char *port_name, MPI_Info info, int root,
                   MPI_Comm comm, MPI_Comm *newcomm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(35, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 35, v, port_name, info, root, comm, newcomm);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_create ================== 36*/

int E_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm, int i,
                  vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(36, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 36, v, comm, group, newcomm);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_create_errhandler
 * ================== 37*/

int E_Comm_create_errhandler(MPI_Comm_errhandler_function *function,
                             MPI_Errhandler *errhandler, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(37, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 37, v, function, errhandler);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_create_group ==================
 * 38*/

int E_Comm_create_group(MPI_Comm comm, MPI_Group group, int tag,
                        MPI_Comm *newcomm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(38, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 38, v, comm, group, tag, newcomm);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_create_keyval
 * ==================39*/

int E_Comm_create_keyval(MPI_Comm_copy_attr_function *comm_copy_attr_fn,
                         MPI_Comm_delete_attr_function *comm_delete_attr_fn,
                         int *comm_keyval, void *extra_state, int i,
                         vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(39, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 39, v, comm_copy_attr_fn, comm_delete_attr_fn,
                        comm_keyval, extra_state);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_delete_attr ==================
 * 40*/

int E_Comm_delete_attr(MPI_Comm comm, int comm_keyval, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(40, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 40, v, comm, comm_keyval);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_disconnect ================== 41*/

int E_Comm_disconnect(MPI_Comm *comm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(41, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 41, v, comm);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_dup ==================42*/

int E_Comm_dup(MPI_Comm comm, MPI_Comm *newcomm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(42, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 42, v, comm, newcomm);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_dup_with_info ==================
 * 43*/

int E_Comm_dup_with_info(MPI_Comm comm, MPI_Info info, MPI_Comm *newcomm, int i,
                         vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(43, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 43, v, comm, info, newcomm);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_free ==================44 */

int E_Comm_free(MPI_Comm *comm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(44, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 44, v, comm);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_free_keyval ==================
 * 45*/

int E_Comm_free_keyval(int *comm_keyval, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(45, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 45, v, comm_keyval);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_get_attr ================== 46*/

int E_Comm_get_attr(MPI_Comm comm, int comm_keyval, void *attribute_val,
                    int *flag, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(46, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 46, v, comm, comm_keyval, attribute_val, flag);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_get_errhandler ==================
 * 47*/

int E_Comm_get_errhandler(MPI_Comm comm, MPI_Errhandler *erhandler, int i,
                          vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(47, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 47, v, comm, erhandler);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_get_info ================== 48*/

int E_Comm_get_info(MPI_Comm comm, MPI_Info *info_used, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(48, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 48, v, comm, info_used);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_get_name ==================49 */

int E_Comm_get_name(MPI_Comm comm, char *comm_name, int *resultlen, int i,
                    vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(49, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 49, v, comm, comm_name, resultlen);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_get_parent ================== 50*/

int E_Comm_get_parent(MPI_Comm *parent, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(50, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 50, v, parent);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_group ==================51 */

int E_Comm_group(MPI_Comm comm, MPI_Group *group, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(51, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 51, v, comm, group);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_idup ================== 52*/

int E_Comm_idup(MPI_Comm comm, MPI_Comm *newcomm, MPI_Request *request, int i,
                vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(52, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 52, v, comm, newcomm, request);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_join ================== 53*/

int E_Comm_join(int fd, MPI_Comm *intercomm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(53, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 53, v, fd, intercomm);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_rank ================== 54*/

int E_Comm_rank(MPI_Comm comm, int *rank, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(54, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 54, v, comm, rank);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_remote_group ==================
 * 55*/

int E_Comm_remote_group(MPI_Comm comm, MPI_Group *group, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(55, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 55, v, comm, group);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_remote_size ==================
 * 56*/

int E_Comm_remote_size(MPI_Comm comm, int *size, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(56, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 56, v, comm, size);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_set_attr ================== 57*/

int E_Comm_set_attr(MPI_Comm comm, int comm_keyval, void *attribute_val, int i,
                    vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(57, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 57, v, comm, comm_keyval, attribute_val);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_set_errhandler
 * ==================58 */

int E_Comm_set_errhandler(MPI_Comm comm, MPI_Errhandler errhandler, int i,
                          vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(58, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 58, v, comm, errhandler);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_set_info ================== 59*/

int E_Comm_set_info(MPI_Comm comm, MPI_Info info, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(59, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 59, v, comm, info);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_set_name ================== 60*/

int E_Comm_set_name(MPI_Comm comm, const char *comm_name, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(60, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 60, v, comm, comm_name);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_size ================== 61*/

int E_Comm_size(MPI_Comm comm, int *size, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(61, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 61, v, comm, size);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_split ================== */

int E_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *newcomm, int i,
                 vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(62, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 62, v, comm, color, key, newcomm);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_split_type ================== */

int E_Comm_split_type(MPI_Comm comm, int split_type, int key, MPI_Info info,
                      MPI_Comm *newcomm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(63, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 63, v, comm, split_type, key, info, newcomm);
    return ret;
}
/* ================== C Wrappers for MPI_Comm_test_inter ================== */

int E_Comm_test_inter(MPI_Comm comm, int *flag, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(64, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 64, v, comm, flag);
    return ret;
}
/* ================== C Wrappers for MPI_Compare_and_swap ================== */

int E_Compare_and_swap(const void *origin_addr, const void *compare_addr,
                       void *result_addr, MPI_Datatype datatype,
                       int target_rank, MPI_Aint target_disp, MPI_Win win,
                       int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(65, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 65, v, origin_addr, compare_addr, result_addr,
                        datatype, target_rank, target_disp, win);
    return ret;
}
/* ================== C Wrappers for MPI_Dims_create ================== */

int E_Dims_create(int nnodes, int ndims, int dims[], int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(66, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 66, v, nnodes, ndims, dims);
    return ret;
}
/* ================== C Wrappers for MPI_Dist_graph_create ================== */

int E_Dist_graph_create(MPI_Comm comm_old, int n, const int nodes[],
                        const int degrees[], const int targets[],
                        const int weights[], MPI_Info info, int reorder,
                        MPI_Comm *newcomm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(67, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 67, v, comm_old, n, nodes, degrees, targets,
                        weights, info, reorder, newcomm);
    return ret;
}
/* ================== C Wrappers for MPI_Dist_graph_create_adjacent
 * ================== */

int E_Dist_graph_create_adjacent(MPI_Comm comm_old, int indegree,
                                 const int sources[], const int sourceweights[],
                                 int outdegree, const int destinations[],
                                 const int destweights[], MPI_Info info,
                                 int reorder, MPI_Comm *comm_dist_graph, int i,
                                 vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(68, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 68, v, comm_old, indegree, sources,
                        sourceweights, outdegree, destinations, destweights,
                        info, reorder, comm_dist_graph);
    return ret;
}
/* ================== C Wrappers for MPI_Dist_graph_neighbors ==================
 */

int E_Dist_graph_neighbors(MPI_Comm comm, int maxindegree, int sources[],
                           int sourceweights[], int maxoutdegree,
                           int destinations[], int destweights[], int i,
                           vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(69, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 69, v, comm, maxindegree, sources,
                        sourceweights, maxoutdegree, destinations, destweights);
    return ret;
}
/* ================== C Wrappers for MPI_Dist_graph_neighbors_count
 * ================== */

int E_Dist_graph_neighbors_count(MPI_Comm comm, int *inneighbors,
                                 int *outneighbors, int *weighted, int i,
                                 vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(70, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 70, v, comm, inneighbors, outneighbors, weighted);
    return ret;
}
/* ================== C Wrappers for MPI_Errhandler_create ================== */

int E_Errhandler_create(MPI_Handler_function *function,
                        MPI_Errhandler *errhandler, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(71, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 71, v, function, errhandler);
    return ret;
}
/* ================== C Wrappers for MPI_Errhandler_free ================== */

int E_Errhandler_free(MPI_Errhandler *errhandler, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(72, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 72, v, errhandler);
    return ret;
}
/* ================== C Wrappers for MPI_Errhandler_get ================== */

int E_Errhandler_get(MPI_Comm comm, MPI_Errhandler *errhandler, int i,
                     vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(73, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 73, v, comm, errhandler);
    return ret;
}
/* ================== C Wrappers for MPI_Errhandler_set ================== */

int E_Errhandler_set(MPI_Comm comm, MPI_Errhandler errhandler, int i,
                     vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(74, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 74, v, comm, errhandler);
    return ret;
}
/* ================== C Wrappers for MPI_Error_class ================== */

int E_Error_class(int errorcode, int *errorclass, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(75, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 75, v, errorcode, errorclass);
    return ret;
}
/* ================== C Wrappers for MPI_Error_string ================== */

int E_Error_string(int errorcode, char *string, int *resultlen, int i,
                   vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(76, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 76, v, errorcode, string, resultlen);
    return ret;
}
/* ================== C Wrappers for MPI_Exscan ================== */

int E_Exscan(const void *sendbuf, void *recvbuf, int count,
             MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, int i,
             vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(77, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 77, v, sendbuf, recvbuf, count, datatype, op, comm);
    return ret;
}
/* ================== C Wrappers for MPI_Fetch_and_op ================== */

int E_Fetch_and_op(const void *origin_addr, void *result_addr,
                   MPI_Datatype datatype, int target_rank, MPI_Aint target_disp,
                   MPI_Op op, MPI_Win win, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(78, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 78, v, origin_addr, result_addr, datatype,
                        target_rank, target_disp, op, win);
    return ret;
}
/* ================== C Wrappers for MPI_File_call_errhandler ==================
 */

int E_File_call_errhandler(MPI_File fh, int errorcode, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(79, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 79, v, fh, errorcode);
    return ret;
}
/* ================== C Wrappers for MPI_File_close ================== */

int E_File_close(MPI_File *fh, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(80, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 80, v, fh);
    return ret;
}
/* ================== C Wrappers for MPI_File_create_errhandler
 * ================== */

int E_File_create_errhandler(MPI_File_errhandler_function *function,
                             MPI_Errhandler *errhandler, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(81, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 81, v, function, errhandler);
    return ret;
}
/* ================== C Wrappers for MPI_File_delete ================== */

int E_File_delete(const char *filename, MPI_Info info, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(82, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 82, v, filename, info);
    return ret;
}
/* ================== C Wrappers for MPI_File_get_amode ================== */

int E_File_get_amode(MPI_File fh, int *amode, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(83, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 83, v, fh, amode);
    return ret;
}
/* ================== C Wrappers for MPI_File_get_atomicity ==================
 */

int E_File_get_atomicity(MPI_File fh, int *flag, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(84, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 84, v, fh, flag);
    return ret;
}
/* ================== C Wrappers for MPI_File_get_byte_offset ==================
 */

int E_File_get_byte_offset(MPI_File fh, MPI_Offset offset, MPI_Offset *disp,
                           int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(85, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 85, v, fh, offset, disp);
    return ret;
}
/* ================== C Wrappers for MPI_File_get_errhandler ==================
 */

int E_File_get_errhandler(MPI_File file, MPI_Errhandler *errhandler, int i,
                          vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(86, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 86, v, file, errhandler);
    return ret;
}
/* ================== C Wrappers for MPI_File_get_group ================== */

int E_File_get_group(MPI_File fh, MPI_Group *group, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(87, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 87, v, fh, group);
    return ret;
}
/* ================== C Wrappers for MPI_File_get_info ================== */

int E_File_get_info(MPI_File fh, MPI_Info *info_used, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(88, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 88, v, fh, info_used);
    return ret;
}
/* ================== C Wrappers for MPI_File_get_position ================== */

int E_File_get_position(MPI_File fh, MPI_Offset *offset, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(89, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 89, v, fh, offset);
    return ret;
}
/* ================== C Wrappers for MPI_File_get_position_shared
 * ================== */

int E_File_get_position_shared(MPI_File fh, MPI_Offset *offset, int i,
                               vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(90, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 90, v, fh, offset);
    return ret;
}
/* ================== C Wrappers for MPI_File_get_size ================== */

int E_File_get_size(MPI_File fh, MPI_Offset *size, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(91, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 91, v, fh, size);
    return ret;
}
/* ================== C Wrappers for MPI_File_get_type_extent ==================
 */

int E_File_get_type_extent(MPI_File fh, MPI_Datatype datatype, MPI_Aint *extent,
                           int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(92, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 92, v, fh, datatype, extent);
    return ret;
}
/* ================== C Wrappers for MPI_File_get_view ================== */

int E_File_get_view(MPI_File fh, MPI_Offset *disp, MPI_Datatype *etype,
                    MPI_Datatype *filetype, char *datarep, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(93, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 93, v, fh, disp, etype, filetype, datarep);
    return ret;
}
/* ================== C Wrappers for MPI_File_iread ================== */

int E_File_iread(MPI_File fh, void *buf, int count, MPI_Datatype datatype,
                 MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(94, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 94, v, fh, buf, count, datatype, request);
    return ret;
}
/* ================== C Wrappers for MPI_File_iread_all ================== */

int E_File_iread_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype,
                     MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(95, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 95, v, fh, buf, count, datatype, request);
    return ret;
}

/* ================== C Wrappers for MPI_File_iread_at ================== */

int E_File_iread_at(MPI_File fh, MPI_Offset offset, void *buf, int count,
                    MPI_Datatype datatype, MPI_Request *request, int i,
                    vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(96, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 96, v, fh, offset, buf, count, datatype, request);
    return ret;
}
/* ================== C Wrappers for MPI_File_iread_at_all ================== */

int E_File_iread_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count,
                        MPI_Datatype datatype, MPI_Request *request, int i,
                        vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(97, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 97, v, fh, offset, buf, count, datatype, request);
    return ret;
}
/* ================== C Wrappers for MPI_File_iread_shared ================== */

int E_File_iread_shared(MPI_File fh, void *buf, int count,
                        MPI_Datatype datatype, MPI_Request *request, int i,
                        vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(98, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 98, v, fh, buf, count, datatype, request);
    return ret;
}
/* ================== C Wrappers for MPI_File_iwrite ================== */

int E_File_iwrite(MPI_File fh, const void *buf, int count,
                  MPI_Datatype datatype, MPI_Request *request, int i,
                  vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(99, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 99, v, fh, buf, count, datatype, request);
    return ret;
}
/* ================== C Wrappers for MPI_File_iwrite_all ================== */

int E_File_iwrite_all(MPI_File fh, const void *buf, int count,
                      MPI_Datatype datatype, MPI_Request *request, int i,
                      vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(100, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 100, v, fh, buf, count, datatype, request);
    return ret;
}
/* ================== C Wrappers for MPI_File_iwrite_at ================== */

int E_File_iwrite_at(MPI_File fh, MPI_Offset offset, const void *buf, int count,
                     MPI_Datatype datatype, MPI_Request *request, int i,
                     vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(101, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 101, v, fh, offset, buf, count, datatype, request);
    return ret;
}
/* ================== C Wrappers for MPI_File_iwrite_at_all ==================
 */

int E_File_iwrite_at_all(MPI_File fh, MPI_Offset offset, const void *buf,
                         int count, MPI_Datatype datatype, MPI_Request *request,
                         int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(102, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 102, v, fh, offset, buf, count, datatype, request);
    return ret;
}
/* ================== C Wrappers for MPI_File_iwrite_shared ==================
 */

int E_File_iwrite_shared(MPI_File fh, const void *buf, int count,
                         MPI_Datatype datatype, MPI_Request *request, int i,
                         vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(103, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 103, v, fh, buf, count, datatype, request);
    return ret;
}
/* ================== C Wrappers for MPI_File_open ================== */

int E_File_open(MPI_Comm comm, const char *filename, int amode, MPI_Info info,
                MPI_File *fh, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(104, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 104, v, comm, filename, amode, info, fh);
    return ret;
}
/* ================== C Wrappers for MPI_File_preallocate ================== */

int E_File_preallocate(MPI_File fh, MPI_Offset size, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(105, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 105, v, fh, size);
    return ret;
}
/* ================== C Wrappers for MPI_File_read ================== */

int E_File_read(MPI_File fh, void *buf, int count, MPI_Datatype datatype,
                MPI_Status *status, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(106, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 106, v, fh, buf, count, datatype, status);
    return ret;
}
/* ================== C Wrappers for MPI_File_read_all ================== */

int E_File_read_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype,
                    MPI_Status *status, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(107, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 107, v, fh, buf, count, datatype, status);
    return ret;
}
/* ================== C Wrappers for MPI_File_read_all_begin ==================
 */

int E_File_read_all_begin(MPI_File fh, void *buf, int count,
                          MPI_Datatype datatype, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(108, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 108, v, fh, buf, count, datatype);
    return ret;
}
/* ================== C Wrappers for MPI_File_read_all_end ================== */

int E_File_read_all_end(MPI_File fh, void *buf, MPI_Status *status, int i,
                        vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(109, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 109, v, fh, buf, status);
    return ret;
}
/* ================== C Wrappers for MPI_File_read_at ================== */

int E_File_read_at(MPI_File fh, MPI_Offset offset, void *buf, int count,
                   MPI_Datatype datatype, MPI_Status *status, int i,
                   vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(110, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 110, v, fh, offset, buf, count, datatype, status);
    return ret;
}
/* ================== C Wrappers for MPI_File_read_at_all ================== */

int E_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count,
                       MPI_Datatype datatype, MPI_Status *status, int i,
                       vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(111, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 111, v, fh, offset, buf, count, datatype, status);
    return ret;
}
/* ================== C Wrappers for MPI_File_read_at_all_begin
 * ================== */

int E_File_read_at_all_begin(MPI_File fh, MPI_Offset offset, void *buf,
                             int count, MPI_Datatype datatype, int i,
                             vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(112, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 112, v, fh, offset, buf, count, datatype);
    return ret;
}
/* ================== C Wrappers for MPI_File_read_at_all_end ==================
 */

int E_File_read_at_all_end(MPI_File fh, void *buf, MPI_Status *status, int i,
                           vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(113, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 113, v, fh, buf, status);
    return ret;
}
/* ================== C Wrappers for MPI_File_read_ordered ================== */

int E_File_read_ordered(MPI_File fh, void *buf, int count,
                        MPI_Datatype datatype, MPI_Status *status, int i,
                        vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(114, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 114, v, fh, buf, count, datatype, status);
    return ret;
}
/* ================== C Wrappers for MPI_File_read_ordered_begin
 * ================== */

int E_File_read_ordered_begin(MPI_File fh, void *buf, int count,
                              MPI_Datatype datatype, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(115, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 115, v, fh, buf, count, datatype);
    return ret;
}
/* ================== C Wrappers for MPI_File_read_ordered_end
 * ================== */

int E_File_read_ordered_end(MPI_File fh, void *buf, MPI_Status *status, int i,
                            vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(116, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 116, v, fh, buf, status);
    return ret;
}
/* ================== C Wrappers for MPI_File_read_shared ================== */

int E_File_read_shared(MPI_File fh, void *buf, int count, MPI_Datatype datatype,
                       MPI_Status *status, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(117, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 117, v, fh, buf, count, datatype, status);
    return ret;
}
/* ================== C Wrappers for MPI_File_seek ================== */

int E_File_seek(MPI_File fh, MPI_Offset offset, int whence, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(118, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 118, v, fh, offset, whence);
    return ret;
}
/* ================== C Wrappers for MPI_File_seek_shared ================== */

int E_File_seek_shared(MPI_File fh, MPI_Offset offset, int whence, int i,
                       vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(119, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 119, v, fh, offset, whence);
    return ret;
}
/* ================== C Wrappers for MPI_File_set_atomicity ==================
 */

int E_File_set_atomicity(MPI_File fh, int flag, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(120, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 120, v, fh, flag);
    return ret;
}
/* ================== C Wrappers for MPI_File_set_errhandler ==================
 */

int E_File_set_errhandler(MPI_File file, MPI_Errhandler errhandler, int i,
                          vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(121, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 121, v, file, errhandler);
    return ret;
}
/* ================== C Wrappers for MPI_File_set_info ================== */

int E_File_set_info(MPI_File fh, MPI_Info info, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(122, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 122, v, fh, info);
    return ret;
}
/* ================== C Wrappers for MPI_File_set_size ================== */

int E_File_set_size(MPI_File fh, MPI_Offset size, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(123, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 123, v, fh, size);
    return ret;
}
/* ================== C Wrappers for MPI_File_set_view ================== */

int E_File_set_view(MPI_File fh, MPI_Offset disp, MPI_Datatype etype,
                    MPI_Datatype filetype, const char *datarep, MPI_Info info,
                    int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(124, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 124, v, fh, disp, etype, filetype, datarep, info);
    return ret;
}
/* ================== C Wrappers for MPI_File_sync ================== */

int E_File_sync(MPI_File fh, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(125, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 125, v, fh);
    return ret;
}
/* ================== C Wrappers for MPI_File_write ================== */

int E_File_write(MPI_File fh, const void *buf, int count, MPI_Datatype datatype,
                 MPI_Status *status, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(126, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 126, v, fh, buf, count, datatype, status);
    return ret;
}
/* ================== C Wrappers for MPI_File_write_all ================== */

int E_File_write_all(MPI_File fh, const void *buf, int count,
                     MPI_Datatype datatype, MPI_Status *status, int i,
                     vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(127, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 127, v, fh, buf, count, datatype, status);
    return ret;
}
/* ================== C Wrappers for MPI_File_write_all_begin ==================
 */

int E_File_write_all_begin(MPI_File fh, const void *buf, int count,
                           MPI_Datatype datatype, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(128, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 128, v, fh, buf, count, datatype);
    return ret;
}
/* ================== C Wrappers for MPI_File_write_all_end ==================
 */

int E_File_write_all_end(MPI_File fh, const void *buf, MPI_Status *status,
                         int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(129, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 129, v, fh, buf, status);
    return ret;
}
/* ================== C Wrappers for MPI_File_write_at ================== */

int E_File_write_at(MPI_File fh, MPI_Offset offset, const void *buf, int count,
                    MPI_Datatype datatype, MPI_Status *status, int i,
                    vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(130, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 130, v, fh, offset, buf, count, datatype, status);
    return ret;
}
/* ================== C Wrappers for MPI_File_write_at_all ================== */

int E_File_write_at_all(MPI_File fh, MPI_Offset offset, const void *buf,
                        int count, MPI_Datatype datatype, MPI_Status *status,
                        int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(131, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 131, v, fh, offset, buf, count, datatype, status);
    return ret;
}
/* ================== C Wrappers for MPI_File_write_at_all_begin
 * ================== */

int E_File_write_at_all_begin(MPI_File fh, MPI_Offset offset, const void *buf,
                              int count, MPI_Datatype datatype, int i,
                              vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(132, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 132, v, fh, offset, buf, count, datatype);
    return ret;
}
/* ================== C Wrappers for MPI_File_write_at_all_end
 * ================== */

int E_File_write_at_all_end(MPI_File fh, const void *buf, MPI_Status *status,
                            int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(133, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 133, v, fh, buf, status);
    return ret;
}
/* ================== C Wrappers for MPI_File_write_ordered ==================
 */

int E_File_write_ordered(MPI_File fh, const void *buf, int count,
                         MPI_Datatype datatype, MPI_Status *status, int i,
                         vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(134, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 134, v, fh, buf, count, datatype, status);
    return ret;
}
/* ================== C Wrappers for MPI_File_write_ordered_begin
 * ================== */

int E_File_write_ordered_begin(MPI_File fh, const void *buf, int count,
                               MPI_Datatype datatype, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(135, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 135, v, fh, buf, count, datatype);
    return ret;
}
/* ================== C Wrappers for MPI_File_write_ordered_end
 * ================== */

int E_File_write_ordered_end(MPI_File fh, const void *buf, MPI_Status *status,
                             int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(136, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 136, v, fh, buf, status);
    return ret;
}
/* ================== C Wrappers for MPI_File_write_shared ================== */

int E_File_write_shared(MPI_File fh, const void *buf, int count,
                        MPI_Datatype datatype, MPI_Status *status, int i,
                        vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(137, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 137, v, fh, buf, count, datatype, status);
    return ret;
}
/* ================== C Wrappers for MPI_Finalized ================== */

int E_Finalized(int *flag, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(139, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 139, v, flag);
    return ret;
}
/* ================== C Wrappers for MPI_Free_mem ================== */

int E_Free_mem(void *base, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(140, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 140, v, base);
    return ret;
}
/* ================== C Wrappers for MPI_Gather ================== */

int E_Gather(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
             void *recvbuf, int recvcount, MPI_Datatype recvtype, int root,
             MPI_Comm comm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(141, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 141, v, sendbuf, sendcount, sendtype, recvbuf,
                        recvcount, recvtype, root, comm);
    return ret;
}
/* ================== C Wrappers for MPI_Gatherv ================== */

int E_Gatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
              void *recvbuf, const int recvcounts[], const int displs[],
              MPI_Datatype recvtype, int root, MPI_Comm comm, int i,
              vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(142, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 142, v, sendbuf, sendcount, sendtype, recvbuf,
                        recvcounts, displs, recvtype, root, comm);
    return ret;
}
/* ================== C Wrappers for MPI_Get ================== */

int E_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
          int target_rank, MPI_Aint target_disp, int target_count,
          MPI_Datatype target_datatype, MPI_Win win, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(143, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 143, v, origin_addr, origin_count, origin_datatype,
                  target_rank, target_disp, target_count, target_datatype, win);
    return ret;
}
/* ================== C Wrappers for MPI_Get_accumulate ================== */

int E_Get_accumulate(const void *origin_addr, int origin_count,
                     MPI_Datatype origin_datatype, void *result_addr,
                     int result_count, MPI_Datatype result_datatype,
                     int target_rank, MPI_Aint target_disp, int target_count,
                     MPI_Datatype target_datatype, MPI_Op op, MPI_Win win,
                     int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(144, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 144, v, origin_addr, origin_count, origin_datatype,
                  result_addr, result_count, result_datatype, target_rank,
                  target_disp, target_count, target_datatype, op, win);
    return ret;
}
/* ================== C Wrappers for MPI_Get_address ================== */

int E_Get_address(const void *location, MPI_Aint *address, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(145, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 145, v, location, address);
    return ret;
}
/* ================== C Wrappers for MPI_Get_count ================== */

int E_Get_count(const MPI_Status *status, MPI_Datatype datatype, int *count,
                int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(146, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 146, v, status, datatype, count);
    return ret;
}
/* ================== C Wrappers for MPI_Get_elements ================== */

int E_Get_elements(const MPI_Status *status, MPI_Datatype datatype, int *count,
                   int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(147, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 147, v, status, datatype, count);
    return ret;
}
/* ================== C Wrappers for MPI_Get_elements_x ================== */

int E_Get_elements_x(const MPI_Status *status, MPI_Datatype datatype,
                     MPI_Count *count, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(148, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 148, v, status, datatype, count);
    return ret;
}
/* ================== C Wrappers for MPI_Get_library_version ==================
 */

int E_Get_library_version(char *version, int *resultlen, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(149, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 149, v, version, resultlen);
    return ret;
}
/* ================== C Wrappers for MPI_Get_processor_name ==================
 */

int E_Get_processor_name(char *name, int *resultlen, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(150, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 150, v, name, resultlen);
    return ret;
}
/* ================== C Wrappers for MPI_Get_version ================== */

int E_Get_version(int *version, int *subversion, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(151, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 151, v, version, subversion);
    return ret;
}
/* ================== C Wrappers for MPI_Graph_create ================== */

int E_Graph_create(MPI_Comm comm_old, int nnodes, const int index[],
                   const int edges[], int reorder, MPI_Comm *comm_graph, int i,
                   vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(152, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 152, v, comm_old, nnodes, index, edges,
                        reorder, comm_graph);
    return ret;
}
/* ================== C Wrappers for MPI_Graph_get ================== */

int E_Graph_get(MPI_Comm comm, int maxindex, int maxedges, int index[],
                int edges[], int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(153, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 153, v, comm, maxindex, maxedges, index, edges);
    return ret;
}
/* ================== C Wrappers for MPI_Graph_map ================== */

int E_Graph_map(MPI_Comm comm, int nnodes, const int index[], const int edges[],
                int *newrank, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(154, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 154, v, comm, nnodes, index, edges, newrank);
    return ret;
}
/* ================== C Wrappers for MPI_Graph_neighbors ================== */

int E_Graph_neighbors(MPI_Comm comm, int rank, int maxneighbors,
                      int neighbors[], int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(155, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 155, v, comm, rank, maxneighbors, neighbors);
    return ret;
}
/* ================== C Wrappers for MPI_Graph_neighbors_count
 * ================== */

int E_Graph_neighbors_count(MPI_Comm comm, int rank, int *nneighbors, int i,
                            vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(156, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 156, v, comm, rank, nneighbors);
    return ret;
}
/* ================== C Wrappers for MPI_Graphdims_get ================== */

int E_Graphdims_get(MPI_Comm comm, int *nnodes, int *nedges, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(157, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 157, v, comm, nnodes, nedges);
    return ret;
}
/* ================== C Wrappers for MPI_Grequest_complete ================== */

int E_Grequest_complete(MPI_Request request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(158, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 158, v, request);
    return ret;
}
/* ================== C Wrappers for MPI_Grequest_start ================== */

int E_Grequest_start(MPI_Grequest_query_function *query_fn,
                     MPI_Grequest_free_function *free_fn,
                     MPI_Grequest_cancel_function *cancel_fn, void *extra_state,
                     MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(159, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 159, v, query_fn, free_fn, cancel_fn,
                        extra_state, request);
    return ret;
}
/* ================== C Wrappers for MPI_Group_compare ================== */

int E_Group_compare(MPI_Group group1, MPI_Group group2, int *result, int i,
                    vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(160, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 160, v, group1, group2, result);
    return ret;
}
/* ================== C Wrappers for MPI_Group_difference ================== */

int E_Group_difference(MPI_Group group1, MPI_Group group2, MPI_Group *newgroup,
                       int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(161, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 161, v, group1, group2, newgroup);
    return ret;
}
/* ================== C Wrappers for MPI_Group_excl ================== */

int E_Group_excl(MPI_Group group, int n, const int ranks[], MPI_Group *newgroup,
                 int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(162, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 162, v, group, n, ranks, newgroup);
    return ret;
}
/* ================== C Wrappers for MPI_Group_free ================== */

int E_Group_free(MPI_Group *group, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(163, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 163, v, group);
    return ret;
}
/* ================== C Wrappers for MPI_Group_incl ================== */

int E_Group_incl(MPI_Group group, int n, const int ranks[], MPI_Group *newgroup,
                 int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(164, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 164, v, group, n, ranks, newgroup);
    return ret;
}
/* ================== C Wrappers for MPI_Group_intersection ==================
 */

int E_Group_intersection(MPI_Group group1, MPI_Group group2,
                         MPI_Group *newgroup, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(165, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 165, v, group1, group2, newgroup);
    return ret;
}
/* ================== C Wrappers for MPI_Group_range_excl ================== */

int E_Group_range_excl(MPI_Group group, int n, int ranges[][3],
                       MPI_Group *newgroup, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(166, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 166, v, group, n, ranges, newgroup);
    return ret;
}
/* ================== C Wrappers for MPI_Group_range_incl ================== */

int E_Group_range_incl(MPI_Group group, int n, int ranges[][3],
                       MPI_Group *newgroup, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(167, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 167, v, group, n, ranges, newgroup);
    return ret;
}
/* ================== C Wrappers for MPI_Group_rank ================== */

int E_Group_rank(MPI_Group group, int *rank, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(168, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 168, v, group, rank);
    return ret;
}
/* ================== C Wrappers for MPI_Group_size ================== */

int E_Group_size(MPI_Group group, int *size, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(169, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 169, v, group, size);
    return ret;
}
/* ================== C Wrappers for MPI_Group_translate_ranks
 * ================== */

int E_Group_translate_ranks(MPI_Group group1, int n, const int ranks1[],
                            MPI_Group group2, int ranks2[], int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(170, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 170, v, group1, n, ranks1, group2, ranks2);
    return ret;
}
/* ================== C Wrappers for MPI_Group_union ================== */

int E_Group_union(MPI_Group group1, MPI_Group group2, MPI_Group *newgroup,
                  int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(171, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 171, v, group1, group2, newgroup);
    return ret;
}
/* ================== C Wrappers for MPI_Iallgather ================== */

int E_Iallgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                 void *recvbuf, int recvcount, MPI_Datatype recvtype,
                 MPI_Comm comm, MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(172, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 172, v, sendbuf, sendcount, sendtype, recvbuf,
                        recvcount, recvtype, comm, request);
    return ret;
}
/* ================== C Wrappers for MPI_Iallgatherv ================== */

int E_Iallgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                  void *recvbuf, const int recvcounts[], const int displs[],
                  MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request,
                  int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(173, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 173, v, sendbuf, sendcount, sendtype, recvbuf,
                        recvcounts, displs, recvtype, comm, request);
    return ret;
}
/* ================== C Wrappers for MPI_Iallreduce ================== */

int E_Iallreduce(const void *sendbuf, void *recvbuf, int count,
                 MPI_Datatype datatype, MPI_Op op, MPI_Comm comm,
                 MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(174, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 174, v, sendbuf, recvbuf, count, datatype, op,
                        comm, request);
    return ret;
}
/* ================== C Wrappers for MPI_Ialltoall ================== */

int E_Ialltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                void *recvbuf, int recvcount, MPI_Datatype recvtype,
                MPI_Comm comm, MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(175, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 175, v, sendbuf, sendcount, sendtype, recvbuf,
                        recvcount, recvtype, comm, request);
    return ret;
}
/* ================== C Wrappers for MPI_Ialltoallv ================== */

int E_Ialltoallv(const void *sendbuf, const int sendcounts[],
                 const int sdispls[], MPI_Datatype sendtype, void *recvbuf,
                 const int recvcounts[], const int rdispls[],
                 MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request,
                 int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(176, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 176, v, sendbuf, sendcounts, sdispls, sendtype,
                        recvbuf, recvcounts, rdispls, recvtype, comm, request);
    return ret;
}
/* ================== C Wrappers for MPI_Ialltoallw ================== */
int E_Ialltoallw(const void *sendbuf, const int sendcounts[],
                 const int sdispls[], const MPI_Datatype sendtypes[],
                 void *recvbuf, const int recvcounts[], const int rdispls[],
                 const MPI_Datatype recvtypes[], MPI_Comm comm,
                 MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(177, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 177, v, sendbuf, sendcounts, sdispls, sendtypes,
                  recvbuf, recvcounts, rdispls, recvtypes, comm, request);
    return ret;
}
/* ================== C Wrappers for MPI_Ibarrier ================== */

int E_Ibarrier(MPI_Comm comm, MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(178, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 178, v, comm, request);
    return ret;
}
/* ================== C Wrappers for MPI_Ibcast ================== */

int E_Ibcast(void *buffer, int count, MPI_Datatype datatype, int root,
             MPI_Comm comm, MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(179, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 179, v, buffer, count, datatype, root, comm,
                        request);
    return ret;
}
/* ================== C Wrappers for MPI_Ibsend ================== */

int E_Ibsend(const void *buf, int count, MPI_Datatype datatype, int dest,
             int tag, MPI_Comm comm, MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(180, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 180, v, buf, count, datatype, dest, tag, comm,
                        request);
    return ret;
}
/* ================== C Wrappers for MPI_Iexscan ================== */

int E_Iexscan(const void *sendbuf, void *recvbuf, int count,
              MPI_Datatype datatype, MPI_Op op, MPI_Comm comm,
              MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(181, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 181, v, sendbuf, recvbuf, count, datatype, op,
                        comm, request);
    return ret;
}
/* ================== C Wrappers for MPI_Igather ================== */

int E_Igather(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
              void *recvbuf, int recvcount, MPI_Datatype recvtype, int root,
              MPI_Comm comm, MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(182, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 182, v, sendbuf, sendcount, sendtype, recvbuf,
                        recvcount, recvtype, root, comm, request);
    return ret;
}
/* ================== C Wrappers for MPI_Igatherv ================== */

int E_Igatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
               void *recvbuf, const int recvcounts[], const int displs[],
               MPI_Datatype recvtype, int root, MPI_Comm comm,
               MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(183, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 183, v, sendbuf, sendcount, sendtype, recvbuf,
                        recvcounts, displs, recvtype, root, comm, request);
    return ret;
}
/* ================== C Wrappers for MPI_Improbe ================== */

int E_Improbe(int source, int tag, MPI_Comm comm, int *flag,
              MPI_Message *message, MPI_Status *status, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(184, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 184, v, source, tag, comm, flag, message, status);
    return ret;
}
/* ================== C Wrappers for MPI_Imrecv ================== */

int E_Imrecv(void *buf, int count, MPI_Datatype type, MPI_Message *message,
             MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(185, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 185, v, buf, count, type, message, request);
    return ret;
}
/* ================== C Wrappers for MPI_Ineighbor_allgather ==================
 */

int E_Ineighbor_allgather(const void *sendbuf, int sendcount,
                          MPI_Datatype sendtype, void *recvbuf, int recvcount,
                          MPI_Datatype recvtype, MPI_Comm comm,
                          MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(186, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 186, v, sendbuf, sendcount, sendtype, recvbuf,
                        recvcount, recvtype, comm, request);
    return ret;
}
/* ================== C Wrappers for MPI_Ineighbor_allgatherv ==================
 */

int E_Ineighbor_allgatherv(const void *sendbuf, int sendcount,
                           MPI_Datatype sendtype, void *recvbuf,
                           const int recvcounts[], const int displs[],
                           MPI_Datatype recvtype, MPI_Comm comm,
                           MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(187, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 187, v, sendbuf, sendcount, sendtype, recvbuf,
                        recvcounts, displs, recvtype, comm, request);
    return ret;
}
/* ================== C Wrappers for MPI_Ineighbor_alltoall ==================
 */

int E_Ineighbor_alltoall(const void *sendbuf, int sendcount,
                         MPI_Datatype sendtype, void *recvbuf, int recvcount,
                         MPI_Datatype recvtype, MPI_Comm comm,
                         MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(188, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 188, v, sendbuf, sendcount, sendtype, recvbuf,
                        recvcount, recvtype, comm, request);
    return ret;
}
/* ================== C Wrappers for MPI_Ineighbor_alltoallv ==================
 */

int E_Ineighbor_alltoallv(const void *sendbuf, const int sendcounts[],
                          const int sdispls[], MPI_Datatype sendtype,
                          void *recvbuf, const int recvcounts[],
                          const int rdispls[], MPI_Datatype recvtype,
                          MPI_Comm comm, MPI_Request *request, int i,
                          vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(189, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 189, v, sendbuf, sendcounts, sdispls, sendtype,
                        recvbuf, recvcounts, rdispls, recvtype, comm, request);
    return ret;
}
/* ================== C Wrappers for MPI_Ineighbor_alltoallw ==================
 */

int E_Ineighbor_alltoallw(const void *sendbuf, const int sendcounts[],
                          const MPI_Aint sdispls[],
                          const MPI_Datatype sendtypes[], void *recvbuf,
                          const int recvcounts[], const MPI_Aint rdispls[],
                          const MPI_Datatype recvtypes[], MPI_Comm comm,
                          MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(190, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 190, v, sendbuf, sendcounts, sdispls, sendtypes,
                  recvbuf, recvcounts, rdispls, recvtypes, comm, request);
    return ret;
}
/* ================== C Wrappers for MPI_Info_create ================== */

int E_Info_create(MPI_Info *info, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(191, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 191, v, info);
    return ret;
}
/* ================== C Wrappers for MPI_Info_delete ================== */

int E_Info_delete(MPI_Info info, const char *key, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(192, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 192, v, info, key);
    return ret;
}
/* ================== C Wrappers for MPI_Info_dup ================== */

int E_Info_dup(MPI_Info info, MPI_Info *newinfo, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(193, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 193, v, info, newinfo);
    return ret;
}
/* ================== C Wrappers for MPI_Info_free ================== */

int E_Info_free(MPI_Info *info, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(194, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 194, v, info);
    return ret;
}
/* ================== C Wrappers for MPI_Info_get ================== */

int E_Info_get(MPI_Info info, const char *key, int valuelen, char *value,
               int *flag, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(195, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 195, v, info, key, valuelen, value, flag);
    return ret;
}
/* ================== C Wrappers for MPI_Info_get_nkeys ================== */

int E_Info_get_nkeys(MPI_Info info, int *nkeys, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(196, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 196, v, info, nkeys);
    return ret;
}
/* ================== C Wrappers for MPI_Info_get_nthkey ================== */

int E_Info_get_nthkey(MPI_Info info, int n, char *key, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(197, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 197, v, info, n, key);
    return ret;
}
/* ================== C Wrappers for MPI_Info_get_valuelen ================== */

int E_Info_get_valuelen(MPI_Info info, const char *key, int *valuelen,
                        int *flag, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(198, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 198, v, info, key, valuelen, flag);
    return ret;
}
/* ================== C Wrappers for MPI_Info_set ================== */

int E_Info_set(MPI_Info info, const char *key, const char *value, int i,
               vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(199, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 199, v, info, key, value);
    return ret;
}

/* ================== C Wrappers for MPI_Init_thread ================== */

int E_Init_thread(int *argc, char ***argv, int required, int *provided, int i,
                  vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(201, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 201, v, argc, argv, required, provided);
    return ret;
}
/* ================== C Wrappers for MPI_Initialized ================== */

int E_Initialized(int *flag, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(202, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 202, v, flag);
    return ret;
}
/* ================== C Wrappers for MPI_Intercomm_create ================== */

int E_Intercomm_create(MPI_Comm local_comm, int local_leader,
                       MPI_Comm bridge_comm, int remote_leader, int tag,
                       MPI_Comm *newintercomm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(203, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 203, v, local_comm, local_leader, bridge_comm,
                        remote_leader, tag, newintercomm);
    return ret;
}
/* ================== C Wrappers for MPI_Intercomm_merge ================== */

int E_Intercomm_merge(MPI_Comm intercomm, int high, MPI_Comm *newintercomm,
                      int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(204, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 204, v, intercomm, high, newintercomm);
    return ret;
}
/* ================== C Wrappers for MPI_Iprobe ================== */

int E_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status,
             int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(205, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 205, v, source, tag, comm, flag, status);
    return ret;
}
/* ================== C Wrappers for MPI_Irecv ================== */

int E_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag,
            MPI_Comm comm, MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(206, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 206, v, buf, count, datatype, source, tag,
                        comm, request);
    return ret;
}
/* ================== C Wrappers for MPI_Ireduce ================== */

int E_Ireduce(const void *sendbuf, void *recvbuf, int count,
              MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm,
              MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(207, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 207, v, sendbuf, recvbuf, count, datatype, op,
                        root, comm, request);
    return ret;
}
/* ================== C Wrappers for MPI_Ireduce_scatter ================== */

int E_Ireduce_scatter(const void *sendbuf, void *recvbuf,
                      const int recvcounts[], MPI_Datatype datatype, MPI_Op op,
                      MPI_Comm comm, MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(208, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 208, v, sendbuf, recvbuf, recvcounts, datatype,
                        op, comm, request);
    return ret;
}
/* ================== C Wrappers for MPI_Ireduce_scatter_block
 * ================== */

int E_Ireduce_scatter_block(const void *sendbuf, void *recvbuf, int recvcount,
                            MPI_Datatype datatype, MPI_Op op, MPI_Comm comm,
                            MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(209, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 209, v, sendbuf, recvbuf, recvcount, datatype,
                        op, comm, request);
    return ret;
}
/* ================== C Wrappers for MPI_Irsend ================== */

int E_Irsend(const void *buf, int count, MPI_Datatype datatype, int dest,
             int tag, MPI_Comm comm, MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(210, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 210, v, buf, count, datatype, dest, tag, comm,
                        request);
    return ret;
}
/* ================== C Wrappers for MPI_Is_thread_main ================== */

int E_Is_thread_main(int *flag, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(211, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 211, v, flag);
    return ret;
}
/* ================== C Wrappers for MPI_Iscan ================== */

int E_Iscan(const void *sendbuf, void *recvbuf, int count,
            MPI_Datatype datatype, MPI_Op op, MPI_Comm comm,
            MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(212, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 212, v, sendbuf, recvbuf, count, datatype, op,
                        comm, request);
    return ret;
}
/* ================== C Wrappers for MPI_Iscatter ================== */

int E_Iscatter(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
               void *recvbuf, int recvcount, MPI_Datatype recvtype, int root,
               MPI_Comm comm, MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(213, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 213, v, sendbuf, sendcount, sendtype, recvbuf,
                        recvcount, recvtype, root, comm, request);
    return ret;
}
/* ================== C Wrappers for MPI_Iscatterv ================== */

int E_Iscatterv(const void *sendbuf, const int sendcounts[], const int displs[],
                MPI_Datatype sendtype, void *recvbuf, int recvcount,
                MPI_Datatype recvtype, int root, MPI_Comm comm,
                MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(214, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 214, v, sendbuf, sendcounts, displs, sendtype,
                        recvbuf, recvcount, recvtype, root, comm, request);
    return ret;
}
/* ================== C Wrappers for MPI_Isend ================== */

int E_Isend(const void *buf, int count, MPI_Datatype datatype, int dest,
            int tag, MPI_Comm comm, MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(215, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 215, v, buf, count, datatype, dest, tag, comm,
                        request);
    return ret;
}
/* ================== C Wrappers for MPI_Issend ================== */

int E_Issend(const void *buf, int count, MPI_Datatype datatype, int dest,
             int tag, MPI_Comm comm, MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(216, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 216, v, buf, count, datatype, dest, tag, comm,
                        request);
    return ret;
}
/* ================== C Wrappers for MPI_Keyval_create ================== */

int E_Keyval_create(MPI_Copy_function *copy_fn, MPI_Delete_function *delete_fn,
                    int *keyval, void *extra_state, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(217, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 217, v, copy_fn, delete_fn, keyval, extra_state);
    return ret;
}
/* ================== C Wrappers for MPI_Keyval_free ================== */

int E_Keyval_free(int *keyval, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(218, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 218, v, keyval);
    return ret;
}
/* ================== C Wrappers for MPI_Lookup_name ================== */

int E_Lookup_name(const char *service_name, MPI_Info info, char *port_name,
                  int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(219, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 219, v, service_name, info, port_name);
    return ret;
}
/* ================== C Wrappers for MPI_Mprobe ================== */

int E_Mprobe(int source, int tag, MPI_Comm comm, MPI_Message *message,
             MPI_Status *status, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(220, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 220, v, source, tag, comm, message, status);
    return ret;
}
/* ================== C Wrappers for MPI_Mrecv ================== */

int E_Mrecv(void *buf, int count, MPI_Datatype type, MPI_Message *message,
            MPI_Status *status, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(221, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 221, v, buf, count, type, message, status);
    return ret;
}
/* ================== C Wrappers for MPI_Neighbor_allgather ==================
 */

int E_Neighbor_allgather(const void *sendbuf, int sendcount,
                         MPI_Datatype sendtype, void *recvbuf, int recvcount,
                         MPI_Datatype recvtype, MPI_Comm comm, int i,
                         vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(222, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 222, v, sendbuf, sendcount, sendtype, recvbuf,
                        recvcount, recvtype, comm);
    return ret;
}
/* ================== C Wrappers for MPI_Neighbor_allgatherv ==================
 */

int E_Neighbor_allgatherv(const void *sendbuf, int sendcount,
                          MPI_Datatype sendtype, void *recvbuf,
                          const int recvcounts[], const int displs[],
                          MPI_Datatype recvtype, MPI_Comm comm, int i,
                          vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(223, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 223, v, sendbuf, sendcount, sendtype, recvbuf,
                        recvcounts, displs, recvtype, comm);
    return ret;
}
/* ================== C Wrappers for MPI_Neighbor_alltoall ================== */

int E_Neighbor_alltoall(const void *sendbuf, int sendcount,
                        MPI_Datatype sendtype, void *recvbuf, int recvcount,
                        MPI_Datatype recvtype, MPI_Comm comm, int i,
                        vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(224, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 224, v, sendbuf, sendcount, sendtype, recvbuf,
                        recvcount, recvtype, comm);
    return ret;
}
/* ================== C Wrappers for MPI_Neighbor_alltoallv ==================
 */

int E_Neighbor_alltoallv(const void *sendbuf, const int sendcounts[],
                         const int sdispls[], MPI_Datatype sendtype,
                         void *recvbuf, const int recvcounts[],
                         const int rdispls[], MPI_Datatype recvtype,
                         MPI_Comm comm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(225, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 225, v, sendbuf, sendcounts, sdispls, sendtype,
                        recvbuf, recvcounts, rdispls, recvtype, comm);
    return ret;
}
/* ================== C Wrappers for MPI_Neighbor_alltoallw ==================
 */

int E_Neighbor_alltoallw(const void *sendbuf, const int sendcounts[],
                         const MPI_Aint sdispls[],
                         const MPI_Datatype sendtypes[], void *recvbuf,
                         const int recvcounts[], const MPI_Aint rdispls[],
                         const MPI_Datatype recvtypes[], MPI_Comm comm, int i,
                         vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(226, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 226, v, sendbuf, sendcounts, sdispls, sendtypes,
                  recvbuf, recvcounts, rdispls, recvtypes, comm);
    return ret;
}
/* ================== C Wrappers for MPI_Op_commutative ================== */

int E_Op_commutative(MPI_Op op, int *commute, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(227, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 227, v, op, commute);
    return ret;
}
/* ================== C Wrappers for MPI_Op_create ================== */

int E_Op_create(MPI_User_function *function, int commute, MPI_Op *op, int i,
                vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(228, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 228, v, function, commute, op);
    return ret;
}
/* ================== C Wrappers for MPI_Op_free ================== */

int E_Op_free(MPI_Op *op, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(229, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 229, v, op);
    return ret;
}
/* ================== C Wrappers for MPI_Open_port ================== */

int E_Open_port(MPI_Info info, char *port_name, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(230, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 230, v, info, port_name);
    return ret;
}
/* ================== C Wrappers for MPI_Pack ================== */

int E_Pack(const void *inbuf, int incount, MPI_Datatype datatype, void *outbuf,
           int outsize, int *position, MPI_Comm comm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(231, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 231, v, inbuf, incount, datatype, outbuf,
                        outsize, position, comm);
    return ret;
}
/* ================== C Wrappers for MPI_Pack_external ================== */

int E_Pack_external(const char datarep[], const void *inbuf, int incount,
                    MPI_Datatype datatype, void *outbuf, MPI_Aint outsize,
                    MPI_Aint *position, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(232, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 232, v, datarep, inbuf, incount, datatype,
                        outbuf, outsize, position);
    return ret;
}
/* ================== C Wrappers for MPI_Pack_external_size ==================
 */

int E_Pack_external_size(const char datarep[], int incount,
                         MPI_Datatype datatype, MPI_Aint *size, int i,
                         vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(233, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 233, v, datarep, incount, datatype, size);
    return ret;
}
/* ================== C Wrappers for MPI_Pack_size ================== */

int E_Pack_size(int incount, MPI_Datatype datatype, MPI_Comm comm, int *size,
                int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(234, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 234, v, incount, datatype, comm, size);
    return ret;
}
/* ================== C Wrappers for MPI_Pcontrol ================== */

int E_Pcontrol(const int level, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(235, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 235, v, level);
    return ret;
}
/* ================== C Wrappers for MPI_Probe ================== */

int E_Probe(int source, int tag, MPI_Comm comm, MPI_Status *status, int i,
            vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(236, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 236, v, source, tag, comm, status);
    return ret;
}
/* ================== C Wrappers for MPI_Publish_name ================== */

int E_Publish_name(const char *service_name, MPI_Info info,
                   const char *port_name, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(237, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 237, v, service_name, info, port_name);
    return ret;
}
/* ================== C Wrappers for MPI_Put ================== */

int E_Put(const void *origin_addr, int origin_count,
          MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp,
          int target_count, MPI_Datatype target_datatype, MPI_Win win, int i,
          vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(238, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 238, v, origin_addr, origin_count, origin_datatype,
                  target_rank, target_disp, target_count, target_datatype, win);
    return ret;
}
/* ================== C Wrappers for MPI_Query_thread ================== */

int E_Query_thread(int *provided, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(239, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 239, v, provided);
    return ret;
}
/* ================== C Wrappers for MPI_Raccumulate ================== */

int E_Raccumulate(const void *origin_addr, int origin_count,
                  MPI_Datatype origin_datatype, int target_rank,
                  MPI_Aint target_disp, int target_count,
                  MPI_Datatype target_datatype, MPI_Op op, MPI_Win win,
                  MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(240, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 240, v, origin_addr, origin_count,
                        origin_datatype, target_rank, target_disp, target_count,
                        target_datatype, op, win, request);
    return ret;
}
/* ================== C Wrappers for MPI_Recv_init ================== */

int E_Recv_init(void *buf, int count, MPI_Datatype datatype, int source,
                int tag, MPI_Comm comm, MPI_Request *request, int i,
                vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(242, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 242, v, buf, count, datatype, source, tag,
                        comm, request);
    return ret;
}
/* ================== C Wrappers for MPI_Reduce ================== */

int E_Reduce(const void *sendbuf, void *recvbuf, int count,
             MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, int i,
             vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(243, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 243, v, sendbuf, recvbuf, count, datatype, op,
                        root, comm);
    return ret;
}
/* ================== C Wrappers for MPI_Reduce_local ================== */

int E_Reduce_local(const void *inbuf, void *inoutbuf, int count,
                   MPI_Datatype datatype, MPI_Op op, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(244, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 244, v, inbuf, inoutbuf, count, datatype, op);
    return ret;
}
/* ================== C Wrappers for MPI_Reduce_scatter ================== */

int E_Reduce_scatter(const void *sendbuf, void *recvbuf, const int recvcounts[],
                     MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, int i,
                     vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(245, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 245, v, sendbuf, recvbuf, recvcounts, datatype,
                        op, comm);
    return ret;
}
/* ================== C Wrappers for MPI_Reduce_scatter_block ==================
 */

int E_Reduce_scatter_block(const void *sendbuf, void *recvbuf, int recvcount,
                           MPI_Datatype datatype, MPI_Op op, MPI_Comm comm,
                           int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(246, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 246, v, sendbuf, recvbuf, recvcount, datatype,
                        op, comm);
    return ret;
}
/* ================== C Wrappers for MPI_Register_datarep ================== */

int E_Register_datarep(const char *datarep,
                       MPI_Datarep_conversion_function *read_conversion_fn,
                       MPI_Datarep_conversion_function *write_conversion_fn,
                       MPI_Datarep_extent_function *dtype_file_extent_fn,
                       void *extra_state, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(247, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 247, v, datarep, read_conversion_fn,
                        write_conversion_fn, dtype_file_extent_fn, extra_state);
    return ret;
}
/* ================== C Wrappers for MPI_Request_free ================== */

int E_Request_free(MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(248, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 248, v, request);
    return ret;
}
/* ================== C Wrappers for MPI_Request_get_status ==================
 */

int E_Request_get_status(MPI_Request request, int *flag, MPI_Status *status,
                         int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(249, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 249, v, request, flag, status);
    return ret;
}
/* ================== C Wrappers for MPI_Rget ================== */

int E_Rget(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
           int target_rank, MPI_Aint target_disp, int target_count,
           MPI_Datatype target_datatype, MPI_Win win, MPI_Request *request,
           int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(250, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 250, v, origin_addr, origin_count,
                        origin_datatype, target_rank, target_disp, target_count,
                        target_datatype, win, request);
    return ret;
}
/* ================== C Wrappers for MPI_Rget_accumulate ================== */

int E_Rget_accumulate(const void *origin_addr, int origin_count,
                      MPI_Datatype origin_datatype, void *result_addr,
                      int result_count, MPI_Datatype result_datatype,
                      int target_rank, MPI_Aint target_disp, int target_count,
                      MPI_Datatype target_datatype, MPI_Op op, MPI_Win win,
                      MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(251, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 251, v, origin_addr, origin_count, origin_datatype,
                  result_addr, result_count, result_datatype, target_rank,
                  target_disp, target_count, target_datatype, op, win, request);
    return ret;
}
/* ================== C Wrappers for MPI_Rput ================== */

int E_Rput(const void *origin_addr, int origin_count,
           MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp,
           int target_cout, MPI_Datatype target_datatype, MPI_Win win,
           MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(252, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 252, v, origin_addr, origin_count,
                        origin_datatype, target_rank, target_disp, target_cout,
                        target_datatype, win, request);
    return ret;
}
/* ================== C Wrappers for MPI_Rsend_init ================== */

int E_Rsend_init(const void *buf, int count, MPI_Datatype datatype, int dest,
                 int tag, MPI_Comm comm, MPI_Request *request, int i,
                 vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(254, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 254, v, buf, count, datatype, dest, tag, comm,
                        request);
    return ret;
}
/* ================== C Wrappers for MPI_Scan ================== */

int E_Scan(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
           MPI_Op op, MPI_Comm comm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(255, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 255, v, sendbuf, recvbuf, count, datatype, op, comm);
    return ret;
}
/* ================== C Wrappers for MPI_Scatter ================== */

int E_Scatter(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
              void *recvbuf, int recvcount, MPI_Datatype recvtype, int root,
              MPI_Comm comm, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(256, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 256, v, sendbuf, sendcount, sendtype, recvbuf,
                        recvcount, recvtype, root, comm);
    return ret;
}
/* ================== C Wrappers for MPI_Scatterv ================== */

int E_Scatterv(const void *sendbuf, const int sendcounts[], const int displs[],
               MPI_Datatype sendtype, void *recvbuf, int recvcount,
               MPI_Datatype recvtype, int root, MPI_Comm comm, int i,
               vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(257, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 257, v, sendbuf, sendcounts, displs, sendtype,
                        recvbuf, recvcount, recvtype, root, comm);
    return ret;
}
/* ================== C Wrappers for MPI_Send_init ================== */

int E_Send_init(const void *buf, int count, MPI_Datatype datatype, int dest,
                int tag, MPI_Comm comm, MPI_Request *request, int i,
                vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(259, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 259, v, buf, count, datatype, dest, tag, comm,
                        request);
    return ret;
}
/* ================== C Wrappers for MPI_Sendrecv_replace ================== */

int E_Sendrecv_replace(void *buf, int count, MPI_Datatype datatype, int dest,
                       int sendtag, int source, int recvtag, MPI_Comm comm,
                       MPI_Status *status, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(261, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 261, v, buf, count, datatype, dest, sendtag,
                        source, recvtag, comm, status);
    return ret;
}
/* ================== C Wrappers for MPI_Ssend_init ================== */

int E_Ssend_init(const void *buf, int count, MPI_Datatype datatype, int dest,
                 int tag, MPI_Comm comm, MPI_Request *request, int i,
                 vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(263, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 263, v, buf, count, datatype, dest, tag, comm,
                        request);
    return ret;
}
/* ================== C Wrappers for MPI_Start ================== */

int E_Start(MPI_Request *request, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(264, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 264, v, request);
    return ret;
}
/* ================== C Wrappers for MPI_Startall ================== */

int E_Startall(int count, MPI_Request array_of_requests[], int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(265, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 265, v, count, array_of_requests);
    return ret;
}
/* ================== C Wrappers for MPI_Status_set_cancelled ==================
 */

int E_Status_set_cancelled(MPI_Status *status, int flag, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(266, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 266, v, status, flag);
    return ret;
}
/* ================== C Wrappers for MPI_Status_set_elements ==================
 */

int E_Status_set_elements(MPI_Status *status, MPI_Datatype datatype, int count,
                          int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(267, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 267, v, status, datatype, count);
    return ret;
}
/* ================== C Wrappers for MPI_Status_set_elements_x
 * ================== */

int E_Status_set_elements_x(MPI_Status *status, MPI_Datatype datatype,
                            MPI_Count count, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(268, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 268, v, status, datatype, count);
    return ret;
}
/* ================== C Wrappers for MPI_Test ================== */

int E_Test(MPI_Request *request, int *flag, MPI_Status *status, int i,
           vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(269, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 269, v, request, flag, status);
    return ret;
}
/* ================== C Wrappers for MPI_Test_cancelled ================== */

int E_Test_cancelled(const MPI_Status *status, int *flag, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(270, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 270, v, status, flag);
    return ret;
}
/* ================== C Wrappers for MPI_Testall ================== */

int E_Testall(int count, MPI_Request array_of_requests[], int *flag,
              MPI_Status array_of_statuses[], int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(271, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 271, v, count, array_of_requests, flag,
                        array_of_statuses);
    return ret;
}
/* ================== C Wrappers for MPI_Testany ================== */

int E_Testany(int count, MPI_Request array_of_requests[], int *index, int *flag,
              MPI_Status *status, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(272, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 272, v, count, array_of_requests, index, flag,
                        status);
    return ret;
}
/* ================== C Wrappers for MPI_Testsome ================== */

int E_Testsome(int incount, MPI_Request array_of_requests[], int *outcount,
               int array_of_indices[], MPI_Status array_of_statuses[], int i,
               vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(273, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 273, v, incount, array_of_requests, outcount,
                        array_of_indices, array_of_statuses);
    return ret;
}
/* ================== C Wrappers for MPI_Topo_test ================== */

int E_Topo_test(MPI_Comm comm, int *status, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(274, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 274, v, comm, status);
    return ret;
}
/* ================== C Wrappers for MPI_Type_commit ================== */

int E_Type_commit(MPI_Datatype *type, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(275, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 275, v, type);
    return ret;
}
/* ================== C Wrappers for MPI_Type_contiguous ================== */

int E_Type_contiguous(int count, MPI_Datatype oldtype, MPI_Datatype *newtype,
                      int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(276, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 276, v, count, oldtype, newtype);
    return ret;
}
/* ================== C Wrappers for MPI_Type_create_darray ==================
 */

int E_Type_create_darray(int size, int rank, int ndims, const int gsize_array[],
                         const int distrib_array[], const int darg_array[],
                         const int psize_array[], int order,
                         MPI_Datatype oldtype, MPI_Datatype *newtype, int i,
                         vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(277, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 277, v, size, rank, ndims, gsize_array,
                        distrib_array, darg_array, psize_array, order, oldtype,
                        newtype);
    return ret;
}
/* ================== C Wrappers for MPI_Type_create_f90_complex
 * ================== */

int E_Type_create_f90_complex(int p, int r, MPI_Datatype *newtype, int i,
                              vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(278, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 278, v, p, r, newtype);
    return ret;
}
/* ================== C Wrappers for MPI_Type_create_f90_integer
 * ================== */

int E_Type_create_f90_integer(int r, MPI_Datatype *newtype, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(279, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 279, v, r, newtype);
    return ret;
}
/* ================== C Wrappers for MPI_Type_create_f90_real ==================
 */

int E_Type_create_f90_real(int p, int r, MPI_Datatype *newtype, int i,
                           vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(280, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 280, v, p, r, newtype);
    return ret;
}
/* ================== C Wrappers for MPI_Type_create_hindexed ==================
 */

int E_Type_create_hindexed(int count, const int array_of_blocklengths[],
                           const MPI_Aint array_of_displacements[],
                           MPI_Datatype oldtype, MPI_Datatype *newtype, int i,
                           vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(281, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 281, v, count, array_of_blocklengths,
                        array_of_displacements, oldtype, newtype);
    return ret;
}
/* ================== C Wrappers for MPI_Type_create_hindexed_block
 * ================== */

int E_Type_create_hindexed_block(int count, int blocklength,
                                 const MPI_Aint array_of_displacements[],
                                 MPI_Datatype oldtype, MPI_Datatype *newtype,
                                 int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(282, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 282, v, count, blocklength,
                        array_of_displacements, oldtype, newtype);
    return ret;
}
/* ================== C Wrappers for MPI_Type_create_hvector ==================
 */

int E_Type_create_hvector(int count, int blocklength, MPI_Aint stride,
                          MPI_Datatype oldtype, MPI_Datatype *newtype, int i,
                          vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(283, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 283, v, count, blocklength, stride, oldtype,
                        newtype);
    return ret;
}
/* ================== C Wrappers for MPI_Type_create_indexed_block
 * ================== */

int E_Type_create_indexed_block(int count, int blocklength,
                                const int array_of_displacements[],
                                MPI_Datatype oldtype, MPI_Datatype *newtype,
                                int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(284, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 284, v, count, blocklength,
                        array_of_displacements, oldtype, newtype);
    return ret;
}
/* ================== C Wrappers for MPI_Type_create_keyval ==================
 */

int E_Type_create_keyval(MPI_Type_copy_attr_function *type_copy_attr_fn,
                         MPI_Type_delete_attr_function *type_delete_attr_fn,
                         int *type_keyval, void *extra_state, int i,
                         vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(285, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 285, v, type_copy_attr_fn, type_delete_attr_fn,
                        type_keyval, extra_state);
    return ret;
}
/* ================== C Wrappers for MPI_Type_create_resized ==================
 */

int E_Type_create_resized(MPI_Datatype oldtype, MPI_Aint lb, MPI_Aint extent,
                          MPI_Datatype *newtype, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(286, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 286, v, oldtype, lb, extent, newtype);
    return ret;
}
/* ================== C Wrappers for MPI_Type_create_struct ==================
 */

int E_Type_create_struct(int count, const int array_of_block_lengths[],
                         const MPI_Aint array_of_displacements[],
                         const MPI_Datatype array_of_types[],
                         MPI_Datatype *newtype, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(287, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 287, v, count, array_of_block_lengths,
                        array_of_displacements, array_of_types, newtype);
    return ret;
}
/* ================== C Wrappers for MPI_Type_create_subarray ==================
 */

int E_Type_create_subarray(int ndims, const int size_array[],
                           const int subsize_array[], const int start_array[],
                           int order, MPI_Datatype oldtype,
                           MPI_Datatype *newtype, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(288, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 288, v, ndims, size_array, subsize_array,
                        start_array, order, oldtype, newtype);
    return ret;
}
/* ================== C Wrappers for MPI_Type_delete_attr ================== */

int E_Type_delete_attr(MPI_Datatype type, int type_keyval, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(289, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 289, v, type, type_keyval);
    return ret;
}
/* ================== C Wrappers for MPI_Type_dup ================== */

int E_Type_dup(MPI_Datatype type, MPI_Datatype *newtype, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(290, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 290, v, type, newtype);
    return ret;
}
/* ================== C Wrappers for MPI_Type_extent ================== */

int E_Type_extent(MPI_Datatype type, MPI_Aint *extent, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(291, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 291, v, type, extent);
    return ret;
}
/* ================== C Wrappers for MPI_Type_free ================== */

int E_Type_free(MPI_Datatype *type, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(292, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 292, v, type);
    return ret;
}
/* ================== C Wrappers for MPI_Type_free_keyval ================== */

int E_Type_free_keyval(int *type_keyval, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(293, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 293, v, type_keyval);
    return ret;
}
/* ================== C Wrappers for MPI_Type_get_attr ================== */

int E_Type_get_attr(MPI_Datatype type, int type_keyval, void *attribute_val,
                    int *flag, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(294, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 294, v, type, type_keyval, attribute_val, flag);
    return ret;
}
/* ================== C Wrappers for MPI_Type_get_contents ================== */

int E_Type_get_contents(MPI_Datatype mtype, int max_integers, int max_addresses,
                        int max_datatypes, int array_of_integers[],
                        MPI_Aint array_of_addresses[],
                        MPI_Datatype array_of_datatypes[], int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(295, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 295, v, mtype, max_integers, max_addresses,
                        max_datatypes, array_of_integers, array_of_addresses,
                        array_of_datatypes);
    return ret;
}
/* ================== C Wrappers for MPI_Type_get_envelope ================== */

int E_Type_get_envelope(MPI_Datatype type, int *num_integers,
                        int *num_addresses, int *num_datatypes, int *combiner,
                        int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(296, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 296, v, type, num_integers, num_addresses,
                        num_datatypes, combiner);
    return ret;
}
/* ================== C Wrappers for MPI_Type_get_extent ================== */

int E_Type_get_extent(MPI_Datatype type, MPI_Aint *lb, MPI_Aint *extent, int i,
                      vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(297, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 297, v, type, lb, extent);
    return ret;
}
/* ================== C Wrappers for MPI_Type_get_extent_x ================== */

int E_Type_get_extent_x(MPI_Datatype type, MPI_Count *lb, MPI_Count *extent,
                        int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(298, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 298, v, type, lb, extent);
    return ret;
}
/* ================== C Wrappers for MPI_Type_get_name ================== */

int E_Type_get_name(MPI_Datatype type, char *type_name, int *resultlen, int i,
                    vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(299, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 299, v, type, type_name, resultlen);
    return ret;
}
/* ================== C Wrappers for MPI_Type_get_true_extent ==================
 */

int E_Type_get_true_extent(MPI_Datatype datatype, MPI_Aint *true_lb,
                           MPI_Aint *true_extent, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(300, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 300, v, datatype, true_lb, true_extent);
    return ret;
}
/* ================== C Wrappers for MPI_Type_get_true_extent_x
 * ================== */

int E_Type_get_true_extent_x(MPI_Datatype datatype, MPI_Count *true_lb,
                             MPI_Count *true_extent, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(301, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 301, v, datatype, true_lb, true_extent);
    return ret;
}
/* ================== C Wrappers for MPI_Type_hindexed ================== */

int E_Type_hindexed(int count, int array_of_blocklengths[],
                    MPI_Aint array_of_displacements[], MPI_Datatype oldtype,
                    MPI_Datatype *newtype, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(302, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 302, v, count, array_of_blocklengths,
                        array_of_displacements, oldtype, newtype);
    return ret;
}
/* ================== C Wrappers for MPI_Type_hvector ================== */

int E_Type_hvector(int count, int blocklength, MPI_Aint stride,
                   MPI_Datatype oldtype, MPI_Datatype *newtype, int i,
                   vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(303, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 303, v, count, blocklength, stride, oldtype,
                        newtype);
    return ret;
}
/* ================== C Wrappers for MPI_Type_indexed ================== */

int E_Type_indexed(int count, const int array_of_blocklengths[],
                   const int array_of_displacements[], MPI_Datatype oldtype,
                   MPI_Datatype *newtype, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(304, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 304, v, count, array_of_blocklengths,
                        array_of_displacements, oldtype, newtype);
    return ret;
}
/* ================== C Wrappers for MPI_Type_lb ================== */

int E_Type_lb(MPI_Datatype type, MPI_Aint *lb, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(305, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 305, v, type, lb);
    return ret;
}
/* ================== C Wrappers for MPI_Type_match_size ================== */

int E_Type_match_size(int typeclass, int size, MPI_Datatype *type, int i,
                      vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(306, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 306, v, typeclass, size, type);
    return ret;
}
/* ================== C Wrappers for MPI_Type_set_attr ================== */

int E_Type_set_attr(MPI_Datatype type, int type_keyval, void *attr_val, int i,
                    vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(307, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 307, v, type, type_keyval, attr_val);
    return ret;
}
/* ================== C Wrappers for MPI_Type_set_name ================== */

int E_Type_set_name(MPI_Datatype type, const char *type_name, int i,
                    vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(308, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 308, v, type, type_name);
    return ret;
}
/* ================== C Wrappers for MPI_Type_size ================== */

int E_Type_size(MPI_Datatype type, int *size, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(309, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 309, v, type, size);
    return ret;
}
/* ================== C Wrappers for MPI_Type_size_x ================== */

int E_Type_size_x(MPI_Datatype type, MPI_Count *size, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(310, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 310, v, type, size);
    return ret;
}
/* ================== C Wrappers for MPI_Type_struct ================== */

int E_Type_struct(int count, int array_of_blocklengths[],
                  MPI_Aint array_of_displacements[],
                  MPI_Datatype array_of_types[], MPI_Datatype *newtype, int i,
                  vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(311, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 311, v, count, array_of_blocklengths,
                        array_of_displacements, array_of_types, newtype);
    return ret;
}
/* ================== C Wrappers for MPI_Type_ub ================== */

int E_Type_ub(MPI_Datatype mtype, MPI_Aint *ub, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(312, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 312, v, mtype, ub);
    return ret;
}
/* ================== C Wrappers for MPI_Type_vector ================== */

int E_Type_vector(int count, int blocklength, int stride, MPI_Datatype oldtype,
                  MPI_Datatype *newtype, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(313, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 313, v, count, blocklength, stride, oldtype,
                        newtype);
    return ret;
}
/* ================== C Wrappers for MPI_Unpack ================== */

int E_Unpack(const void *inbuf, int insize, int *position, void *outbuf,
             int outcount, MPI_Datatype datatype, MPI_Comm comm, int i,
             vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(314, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 314, v, inbuf, insize, position, outbuf,
                        outcount, datatype, comm);
    return ret;
}
/* ================== C Wrappers for MPI_Unpack_external ================== */

int E_Unpack_external(const char datarep[], const void *inbuf, MPI_Aint insize,
                      MPI_Aint *position, void *outbuf, int outcount,
                      MPI_Datatype datatype, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(315, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 315, v, datarep, inbuf, insize, position,
                        outbuf, outcount, datatype);
    return ret;
}
/* ================== C Wrappers for MPI_Unpublish_name ================== */

int E_Unpublish_name(const char *service_name, MPI_Info info,
                     const char *port_name, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(316, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 316, v, service_name, info, port_name);
    return ret;
}
/* ================== C Wrappers for MPI_Wait ================== */

int E_Wait(MPI_Request *request, MPI_Status *status, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(317, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 317, v, request, status);
    return ret;
}
/* ================== C Wrappers for MPI_Waitall ================== */

int E_Waitall(int count, MPI_Request array_of_requests[],
              MPI_Status *array_of_statuses, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(318, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 318, v, count, array_of_requests, array_of_statuses);
    return ret;
}
/* ================== C Wrappers for MPI_Waitany ================== */

int E_Waitany(int count, MPI_Request array_of_requests[], int *index,
              MPI_Status *status, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(319, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 319, v, count, array_of_requests, index, status);
    return ret;
}
/* ================== C Wrappers for MPI_Waitsome ================== */

int E_Waitsome(int incount, MPI_Request array_of_requests[], int *outcount,
               int array_of_indices[], MPI_Status array_of_statuses[], int i,
               vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(320, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 320, v, incount, array_of_requests, outcount,
                        array_of_indices, array_of_statuses);
    return ret;
}
/* ================== C Wrappers for MPI_Win_allocate ================== */

int E_Win_allocate(MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm,
                   void *baseptr, MPI_Win *win, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(321, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 321, v, size, disp_unit, info, comm, baseptr, win);
    return ret;
}
/* ================== C Wrappers for MPI_Win_allocate_shared ==================
 */

int E_Win_allocate_shared(MPI_Aint size, int disp_unit, MPI_Info info,
                          MPI_Comm comm, void *baseptr, MPI_Win *win, int i,
                          vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(322, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 322, v, size, disp_unit, info, comm, baseptr, win);
    return ret;
}
/* ================== C Wrappers for MPI_Win_attach ================== */

int E_Win_attach(MPI_Win win, void *base, MPI_Aint size, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(323, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 323, v, win, base, size);
    return ret;
}
/* ================== C Wrappers for MPI_Win_call_errhandler ==================
 */

int E_Win_call_errhandler(MPI_Win win, int errorcode, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(324, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 324, v, win, errorcode);
    return ret;
}
/* ================== C Wrappers for MPI_Win_complete ================== */

int E_Win_complete(MPI_Win win, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(325, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 325, v, win);
    return ret;
}
/* ================== C Wrappers for MPI_Win_create ================== */

int E_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info,
                 MPI_Comm comm, MPI_Win *win, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(326, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret =
        EXEC_FUNC(f_dl, i, 326, v, base, size, disp_unit, info, comm, win);
    return ret;
}
/* ================== C Wrappers for MPI_Win_create_dynamic ==================
 */

int E_Win_create_dynamic(MPI_Info info, MPI_Comm comm, MPI_Win *win, int i,
                         vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(327, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 327, v, info, comm, win);
    return ret;
}
/* ================== C Wrappers for MPI_Win_create_errhandler
 * ================== */

int E_Win_create_errhandler(MPI_Win_errhandler_function *function,
                            MPI_Errhandler *errhandler, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(328, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 328, v, function, errhandler);
    return ret;
}
/* ================== C Wrappers for MPI_Win_create_keyval ================== */

int E_Win_create_keyval(MPI_Win_copy_attr_function *win_copy_attr_fn,
                        MPI_Win_delete_attr_function *win_delete_attr_fn,
                        int *win_keyval, void *extra_state, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(329, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 329, v, win_copy_attr_fn, win_delete_attr_fn,
                        win_keyval, extra_state);
    return ret;
}
/* ================== C Wrappers for MPI_Win_delete_attr ================== */

int E_Win_delete_attr(MPI_Win win, int win_keyval, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(330, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 330, v, win, win_keyval);
    return ret;
}
/* ================== C Wrappers for MPI_Win_detach ================== */

int E_Win_detach(MPI_Win win, const void *base, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(331, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 331, v, win, base);
    return ret;
}
/* ================== C Wrappers for MPI_Win_fence ================== */

int E_Win_fence(int assert, MPI_Win win, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(332, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 332, v, assert, win);
    return ret;
}
/* ================== C Wrappers for MPI_Win_flush ================== */

int E_Win_flush(int rank, MPI_Win win, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(333, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 333, v, rank, win);
    return ret;
}
/* ================== C Wrappers for MPI_Win_flush_all ================== */

int E_Win_flush_all(MPI_Win win, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(334, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 334, v, win);
    return ret;
}
/* ================== C Wrappers for MPI_Win_flush_local ================== */

int E_Win_flush_local(int rank, MPI_Win win, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(335, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 335, v, rank, win);
    return ret;
}
/* ================== C Wrappers for MPI_Win_flush_local_all ==================
 */

int E_Win_flush_local_all(MPI_Win win, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(336, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 336, v, win);
    return ret;
}
/* ================== C Wrappers for MPI_Win_free ================== */

int E_Win_free(MPI_Win *win, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(337, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 337, v, win);
    return ret;
}
/* ================== C Wrappers for MPI_Win_free_keyval ================== */

int E_Win_free_keyval(int *win_keyval, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(338, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 338, v, win_keyval);
    return ret;
}
/* ================== C Wrappers for MPI_Win_get_attr ================== */

int E_Win_get_attr(MPI_Win win, int win_keyval, void *attribute_val, int *flag,
                   int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(339, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 339, v, win, win_keyval, attribute_val, flag);
    return ret;
}
/* ================== C Wrappers for MPI_Win_get_errhandler ==================
 */

int E_Win_get_errhandler(MPI_Win win, MPI_Errhandler *errhandler, int i,
                         vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(340, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 340, v, win, errhandler);
    return ret;
}
/* ================== C Wrappers for MPI_Win_get_group ================== */

int E_Win_get_group(MPI_Win win, MPI_Group *group, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(341, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 341, v, win, group);
    return ret;
}
/* ================== C Wrappers for MPI_Win_get_info ================== */

int E_Win_get_info(MPI_Win win, MPI_Info *info_used, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(342, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 342, v, win, info_used);
    return ret;
}
/* ================== C Wrappers for MPI_Win_get_name ================== */

int E_Win_get_name(MPI_Win win, char *win_name, int *resultlen, int i,
                   vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(343, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 343, v, win, win_name, resultlen);
    return ret;
}
/* ================== C Wrappers for MPI_Win_lock ================== */

int E_Win_lock(int lock_type, int rank, int assert, MPI_Win win, int i,
               vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(344, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 344, v, lock_type, rank, assert, win);
    return ret;
}
/* ================== C Wrappers for MPI_Win_lock_all ================== */

int E_Win_lock_all(int assert, MPI_Win win, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(345, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 345, v, assert, win);
    return ret;
}
/* ================== C Wrappers for MPI_Win_post ================== */

int E_Win_post(MPI_Group group, int assert, MPI_Win win, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(346, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 346, v, group, assert, win);
    return ret;
}
/* ================== C Wrappers for MPI_Win_set_attr ================== */

int E_Win_set_attr(MPI_Win win, int win_keyval, void *attribute_val, int i,
                   vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(347, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 347, v, win, win_keyval, attribute_val);
    return ret;
}
/* ================== C Wrappers for MPI_Win_set_errhandler ==================
 */

int E_Win_set_errhandler(MPI_Win win, MPI_Errhandler errhandler, int i,
                         vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(348, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 348, v, win, errhandler);
    return ret;
}
/* ================== C Wrappers for MPI_Win_set_info ================== */

int E_Win_set_info(MPI_Win win, MPI_Info info, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(349, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 349, v, win, info);
    return ret;
}
/* ================== C Wrappers for MPI_Win_set_name ================== */

int E_Win_set_name(MPI_Win win, const char *win_name, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(350, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 350, v, win, win_name);
    return ret;
}
/* ================== C Wrappers for MPI_Win_shared_query ================== */

int E_Win_shared_query(MPI_Win win, int rank, MPI_Aint *size, int *disp_unit,
                       void *baseptr, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(351, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 351, v, win, rank, size, disp_unit, baseptr);
    return ret;
}
/* ================== C Wrappers for MPI_Win_start ================== */

int E_Win_start(MPI_Group group, int assert, MPI_Win win, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(352, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 352, v, group, assert, win);
    return ret;
}
/* ================== C Wrappers for MPI_Win_sync ================== */

int E_Win_sync(MPI_Win win, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(353, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 353, v, win);
    return ret;
}
/* ================== C Wrappers for MPI_Win_test ================== */

int E_Win_test(MPI_Win win, int *flag, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(354, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 354, v, win, flag);
    return ret;
}
/* ================== C Wrappers for MPI_Win_unlock ================== */

int E_Win_unlock(int rank, MPI_Win win, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(355, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 355, v, rank, win);
    return ret;
}
/* ================== C Wrappers for MPI_Win_unlock_all ================== */

int E_Win_unlock_all(MPI_Win win, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(356, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 356, v, win);
    return ret;
}
/* ================== C Wrappers for MPI_Win_wait ================== */

int E_Win_wait(MPI_Win win, int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(357, &f_dl, (*VECTOR_GET(v, i)).table);
    int ret = EXEC_FUNC(f_dl, i, 357, v, win);
    return ret;
}
/* ================== C Wrappers for MPI_Wtick ================== */

double E_Wtick(int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(358, &f_dl, (*VECTOR_GET(v, i)).table);

    // int ret=EXEC_FUNC(f_dl,i,358,v);
    int level = QMPI_GET_LEVEL(i, 358, v);
    int ret = 0;
    if (f_dl == NULL) {
        printf("function pointer for execution is null,function can't be "
               "executed , returns \n");
        return ret;
    } else {
        typedef double (*wtick_func)(int level, vector *v);
        wtick_func QMPI_Wtick_ptr = (wtick_func)f_dl;
        double ret_Wtick = (QMPI_Wtick_ptr)(level, v);
        return ret_Wtick;
    }
}
/* ================== C Wrappers for MPI_Wtime ================== */

double E_Wtime(int i, vector *v) {
    void *f_dl = NULL;
    QMPI_TABLE_QUERY(359, &f_dl, (*VECTOR_GET(v, i)).table);

    // int ret=EXEC_FUNC(f_dl,i,359,v);
    int level = QMPI_GET_LEVEL(i, 359, v);
    int ret = 0;
    if (f_dl == NULL) {
        printf("function pointer for execution is null,function can't be "
               "executed , returns \n");
        return ret;
    } else {
        typedef double (*wtime_func)(int level, vector *v);
        wtime_func QMPI_Wtime_ptr = (wtime_func)f_dl;
        double ret_Wtime = (QMPI_Wtime_ptr)(level, v);
        return ret_Wtime;
    }
}
