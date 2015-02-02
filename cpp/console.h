/*****************************************************************
 *
 *		console.h
 *
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
 *-----------------------------------------------------------------
 *
 *					Version 1.0.7 du 16 fÃ©vrier 2011
 *
 *
 *		Ce fichier contient la macro-instruction "INIT_CONSOLE"
 *		permettant de gÃ©rer deux situations sous les environnements
 *		windows et linux :
 *
 *		1.  La console fonctionne toujours en UTF8 (spÃ©cifique windows)
 *
 *		2. 	La possibilitÃ© d'appeler une fonction de nettoyage si
 *			le programme s'arrÃªte, quelle que soit la raison
 *			de l'arrÃªt.
 *
 *		3. 	Mise Ã  disposition de fonctions de conversion entre
 *			le codage de la machine et l'utf8 (afin d'autoriser
 *			les appels systÃ¨mes avec des noms de fichier codÃ©s
 *			en code machine).
 *
 *
 *
 *
 *		----------------------------------------------
 *		Pour utiliser les possibilitÃ©s de ce fichier :
 *		----------------------------------------------
 *
 *
 *		---------------------------------------------------------
 *		APPEL DEPUIS LE FICHIER PRINCIPAL (celui qui contient la
 *		fonction "main") :
 *		---------------------------------------------------------
 *
 *		--> gestion du codage des caractÃ¨res uniquement (utf8) :
 *
 *				#include "console.h"
 *
 *				int main(int argc, char * * argv) {
 *					INIT_CONSOLE
 *					... contenu normal de main ...
 *				}
 *
 *
 *		--> gestion du codage des caractÃ¨res et du nettoyage :
 *
 *
 *				#define _CLEANUP_
 *				#include "console.h"
 *
 *				Ã©crire une fonction : void cleanup(void)
 *
 *				int main(int argc, char * * argv) {
 *					INIT_CONSOLE
 *					... contenu normal de main ...
 *				}
 *
 *
 *		--> il est possible d'intercepter la frappe de CTRL+C au
 *			au clavier et de demander de ne pas quitter le programme.
 *			Dans ce cas, il est sous-entendu que soit la fonction
 *			cleanup elle-mÃªme, soit le programme principal assure
 *			de quitter proprement. La fonction cleanup peut, par exemple,
 *			positionner une variable indiquant au programme principal
 *			qu'il doit quitter rapidement.
 *			Pour obtenir cet effet, faire avant l'include :
 *
 *
 *				#define _NOEXITONSIGNAL_
 *				#define _CLEANUP_
 *				#include "console.h"
 *
 *				Ã©crire une fonction : void cleanup(void)
 *
 *				int main(int argc, char * * argv) {
 *					INIT_CONSOLE
 *					... contenu normal de main ...
 *				}
 *
 *
 *
 *			ATTENTION : si cette possibilitÃ© est utilisÃ©e, le programme
 *			principal continue de tourner aprÃ¨s le cleanup.
 *			NOTE : seuls les signaux d'arrÃªt sont interceptÃ©s. Les
 *			erreurs fatales continuent Ã  arrÃªter normalement le programme.
 *
 *
 *		--> utilisation des fonctions de conversion (depuis main) :
 *
 *			#define _UTF_CONVERSION_
 *			#include "console.h"
 *
 *			On peut utiliser les fonctions :
 *
 *				void HostToUTF8(char * s, char * resultat);
 *				void UTF8ToHost(char * s, char * resultat);
 *
 *
 *		Que le programme soit destinÃ© Ã  Windows ou Ã  Linux, tous
 *		les fichiers sources doivent Ãªtre Ã©crits en utilisant le
 *		codage des caractÃ¨res UTF8 (encodage des fichiers sources
 *		en UTF8).
 *
 *		A noter que "emacs" (sous Linux) et "notepad2" (sous Windows)
 *		Ã©crivent en UFT8 (choix d'option dans Notepad2).
 *
 *
 *		Code de sortie :
 *		----------------
 *
 *		--> Si le programme s'arrÃªte de maniÃ¨re "normale" (return de main
 *			ou appel de la fonction exit), le code de sortie est celui
 *			indiquÃ© par le programme.
 *
 *		--> Si le programme s'arrÃªte par frappe de CTRL + C ou un arrÃªt
 *			dÃ» Ã  l'arrÃªt du pÃ¨re, le code de sortie est 254.
 *
 *		--> Si le programme s'arrÃªte du fait d'une erreur (mÃ©moire, division
 *			par zÃ©ro, ...), le code de sortie est 255.
 *
 *
 *
 *
 *		---------------------------------------------------------
 *		APPEL DEPUIS UN AUTRE FICHIER (ne contenant pas la fonction "main") :
 *		---------------------------------------------------------
 *
 *		-->	Utilisation des fonctions de conversion uniquement :
 *
 *			#define _UTF_CONVERSION_ONLY_
 *			#include "console.h"
 *
 *			On peut utiliser les fonctions :
 *
 *				void HostToUTF8(char * s, char * resultat);
 *				void UTF8ToHost(char * s, char * resultat);
 *
 *
 *		ATTENTION : cela nÃ©cessite que par ailleurs le fichier main
 *		utilise bien cet include avec l'option _UTF_CONVERSION_. Dans
 *		le cas contraire, il y aura une erreur Ã  l'Ã©dition des liens.
 *
 *-----------------------------------------------------------------
 *
 *	Situation 1 : mode UTF8 :
 *
 *
 *		L'Ã©criture de programmes qui puissent fonctionner de maniÃ¨re
 *		similaire sur Linux et sur Windows pose un problÃ¨me d'utilisation
 *		de jeux de caractÃ¨res.
 *		En effet, le codage des caractÃ¨res est dÃ©fini par le comportement
 *		de la console (terminal) qui permet de faire fonctionner
 *		le programme (entrÃ©es/sorties standard, stdin, stdout, stderr).
 *
 *		Sous Linux, il est possible d'obtenir un terminal de deux
 *		maniÃ¨re diffÃ©rentes :
 *		- soit par connexion en mode terminal (ssh), auquel cas
 *		  c'est le terminal qui fixe le jeu de caractÃ¨res utilisÃ©,
 *		- soit par l'ouverture sous XWindows d'une commande xterm
 *		  (le jeu de caractÃ¨res est alors UTF8).
 *
 *		Sous Windows, les invites de commande (cmd) utilisent par
 *		dÃ©faut la page de code qui peut Ãªtre variable d'un pays Ã  l'autre.
 *		Il est cependant possible de modifier la page de code par
 *		la commande "CHCP", ou directement par le programme.
 *
 *		Notons que l'Ã©diteur de texte qui permet de modifier le code
 *		source du programme en cours de dÃ©veloppement doit utiliser
 *		le mÃªme codage de caractÃ¨res que le terminal pour que les
 *		affichages se fassent correctement.
 *
 *
 *		J'ai donc choisi de coder systÃ©matiquement en UTF8.
 *
 *		Ainsi, tous les fichiers sources du programme sont codÃ©s
 *		en UTF8 et tous les terminaux doivent fonctionner dans ce mode.
 *
 *
 *		Sous linux :
 *		- les terminaux lancÃ©s sous XWindows (xterm) sont en UTF8
 *		- les connexions Ã  distance en ssh (putty) se feront en 
 *		  UTF8, putty pouvant Ãªtre programmÃ© pour rÃ©aliser cela
 *		  systÃ©matiquement. Il est alors compatible avec les messages
 *	      Ã©mis Linux quelque soit le choix de la langue (localisation).
 *
 *		Sous Windows :
 *		- le programme exÃ©cutable doit placer la console (invite
 *		  de commandes) cmd en mode UTF8 et la remettre dans son
 *		  mode initial lorsque le programme s'arrÃªte.
 *		  La possibilitÃ© d'intercepter les arrÃªts du programme
 *		  permet de remettre la console dans son mode d'origine.
 *
 *
 *-----------------------------------------------------------------
 *
 *	Situation 2 : nettoyage Ã  l'arrÃªt du programme :
 *
 *
 *		Si la macro _CLEANUP_ est dÃ©finie, alors la fonction
 *		"void cleanup(void)" doit Ãªtre dÃ©finie dans le programme principal
 *		et sera appelÃ©e automatiquement lors de l'arrÃªt du programme,
 *		quelle que soit la cause d'arrÃªt.
 *		Il est garanti que cette fonction ne sera appelÃ©e qu'une 
 *		seule fois.
 *
 *		Pour utiliser cette possibilitÃ© :
 *
 *			1. Placez un #define _CLEANUP_ avant le #include "console.h"
 *
 *			2. Fournissez dans le programme main une fonction
 *				void cleanup(void).
 *
 *			Cette fonction sera appelÃ© si le programme s'arrÃªte,
 *			quelle que soit la cause de l'arrÃªt (terminaison normale,
 *			arrÃªt par CTRL + C au clavier, arrÃªt par erreur).
 *
 *		Le programme peut alors assurer de faire les nettoyages
 *		nÃ©cessaires lors de son arrÃªt.
 *
 *
 *****************************************************************
 *
 *  	Interaction systÃ¨me :
 *
 *			Les interactions systÃ¨me doivent se faire en utilisant
 *		le jeu de caractÃ¨re du systÃ¨me et non l'UTF-8. Il faut donc
 *		donc convertir les chaÃ®nes de caractÃ¨res si de telles
 *		interactions sont nÃ©cessaires.
 *
 *		Les deux fonctions donnÃ©es ci-dessous font cela
 *		(les versions Linux, de fait, ne font rien).
 *		Remarque : la macro _UTF_CONVERSION_ doit Ãªtre dÃ©finie pour
 *		donner accÃ¨s Ã  ces fonctions :
 *
 *			#define _UTF_CONVERSION_
 *			#include "console.h"
 *
 *			void HostToUTF8(char * s, char * resultat);
 *			void UTF8ToHost(char * s, char * resultat);
 *
 *		Note : le programme utilisateur doit assurer que le pointeur
 *		rÃ©sultat pointe vers une mÃ©moire de taille suffisante.
 *		Dans tous les cas, une taille de "2 * strlen(s)" est suffisante.
 *
 *		Principe de la conversion UTF-8 :
 *
 *		Il existe diffÃ©rents codes inclus les uns dans les autres :
 *		- ASCII (7 bits)
 *		- ANSI (8 bits)
 *		- UNICODE (16 bits)
 *		- code universel 32 bits.
 *
 *		Pour chacun de ces codes, un caractÃ¨re est un numÃ©ro
 *		(le mÃªme pour tous ces codes, mais les codes les plus
 *		petits ne peuvent pas coder certaines langues).
 *
 *		Le principe de l'UTF-8 consiste Ã  encoder sur 8 bits
 *		en utilisant Ã©ventuellement plusieurs octets, le numÃ©ro
 *		du caractÃ¨re (http://fr.wikipedia.org/wiki/UTF-8) :
 *
 *		- < 7 bits (ASCII)			--> encodÃ© : 0xxxxxxx
 *		- 8 Ã  11 bits				--> encodÃ© : 110xxxxx 10xxxxxx
 *		- 12 Ã  16 bits				--> encodÃ© : 1110xxxx 10xxxxxx 10xxxxxx
 *		- 17 Ã  21 bits				--> encodÃ© : 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 *
 *		Dans notre cas, seules les deux premiÃ¨res lignes sont utilisÃ©es.
 *
 *
 *
 *
 *		Utilisation des deux fonctions de conversion dans d'autres fichiers
 *		que le fichier principal : il est possible d'appeler les fonctions de
 *		conversion dans d'autres fichiers que le fichier principal. Dans ce
 *		cas, les autres fonctions dÃ©finies dans ce fichier ne sont pas nÃ©cessaires,
 *		et ne doivent pas Ãªtre dÃ©finies.
 *		On rÃ©alisera alors l'inclusion de la maniÃ¨re suivante :
 *
 *			#define _UTF_CONVERSION_ONLY_
 *			#include "console.h"
 *
 *
 *****************************************************************/


