#include <iostream>
#include <fstream>
#include <cstdlib>
#include <math.h>
#include <stdarg.h>
#include <vector>
#include "triangular_sources.h"


//save output to file
ofstream out0 ("results.txt", ios::out);

int main(){
    //Initialising mandatory integers & system characteristics
    const int batch_size = 20; // Packet size batch, "k" in the paper

    const int zero_bits = 8; // The maximum of zero bits for which add to packet

    const int data_length = 5; // Data size of original packet in bytes

    const int packet_size = (data_length*8) + zero_bits;

    const int max_degree = 50; //The maximum number of original packets
    //that can participate in the encoding of a single coded packet.

    const int pks_pool = batch_size * 2;
    // pks_pool is the maximum expected number of packets which the decoder need to
    // have before successful decoding. At minimum a decoder needs to collect at
    // least k packets before decoding could be successful. pks_pool>=pk_received
    // because some of the received coded packet maWhere:y be linearly dependent.

    const double receive_p = 1; // Probability of receiving a packet

    const double avg_sparsity = log(batch_size); //The expected number of source packets
    //involved in each coded packet, on average

    int crahs = 0; // The crash variable keeps a track of the decoder failure. In an event of
    // decoder failure, the decoder "requests" for additional coded packet from
    // the sender.

    //Generating original data packets
    vector<packet*> original_packets(batch_size);
    //cout<<"[DEBUG]"<<endl<<"Creating original packets"<<endl;
    generate_original_packets(original_packets, batch_size, (data_length*8));
    int packets_in_matrix = ceil(avg_sparsity);

    //Print generated otiginal packets
    //cout<<"[DEBUG]\n"<<original_packets.size()<<endl;
    /*for(size_t i=0;i<original_packets.size();i++){
        print_packet(original_packets[i]);
    }
    cout<<"[DEBUG]\nEncoding matrixes: allocating"<<endl;*/

    //Filling encoding matrixes
    vector<matrix*> encoding_matrixes(batch_size);
    for(int i=0;i<batch_size;i++){
    encoding_matrixes[i] = new matrix(packet_size,packets_in_matrix);
    }
    //cout<<"[DEBUG]\nEncoding matrixes are allocated"<<endl;
    for(int i=0;i<batch_size;i++){
        fill_matrix(encoding_matrixes[i],i,&original_packets, data_length*8, zero_bits, packets_in_matrix, batch_size);
    }

    //Print encoding matrixes
    /*cout<<"[DEBUG]\nEncoding matrixes are filled:"<<endl;
    for(int i=0;i<batch_size;i++){
        print_matrix(encoding_matrixes[i]);
        }*/

    //Initializing coded packets vector
    vector<packet*> coded_packets(batch_size);
    for(int i=0;i<batch_size;i++){
        coded_packets[i] = new packet(packet_size,i);
    }

    //Encoding packets
    for(int i=0;i<batch_size;i++){
        encode_packet(encoding_matrixes[i],coded_packets[i]);
    }

    //Print encoded coding packets and corresponding encoding matrixes
    /*for(size_t i=0;i<batch_size;i++){
        cout<<endl;
        cout<<"encoding matrix:\n";
        print_matrix(encoding_matrixes[i]);
        cout<<"coded packet:\n";
        print_packet(coded_packets[i]);
        cout<<endl;
    }*/


    //Send packets simulation
    double rng = (rand()%100)/100;





    return 0;
}
