TODO:
######

* -not so ok- Distinguer l'usage d'un seul ou des deux drivers à la fois
* Commenter le code
* Tester l'inversion 0->1 du pca99x6B au repos
* Retourner un code d'erreur si aucun pca995xB n'est trouvé
* Gérer les erreurs au niveau du pca995xB lui-même
* Écrire un vrai README
* Licencier le code correctement (À présent il y a du Apache/MIT)
* Écrire une doc pour la carte elle-même
* Créer un wrapper midi (et une interface qui émanciperait de puredata)

BUGS:
######

* Fonction tone cassée depuis la MAJ
* Encore quelques derniers problèmes d'IP : /debug ss "SURAIGU mod. (v0.1):" "255.255.255.255"
* -ok- OVERCURRENT fonction un peu bavarde ! Il faut tester avec un bon bouncing