// A compiler avec :
// g++ -o client client.cc -O3 `pkg-config --libs --cflags bkbd-1.0`

#include <cstdlib>
#include <string>
#include <math.h>
#include <bkbd.h>

#define SUPELECBOARD_HOSTNAME "localhost"
#define SUPELECBOARD_PORT 10000
#define VECTOR_RESNAME "Vector"
#define VECTOR_ENTNAME "action-wheel-speed"

// Some typedefs for bkbd
typedef bkbd::Vector                               VectorType;
typedef bkbd::Repository<VectorType,1,0>::Client VectorClient;

int main(int argc, char * argv[])
{
    VectorType v;
    VectorClient* client;  
    struct timeval tv;

    std::string board_hostname = SUPELECBOARD_HOSTNAME;
    int board_port = SUPELECBOARD_PORT;
    std::string vec_resname = VECTOR_RESNAME;
    std::string vec_entname = VECTOR_ENTNAME;

    // Connect the bkbd client
    client = new VectorClient(vec_resname);
    if(!client->Connect(board_hostname,board_port)) {
      std::cerr << "Connection failed to supelecboard." << std::endl;
      return -1;
    }

    // Reset the speed
    v.data.clear();
    v.data.push_back(0.0);
    v.data.push_back(0.0);
    client->post(vec_entname,v);

    // Make a turn to the right
    v.data[0] = 25.0;
    v.data[1] = -25.0;
    client->post(vec_entname,v);

    // Sleep 
    sleep(2);

    // Make a turn to the left
    v.data[0] = -25.0;
    v.data[1] = 25.0;
    client->post(vec_entname,v);

    // Sleep 
    sleep(2);

    // Move ahead
    v.data[0] = 25.0;
    v.data[1] = 25.0;
    client->post(vec_entname,v);

    // Sleep 
    sleep(2);

    // Move back
    v.data[0] = -25.0;
    v.data[1] = -25.0;
    client->post(vec_entname,v);

    // Sleep 
    sleep(2);

    // Reset the speed
    v.data[0] = 0.0;
    v.data[1] = 0.0;
    client->post(vec_entname,v);


    // That's it !
    client->Disconnect();
}