#ifdef __cplusplus
#include <exception>
#endif


#ifndef _UTF_CONVERSION_ONLY_
	#ifdef _CLEANUP_
	void cleanup(void);				// Le programme appelant dÃ©finit
										// une fonction de nettoyage.
	static int __console__cleanup_made__ = 0;	// Evite l'appel en double.
	#endif

	// Message Ã  afficher en cas d'erreur (exception) et code d'erreur
	// renvoyÃ© si sortie sur erreur. Remarque : sous Linux, les codes
	// d'erreurs traitÃ©s par sh sont entre 0 et 255.
	// Je choisis donc le plus Ã©levÃ© (255) pour la sortie sur erreur
	// et le prÃ©cÃ©dent (254) pour la sortie par arrÃªt forcÃ©.
	// Dans le cas d'une sortie "normale", le code d'erreur indiquÃ© par
	// le programme est respectÃ©.
	#define MESSAGE_ERREUR "ArrÃªt du programme suite Ã  erreur fatale : exception non gÃ©rÃ©e.\n"
	#ifdef __cplusplus
	#define MESSAGE_ERREUR_CPP "ArrÃªt du programme suite Ã  erreur C++ fatale : exception non gÃ©rÃ©e.\n"
	#endif
	#define __CONSOLE__ERROR_EXIT_CODE__ 255
	#define __CONSOLE__SIGNAL_EXIT_CODE__ 254
