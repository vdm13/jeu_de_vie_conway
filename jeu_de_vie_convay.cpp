#include <iostream>
#include <string>
using namespace std;

// Constantes : dimensions, duree et actions du jeu.
const int WIDTH = 20;
const int HEIGHT = 15;
const int PLAYERS = 2;
const int TURNS_PER_PLAYER = 15;
const int TOTAL_TURNS = PLAYERS * TURNS_PER_PLAYER;
const int HALF = WIDTH / PLAYERS;
const int DEAD = 0;
const int ALIVE = 1;
const int ADD = 1;
const int REMOVE = 2;
const int MOVE = 3;
const int PASS = 4;
const int QUIT = 0;
const int END_INPUT = -1;
const int BIRTH = 3;
const int SURVIVAL = 2;
const int DISTANCE = 1;

// Verifie chaque limite separement avant d'acceder au tableau.
bool in_bounds(int x, int y) {
    if (x < 0) {
        return false;
    }
    if (x >= WIDTH) {
        return false;
    }
    if (y < 0) {
        return false;
    }
    if (y >= HEIGHT) {
        return false;
    }
    return true;
}

// Les cases hors de la grille sont considerees comme mortes.
int cell_cur_state(int world[HEIGHT][WIDTH], int x, int y) {
    if (in_bounds(x, y)) {
        return world[y][x];
    }
    return DEAD;
}

// Place les trois motifs initiaux, puis les reproduit en miroir.
void initialize_world(int world[HEIGHT][WIDTH]) {
    // Bloc stable.
    world[2][2] = ALIVE;
    world[2][3] = ALIVE;
    world[3][2] = ALIVE;
    world[3][3] = ALIVE;

    // Clignotant : oscillateur de trois cellules.
    world[5][5] = ALIVE;
    world[5][6] = ALIVE;
    world[5][7] = ALIVE;

    // Planeur : motif mobile de cinq cellules.
    world[9][4] = ALIVE;
    world[10][5] = ALIVE;
    world[11][3] = ALIVE;
    world[11][4] = ALIVE;
    world[11][5] = ALIVE;

    // La moitie droite est le miroir de la moitie gauche.
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < HALF; x++) {
            int mirror = WIDTH - 1 - x;
            world[y][mirror] = world[y][x];
        }
    }
}

// Affiche les coordonnees a partir de 1 pour faciliter la saisie.
void display_world(int world[HEIGHT][WIDTH]) {
    cout << "       Territoire J1                 Territoire J2\n    ";
    for (int x = 0; x < WIDTH; x++) {
        if (x == HALF) {
            cout << "| ";
        }
        if (x + 1 < 10) {
            cout << " ";
        }
        cout << x + 1 << " ";
    }
    cout << '\n';
    for (int y = 0; y < HEIGHT; y++) {
        if (y + 1 < 10) {
            cout << " ";
        }
        cout << y + 1 << "  ";
        for (int x = 0; x < WIDTH; x++) {
            if (x == HALF) {
                cout << "| ";
            }
            if (world[y][x] == ALIVE) {
                cout << " # ";
            } else {
                cout << " . ";
            }
        }
        cout << '\n';
    }
}

// Examine les neuf positions du carre 3 x 3, sauf la case centrale.
int count_alive_neighbors(int world[HEIGHT][WIDTH], int x, int y) {
    int count = 0;
    for (int dy = -DISTANCE; dy <= DISTANCE; dy++) {
        for (int dx = -DISTANCE; dx <= DISTANCE; dx++) {
            bool center = false;
            if (dx == 0) {
                if (dy == 0) {
                    center = true;
                }
            }
            if (center == false) {
                if (cell_cur_state(world, x + dx, y + dy) == ALIVE) {
                    count = count + 1;
                }
            }
        }
    }
    return count;
}

// Calcule le futur d'une cellule sans modifier la grille actuelle.
int cell_compute_next_state(int world[HEIGHT][WIDTH], int x, int y) {
    int neighbors = count_alive_neighbors(world, x, y);
    if (world[y][x] == ALIVE) {
        // Une cellule vivante survit avec au moins deux voisins.
        // La surpopulation ne provoque plus la mort.
        if (neighbors >= SURVIVAL) {
            return ALIVE;
        }
        // Mort uniquement par isolement (zero ou un voisin).
        return DEAD;
    } else {
        // Une cellule morte nait avec exactement trois voisins.
        if (neighbors == BIRTH) {
            return ALIVE;
        }
        return DEAD;
    }
}

// Toutes les cellules doivent evoluer simultanement.
void next_generation(int world[HEIGHT][WIDTH]) {
    int future[HEIGHT][WIDTH];
    // Etape 1 : calculer tous les nouveaux etats.
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            future[y][x] = cell_compute_next_state(world, x, y);
        }
    }
    // Etape 2 : copier la grille future dans la grille actuelle.
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            world[y][x] = future[y][x];
        }
    }
}

// Compte les cellules vivantes dans la moitie du joueur.
int score(int world[HEIGHT][WIDTH], int player) {
    int start = 0;
    if (player == 2) {
        start = HALF;
    }
    int end = start + HALF;
    int total = 0;
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = start; x < end; x++) {
            if (world[y][x] == ALIVE) {
                total = total + 1;
            }
        }
    }
    return total;
}

// Affiche les scores avant l'action du joueur.
void display_scores(int world[HEIGHT][WIDTH], int turn, int player) {
    int s1 = score(world, 1);
    int s2 = score(world, 2);
    int individual = turn / PLAYERS + 1;
    cout << "\n==========================================\n";
    cout << "TOUR " << turn + 1 << " / " << TOTAL_TURNS << '\n';
    cout << "Generation : " << turn << '\n';
    cout << "Joueur actif : " << player << '\n';
    cout << "Tour individuel : " << individual << " / " << TURNS_PER_PLAYER << '\n';
    cout << "Score joueur 1 : " << s1 << " cellules vivantes\n";
    cout << "Score joueur 2 : " << s2 << " cellules vivantes\n";
    cout << "Total : " << s1 + s2 << '\n';
    cout << "==========================================\n";
}

