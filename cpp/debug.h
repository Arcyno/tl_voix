#pragma once

#include <iostream>

// Cette surcharge de l'operateur << pour les vecteurs
// nous permet d'afficher les premiers elements d'un vecteur
// (utile pour  debugger)

std::ostream& operator<<( std::ostream &out, const double* tab){
	out << std::endl << "10 premiers éléments du tableau : " << std::endl;
	for(int i=0; i<10; ++i){
		out << tab[i] << std::endl;
	}
	return out ;
};

///on comprend pas pk on peut pas l'include dans frame.h ou 2 fois ???