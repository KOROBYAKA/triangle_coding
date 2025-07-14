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

    double receive_p = 1; // Probability of receiving a packet

    double avg_sparsity = log(batch_size); //The expected number of source packets
    //involved in each coded packet, on average

    int encoding_redundant_bits[pks_pool]; // used to store the number of redundant
    //bits added to each coded packets.

    int decoding_redundant_bits[pks_pool]; // the effective redundant bits needed for a
    //sparse coded packet

    int crahs = 0; // The crash variable keeps a track of the decoder failure. In an event of
    // decoder failure, the decoder "requests" for additional coded packet from
    // the sender.

    //Generating original data packets
    vector<packet*> original_packets;
    generate_original_packets(original_packets,batch_size,(data_length*8));
    int ceil(avg_sparsity);









    return 0;
}