// Verifie et applique une action. Aucun affichage ni saisie ici.
bool apply_action(int world[HEIGHT][WIDTH], int action,
                  int x, int y, int nx, int ny) {
    if (action == PASS) {
        return true;
    }
    if (in_bounds(x, y) == false) {
        return false;
    }
    if (action == ADD) {
        if (world[y][x] == DEAD) {
            world[y][x] = ALIVE;
            return true;
        }
        return false;
    }
    if (action == REMOVE) {
        if (world[y][x] == ALIVE) {
            world[y][x] = DEAD;
            return true;
        }
        return false;
    }
    if (action == MOVE) {
        // La source doit etre vivante et la destination vide.
        if (world[y][x] == DEAD) {
            return false;
        }
        if (in_bounds(nx, ny) == false) {
            return false;
        }
        if (world[ny][nx] == ALIVE) {
            return false;
        }
        // Le deplacement est limite a une case dans chaque direction.
        if (nx - x > DISTANCE) {
            return false;
        }
        if (x - nx > DISTANCE) {
            return false;
        }
        if (ny - y > DISTANCE) {
            return false;
        }
        if (y - ny > DISTANCE) {
            return false;
        }
        world[y][x] = DEAD;
        world[ny][nx] = ALIVE;
        return true;
    }
    return false;
}

// Lit une ligne complete et verifie qu'elle contient un entier autorise.
// La lecture caractere par caractere evite les erreurs de conversion.
int read_number(int minimum, int maximum) {
    string line;
    while (true) {
        getline(cin, line);
        if (cin.fail()) {
            return END_INPUT;
        }
        bool valid = true;
        int value = 0;
        if (line.empty()) {
            valid = false;
        }
        int length = line.length();
        for (int i = 0; i < length; i++) {
            char digit = line[i];
            if (digit < '0') {
                valid = false;
            } else if (digit > '9') {
                valid = false;
            }
            if (valid == true) {
                value = value * 10 + digit - '0';
                if (value > maximum) {
                    valid = false;
                }
            }
        }
        if (value < minimum) {
            valid = false;
        }
        if (valid == true) {
            return value;
        }
        cout << "Entrez un entier entre " << minimum << " et " << maximum << " : ";
    }
}

// Demande une action jusqu'a ce qu'elle soit valide ou que le joueur quitte.
bool player_turn(int world[HEIGHT][WIDTH], int player) {
    while (true) {
        cout << "\nJoueur " << player << " : choisissez une action\n";
        cout << "1. Ajouter\n2. Retirer\n3. Deplacer\n4. Passer\n0. Quitter\nChoix : ";
        int action = read_number(QUIT, PASS);
        if (action <= QUIT) {
            return false;
        }
        int x = 0;
        int y = 0;
        int nx = 0;
        int ny = 0;
        if (action != PASS) {
            cout << "Colonne (1-" << WIDTH << ") : ";
            x = read_number(1, WIDTH);
            if (x == END_INPUT) {
                return false;
            }
            cout << "Ligne (1-" << HEIGHT << ") : ";
            y = read_number(1, HEIGHT);
            if (y == END_INPUT) {
                return false;
            }
            if (action == MOVE) {
                cout << "Colonne destination : ";
                nx = read_number(1, WIDTH);
                if (nx == END_INPUT) {
                    return false;
                }
                cout << "Ligne destination : ";
                ny = read_number(1, HEIGHT);
                if (ny == END_INPUT) {
                    return false;
                }
            }
        }
        // Les tableaux commencent a 0, les coordonnees saisies a 1.
        bool success = apply_action(world, action, x - 1, y - 1, nx - 1, ny - 1);
        if (success == true) {
            return true;
        }
        cout << "Action impossible. Reessayez sans perdre votre tour.\n";
    }
}

// Compare les scores apres la derniere generation.
void display_winner(int world[HEIGHT][WIDTH]) {
    int s1 = score(world, 1);
    int s2 = score(world, 2);
    cout << "\nFIN DE PARTIE - " << TOTAL_TURNS << " tours joues\n";
    cout << "Score final joueur 1 : " << s1 << '\n';
    cout << "Score final joueur 2 : " << s2 << '\n';
    cout << "Total : " << s1 + s2 << '\n';
    if (s1 > s2) {
        cout << "Joueur 1 gagne !\n";
    } else if (s2 > s1) {
        cout << "Joueur 2 gagne !\n";
    } else {
        cout << "Match nul !\n";
    }
}

int main() {
    // Toutes les cases sont mortes au depart.
    int world[HEIGHT][WIDTH] = {0};
    initialize_world(world);
    cout << "CONWAY SURVIVAL - # vivante, . morte\n";
    cout << "Variante sans surpopulation : survie avec au moins deux voisins.\n";
    cout << TURNS_PER_PLAYER << " tours par joueur. Toute la grille est accessible.\n";
    int player = 1;
    for (int turn = 0; turn < TOTAL_TURNS; turn++) {
        display_scores(world, turn, player);
        display_world(world);
        bool playing = player_turn(world, player);
        if (playing == false) {
            cout << "Partie interrompue.\n";
            return 0;
        }
        next_generation(world);
        // Alterner les joueurs apres chaque action valide.
        if (player == 1) {
            player = 2;
        } else {
            player = 1;
        }
    }
    display_world(world);
    display_winner(world);
    return 0;
}
