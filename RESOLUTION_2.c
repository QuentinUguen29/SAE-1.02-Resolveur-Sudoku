/**
 *
 * \file RESOLUTION_2.c
 * 
 * \brief Programme pour résoudre des grilles de Sudoku
 * 
 * \author Quentin Uguen et Antoine Guillerm
 * 
 * \version 3.0 
 *
 * \date 28 janvier 2024 
 *
 * Ce programme utilise la technique d'une liste de candidat
 * pour chaque case ainsi que les techniques du singleton nu,
 * singleton caché, paires nues et du backtracking
 * 
 */

// Optimisation par rapport au programme RESOLUTION_1.c (backtracking seulement):
// ajout des techniques du singleton nu, singleton caché et paires nues 
// + optimisation de ces dernières. Nous avons également regroupé les 
// trois fonctions "absentSurLigne", "absentSurColonne" et "absentSurBloc" 
// dans une seule fonction "possible"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 4
#define TAILLE (N * N)

typedef struct {
  int valeur;
  int candidats[TAILLE];
  int nbCandidats;
} tCase1;

typedef tCase1 tGrille2[TAILLE][TAILLE];
typedef int tGrille[TAILLE][TAILLE];

void chargerGrille(tGrille g);
void afficherGrille(tGrille2 g);
bool backtracking(tGrille2 grille, int numeroCase);
bool possible(tGrille2 g, int ligne, int colonne, int valeur);

// AUTRES FONCTIONS //
void initGrille(tGrille grille1, tGrille2 grille2);
void initialiserCandidats(tGrille2 g);
void ajouterCandidat(tCase1 *Case, int valeur);
void retirerCandidat(tCase1 *Case, int valeur);
void retirerTousCandidats(tGrille2 g, int ligne, int colonne, int valeur);
bool estGrillePleine(tGrille2 g);
void singletonNu(tGrille2 g);
void retirerCandidatCase(tCase1 *Case);
void singletonCache(tGrille2 g);
void pairesNues(tGrille2 g);
bool estCandidat(tCase1 laCase, int val);
void afficherCandidats(tCase1 Case);

bool progression = true;

