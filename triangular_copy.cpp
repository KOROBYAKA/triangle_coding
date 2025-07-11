#include <iostream>
#include <fstream>
#include <cstdlib>
#include <math.h>
#include <stdarg.h>

using namespace std;

ofstream out0 ("results.txt", ios::out);

int const M = 20; //Packet batch size ("k"" in the paper)

int const zero_bits = 8; //The maximum of zero bits for which add to packet
int interval = zero_bits;

int const packet_length = 5; //Number of bytes in a packet

int const B = zero_bits + (8*packet_length); //A packet size with padding

int const pks_pool = M + 100;


double p_receive = 1; //Packet receive probability, 0<p_receive<=1

double average_sparsity = log(M); //

int const max_degree = 50; //The maximum number of original packets (M)
//that can participate in the encoding of a single coded packet.

int r[pks_pool]; //Used to store the number of redundant bits added to any packet
int redundant_bits[pks_pool]; //the effective redundant bits needed for a
//sparse coded packet

int minimum[pks_pool];//the minimum number of redundant bits added to any
//packet before coding, assuming sparse coding.

int pk_header[pks_pool][M]; //packet header to include information about the number of
//'0' bits added at the head of each packet before coding.

bool matrices[pks_pool][max_degree][B+zero_bits]; //used for decoding, back-substitution.

int pks_ids[M]; // (0,1,2,...,M-1)_1

int last_known_col[pks_pool]; //last unknown column from each of the pks_pool matrices.
int last_unknown[pks_pool][M]; //used in substitution.

double rng; // gives a uniform random number between 0 and 1.
int seed = 0;
int const total_seeds = 50; //seed + 500;


int degree_one = 0; //number of degree one packets.

int pk_received = 0;
double transmitted = 0;

void create_matrix(int);
int redundancy(int);

int find_one_unknown();
void substitution(int);
bool B_solved();

void refresh_set(int);

int available_set[M];
int set_cardinality = 0;

int last_matrix = 0;

int M_size[pks_pool]; //0<=M_size<=sparsity, i.e. the number of
//source packets used to encode a packet.
int pk_codedin[pks_pool][max_degree]; //stores the id of source
//packet to generate a coded packet.

//the followings are used to reduce the computation cost of substitution()
int row_used[M];
int row_codedin[M][M];

int row_id[M][M];

int min(double, double);


int main()
{
    int crash = 0; // The crash variable keeps a track of the decoder failure.
    //In an event of decoder failnture, the decoder "requests" for additional
    //coded packet from the sender.
    double total_received = 0;
    double optimal = M * total_seeds;

    double total_redundant_bits = 0;

    for(int seed=0; seed<total_seeds;seed++){
            cout <<"p_receive:"<< p_receive << " seed:" << seed << "  ";
            pk_received = 0;
            set_cardinality = M;


            //initialisation
            for(int p=0;p<pks_pool;p++){
                redundant_bits[p] = 0;
                last_known_col[p] = 0;
                M_size[p] = 0;
                row_used[p] = 0;

                for(int m=0;m<max_degree;m++){
                    pk_codedin[p][m] = 0;

                    for(int b=0;b<(B+zero_bits);b++){
                        matrices[p][m][b] = true;
                    }
                }

                for(int m=0;m<M;m++){
                    pk_header[p][m] = 0;
                    last_unknown[p][m] = 0;
                    available_set[m] = m;
                }
            }
    }
    //Initialisation is done

    srand(seed);

    degree_one = 0; //Number of degree one packets(?)
    int one_sparse[M]; //The ID of degree one packets

    double  sparsity = ( ( (average_sparsity)
        - (double(pk_received)/double(M)) )
        /(double(M)-double(pk_received)) );

    int interval = zero_bits;

    int row = 0;
    transmitted = 0;

    for(int m=0;m<M;m++){
        pks_ids[m]=m;
    }

    int counter = 0;

    do{


    while( (pk_received<M) || (row==-1) ){
            transmitted += 1;


            for(int m=0;m<M;m++){
                pks_ids[m] = -1;
            }
            rng = rand()/(static_cast<double>(RAND_MAX) + 1);
            int degree = ceil(sparsity * double(M));
            // ******************************************
    		// LT Code degree distribution should be
    		// inserted here. When LT code is not used
    		// this codeblock should be commented.
    		// ******************************************
    		// K=8

            if (rng<0.268) {degree=1;}
            else if (rng<0.759) {degree=2;}
            else if (rng<0.844) {degree=3;}
            else if (rng<0.943) {degree=4;}
            else if (rng<0.956) {degree=5;}
            else if (rng<0.983) {degree=6;}
            else if (rng<0.993) {degree=7;}
            else {degree=8;}

            for(int m=0;m<degree;m++){
            int index = floor(rand()/(static_cast<double>(RAND_MAX) + 1)*set_cardinality);
            // identifies the set of source packets to be coded
            pks_ids[available_set[index]] = floor(rand()/(static_cast<double>(RAND_MAX) + 1)*interval);
            // assigns the shift which will be applied to the packet before coding.

            refresh_set(index);//refresh the available_set values
            }

            rng = rand()/(static_cast<double>(RAND_MAX)+1);

            if(rng<p_receive){ //packet loss simulation
                minimum[pk_received] = B; //minimum packet size with shifting
                int max = 0; //largest shift

                for(int m=0;m<M;m++){ //iterating all source packets
                    pk_header[pk_received][m] = pks_ids[m]; //stores the shift
                    //(number of leading zeroes) that was applied to the packet

                    if(pks_ids[m]!=-1){
                        pk_codedin[pk_received][M_size[pk_received]] = m;
                        row_codedin[m][row_used[m]] = pk_received;
                        row_id[m][row_used[m]] = M_size[pk_received];
                        row_used[m]++;
                        M_size[pk_received]++;
                    }
                    if((pks_ids[m]>max)&&(pks_ids[m]!=-1)){
                        max = pks_ids[m];
                    }
                    if ((pks_ids[m]<minimum[pk_received])&&(pks_ids[m]!=-1)){
                        minimum[pk_received] = pks_ids[m];
                    }
                }

                for (int m=0;m<M;m++){
                    if(pk_header[pk_received][m]!=-1){
                        pk_header[pk_received][m] = pk_header[pk_received][m]-minimum[pk_received];
                    }
                }

                last_known_col[pk_received] = 0;//minimum[pk_received]

                redundant_bits[pk_received] = max - minimum[pk_received];
                total_redundant_bits += redundant_bits[pk_received];

                r[pk_received] = redundant_bits[pk_received];

                create_matrix(pk_received);

                pk_received++;
                row=0;
            }
        }
        do{
            row = find_one_unknown();

            if(row==-1){
                crash++;

                //create original matrix from scratch
                for(int p=0;p<pk_received;p++){
                    create_matrix(p);
                    last_known_col[p] = 0;

                    for(int m=0;m<M;m++){
                        last_unknown[p][m]=0;
                    }
                }
                last_matrix = degree_one;
                break;
            }
            else if (row==-2){
                break;
            }
            substitution(row);
        } while(row>-1);
    } while(row==-1);
}


  //----------//
 //FUNCTIONS//
