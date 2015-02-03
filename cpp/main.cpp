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


Morceau lecture(char* fileName, int classe_donne = 0){
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
	    return Morceau();
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
	Morceau morceau = Morceau(frames, n_frames, classe_donne);
	
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
		dist += std::sqrt((lpc1[i] - lpc2[i])*(lpc1[i] - lpc2[i]));
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
	std::cout << "nom du fichier : " << nom << std::endl;

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
		// std::cout << "distance : " << it->first << ", classe : " << it->second << std::endl;
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

	//Ouverture du port serie
	char nomUSB[] = "/dev/ttyUSB0";
	portSerie port = ouvrePortSerieBloquant(nomUSB, 38400, 8, 0, 1, 1);

	// Creation de la base
	int l_base = 16;
	Morceau base[l_base];

    //base d'origine
/*	char nom1[] = "../test3/adroite.wav";
	base[0] = lecture(nom1, 1);
	char nom2[] = "../test3/agauche.wav";
	base[4] = lecture(nom2, 2);
	char nom3[] = "../test3/enavant.wav";
	base[8] = lecture(nom3, 3);
	char nom4[] = "../test3/stop.wav";
	base[12] = lecture(nom4, 4);

	char nom5[] = "../test3/adroite2.wav";
	base[1] = lecture(nom5, 1);
	char nom6[] = "../test3/agauche2.wav";
	base[5] = lecture(nom6, 2);
	char nom7[] = "../test3/enavant2.wav";
	base[9] = lecture(nom7, 3);
	char nom8[] = "../test3/stop2.wav";
	base[13] = lecture(nom8, 4);

	char nom9[] = "../test3/adroite3.wav";
	base[2] = lecture(nom9, 1);
	char nom10[] = "../test3/agauche3.wav";
	base[6] = lecture(nom10, 2);
	char nom11[] = "../test3/enavant3.wav";
	base[10] = lecture(nom11, 3);
	char nom12[] = "../test3/stop3.wav";
	base[14] = lecture(nom12, 4);*/



    // Base creee par nos soins (un seul locuteur)
	char nom1[] = "../test3/creations/adroite7.Nouveau.wav";
	base[0] = lecture(nom1, 1);
	char nom2[] = "../test3/creations/agauche7.Nouveau.wav";
	base[4] = lecture(nom2, 2);
	char nom3[] = "../test3/creations/enavant7.Nouveau.wav";
	base[8] = lecture(nom3, 3);
	char nom4[] = "../test3/creations/stop7.Nouveau.wav";
	base[12] = lecture(nom4, 4);

	char nom5[] = "../test3/creations/adroite8.Nouveau.wav";
	base[1] = lecture(nom5, 1);
	char nom6[] = "../test3/creations/agauche8.Nouveau.wav";
	base[5] = lecture(nom6, 2);
	char nom7[] = "../test3/creations/enavant8.Nouveau.wav";
	base[9] = lecture(nom7, 3);
	char nom8[] = "../test3/creations/stop8.Nouveau.wav";
	base[13] = lecture(nom8, 4);

	char nom9[] = "../test3/creations/adroite9.Nouveau.wav";
	base[2] = lecture(nom9, 1);
	char nom10[] = "../test3/creations/agauche9.Nouveau.wav";
	base[6] = lecture(nom10, 2);
	char nom11[] = "../test3/creations/enavant9.Nouveau.wav";
	base[10] = lecture(nom11, 3);
	char nom12[] = "../test3/creations/stop9.Nouveau.wav";
	base[14] = lecture(nom12, 4);

	char nom13[] = "../test3/creations/adroite5.Nouveau.wav";
	base[3] = lecture(nom13, 1);
	char nom14[] = "../test3/creations/agauche5.Nouveau.wav";
	base[7] = lecture(nom14, 2);
	char nom15[] = "../test3/creations/enavant5.Nouveau.wav";
	base[11] = lecture(nom15, 3);
	char nom16[] = "../test3/creations/stop5.Nouveau.wav";
	base[15] = lecture(nom16, 4);


	// On presente des exemples :
	{
		char nom0[] = "../test3/creations/adroite6.Nouveau.wav";
		int classe = classif(base, l_base, nom0, 2);
		commande(port, classe);
    }
	{
		char nom0[] = "../test3/creations/agauche6.Nouveau.wav";
		int classe = classif(base, l_base, nom0, 2);
		commande(port, classe);
    }
    {
		char nom0[] = "../test3/creations/enavant6.Nouveau.wav";
		int classe = classif(base, l_base, nom0, 2);
		commande(port, classe);
    }   	
    {
		char nom0[] = "../test3/creations/stop6.Nouveau.wav";
		int classe = classif(base, l_base, nom0, 2);
		commande(port, classe);
    }

    // par defaut, arret du robot quand on n'a plus d'instructions
	char consigneSTOP[] = "D,0,0\n";
	ecriturePortSerie(port, consigneSTOP, strlen(consigneSTOP));

	// fermeture du port serie
	fermePortSerie(port);

	return 1;
}