#endif


#ifdef _POUR_WINDOWS_

	// Ici nous compilons pour Windows. Nous devons donc effectuer
	// les actions prÃ©vues :

	#ifndef _UTF_CONVERSION_ONLY_

		#include <windows.h>
		#include <stdio.h>

		#define WINDOWS_CP 65001             // Code de page pour console en UTF8

		static int __console__consoleMode__ = 850;
		

		// Appel de la fonction de nettoyage. Cet appel ne peut avoir lieu
		// qu'une seule fois. Ainsi, la variable globale "__console__cleanup_made__"
		// me permet de savoir si la fonction a dÃ©jÃ  Ã©tÃ© appelÃ©e.
		
		static void __console__cleanupcall__(void) {
			#ifdef _CLEANUP_
			if (__console__cleanup_made__ == 0) {
				__console__cleanup_made__ = 1;
				cleanup();		// Si le programme client dÃ©finit une fonction
								// de nettoyage, on l'appelle ici.
			}
			#endif
			fflush(stdout);
			fflush(stderr);
		}
		
		// Restauration du mode de console initial : plusieurs programmes peuvent
		// Ãªtre lancÃ©s dans un tuyau. Certains vont donc noter le mode de console
		// dÃ©jÃ  modifiÃ© et donc effectuer une restauration erronÃ©e.
		// ConsidÃ©rant que le mode UTF8 ne peut pas Ãªtre utilisÃ© par dÃ©faut sous
		// windows (car il empÃªche les fichiers batch de fonctionner), je ne restaure
		// le mode prÃ©cÃ©dent que s'il n'est pas UTF8.

		static void __console__restaureConsole__(void) {
			fflush(stdout);
			fflush(stderr);
			if (__console__consoleMode__ != WINDOWS_CP) {
				SetConsoleOutputCP(__console__consoleMode__);
				SetConsoleCP(__console__consoleMode__);
			}
		}
		
		
		
		// RÃ©cupÃ©ration du signal d'arrÃªt (CTRL C) au clavier.
		// Ce signal provoque la restauration du mode de console initial
		// et se propage aux autres actions dÃ©finies par dÃ©faut (arrÃªt
		// du programme).
		// Remarque : si la macro _NOEXITONSIGNAL_ est dÃ©finie, la
		// frappe de CTRL + C ne provoque pas l'arrÃªt du programme.
		// La fonction cleanup est appelÃ©e, puis le programme continue.

		
		static BOOL WINAPI __console__signalfct__(DWORD n) {
			switch (n) {
				case CTRL_C_EVENT :
				case CTRL_BREAK_EVENT :
				case CTRL_CLOSE_EVENT :
					__console__cleanupcall__();
					#ifndef _NOEXITONSIGNAL_
					__console__restaureConsole__();	// Si je dÃ©tecte CTRL C, je restaure la page de code.
					exit(__CONSOLE__SIGNAL_EXIT_CODE__);
					#endif
					break;
			}
			#ifdef _NOEXITONSIGNAL_
			return TRUE;		// Interdiction aux autre handlers de s'activer, en
								// consÃ©quence, le programme n'est pas arrÃªtÃ©.
			#else
			return FALSE;		// Quoi qu'il arrive, j'indique que je ne m'en
								// occupe pas, laissant les handlers prÃ©vus faire
								// leur effet.
			#endif
		}



		// Cette fonction est appelÃ©e en cas d'exception (pointeur invalide, division par zÃ©ro...).

		static LONG WINAPI __console__myExceptionFilter__(struct _EXCEPTION_POINTERS *ExceptionInfo) {
			fprintf(stderr, MESSAGE_ERREUR);
			__console__cleanupcall__();
			__console__restaureConsole__();
			exit(__CONSOLE__ERROR_EXIT_CODE__);
			return EXCEPTION_EXECUTE_HANDLER;
		}
		#ifdef __cplusplus
		static void __CPP__myExceptionFilter__(void) {
			fprintf(stderr, MESSAGE_ERREUR_CPP);
			__console__cleanupcall__();
			__console__restaureConsole__();
			exit(__CONSOLE__ERROR_EXIT_CODE__);
		}
		#endif
		


		// Cette fonction est appelÃ©e en cas d'arrÃªt normal du programme (exit).

		static void __console__exitConsole__(void) {
			__console__cleanupcall__();
			__console__restaureConsole__();
		}




		// Fonctions de conversion :

		#ifdef _UTF_CONVERSION_

		#ifdef __cplusplus
		extern "C" void HostToUTF8(char * s, char * resultat);
		#endif
		void HostToUTF8(char * s, char * resultat) {
			int i, j;
			unsigned int c;

			if ((s == NULL) || (resultat == NULL)) return;
			j = 0;
			resultat[j] = 0;
			for(i = 0; s[i] != 0; i++) {
				if (((unsigned char)(s[i])) < (1L<<7)) {
					resultat[j] = s[i];
					resultat[j+1] = 0;
					j = j + 1;
				} else {
					c = ((unsigned int)s[i]) & 0x00FF;
					resultat[j] = 0xC0 + ((c >> 6) & 0x1F);
					resultat[j+1] = 0x80 + (c & 0x3F);
					resultat[j+2] = 0;
					j = j + 2;
				}
			}
			return;
		}
		
		#ifdef __cplusplus
		extern "C" void UTF8ToHost(char * s, char * resultat);
		#endif
		void UTF8ToHost(char * s, char * resultat) {
			int i, j;

			if ((s == NULL) || (resultat == NULL)) return;
			j = 0;
			resultat[j] = 0;
			for(i = 0; s[i] != 0; i++) {
				if (((unsigned char)(s[i])) < (1L<<7)) {
					resultat[j] = s[i];
					resultat[j+1] = 0;
					j = j + 1;
				} else {
					if (((s[i] & 0xE0) == 0xC0)&&((s[i+1] & 0xC0) == 0x80)) {
						unsigned int r;
						r = (((unsigned char)(s[i]) & 0x1F) << 6) 
							+ ((unsigned char)(s[i+1]) & 0x3F);
						if ((r < 256) && (r >= 128)) {
							resultat[j] = r;
							resultat[j+1] = 0;
						} else { 
							resultat[j] = '?';
							resultat[j+1] = 0;
						}
						if (s[i+1] != 0) i = i + 1;
					} else {
						resultat[j] = '?';
						resultat[j+1] = 0;
					}
					j = j + 1;
				}
			}
			return;
		}

		#endif

		// DÃ©finition de la macro-instruction Ã  placer au dÃ©but de "main"
		// pour que les choses se fassent.


		static void __windows__init_console__(void) {
			__console__consoleMode__ = GetConsoleCP();
			SetConsoleOutputCP(WINDOWS_CP);
			SetConsoleCP(WINDOWS_CP);
			atexit(__console__exitConsole__);
			SetConsoleCtrlHandler(__console__signalfct__, TRUE);
			SetUnhandledExceptionFilter(__console__myExceptionFilter__);
			#ifdef _UTF_CONVERSION_
			HostToUTF8(NULL, NULL);	// Evite le warning de non utilisation.
			UTF8ToHost(NULL, NULL);	// Evite le warning de non utilisation.
			#endif
			#ifdef __cplusplus
			std::set_terminate(__CPP__myExceptionFilter__);
			#endif
		}


		#define INIT_CONSOLE __windows__init_console__ ();


	#endif



	#ifdef _UTF_CONVERSION_ONLY_
		#ifdef __cplusplus
		extern "C" {
		#endif
		void HostToUTF8(char * s, char * resultat);
		void UTF8ToHost(char * s, char * resultat);
		#ifdef __cplusplus
		}
		#endif
	#endif


