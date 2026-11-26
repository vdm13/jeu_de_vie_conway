# Conway Survival
Jeu de console en C pour deux joueurs locaux. La grille 20 × 15 est finie, non torique : hors grille, les cellules sont mortes. Les deux moitiés commencent avec des motifs symétriques (bloc, clignotant, planeur).

## Règles
J1 joue en premier. Chacun joue 15 tours en alternance. Une action par tour, sur n'importe quel territoire : ajouter une cellule morte, retirer une cellule vivante, déplacer une cellule vivante vers une des huit cases voisines mortes, ou passer. Une action invalide est refusée sans consommer de tour. Après chaque action valide, une génération simultanée de Conway est calculée (naissance à 3 voisins, survie à 2 ou 3). Les colonnes 1–10 appartiennent à J1, 11–20 à J2 ; la frontière ne bloque pas les interactions. Après 30 générations, le plus grand nombre de cellules vivantes sur son territoire gagne ; égalité possible. Quitter interrompt la partie sans vainqueur. J2 bénéficie du dernier coup : ce choix peut être discuté lors de la soutenance.

## Compilation
```sh
gcc -std=c11 -Wall -Wextra -pedantic program.c -o conway
./conway
```
Sous Windows, exécuter `conway.exe` après compilation avec MinGW.

## Implémentation
Le tableau conserve le codage décimal du programme initial : unités = état actuel, dizaines = état futur. Les deux phases de mise à jour garantissent la simultanéité. La logique des actions, des scores et des générations est séparée de la saisie et de l'affichage. Les coordonnées affichées commencent à 1, les indices internes à 0. Les tableaux passés en paramètres utilisent la syntaxe de tableaux du cours, sans déclaration explicite de pointeur.

## À compléter avant remise
Créer les logigrammes avec Flow-C dans `flows/`, relire et comprendre le code, effectuer les commits Git au fil du travail et publier le dépôt GitHub. Vérifier les conventions de constantes avec l'enseignant (les coordonnées des motifs sont des données de configuration).
