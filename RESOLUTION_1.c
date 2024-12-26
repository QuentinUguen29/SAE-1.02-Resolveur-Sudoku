/**
 * @file RESOLUTION_1.c
 * 
 * @brief Programme qui résout une grille de sudoku avec la technique du Backtracking
 * 
 * @author UGUEN Quentin & GUILLERM Antoine
 * 
 * @date 17 janvier 2024
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 4
#define TAILLE (N * N)

typedef int tGrille[TAILLE][TAILLE];

void chargerGrille(tGrille g);
void afficherGrille(tGrille g);
bool backtracking(tGrille grille, int numeroCase);
bool absentSurLigne(int valeur, tGrille grille, int ligne);
bool absentSurColonne(int valeur, tGrille grille, int colonne);
bool absentSurBloc(int valeur, tGrille grille, int ligne, int colonne);

int main(){
    tGrille grille;


    clock_t begin = clock();
    chargerGrille(grille);
    afficherGrille(grille);
  

    backtracking(grille, 0);
    afficherGrille(grille);


    clock_t end = clock();
    double tmpsCPU = (end - begin)*1.0 / CLOCKS_PER_SEC;
    printf( "Temps CPU = %.3f secondes\n",tmpsCPU);

    return EXIT_SUCCESS;
}

void chargerGrille(tGrille g) {
  char nomFichier[30];
  FILE *f;
  do {
    printf("Nom du fichier ? ");
    scanf("%s", nomFichier);
    f = fopen(nomFichier, "rb");
    if (f == NULL) {
      printf("\nERREUR sur le fichier %s\n", nomFichier);
    }
  } while (f == NULL);
  fread(g, sizeof(int), TAILLE * TAILLE, f);

  fclose(f);
}

void afficherGrille(tGrille g) {
  // Affichage des n° de colonnes
  printf("     1  2  3  4   5  6  7  8   9 10 11 12  13 14 15 16\n");
  for (int ligne = 0; ligne < TAILLE; ligne++) {
    // Affichage des n° de lignes
    if (ligne == N*0 || ligne == N*1 || ligne == N*2 || ligne == N*3) {
      printf("   +------------+------------+------------+------------+\n");
    }
printf("%2d |", ligne + 1);
    for (int colonne = 0; colonne < TAILLE; colonne++) {
      // Affichage des chiffres ou des points (si 0/ case vide)
      if (g[ligne][colonne] == 0) {
        printf(" . ");
      } else {
        printf("%2d ", g[ligne][colonne] );
      }
      // Affichage des barres verticales entre les carrés
      if (colonne == N*1-1 || colonne == N*2-1 || colonne == N*3-1) {
        printf("|");
      }
    }
    printf("|\n");
  }
  // Affichage de la dernière ligne
  printf("   +------------+------------+------------+------------+\n\n");
}

bool backtracking(tGrille grille, int numeroCase){
    int ligne, colonne;
    bool resultat;

    resultat = false;

    if (numeroCase == TAILLE*TAILLE) {
        resultat = true;
    } 
    else {
        ligne = numeroCase / TAILLE;
        colonne = numeroCase % TAILLE;
        if (grille[ligne][colonne]!=0){
            resultat = backtracking(grille, numeroCase+1);
        } 
        else {
            for (int valeur = 1; valeur <= TAILLE; valeur++){
                if (absentSurLigne(valeur, grille, ligne) && absentSurColonne(valeur, grille, colonne) && absentSurBloc(valeur, grille, ligne, colonne)) {
                    grille[ligne][colonne] = valeur;
                    if (backtracking(grille,numeroCase+1)){
                        resultat = true;
                    } 
                    else {
                        grille[ligne][colonne] = 0;
                    }
                }
            }
        }
    }
    return resultat;
}


bool absentSurLigne(int valeur, tGrille grille, int ligne) {
    bool res = true;
    for (int l = 0; l < TAILLE; l++) {
        if (grille[ligne][l] == valeur) {
            res = false;
        }
    }
    return res;
}

bool absentSurColonne(int valeur, tGrille grille, int colonne) {
    bool res = true;
    for (int c = 0; c < TAILLE; c++) {
        if (grille[c][colonne] == valeur) {
            res = false;
        }
    }
    return res;
}

bool absentSurBloc(int valeur, tGrille grille, int ligne, int colonne){
  bool res = true;
  int debutLigne = N * ((ligne) / N);
  int debutColonne = N * ((colonne) / N);
  for (int l = debutLigne; l < debutLigne + N; l++) {
    for (int c = debutColonne; c < debutColonne + N; c++) {
      if (grille[l][c] == valeur) {
        res = false;
      }
    }
  }
  return res;
}