#else
#ifdef _POUR_LINUX_

	#ifndef _UTF_CONVERSION_ONLY_
		#include <stdio.h>
		#include <signal.h>
		#include <stdlib.h>
		#ifdef _UTF_CONVERSION_
			#include <string.h>
		#endif

		// Ici, nous compilons pour Linux.
		static void __console__cleanupcall__(void) {
			#ifdef _CLEANUP_
			if (__console__cleanup_made__ == 0) {
				__console__cleanup_made__ = 1;
				cleanup();		// Si le programme client dÃ©finit une fonction
								// de nettoyage, on l'appelle ici.
			}
			#endif
			fflush(stdout);
			fflush(stderr);
		}

		static void __console__signal__handler__(int i) {
			__console__cleanupcall__();
			#ifndef _NOEXITONSIGNAL_
			exit(__CONSOLE__SIGNAL_EXIT_CODE__);
			#endif
		}

		static void __console__error__handler__(int i) {
			fprintf(stderr, MESSAGE_ERREUR);
			__console__cleanupcall__();
			exit(__CONSOLE__ERROR_EXIT_CODE__);
		}
		#ifdef __cplusplus
		static void __CPP__error__handler__(void) {
			fprintf(stderr, MESSAGE_ERREUR_CPP);
			__console__cleanupcall__();
			exit(__CONSOLE__ERROR_EXIT_CODE__);
		}
		#endif


		static void __linux__init_console__(void) {
			atexit(__console__cleanupcall__);
			signal(SIGHUP, __console__signal__handler__);
			signal(SIGINT, __console__signal__handler__);
			signal(SIGQUIT, __console__signal__handler__);
			signal(SIGKILL, __console__error__handler__);
			signal(SIGABRT, __console__error__handler__);
			signal(SIGFPE, __console__error__handler__);
			signal(SIGKILL, __console__error__handler__);
			signal(SIGSEGV, __console__error__handler__);
			signal(SIGPIPE, __console__error__handler__);
			signal(SIGALRM, __console__error__handler__);
			signal(SIGTERM, __console__error__handler__);
			signal(SIGUSR1, __console__error__handler__);
			signal(SIGUSR2, __console__error__handler__);
			signal(SIGBUS, __console__error__handler__);
			signal(SIGPOLL, __console__error__handler__);
			signal(SIGPROF, __console__error__handler__);
			signal(SIGTRAP, __console__error__handler__);
			signal(SIGVTALRM, __console__error__handler__);
			signal(SIGXCPU, __console__error__handler__);
			signal(SIGXFSZ, __console__error__handler__);
			signal(SIGIOT, __console__error__handler__);
			signal(SIGSTKFLT, __console__error__handler__);
			signal(SIGIO, __console__error__handler__);
			signal(SIGPWR, __console__error__handler__);
			signal(SIGUNUSED, __console__error__handler__);
			#ifdef __cplusplus
			std::set_terminate(__CPP__error__handler__);
			#endif
		}

		#define INIT_CONSOLE    __linux__init_console__ ();


		// DÃ©finition de macro-instructions vides pour les fonctions de conversion :


		#ifdef _UTF_CONVERSION_
		#ifdef __cplusplus
		extern "C" void HostToUTF8(char * s, char * resultat);
		#endif
		void HostToUTF8(char * s, char * resultat) {
			strcpy(resultat, s);
		}
		#ifdef __cplusplus
		extern "C" void UTF8ToHost(char * s, char * resultat);
		#endif
		void UTF8ToHost(char * s, char * resultat) {
			strcpy(resultat, s);
		}
		#endif
	#endif

	#ifdef _UTF_CONVERSION_ONLY_
		#ifdef __cplusplus
		extern "C" {
		#endif
		void HostToUTF8(char * s, char * resultat);
		void UTF8ToHost(char * s, char * resultat);
		#ifdef __cplusplus
		}
		#endif
	#endif

#else

	// Ici, nous ne savons pas pour qui nous compilons. Il faut mettre un message d'erreur.

	#error -----------------------------------------------------------
	#error Pour utiliser console.h : 
	#error Definir une des deux macros _POUR_WINDOWS_ ou _POUR_LINUX_.
	#error -----------------------------------------------------------

#endif
#endif