int main() {
  clock_t begin = clock();
  tGrille grilleInit;
  tGrille2 grille;
  chargerGrille(grilleInit);
  initGrille(grilleInit, grille);
  afficherGrille(grille);
  initialiserCandidats(grille);

  while (!(estGrillePleine(grille)) && progression) {
    progression = false;
    singletonNu(grille);
    singletonCache(grille);
    pairesNues(grille);
  }
  backtracking(grille, 0);
  afficherGrille(grille);

  clock_t end = clock();
  double tmpsCPU = (end - begin) * 1.0 / CLOCKS_PER_SEC;
  printf("Temps CPU = %f secondes\n", tmpsCPU);

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

void afficherGrille(tGrille2 g) {
  // Affichage des n° de colonnes
  printf("     1  2  3  4   5  6  7  8   9 10 11 12  13 14 15 16\n");
  for (int ligne = 0; ligne < TAILLE; ligne++) {
    // Affichage des n° de lignes
    if (ligne == N * 0 || ligne == N * 1 || ligne == N * 2 || ligne == N * 3) {
      printf("   +------------+------------+------------+------------+\n");
    }
    printf("%2d |", ligne + 1);
    for (int colonne = 0; colonne < TAILLE; colonne++) {
      // Affichage des chiffres ou des points (si 0/ case vide)
      if (g[ligne][colonne].valeur == 0) {
        printf(" . ");
      } else {
        printf("%2d ", g[ligne][colonne].valeur);
      }
      // Affichage des barres verticales entre les carrés
      if (colonne == N * 1 - 1 || colonne == N * 2 - 1 ||
          colonne == N * 3 - 1) {
        printf("|");
      }
    }
    printf("|\n");
  }
  // Affichage de la dernière ligne
  printf("   +------------+------------+------------+------------+\n\n");
}

bool backtracking(tGrille2 grille, int numeroCase) {
  int ligne, colonne;
  bool resultat= false;

  if (numeroCase == TAILLE * TAILLE) {
    resultat = true;
  } else {
    ligne = numeroCase / TAILLE;
    colonne = numeroCase % TAILLE;
    if (grille[ligne][colonne].valeur != 0) {
      resultat = backtracking(grille, numeroCase + 1);
    } else {
      int valeur = 1;
      while (valeur <= TAILLE && !resultat){
        if (possible(grille, ligne, colonne, valeur)) {
          grille[ligne][colonne].valeur = valeur;
          if (backtracking(grille, numeroCase + 1)) {
            resultat = true;
          } else {
            grille[ligne][colonne].valeur = 0;
          }
        }
        valeur++;
      }
    }
  }
  return resultat;
}

bool possible(tGrille2 g, int ligne, int colonne, int valeur) {
  bool possible = true;
  int debutLigne = N * (ligne / N);
  int debutColonne = N * (colonne / N);

  int l = 0;
  while (l < TAILLE && possible){
      if (g[l][colonne].valeur == valeur) {
        possible = false;
      }
      l++;
  } 

  int c = 0;
  while (c < TAILLE && possible){
      if (g[ligne][c].valeur == valeur) {
        possible = false;
      }
      c++;
  }

  l = debutLigne;
  c = debutColonne;
  while (l < debutLigne + N && possible){
    while (c < debutColonne + N && possible){
      if (g[l][c].valeur == valeur) {
          possible = false;
      }
      c++;
    }
    l++;
    c = debutColonne;
  }

  return possible;
}

/////AUTRES FONCTIONS//////

void initGrille(tGrille grille1, tGrille2 grille2) {
  for (int i = 0; i < TAILLE; i++) {
    for (int j = 0; j < TAILLE; j++) {
      (grille2[i][j]).valeur = grille1[i][j];
      (grille2[i][j]).nbCandidats = 0;
    }
  }
}

void initialiserCandidats(tGrille2 g) {
  for (int ligne = 0; ligne < TAILLE; ligne++) {
    for (int colonne = 0; colonne < TAILLE; colonne++) {
      for (int valeur = 1; valeur <= TAILLE; valeur++) {
        if (g[ligne][colonne].valeur == 0 && possible(g, ligne, colonne, valeur)) {
          ajouterCandidat(&g[ligne][colonne], valeur);
        }
      }
    }
  }
}

void ajouterCandidat(tCase1 *Case, int valeur) {
  Case->candidats[(Case->nbCandidats)] = valeur;
  Case->nbCandidats++;
}

void retirerCandidat(tCase1 *Case, int valeur) {
  int i = 0;

  while (i < (Case->nbCandidats)) {
    if (Case->candidats[i] == valeur) {
      if (Case->nbCandidats != 1) {
        for (int j = i; j < (Case->nbCandidats) - 1; j++) {
          Case->candidats[j] = Case->candidats[j + 1];
        }
      }
      (Case->nbCandidats)--;
    } else {
      i++;
    }
  }
}

void retirerTousCandidats(tGrille2 grille, int ligne, int colonne, int valeur){
    for (int i = 0; i < TAILLE; i++)
    {
        retirerCandidat(&grille[ligne][i],valeur);
        retirerCandidat(&grille[i][colonne],valeur);
    }
    int coinLigne = N * (ligne / N);
    int coinColonne = N * (colonne / N);
    for (int i = coinLigne; i < coinLigne + N; ++i) {
        for (int j = coinColonne; j < coinColonne + N; ++j) {
            retirerCandidat(&grille[i][j],valeur);
        }
    }
}

bool estGrillePleine(tGrille2 g) {
  bool complet = true;
  int l = 0, c = 0;
  while (l < TAILLE && complet){
    while (c < TAILLE && complet){
      if (g[l][c].valeur == 0) {
          complet = false;
      }
      c++;
    }
    l++;
    c = 0;
  }
  return complet;
}

void singletonNu(tGrille2 g) {
  for (int ligne = 0; ligne < TAILLE; ligne++) {
    for (int colonne = 0; colonne < TAILLE; colonne++) {
      if (g[ligne][colonne].valeur == 0 && g[ligne][colonne].nbCandidats == 1) {
        (g[ligne][colonne]).valeur = (g[ligne][colonne]).candidats[0];
        retirerTousCandidats(g, ligne, colonne, (g[ligne][colonne]).valeur);
        progression = true;
      }
    }
  }
}

void retirerCandidatCase(tCase1 *Case) {
  Case->nbCandidats = 0;
}

void singletonCache(tGrille2 g) {
  bool condition;
  for (int ligne = 0; ligne < TAILLE; ligne++) {
    for (int colonne = 0; colonne < TAILLE; colonne++) {
      for (int candidatActuel = 0; candidatActuel < g[ligne][colonne].nbCandidats; candidatActuel++) {
        condition = true;

        for (int ligneCandidat = 0; ligneCandidat < TAILLE && condition; ligneCandidat++) {

          if (ligneCandidat != colonne) {
            for (int candidat = 0; candidat < g[ligne][ligneCandidat].nbCandidats && condition; candidat++) {
              if (g[ligne][ligneCandidat].candidats[candidat] == g[ligne][colonne].candidats[candidatActuel]) {
                condition = false;
              }
            }
          }
        }

        if (condition) {
          g[ligne][colonne].valeur = g[ligne][colonne].candidats[candidatActuel];
          retirerCandidatCase(&g[ligne][colonne]);
          retirerTousCandidats(g, ligne, colonne, g[ligne][colonne].valeur);
          progression = true;
        }
      }
    }
  }

  for (int ligne = 0; ligne < TAILLE; ligne++) {
    for (int colonne = 0; colonne < TAILLE; colonne++) {
      for (int candidatActuel = 0;
           candidatActuel < g[ligne][colonne].nbCandidats; candidatActuel++) {
        condition = true;

        for (int colonneCandidat = 0; colonneCandidat < TAILLE && condition; colonneCandidat++) {
          if (colonneCandidat != ligne) {
            for (int candidat = 0; candidat < g[colonneCandidat][colonne].nbCandidats && condition; candidat++) {
              if (g[colonneCandidat][colonne].candidats[candidat] == g[ligne][colonne].candidats[candidatActuel]) {
                condition = false;
              }
            }
          }
        }

        if (condition) {
          g[ligne][colonne].valeur = g[ligne][colonne].candidats[candidatActuel];
          retirerCandidatCase(&g[ligne][colonne]);
          retirerTousCandidats(g, ligne, colonne, g[ligne][colonne].valeur);
          progression = true;
        }
      }
    }
  }
  
  int debutLigne, debutColonne;

  for (int ligne = 0; ligne < TAILLE; ligne++) {
    for (int colonne = 0; colonne < TAILLE; colonne++) {
      debutLigne = N * ((ligne) / N);
      debutColonne = N * ((colonne) / N);

      for (int candidatActuel = 0; candidatActuel < g[ligne][colonne].nbCandidats; candidatActuel++) {
        condition = true;

        for (int ligneCandidat = debutLigne; ligneCandidat < debutLigne + N && condition; ligneCandidat++) {
          for (int colonneCandidat = debutColonne; colonneCandidat < debutColonne + N && condition; colonneCandidat++) {

            if ((ligne != ligneCandidat && colonne != colonneCandidat) || (ligne >= debutLigne && ligne < debutLigne + N && colonne >= debutColonne && colonne < debutColonne + N)) {
              for (int candidat = 0; candidat < g[ligneCandidat][colonneCandidat].nbCandidats && condition; candidat++) {
                if (g[ligneCandidat][colonneCandidat].candidats[candidat] == g[ligne][colonne].candidats[candidatActuel]) {
                  condition = false;
                }
              }
            }
          }
        }

        if (condition) {
          g[ligne][colonne].valeur = g[ligne][colonne].candidats[candidatActuel];
          retirerCandidatCase(&g[ligne][colonne]);
          retirerTousCandidats(g, ligne, colonne, g[ligne][colonne].valeur);
          progression = true;
        }
      }
    }
  }
}

bool estCandidat(tCase1 laCase, int val) {
  bool trouve = false;
  int i = 0;
  while (i < laCase.nbCandidats && !trouve)
  {
    if (laCase.candidats[i] == val) {
      trouve = true;
    }
    i++;
  }

  return trouve;
}

void pairesNues(tGrille2 g) {
  int debutLigne;
  int debutColonne;
  bool paireTrouve;
  int candidat1, candidat2;
  int cooCase1X, cooCase1Y, cooCase2X, cooCase2Y;
  int L, C;

  for (int ligne = 0; ligne < TAILLE; ligne++) {
    for (int colonne = 0; colonne < TAILLE; colonne++) {
      paireTrouve = false;
      if (g[ligne][colonne].nbCandidats == 2) {
        candidat1 = g[ligne][colonne].candidats[0];
        candidat2 = g[ligne][colonne].candidats[1];

        cooCase1X = ligne;
        cooCase1Y = colonne;

        debutLigne = N * (ligne / N);
        debutColonne = N * (colonne / N);
        L = debutLigne;
        C = debutColonne;
        while (L < debutLigne + N && !paireTrouve) {
          while (C < debutColonne + N && !paireTrouve) {
            if ((cooCase1X != L && cooCase1Y != C) &&
                (g[L][C].nbCandidats == 2) &&
                ((g[L][C].candidats[0] == candidat1 &&
                  g[L][C].candidats[1] == candidat2) ||
                 (g[L][C].candidats[0] == candidat2 &&
                  g[L][C].candidats[1] == candidat1))) {
              paireTrouve = true;
              cooCase2X = L;
              cooCase2Y = C;
            }
            C++;
          }
          L++;
          C = debutColonne;
        }

        if (paireTrouve) {
          for (int l = debutLigne; l < debutLigne + N; l++) {
            for (int c = debutColonne; c < debutColonne + N; c++) {
              if ((l != cooCase1X || c != cooCase1Y) &&
                  (l != cooCase2X || c != cooCase2Y)) {
                if (estCandidat(g[l][c], candidat1)) {
                  retirerCandidat(&g[l][c], candidat1);
                  progression = true;
                }
                if (estCandidat(g[l][c], candidat2)) {
                  retirerCandidat(&g[l][c], candidat2);
                  progression = true;
                }
              }
            }
          }
        }
      }
    }
  }
}