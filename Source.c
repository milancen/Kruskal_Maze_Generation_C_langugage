#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
typedef struct Komsija {
	int indeks_komsija1;
	int indeks_komsija2;
	struct Komsija* sledeci;
	struct Komsija* prethodni;
}Komsija;


char* grid;
int* set;
int* sz;
int* matricaGrafa;
Komsija* susedniCvorovi = NULL;
int rows;
int columns;
int rowsMatriceGrafa;
int columnsMatriceGrafa;
int defaultOpcija = 1;
int ciljX;
int ciljY;

Komsija* napraviNovi(int prvi, int drugi) {
	Komsija* novi = (Komsija*)malloc(sizeof(Komsija));
	novi->indeks_komsija1 = prvi;
	novi->indeks_komsija2 = drugi;
	novi->sledeci = NULL;
	novi->prethodni = NULL;
	return novi;
}
void ubaciUListu(Komsija* novi, Komsija** lista) {
	if (*lista == NULL) {
		(*lista) = novi;
		return;
	}

	Komsija* pom = (*lista);
	(*lista) = novi;
	novi->sledeci = pom;
	pom->prethodni = novi;
}
void ispisi(Komsija* lista) {
	Komsija* pom = lista;
	while (pom != NULL) {
		printf("%d - %d\n", pom->indeks_komsija1, pom->indeks_komsija2);
		pom = pom->sledeci;
	}
}
int brojCvorovaUListi(Komsija* lista) {
	Komsija* pom = lista;
	int brojCvorova = 0;

	while (pom != NULL) {
		brojCvorova++;
		pom = pom->sledeci;
	}

	return brojCvorova;
}
Komsija* pronadjiCvorPoIndeksu(int indeks, Komsija* lista) {
	if (lista == NULL) {
		return NULL;
	}
	Komsija* pom = lista;
	int i = 0;
	while (i != indeks) {
		pom = pom->sledeci;
		i++;
	}

	return pom;
}
void obrisiCvor(Komsija** lista, Komsija* cvor) {
	if ((*lista) == NULL || cvor == NULL) {
		return;
	}
	else if (cvor->sledeci == NULL && cvor->prethodni != NULL) {
		cvor->prethodni->sledeci = NULL;
		free(cvor);
	}
	else if (cvor->prethodni == NULL && cvor->sledeci != NULL) {
		(*lista) = (*lista)->sledeci;
		(*lista)->prethodni = NULL;
		free(cvor);
	}
	else if (cvor->prethodni == NULL && cvor->sledeci == NULL) {
		(*lista) = NULL;
		free(cvor);
	}
	else {
		cvor->sledeci->prethodni = cvor->prethodni;
		cvor->prethodni->sledeci = cvor->sledeci;
		free(cvor); 
	}
}  


void inicijalizacija() {
	grid = (char*)malloc(sizeof(char) * rows * columns);
	set = (int*)malloc(sizeof(int) * rows * columns);
	sz = (int*)malloc(sizeof(int) * rows * columns);

	for (int i = 0; i < rows * columns; i++) {
		grid[i] = ' ';
		set[i] = i;
		sz[i] = 1;
	}
	rowsMatriceGrafa = rows * columns;
	columnsMatriceGrafa = rows * columns;


	int n = rows * columns;

	//generisati matricu grada
	matricaGrafa = (int*)malloc(sizeof(int) * (n * n));

	//
	if (defaultOpcija) {
		grid[(rows - 1) * columns + (columns - 1)] = '*';
	}
	else {
		grid[(ciljX) * columns + (ciljY)] = '*';
	}

}


int komsijaNaIstokuPostoji(int trenutniCvor){
	int row = trenutniCvor / columns;
	int column = trenutniCvor % columns;

	if (column + 1 >= columns) {
		return 0;
	}

	return 1;
}
int komsijaNaIstoku(int trenutniCvor) {
	int row = trenutniCvor / columns;
	int column = trenutniCvor % columns;

	return row * columns + (column + 1);

}
int komsijaNaJuguPostoji(int trenutniCvor) {
	int row = trenutniCvor / columns;
	int column = trenutniCvor % columns;

	if (row + 1 >= rows) {
		return 0;
	}

	return 1;
}
int komsijaNaJugu(int trenutniCvor) {
	int row = trenutniCvor / columns;
	int column = trenutniCvor % columns;

	return (row + 1) * columns + column;

}

void generisiSvePutanje() {
		for (int i = 0; i < rows * columns; i++) {
		if (komsijaNaIstokuPostoji(i)) {
			ubaciUListu(napraviNovi(i, komsijaNaIstoku(i)), &susedniCvorovi);
		}
		if (komsijaNaJuguPostoji(i)) {
			ubaciUListu(napraviNovi(i, komsijaNaJugu(i)), &susedniCvorovi);
		}
	}
}


