## TODO

###### Software

* faire un registre précis de l'état des configs, que puredata peut rappeler
* créer un pwm software pour les ENABLE
* -ok- régler le problème des deux réseaux et de l'initialisation des IP sur puredata
* -not so ok- Distinguer l'usage d'un seul ou des deux drivers à la fois
* -ok- Tester le reliability d'UDP, et tenter d'implémenter un ack avec puredata
* -ok- Commenter le code
* Tester l'inversion 0->1 du pca99x6B au repos
* Retourner un code d'erreur si aucun pca995xB n'est trouvé
* Écrire une doc pour la carte elle-même
* Gérer les erreurs au niveau du pca995xB lui-même
* -ok- Écrire un vrai README
* Discriminer les messages de debug
* Chercher à créer un fichier de config directement dans le système de fichier du NUCLEO ?
* -ok- Licencier le code correctement (À présent il y a du Apache/MIT)
* Créer un wrapper midi (et une interface qui émanciperait de puredata)

###### Hardware

* -ok- Envisager sérieusement une carte d'extension qui relèverait le courant.
* -ok- Prévoir des entretoises et un support de fixation contre les sommiers
* Envisager une boîte de protection ?

## BUGS :

* Bug entre coil et motor quand on utilise les deux en même temps ?
* -ok- Fonction tone cassée depuis la MAJ
* Encore quelques derniers problèmes d'IP : /debug ss "SURAIGU mod. (v0.1):" "255.255.255.255"
* -ok- OVERCURRENT fonction un peu bavarde ! Il faut tester avec un bon bouncing

## MBED INSTALLATION

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
