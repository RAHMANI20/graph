// num_etud1 : 22010400  
// nom : Rahmani
// prenom : Faical Sid Ahmed 

// num_etud2 : 21810250
// nom : Le Meur
// prenom : Brice 


// pour compiler :  g++ ex2.cpp -llpsolve55 -lcolamd -ldl -o ex2.exe
// pour executer :  ./ex2.exe <chemin _relatif_du_fichier_graph> <nombre_de_couleurs>



#include<stdio.h>
#include<stdlib.h>
#include<lpsolve/lp_lib.h>
#include<iostream>
#include<fstream>
#include<math.h>
#include<cstdlib>
#include<string>
#include<sstream>


using namespace std;



// cette fonction permet de vider le tableau
void vider_row (REAL row[], const int& taille){
	
	for(int i = 0; i < taille ; i++){
		row[i] = 0.0;
	}
}

// cette fonction permet de retourner le nombre de couleurs minimale utilisées
int longtab (int row[], const int& taille){
	int cpt=0;
	for(int i = 0; i < taille ; i++){
		if (row[i] == 1) cpt++;
	}
	return cpt;
}





int main(int argc,char* argv[]){
	
  // on vérifie si l'utilisateur a utilisé la bonne commande pour l'éxecution 
 
	if(argc != 3){
		std::cout << "<error : usage> " << argv[0] << " <Path_File> <Nbr_Color>"<< std::endl;
        exit(EXIT_FAILURE);
	}
	
  // covertir le nombre de couleurs 
	
    std::istringstream ss (argv[2]);
	int nbrColor; ss >> nbrColor; 
 
  // ouvrir le fichier 
  
    std::string ligne;
    std::ifstream fichier (argv[1]);
  
  // verifier si le fichier est ouvert sans erreur

    if(!fichier.is_open()){
		std::cout << "impossible d'ouvrir le fichier " << std::endl;
		exit(EXIT_FAILURE);
	}
 
 
    lprec* lp; // pointeur sur le programme linéaire
    int nbrVariable; // stocke le nombre de variable utilisé
    int nbrArcs,nbrSommet; // pour stocker le nombre d'arcs et sommet 
    int taille; // pour stocker la taille du tableau
    
    while( getline(fichier,ligne) ) { // lecture ligne par ligne du fichier
  
        char type;
        std::istringstream iss (ligne);
    
    
        iss >> type; // récupérer le premier mot pour chaque ligne
    
        switch(type){
      
            case 'p' : { // un problème 
      
                std::string format;
          
                iss >> format >> nbrSommet >> nbrArcs; // récupérer format et nombre de sommets et le nombre d'arcs
                std::cout << "le graphe possède " << nbrSommet << " sommets et " << nbrArcs << " arcs." << std::endl;
                
          
	            nbrVariable = nbrSommet * nbrColor + nbrArcs * nbrColor; // 'nbrColor' variable pour chaque sommet et 'nbrColor' variable pour chaque arc
	            lp = make_lp(0,nbrVariable); // création du lp avec l'ajout des variable et les contraintes
	            // imposer le type binaire aux variables du problème
                for (int i = 1 ; i <= nbrVariable ; i++){
                    set_binary(lp,i,true);
                }
          
                taille = nbrVariable + 1;
                REAL row[taille]; // permet d'ajouter les contraintes 
          
                // Un sommet du graphe ne doit etre colorié que par une seule couleur 
                for(int i = 0 ; i <= nbrSommet-1 ; i++){
	                vider_row(row,nbrVariable+1);
		            for(int j = (i*nbrColor)+1 ; j <= (i+1)*nbrColor ; j++){
		                row[j] = 1.0;
	                }
		            add_constraint(lp,row,EQ,1.0); // ajouter la contrainte
                }
          
          
          
            } break;
      
            case 'e' : { // un arc 
      
                REAL row[taille]; // permet d'ajouter les contraintes          
                int sommet1,sommet2;
                iss >> sommet1 >> sommet2; // récupérer les deux sommets adjacents 'arc entre sommet1 et sommet2'
                std::cout << "le graphe possède l'arete (" << sommet1 << "," << sommet2 << ") ." << std::endl;
          
               // Si deux sommets adjacents sont coloriés par la mème couleur alors la variable binaire
               // associée à l'arète des deux sommets doit etre égale 1. Sinon, elle doit etre égale à 0
                for (int i = 1; i <= nbrColor; i++){
                    vider_row(row, nbrVariable+1);
                    row[sommet1 * nbrColor - nbrColor + i] = 1.0;
                    row[sommet2 * nbrColor - nbrColor + i] = 1.0;
                    row[sommet1 * nbrColor + sommet2 * nbrColor + i] = -1.0;
                    add_constraint(lp, row, LE, 1.0); // ajouter la contrainte
                }
          
          
            } break;
    
        }
  
    }
  
    // fermer le fichier 
    fichier.close();
   
    REAL row[taille];  // permet d'ajouter la fonction objective
    
    vider_row(row,nbrVariable); 
    for(int i = nbrSommet * nbrColor + 1 ; i <= nbrVariable ; i++){
        row[i] = 1.0;
	}
	
	set_obj_fn(lp,row); // ajouter la fonction objective par default : minimiser
	
	// résoudre le problème
	
    if(solve(lp) == 0){ // en cas de succés
		
		int tab[nbrColor]; // on utilise ce tableau pour déduire le nombre minimal de couleurs utilisées 
        // remarque : on a constaté que la fonction objective permet de minimiser mais elle donne pas
        // le nombre minimale , si on prend l'exemple que vous avez donné elle retourne 0 .		
		
		int numcol ;
		get_variables(lp,row); // récupérer les valeurs des variables
		
		for(int i = 0 ; i <= nbrSommet-1 ; i++){
            numcol = 0;
		    for(int j = i*nbrColor ; j <= i*nbrColor+nbrColor-1 ; j++){
			    if(row[j] == 1){
			        std::cout << "la couleur num " << numcol+1 << " est assignée au sommet " << i+1 << std::endl;
	                tab[numcol] = 1;
			    }
			    numcol++;
            }
		}
    
		
		std::cout << "le nombre minimale de couleur utilisées sur tous les sommets = " << longtab(tab,nbrColor) << std::endl;
		
		
	}
	
	

    
	exit(EXIT_SUCCESS);
	
	
	
}
