#include <vector>
using namespace std;

typedef vector<vector<char>> matrix2D;

//Matrix struct constructor
//x - amount of coding matrixes
//y - amount of original packets used in one coded packet
//z - packet length
struct Matrix{
    vector<vector<vector<char>>> data;
    int x,y,z;

    Matrix();
    Matrix(int x, int y, int z);
};
typedef struct Matrix matrix;


//Structure for packets, size in bits
struct Packet{
    vector<char> data;
    int size;

    Packet();
    Packet(int size);
};
typedef struct Packet packet;

//Generates original packets, full fills them with some pseudorandom char values
//Packet size must be in bits
void generate_original_packets(vector<packet*> packets_vector, int batch_size, int packet_size);

//packet_lentgh = original packet size in bits
void fill_coding_matrix(matrix* matrix_ptr,int matrix_id,vector<packet*> packets_vector, int packet_length,
    int zero_bits, int packets_amount, int batch_size)





//void fill_matrix(int size, matrix& to_fill, packet_header& header);
