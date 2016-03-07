#!/bin/bash
# Author: Nadia Tahiri -- UQAM -- october 2015
echo Mon premier script

echo Liste des fichiers :
ls -al
echo 

echo Mon repertoire courant :
pwd
echo

processusStart()
{
	cd phylip-3.69/src/
	make install
	cp ../exe/consense ../../ConsensusTrees/
	
	cd ../../distance_RF/
	g++ source_old2.cpp -o rf
	cp rf ../ConsensusTrees/
	
	cd ..
}


processusCall()
{
	make all
	chmod 777 -R *
}

processusStart
echo Mon repertoire courant :
pwd
echo

cd ConsensusTrees/
echo Mon repertoire courant :
pwd
echo
processusCall
echo 


echo FIN DU SCRIPT BASH!!
exit 0