int root(int indeks) {
	int i = indeks;
	while (set[i] != i) {
		i = set[i];
	}
	return set[i];
}
int postojiCiklus(int i, int j) {
	return root(i) == root(j) ? 1 : 0;
}
void unija(int p, int q) {
	int i = root(p);
	int j = root(q);
	if (i == j) return;

	if (sz[i] < sz[j]) {
		set[i] = j;
		sz[j] += sz[i];
	}
	else {
		set[j] = i;
		sz[i] += sz[j]; 
	}
}


void KruskalovAlgoritam() {
	//izaberi random cvor iz komsija dokle god ima komsija u listi
	int ukupanBrojCvorovaUListi = brojCvorovaUListi(susedniCvorovi);
	while (susedniCvorovi != NULL) {
		srand(time(NULL));
		int broj = rand() % ukupanBrojCvorovaUListi;
		Komsija* cvor = pronadjiCvorPoIndeksu(broj, susedniCvorovi);
		//popuni matricu grafa sa dobijenim podacija
		int prviCvor = cvor->indeks_komsija1;
		int drugiCvor = cvor->indeks_komsija2;

		////check if when merged into one set will form a cycle
		if (!postojiCiklus(prviCvor, drugiCvor)) {
			unija(prviCvor, drugiCvor);

			//popuni matricu grafa
			matricaGrafa[prviCvor * columnsMatriceGrafa + drugiCvor] = 1;
			matricaGrafa[drugiCvor * columnsMatriceGrafa + prviCvor] = 1;
			obrisiCvor(&susedniCvorovi, cvor);
			ukupanBrojCvorovaUListi--;
		}
		else {

			obrisiCvor(&susedniCvorovi, cvor);
			ukupanBrojCvorovaUListi--;
		}

	}
}


void nacrtajMapU() {
	//top
	printf("+");
	for (int i = 0; i < columns; i++) {
		printf("---+");
	}
	printf("\n");


	for (int i = 0; i < rows; i++) {
		char top = '|';
		char bottom = '+';

		printf("%c", top);
		for (int j = 0; j < columns; j++) {

			//char telo[4] = "   ";
			char istok = '|';
			int indeksTrenutnog = i * columns + j;
			if (komsijaNaIstokuPostoji(indeksTrenutnog)) {
				int indeksKomsije = komsijaNaIstoku(indeksTrenutnog);
				if (matricaGrafa[indeksTrenutnog * columnsMatriceGrafa + indeksKomsije] == 1) {
					istok = ' ';
				}
				else {
					istok = '|';
				}
			}
			else {
				istok = '|';
			}

			printf(" %c %c", grid[i * columns + j], istok);
		}
		printf("\n");
		printf("%c", bottom);
		for (int j = 0; j < columns; j++) {

			char jug[4] = "   ";
			char ugao = '+';
			int indeksTrenutnog = i * columns + j;
			if (komsijaNaJuguPostoji(indeksTrenutnog)) {
				int indeksKomsije = komsijaNaJugu(indeksTrenutnog);
				if (matricaGrafa[indeksTrenutnog * columnsMatriceGrafa + indeksKomsije] == 1) {
					strcpy(jug, "   ");
				}
				else {
					strcpy(jug, "---");
				}
			}
			else {
				strcpy(jug, "---");
			}
			printf("%s%c", jug, ugao);

		}

		printf("\n");
	}
}
int proveraUnetihVrednosti(int a, int b) {
	if (a >= rows || a < 0 || b >= columns || b < 0) {
		printf("Koordinate cilje moraju da budu u opsegu x:[0, %d]; y:[0, %d]\n\n", rows -1 , columns-1);
		return 0;
	}
	return 1;
}

int main(int argc, char* argv[]) {

	if (argc == 1) {
		printf("Niste uneli nijedan parametar putem komandne linije\n");
		printf("Unesi broj redova: \n");
		scanf(" %d", &rows);
		printf("unesi broj kolona: \n");
		scanf(" %d", &columns);
	}
	else if (argc == 3) {
		rows = atoi(argv[1]);
		columns = atoi(argv[2]);
		printf("broj redova: %s\n", argv[1]);
		printf("broj kolona: %s\n", argv[2]);
	}
	else if (argc == 6) {
		rows = atoi(argv[1]);
		columns = atoi(argv[2]);
		printf("broj redova: %s\n", argv[1]);
		printf("broj kolona: %s\n", argv[2]);
		if (strcmp(argv[3], "-dest") == 0) {
			if (proveraUnetihVrednosti(atoi(argv[4]), atoi(argv[5]))) {
				ciljX = atoi(argv[4]);
				ciljY = atoi(argv[5]);
				defaultOpcija = 0;
			}
		}
		else {
			printf("Nepravilan unos koordinata cilja\n");
		}
	}
	else {
		printf("pokusajte ponovo\n");
	}


	
	inicijalizacija();

	generisiSvePutanje();

	KruskalovAlgoritam();
	
	nacrtajMapU();

	

	return 0;

}