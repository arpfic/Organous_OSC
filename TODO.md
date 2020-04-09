## TODO:

### Software

* -not so ok- Distinguer l'usage d'un seul ou des deux drivers à la fois
* Commenter le code
* Tester l'inversion 0->1 du pca99x6B au repos
* Retourner un code d'erreur si aucun pca995xB n'est trouvé
* Gérer les erreurs au niveau du pca995xB lui-même
* Écrire un vrai README
* Licencier le code correctement (À présent il y a du Apache/MIT)
* Écrire une doc pour la carte elle-même
* Créer un wrapper midi (et une interface qui émanciperait de puredata)

### Hardware

* Prévoir des entretoises et un support de fixation contre les sommiers
* Envisager une boîte de protection ?

## BUGS:

* Fonction tone cassée depuis la MAJ
* Encore quelques derniers problèmes d'IP : /debug ss "SURAIGU mod. (v0.1):" "255.255.255.255"
* -ok- OVERCURRENT fonction un peu bavarde ! Il faut tester avec un bon bouncing

## INSTALLATION -- MBED-CLI

* Télécharger : https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads
```
sudo apt install python2.7 python-pip git mercurial
pip install mbed-cli
mbed import https://github.com/arpfic/Organous_OSC.git
cd Organous_OSC/mbed-os
pip install -U -r requirements.txt
cd ..
mbed config -G GCC_ARM_PATH ~/Travaux/gcc-arm-none-eabi-9-2019-q4-major/bin
mbed target NUCLEO_F767ZI
mbed compile
``````
