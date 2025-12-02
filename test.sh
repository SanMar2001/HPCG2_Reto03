#!/bin/bash

SEQ="./sequential"
MPI="./mpi_version"
HOSTFILE="hosts.txt"

SIZES=(100000 500000 1000000 5000000 10000000)
PROCS=(2 4 6 8)

SEQ_OUT="seq_results.txt"
MPI_OUT="mpi_results.txt"

> "$SEQ_OUT"
> "$MPI_OUT"


for N in "${SIZES[@]}"; do
    for i in {1..10}; do
        RESULT=$($SEQ $N)
        echo "$N $RESULT" >> $SEQ_OUT
    done
done

for N in "${SIZES[@]}"; do
    for P in "${PROCS[@]}"; do
        for i in {1..10}; do
            RESULT=$(mpirun -np $P -hostfile $HOSTFILE $MPI $N)
            echo "$N $P $RESULT" >> $MPI_OUT
        done
    done
done
