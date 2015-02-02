/****************************************************
 *
 *		serie.h
 *
 *
 *   Copyright (C) 2009,  Supelec
 *
 *   Author : Jean-Louis Gutzwiller
 *
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or any later version.
 *   
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *   Lesser General Public License for more details.
 *   
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 *   Contact : Jean-Louis.Gutzwiller@supelec.fr
 *
 *
 *-----------------------------------------------------
 *
 *		serie.h
 *
 *
 *		Utilisation d'un port sÃ©rie pour Linux
 *		ou pour Windows.
 *
 *					Version 1.0
 *					Jean-Louis Gutzwiller, le 26 mai 2010
 *
 *
 *******************************************************/



typedef void * portSerie;

// Les fonctions suivantes permettent d'utiliser le port sÃ©rie.
// La fonction d'ouverture prend en paramÃ¨tre le nom (sous Linux,
// par exemple /dev/ttyUSB0 ; sous Windows par exemple COM1).
//
// Les valeurs possibles :
//
//	--> name : un nom d'un port sÃ©rie qui existe sur la machine
//			(/dev/ttyUSBn pour Linux, COMn pour Windows).
//  --> speed : une vitesse standard : 110, 300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200.
//	--> bits : 7 ou 8
//  --> parity : 0 ou 1
//		Noter que sur certains systÃ¨mes, bits + parity = 8.
// 	--> stops : 10, 15 ou 20 pour 1, 1.5 ou 2 bits de stop.
//
// Les fonctions de rÃ©ception et d'Ã©mission sont bloquantes. Bien que l'ensemble
// de ces fonctions ne soit pas "multi-thread safe", il est possible
// d'utiliser la fonction d'Ã©mission et la fonction de rÃ©ception dans deux
// threads diffÃ©rents.
//
// Les fonctions de lecture et d'Ã©criture renvoient le nombre de caractÃ¨res
// lus ou Ã©crits.
//
// La fonction d'ouverture dite "bloquante" permet de choisir le fonctionnement
// bloquant ou non de la fonction de lecture. Si le paramÃ¨tre "bloque" est 1,
// alors la fonction de lecture est bloquante jusqu'Ã  ce que le nombre de caractÃ¨res
// demandÃ© soit obtenu, sinon, elle est non bloquante et renvoie le nombre de 
// caractÃ¨res actuellement disponible. Notons que afin d'Ã©viter des boucles
// de consommation de processeur Ã  pleine puissance, la fonction de lecture
// attend 1 ms si le nombre de caractÃ¨res demandÃ© n'est pas atteint.
// La fonction d'ouverture par dÃ©faut utilise l'option bloquante.


#ifdef __cplusplus
extern "C" {
#endif

portSerie ouvrePortSerie(char * nom, int vitesse, int bits, int parite, int stops);
portSerie ouvrePortSerieBloquant(char * nom, int vitesse, int bits, int parite, int stops, int bloque);
void fermePortSerie(portSerie h);
int lecturePortSerie(portSerie h, char * c, int len);
int ecriturePortSerie(portSerie h, char * c, int len);

#ifdef __cplusplus
}
#endif
