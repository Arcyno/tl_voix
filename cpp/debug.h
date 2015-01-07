#pragma once

#include <iostream>


std::ostream& operator<<( std::ostream &out, const float* tab){
	out << std::endl << "10 premiers éléments du tableau : " << std::endl;
	for(int i=0; i<10; ++i){
		out << tab[i] << std::endl;
	}
	return out ;
};

///on comprend pas pk on peut pas l'include dans frame.h ou 2 fois ???