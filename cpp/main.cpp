#include "frame.h"
#include "morceau.h"
#include <sndfile.h>
#include <stdio.h>
#include <iostream>
#include <list>
#include "parameters.h"
#include "debug.h"
#include <cmath>
#include <map>
#include "serie.h"
#include <string.h>
#include <unistd.h>
#include <sstream>


// regarder avec ancienne base
// delta

Morceau lecture(const char* fileName, int classe_donne = 0){
    /*
    Cette fonction prend en argument le nom d'un fichier .wav
    et sa classe si elle est connue
    Elle renvoie l'objet de type Morceau correspondant
    */

    SF_INFO sfinfo;
    SNDFILE* infile;
    if ( !(infile = sf_open(fileName, SFM_READ, &sfinfo)) ){   
		// Open failed so print an error message.
    	printf ("Not able to open input file %s.\n", fileName) ;
    	sf_perror (NULL);
    	// return Morceau();
    } 	

    int f_ech = sfinfo.samplerate;
    
    // Chargement de constantes depuis parameters.h
    int ordre_lpc = ORDRE_LPC;
    int nb_mfcc = NB_MFCC;	
    int frames_length = TAILLE_FRAME;

    // Initialisations
    double data[frames_length/2];
    auto size_data = frames_length/2;
    std::list<Frame> frames;
    int n_frames = 0;
    double concat[frames_length];
    
    sf_read_double (infile, data, frames_length/2);
    std::copy(data, data + size_data, concat);
    // la variable concat contient les Frames qui sont ajoutees à la liste frames
    // elle est obtenue par concatenation de la deuxième moitié de la frame precedente
    // et du fruit de la lecture de la demi-frame suivante (data)
    // On a ainsi des trames qui se chevauchent
    while(frames_length/2 == sf_read_double (infile, data, frames_length/2)){
    	n_frames++;
    	std::copy(data, data + size_data, concat + size_data);
    	Frame frame = Frame(concat, frames_length, f_ech, ordre_lpc, nb_mfcc);
    	std::copy(data, data + size_data, concat);
    	frames.push_back(frame);
		//std::cout<< "dernier point de la frame n°" << n_frames << " : " << data[frames_length-1] << std::endl;
    }

    sf_close(infile);
    
    // On utilise le constructeur pour obtenir l'objet a renvoyer
    Morceau morceau = Morceau(frames, n_frames,fileName, classe_donne);

    return morceau;
};



double distance(Frame &f1, Frame &f2){
    /*
    Cette fonction renvoie la distance locale entre deux Frames passees en argument
    Il s'agit de la distance euclidienne entre les vecteurs de caracteristiques :
    caracteristiques = [LPC MFCC]
    */
    double dist = 0;
    double * mfcc1 = f1.get_mfcc();
    double * mfcc2 = f2.get_mfcc();
    for(int i = 0; i < NB_MFCC; i++){
    	dist += std::sqrt((mfcc1[i] - mfcc2[i])*(mfcc1[i] - mfcc2[i]));
    }
    double * lpc1 = f1.get_lpc();
    double * lpc2 = f2.get_lpc();
    for(int i = 0; i < ORDRE_LPC; i++){
    	dist += std::sqrt((lpc1[i] - lpc2[i])*(lpc1[i] - lpc2[i]))*0;
    }

    return dist;
}



double dtw(Morceau &m1, Morceau &m2){
    /*
    Cette fonction renvoie la distance globale entre deux morceaux passes en argument
    en utilisant l'algorithme DTW
    */
    // Le tableau distances contient les distances cumulees minimales en partant de la case (0,0)
    double distances[m1.get_n_frames()][m2.get_n_frames()];
    Frame* frames1 = m1.get_frames();
    Frame* frames2 = m2.get_frames();
    int i = 0;
    int j = 0;
    // On commence par renseigner la premiere ligne
    distances[0][0] = distance(frames1[0], frames2[0]);
    for(j = 1; j < m2.get_n_frames(); j++){
    	distances[0][j] = distances[0][j-1] + distance(frames1[0], frames2[j]);
    }
    //On renseigne les autres lignes
    for(i = 1; i < m1.get_n_frames(); i++){
    	distances[i][0] = distances[i-1][0] + distance(frames1[i], frames2[0]);
    	for(j = 1; j < m2.get_n_frames(); j++){
			// distances[i][j] = std::min(distances[i-1][j],distances[i][j-1]) + distance(frames1[i], frames2[j]);
    		distances[i][j] = std::min(distances[i-1][j-1],std::min(distances[i-1][j],distances[i][j-1])) + distance(frames1[i], frames2[j]);
    	}
    }
    // La valeur renvoyee est normalisee par la somme des longueurs des morceaux 
    // pour ne pas penaliser les morceaux de grande taille
    // (ou pour ne pas avantager les morceaux de petite taille)
    return distances[i-1][j-1]/(m1.get_n_frames()+m2.get_n_frames());
}