//--------//


void refersh_set(int index){
    available_set[index] = available_set[set_cardinality-1];
    set_cardinality--;

    if(set_cardinality==0){
        set_cardinality=M;

        for(int m=0;m<M;m++){
            available_set[m]=m;
        }
    }
}

//Finds an equation with one unknown
int find_one_unknown(){

    int counts = pk_received-degree_one;
    int counter = 0;

    for(int pks=last_matrix;counter<counts;pks++){

        if(pks==pk_received)pks=degree_one;

        double bits = min(B+r[pks], last_known_col[pks]+ ceil(( (transmitted/M)+1) *M) );

        for(int b=last_known_col[pks];b<bits;b++){
            int unknown = 0;
            int row_index = -1;

            //only add the entries of those row, which have been used for coding
            for(int m=0;m<M_size[pks];m++){
                if(!matrices[pks][m][b]){
                    unknown++;
                    row_index = pk_codedin[pks][m];
                }

            if(unknown==1){
                last_known_col[pks] = b;
                last_matrix = pks + 1;
                if(last_matrix==pk_received) last_matrix = degree_one;
                return row_index;
            }
            if(unknown>1) break;
            }
        counter++;
        }
    }
}


void create_matrix(int p){
    for(int m=0;m<M_size[p];m++){
        int pk = pk_codedin[p][m];

        for(int z=0;z<pk_header[p][pk]; z++){
            matrices[p][m][z] = true;//known '0'bits
        }

        for(int t=pk_header[p][pk];t<B+pk_header[p][pk];t++){
            matrices[p][m][t] = false;//unknown bits of packet
        }

        for(int z=B+pk_header[p][pk]; z<B+r[p]; z++){
            matrices[p][m][z] = true; //known '0' bits
        }
    }
}

int min(double a, double b){
    return a>b? a:b;
}


bool B_solved() {
    for(int pks=0;pks<pk_received;pks++){

        for(int m=0;m<M_size[pks];m++){
            for(int b=0;b<B+r[pks];b++){
                if(matrices[pks][m][b]==false)return false;
            }
        }
    }
    return true;
}

void substitution(int row){
    for(int pks=0;pks<row_used[row];pks++){
        int p = row_codedin[row][pks];
        int q = row_id[row][pks];

        for(int b=last_unknown[p][q]; b<B+r[p];b++){

            if(!matrices[p][q][b]){
                matrices[p][q][b]=true;
                last_unknown[p][q]=b+1;
                break;
            }
        }
    }
}
