#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#ifdef _OPENMP
#include <omp.h>
#endif


#include "K-means.cpp"

using namespace std;

int main_consense(char **argv,vector<int> tabIndices,int pLeavesAbsent){
	 // Variables
    time_t tbegin,tend;
    double texec=0.;

    // Start timer
    tbegin=time(NULL);				// get the current calendar time
	
	
/* 	printf ("Consensus'tree K-means partitioning\n");
	printf("Nadia Tahiri and Vladimir Makarenkov - Departement d'informatique - Universite du Quebec a Montreal\n");
	printf ("Original code by :  Pierre Legendre - Departement de sciences biologiques - Universite de Montreal.\n");
	printf ("(c) Pierre Legendre, 1999\n");
 */
	//Varriables
	double **Matrice_RF;
	double **Ww;
	double **n_identique;
	
	vector <string> monTableau;
	double *distances = new double[4];
	string tree1;
	string tree2;
	
	//Varriables(suite)
	fstream fichier(argv[1]);
	int ligne = 1;

	if( !fichier )
		cout << "File "<<argv[1]<<" no exist."<<endl;
	else{
		while( !fichier.eof()){
			monTableau.push_back("");//creation d'une ligne vide
			getline(fichier, monTableau.back());//lecture d'une ligne du fichier
			ligne = monTableau.size() - 1;//je recupere la taille du tableau (-1 pour la ligne 0)
			
			if(monTableau[ligne].empty())//si la ligne est vide
				monTableau.pop_back();//on la retire du tableau
		}
		
		//Création de la matrice carrée et symétrique (monTableau.size()*monTableau.size()) : Matrice_RF
		Matrice_RF= new double*[monTableau.size()];
		Ww= new double*[monTableau.size()];
		n_identique= new double*[monTableau.size()];
		
		#pragma omp parallel for	
		for(int lineDist=0;lineDist<monTableau.size();lineDist++){
			Matrice_RF[lineDist]= new double[monTableau.size()];
			Ww[lineDist]= new double[monTableau.size()];
			n_identique[lineDist]= new double[monTableau.size()];
		}
		
		
		double MoinsUn = -1.0;
		double PlusUn = 1.0;
		double Tirage1 = 0.0;
		double Tirage2 = 0.0;
		
		// Remplissage de la matrice des distances RF en faisant appel à main_hgt 
		// qui calcule la distance RF entre chaque paire d'arbre
		
		#pragma omp parallel for	
		for(int line=0;line<monTableau.size();line++){
			//mettre des valeurs 0.0 pour la diagonale de la matrice RF
			Matrice_RF[line][line]=0.0;
			
			for(int column=line;column<monTableau.size();column++){

				// Affectation de deux arbres : tree1 et tree2
				tree1=monTableau[line];
				tree2=monTableau[column];

				// Appel des algorithmes des calcules des distances : RF
				main_hgt(tree1,tree2,distances);
				Matrice_RF[line][column]=distances[0];
				
				if(Matrice_RF[line][column]<=0.0){
					Matrice_RF[line][column]=0.0;
				}
				
				if(isnan(Matrice_RF[line][column])){
					Matrice_RF[line][column]=0.0;
				}

				//Recuperer le nombre d'espèces communes
				n_identique[line][column]=distances[3];
				//Et la symétrie
				n_identique[column][line]=distances[3];	
				
				// pour remplir la symétrique de la matrice RF sans réaliser de calcul (car matrice carrée symétrique)
				Matrice_RF[column][line]=Matrice_RF[line][column];		

			}
		}
		
		
		//pour supprimer le fichier argv[1].statistics.txt
		
		char input_file[255];
		strcpy (input_file, argv[1]);
		strcat (input_file,".statistics.txt");
		int i=remove(input_file);
		
		strcpy (input_file, argv[1]);
		strcat (input_file,".groups.txt");
		
		i=remove(input_file);
		
		//creation de la matrice de distances RF : mat
		#pragma omp parallel for	
		 for (int i=0; i<monTableau.size(); i++)
		 {
			 for (int j=0; j<monTableau.size(); j++)
			 {
				if(n_identique[i][j]<=3){
					Ww[i][j]=0.0;
				}else{
					Ww[i][j]=1.0;
				}
			 }
		 }
	 

		//appel de l'algorithme de K-means:
		if(monTableau.size()>3){	
			//for RF and criterion W
			cout<<"STAT POUR CH WITH CONSENSUS"<<endl;
			string parametres = "OldCH";
			main_kmeans(argv,monTableau,Matrice_RF,parametres,n_identique,Ww,tabIndices,pLeavesAbsent);
			
		}	
	}

	
	//Liberation of memory
	for (int i=0;i<monTableau.size();i++){
		delete [] Matrice_RF[i];
		delete [] Ww[i];
		delete [] n_identique[i];
	}
	delete [] Matrice_RF;
	delete [] Ww;
	delete [] n_identique;
	delete [] distances;
	// End timer
    tend=time(NULL);				// get the current calendar time

	// Compute execution time
    texec=difftime(tend,tbegin);	// tend-tbegin (result in second)
	
	/* cout<<"\nTEMPS D'EXECUTION "<<texec<<endl; */
	return 0;
}
