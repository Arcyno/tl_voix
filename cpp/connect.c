/****************************************************
 *
 *		connect.c
 *
 *
 *		Jean-Louis Gutzwiller.
 *
 *
 *		Appel par :
 *
 *
 *				connect  machine port		: pour le mode client
 *
 *				connect	 port				: pour le mode serveur
 *
 *
 *
 *	Ce programme montre comment réaliser un client ou un serveur.
 *	Le serveur ne peut traiter qu'un seul client à la fois.
 *
 *
 *	Ce code peut être compilé indifféremment sous Windows
 *  (en natif ou sous Wygwin) ou encore sous Linux.
 *  Pour la compilation en natif sous Windows, utiliser
 *	le fichier make.bat.
 *
 ****************************************************/

// compilation : gcc connect.c -o connect -Wall -g -D_LINUX_

#ifdef _POUR_WINDOWS_
	#ifdef _LINUX_
	#undef _LINUX_
	#endif
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include <windows.h>
	#include <math.h>
	#include <winsock.h>
#else
	#define _POUR_LINUX_
	#define closesocket(x) close(x)
	#include <stdio.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <string.h>
	#include <stdlib.h>
	#include <unistd.h>
#endif


#ifdef _POUR_WINDOWS_
static int WSAStarted = 0;			// Si 0, DLL windows non encore initialisée.
#endif

/*********************************************************************/

#ifdef _POUR_WINDOWS_
void bzero(char * b, int s) {
	int i;
	for(i = 0; i < s; i++) b[i] = 0;
}
#endif

/*********************************************************************/

int demarreServeur(int port, char * name);

	// Démarre un serveur.
	// Renvoit le socket sur lequel on réalise un accept.
	// Si nom est NULL, le serveur démarre sur l'adresse
	// 0.0.0.0, ce qui lui permet d'accepter des clients
	// sur n'importe quel coupleur. Sinon, il démarre sur
	// l'adresse IP liée au nom, ce qui ne lui fait accepter
	// des requêtes que depuis le coupleur correspondant.
	// L'utilisation du nom "localhost" ou de l'adresse
	// ip "127.0.0.1" permet en particulier de se protéger
	// contre les accès depuis le réseau. Seuls les accès
	// depuis la machine locale sont autorisés.

int demarreServeur(int port, char * name) {
	#ifdef _POUR_WINDOWS_
	WSADATA wsaData;
	#endif
    struct sockaddr_in server;
    int Socket;
    char hostname[30];
    struct hostent *hostinfo;


	#ifdef _POUR_WINDOWS_
	if (WSAStarted == 0) {
		WSAStartup(MAKEWORD( 2, 0 ), &wsaData);
		WSAStarted = 1;
	}
	#endif
	if (name == NULL)
		strcpy(hostname, "0.0.0.0");
	else if (strcmp(name, "-") == 0)
		strcpy(hostname, "0.0.0.0");
	else
		strcpy(hostname, name);

	hostinfo=gethostbyname(hostname);
	if (hostinfo == NULL) exit(1);
    bzero((char*)&server, sizeof(server));
    memcpy(&server.sin_addr, hostinfo->h_addr, hostinfo->h_length);
    server.sin_family=AF_INET;
    server.sin_port=htons(port);
    Socket=socket(AF_INET,SOCK_STREAM,0);
    if (Socket<=0) { 
        fprintf(stderr, "Erreur à l'allocation du socket serveur\n");
        exit(1);
    };
    if (bind(Socket,(struct sockaddr *)&server,sizeof(server))) {
        printf("Erreur au bind.\n");
        exit(1);
    };
    if (listen(Socket,10)) {
        printf("Erreur au listen\n");
        exit(1);
    };

	return Socket;
}

/*********************************************************************/

int accepteClient(int Socket, struct sockaddr_in * client);

	// Accepte un client sur le socket indiqué.
	// Le serveur doit avoir été démarré au préalable
	// en utilisant la fonction "demarreServeur"
	// ci-dessus.

int accepteClient(int Socket, struct sockaddr_in * client) {
	#ifdef _POUR_WINDOWS_
	int l;
	#endif
	#ifdef _POUR_LINUX_
	unsigned int l;
	#endif

	// Acceptation du client courant et détermination
	// de sa provenance :

	l = sizeof(struct sockaddr_in);
	return accept(Socket,(struct sockaddr *)client,&l);	
}

/*********************************************************************/

int appelleClient(int port, char * name);

	// Appelle un client par son nom et un numéro de port.
	// Un serveur doit avoir été démarré sur la machine
	// cliente pour que cet appel puisse fonctionner.

int appelleClient(int port, char * name) {
	#ifdef _POUR_WINDOWS_
	WSADATA wsaData;
	#endif
    struct hostent *hostinfo;
	struct sockaddr_in server;
	int Socket;

	#ifdef _POUR_WINDOWS_
	if (WSAStarted == 0) {
		WSAStartup(MAKEWORD( 2, 0 ), &wsaData);
		WSAStarted = 1;
	}
	#endif

	hostinfo=gethostbyname(name);
	if (hostinfo == NULL) return 0;
	bzero((char*)&server, sizeof(server));
    memcpy(&server.sin_addr, hostinfo->h_addr, hostinfo->h_length);
    server.sin_family=AF_INET;
    server.sin_port=htons(port);
    Socket=socket(AF_INET,SOCK_STREAM,0);
    if (connect(Socket, (struct sockaddr *)&server, sizeof(server)))
		return 0;
	else
		return Socket;
}


/*********************************************************************/


int main(int argc, char * argv[]) {
    int Socket;


	// Vérification des arguments :
	if (argc < 2) {
		fprintf(stderr, "Appel par : %s <machine> <port>.\n", argv[0]);
		exit(1);
	}



	if (argc > 2) {		// Mode client :
		char message[100];
		int longueur;


		// Appel d'un client :
		Socket = appelleClient(atoi(argv[2]), argv[1]);
		if (Socket == 0) {
			fprintf(stderr, "Erreur d'ouverture de la liaison vers le client.\n");
			exit(1);
		}

		longueur = recv(Socket, message, 100, 0);
		message[longueur] = 0;
		printf("%s\n", message);

		// Fin : termine le client.
		closesocket(Socket);
		return 0;
	}
	
	
	else {				// Mode serveur :
		struct sockaddr_in client;
		Socket = demarreServeur(atoi(argv[1]), NULL);
		if (Socket == 0) {
			fprintf(stderr, "Erreur au démarrage du serveur.\n");
			exit(1);
		}
		while (1) {
			int s;
			s = accepteClient(Socket, &client);
			if (s < 0) {
				fprintf(stderr, "Erreur a l'acceptation du client.\n");
			} else {
				send(s, "Bonjour.\n", strlen("Bonjour.\n"), 0);
				closesocket(s);
			}
		}
	}
}
