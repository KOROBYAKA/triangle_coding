#include <cstdlib>
#include <fstream>
#include <iostream>
#include <math.h>
#include <stdarg.h>
#include "triangular_sources.h"

/*void fill_matrix(int size, matrix& to_fill, packet_header& header){};*/

//Matrix struct constructor
//x - amount of coding matrixes
//y - amount of original packets used in one coded packet (lines)
//z - packet length (lines length)
Matrix::Matrix(int x, int y, int z){
    this->x = x;
    this->y = y;
    this->z = z;
    this->data.resize(x,vector<vector<char>>(y,vector<char>(z, 'u')));
};

//packet_length = original packet size in bits
void fill_coding_matrix(matrix* matrix_ptr,int matrix_id,vector<packet*> packets_vector, int packet_length,
    int zero_bits, int packets_amount, int batch_size)
{
    matrix2D& matr = matrix_ptr->data[matrix_id];//Takes desirable coding matrix from all matrixes
    int j;
    for(int i=0;i<packets_amount-1;i++){
        packet* pk = packets_vector[(i+matrix_id)%batch_size];
        vector<char>& row = matr[i];
        //Adding header zeroes
        for(j=0;j<i;j++){
            row[j]='0';
        }
        //Filling matrix with original packet data
        for(j=j;j<(i+packet_length);j++){
            row[j] = pk->data[j-i];
        }
        //Padding with zeroes
        for(j=j;j<packet_length;j++){
            row[j]='0';
        }
    }
    //Filling last packet
    for(j=0;j<zero_bits;j++){
        matr[packets_amount-1][j] = '0';
    }
    for(j=j;j<(packet_length);j++){
        matr[packets_amount-1][j] = packets_vector[(matrix_id+packets_amount-1)%batch_size]->data[j-zero_bits];
    }
}

//Packet struct constructor
Packet::Packet(int size){
    data = vector<char>(size,'u');
}

void generate_original_packets(vector<packet*> packets_vector, int batch_size, int packet_size){
    for(int i=0;i<batch_size;i++){
        packet pk = packet(packet_size);
        packets_vector[i] = &pk;
        for(int j=0;j<packet_size;j++){
            //filling a packet with random 0 or 1
            packets_vector[i][j] = rand()%2;
        }
    }
}
