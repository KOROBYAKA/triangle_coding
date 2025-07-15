#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <math.h>
#include <stdarg.h>
#include <vector>
#include "triangular_sources.h"

#define XOR(a,b)  (a == b)? '0':'1'

//Matrix struct constructor
//x - row length in matrix
//y - column length in matrix
Matrix::Matrix(int x, int y){
    this->x = x;
    this->y = y;
    this->data = vector(y,vector<char>(x, 'u'));
};

void print_matrix(matrix* matrix_ptr){
    cout<<"Matrix params(rows:"<<matrix_ptr->y<<",columns"<<matrix_ptr->x<<")"<<endl;
    for(int i=0;i<matrix_ptr->y;i++){
        for(int j=0;j<matrix_ptr->x;j++){
            cout<<matrix_ptr->data[i][j];
        }
        cout<<endl;
    }
}



//packet_length = original packet size in bits
void fill_matrix(matrix* to_fill,int matrix_id,vector<packet*>* packets_vector, int packet_length, int zero_bits, int packets_amount, int batch_size){
    int j;
    //cout<<"[DEBUG]\nStart encoding"<<endl;
    for(int i=0;i<packets_amount-1;i++){
        //cout<<"[DEBUG]\nPacket ["<<i<<"] encoding: start"<<endl;
        packet* pk = (*packets_vector)[(i+matrix_id)%batch_size];
        vector<char>& row = to_fill->data[i];
        //Adding header zero
        for(j=0;j<i;j++){
            row[j]='0';
        }
        //cout<<"[DEBUG]\nPacket ["<<i<<"] encoding: header zeroes added"<<endl;
        //Filling matrix with original packet data
        for(j=j;j<(i+packet_length);j++){
            row[j] = pk->data[j-i];
        }
        //cout<<"[DEBUG]\nPacket ["<<i<<"] encoding: packet data added"<<endl;
        //Padding with zeroes
        for(j=j;j<(packet_length+zero_bits);j++){
            row[j]='0';
        }
        //cout<<"[DEBUG]\nPacket ["<<i<<"] encoding: padding zeroes aded"<<endl;
    }
    //Filling last packet
    //cout<<"[DEBUG]\nEncoding last packet/row"<<endl;
    for(j=0;j<zero_bits;j++){
        to_fill->data[packets_amount-1][j] = '0';
    }
    for(j=j;j<(packet_length+zero_bits);j++){
        to_fill->data[packets_amount-1][j] = (*packets_vector)[(matrix_id+packets_amount-1)%batch_size]->data[j-zero_bits];
    }
}

//Packet struct constructor
Packet::Packet(int size, int packet_id){
    this->data = vector<char>(size,'u');
    this->packet_id = packet_id;
    this->size = size;
}

void print_packet(packet* pk){
    cout<<"Printing a packet[ID"<<pk->packet_id<<"]:";
    for(int i=0;i<pk->size;i++){
        cout<<pk->data[i];
    }
    cout<<endl;
}

void generate_original_packets(vector<packet*>& packets_vector, int batch_size, int packet_size){
    for(int i=0;i<batch_size;i++){
        packet* pk = new packet(packet_size, i);
        packets_vector[i] = pk;
        //cout<<"generating a packet:";
        for(int j=0;j<packet_size;j++){
            //filling a packet with random 0 or 1
            pk->data[j] = (rand() % 2) ? '1' : '0';
            //cout<<packets_vector[i]->data[j];
        }
        //cout<<endl;
    }
}

void encode_packet(matrix* matrix_ptr, packet* pk){
    vector<char> buff(matrix_ptr->y);
    char res;
    int j;
    for(int i=0;i<matrix_ptr->x;i++){
        for(j=0;j<matrix_ptr->y;j++){
            buff[j] = matrix_ptr->data[i][j];
        }
        res = XOR(buff[0],buff[1]);
        for(j=2;size_t(j)<buff.size();j++){
            res = XOR(res,buff[j]);
        }
        pk->data[i] = res;
    }
}