int classif(Morceau* base, int l_base, char* nom, int k){
    /*
    Cette fonction prend en argument une base de morceaux de classe connue
    et l'emplacement d'un morceau de classe inconnue
    k : parametre de l'algorithme de classification
    */
    // lecture du morceau de classe inconnue
    Morceau m = lecture(nom);
    // std::cout << "nom du fichier : " << nom << std::endl;

    // On construit une hashmap qui aura pour clefs les distances globales entre 
    // les morceaux de la base et le morceau de classe inconnue
    // et pour valeurs la classe du morceau de la base.
    std::map<double, int> hashmap;
    for (int i = 0; i < l_base; i++){
    	hashmap[dtw(m, base[i])] = base[i].get_classe();
    }
    
    // Comme une hashmap est triee par ordre croissant, 
    // il suffit de la parcourir dans l'ordre pour obtenir les plus petites distances
    int classes[4] = {0};
    for(auto it = hashmap.cbegin(); it != hashmap.cend(); ++it){
        // dans l'ordre de proximite des morceaux, on enregistre les classes obtenues
        // jusqu'à ce qu'on obtienne k fois la meme classe
        // C'est alors cette classe qui est choisie
    	classes[it->second-1] += 1;
		std::cout << "distance : " << it->first << ", classe : " << it->second << std::endl;
    	if(classes[it->second-1] == k){
    		// std::cout << "classe1 : " << classes[0] << std::endl;
    		// std::cout << "classe2 : " << classes[1] << std::endl;
    		// std::cout << "classe3 : " << classes[2] << std::endl;
    		// std::cout << "classe4 : " << classes[3] << std::endl;
    		std::cout << "classe choisie : " << it->second << std::endl;
    		return it->second;
    	}
    }
    
    // L'algorithme est mal dimensionne si on n'atteint jamais k fois la meme classe
    // Dans ce cas, adapter la valeur de k
    std::cout << "----- oups -----" << std::endl;
    return -1;
}

void commande(portSerie &port, int classe){
    /*
    Cette fonction prend en argument un port serie prealablement ouvert
    et lui transmet une consigne de deplacement
    correspondant a l'ordre passeen argument : classe
    */
    char consigneSTOP[] = "D,0,0\n";
    char consigne3[] = "D,10,10\n";
    char consigne2[] = "D,-20,20\n";
    char consigne1[] = "D,20,-20\n";
    switch (classe){
    	case 4:
    	std::cout << "STOP !" << std::endl;
    	ecriturePortSerie(port, consigneSTOP, strlen(consigneSTOP));
    	break;
    	case 3:
    	std::cout << "En avant !" << std::endl;
    	ecriturePortSerie(port, consigne3, strlen(consigne3));
    	break;
    	case 2:
    	std::cout << "A gauche !" << std::endl;
    	ecriturePortSerie(port, consigne2, strlen(consigne2));
    	break;
    	case 1:
    	std::cout << "A droite !" << std::endl;
    	ecriturePortSerie(port, consigne1, strlen(consigne1));
    	break;
    	default:
    	ecriturePortSerie(port, consigneSTOP, strlen(consigneSTOP));
    	break;
    }
    sleep(2);
    // On termine en arretant le robot, pour des raisons de securite
    ecriturePortSerie(port, consigneSTOP, strlen(consigneSTOP));
}



int main(int argc, char *argv[]) {
	clock_t start = clock();
	//Ouverture du port serie
	char nomUSB[] = "/dev/ttyUSB0";
	portSerie port = ouvrePortSerieBloquant(nomUSB, 38400, 8, 0, 1, 1);

	// Creation de la base
	int l_base = NB_FICHIERS;
	Morceau base[l_base];

    int i = 0;
    std::istringstream f(FICHIERS);
    std::string line;   
    char nom[50];

    while (std::getline(f, line)) {
        strcpy (nom,line.c_str());
        std::getline(f, line);
        base[i] = lecture(nom, atoi(line.c_str()));
        i++;
        std::cout << "ajout dans la base de : " << nom << std::endl;
    }
	
	clock_t tbase = clock();
	std::cout << "base crée: " << tbase-start << std::endl;

	// On presente des exemples :


	char nom0[50];
	while(true){
		std::cin >> nom0;
		std::cout << "analyse de " << nom0 << "..." << std::endl;
        char play[] = "play ";
        char command[100]="";
        strcat(command, play);
        strcat(command, nom0);

        // std::cout << command << std::endl;
        auto b = system(command);
        std::cout << "============================================================================================ " << std::endl;
		commande(port, classif(base, l_base, nom0, 3));
	}




	// {
	// 	char nom0[] = "../test3/creations/adroite6.wav";
	// 	int classe = classif(base, l_base, nom0, 2);
	// 	commande(port, classe);
 //    }
	// {
	// 	char nom0[] = "../test3/creations/agauche6.wav";
	// 	int classe = classif(base, l_base, nom0, 2);
	// 	commande(port, classe);
 //    }
 //    {
	// 	char nom0[] = "../test3/creations/enavant6.wav";
	// 	int classe = classif(base, l_base, nom0, 2);
	// 	commande(port, classe);
 //    }   	
 //    {
	// 	char nom0[] = "../test3/creations/stop6.wav";
	// 	int classe = classif(base, l_base, nom0, 3);
	// 	commande(port, classe);
 //    }

    // par defaut, arret du robot quand on n'a plus d'instructions
	char consigneSTOP[] = "D,0,0\n";
	ecriturePortSerie(port, consigneSTOP, strlen(consigneSTOP));

	// fermeture du port serie
	fermePortSerie(port);
	clock_t end = clock();
	std::cout << "temps d'execution : " << end-start << std::endl;
	return 1;
}

