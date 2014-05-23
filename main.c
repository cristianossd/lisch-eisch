#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define TAMANHO_ARQUIVO 11

typedef struct Registro {
	int 	chave, idade, prox;
	char 	nome[20];
	bool 	marcador;
} Registro;

int hashing(int chave) {
	return chave % TAMANHO_ARQUIVO;
}

void popular(char tipo_colisao) {
	FILE *f;
	Registro reg;
	int i, r = TAMANHO_ARQUIVO;

	switch (tipo_colisao) {
		case 'l':
			f = fopen("files/lisch.dat", "r+b");
			break;
		case 'e':
			f = fopen("files/eisch.dat", "r+b");
			break;
	}
	for (i=0; i<TAMANHO_ARQUIVO; i++) {
		fseek(f, i*sizeof(Registro), SEEK_SET);
		reg.marcador = false;
		fwrite(&reg, sizeof(Registro), 1, f);
	}

	// seta o r na última posição vazia, nesse caso, a TAMANHO_ARQUIVO
	fseek(f, TAMANHO_ARQUIVO*sizeof(Registro), SEEK_SET);
	fwrite(&r, sizeof(int), 1, f);
	fclose(f);
}

_Bool consulta(int chave, char tipo_colisao) {
	FILE *f;
	Registro reg;
	bool result = false;
	int i=0;

	printf("Vai consultar\n");
	switch (tipo_colisao) {
		case 'l':
			f = fopen("files/lisch.dat", "r+b");
			break;
		case 'e':
			f = fopen("files/eisch.dat", "r+b");
			break;
	}
	while ((!result)&&(i<TAMANHO_ARQUIVO)) {
		fseek(f, i*sizeof(Registro), SEEK_SET);
		fread(&reg, sizeof(Registro), 1, f);
		if (reg.chave == chave) {
			result = true;
		}
		i++;
	}

	fclose(f);
	return result;
}

void reposicionar(tipo_colisao) {
	FILE *f;
	Registro reg;
	int r, i=TAMANHO_ARQUIVO-1;
	bool result = false;

	switch (tipo_colisao) {
		case 'l':
			f = fopen("files/lisch.dat", "r+b");
			break;
		case 'e':
			f = fopen("files/eisch.dat", "r+b");
			break;
	}
	while ((!result)&&(i>=0)) {
		fseek(f, i*sizeof(Registro), SEEK_SET);
		fread(&reg, sizeof(Registro), 1, f);
		if (!reg.marcador) {
			result = true;
			r = i;
		}
		i--;
	}
	if (!result) {
		r = -1;
	}
	fseek(f, TAMANHO_ARQUIVO*sizeof(Registro), SEEK_SET);
	fwrite(&r, sizeof(int), 1, f);
	fclose(f);
}

void lisch(int chave, char *nome, int idade, int hash, int r) {
	FILE *f;
	Registro reg, novo_reg;
	int prox_reg;

	f = fopen("files/lisch.dat", "r+b");
	fseek(f, hash*sizeof(Registro), SEEK_SET);
	fread(&reg, sizeof(Registro), 1, f);
	while (reg.prox != -1) {
		prox_reg = reg.prox;
		fseek(f, prox_reg*sizeof(Registro), SEEK_SET);
	}
	novo_reg.chave = chave;
	strcpy(novo_reg.nome, nome);
	novo_reg.idade = idade;
	novo_reg.prox = -1;
	novo_reg.marcador = true;

	fseek(f, r*sizeof(Registro), SEEK_SET);
	fwrite(&novo_reg, sizeof(Registro), 1, f);
	fseek(f, TAMANHO_ARQUIVO*sizeof(Registro), SEEK_SET);
	r = -1;
	fwrite(&r, sizeof(int), 1, f);
	fclose(f);
	reposicionar('l');
}

void eisch(int chave, char *nome, int idade, int hash, int r) {
	FILE *f;
	Registro reg, novo_reg;
	int prox_reg;

	f = fopen("files/eisch.dat", "r+b");
	fseek(f, hash*sizeof(Registro), SEEK_SET);
	fread(&reg, sizeof(Registro), 1, f);
	novo_reg.chave = chave;
	strcpy(novo_reg.nome, nome);
	novo_reg.idade = idade;
	novo_reg.prox = reg.prox;
	novo_reg.marcador = true;

	reg.prox = r;
	fwrite(&reg, sizeof(Registro), 1, f);

	fseek(f, r*sizeof(Registro), SEEK_SET);
	fwrite(&novo_reg, sizeof(Registro), 1, f);
	fseek(f, TAMANHO_ARQUIVO*sizeof(Registro), SEEK_SET);
	r = -1;
	fwrite(&r, sizeof(int), 1, f);
	fclose(f);
	reposicionar('e');
}

_Bool inserir(int chave, char *nome, int idade, char tipo_colisao) {
	FILE *f;
	Registro reg;
	bool result;
	int r, hash;

	printf("Vai inserir\n");
	result = consulta(chave, tipo_colisao);
	if (result) {
		return false;
	}
	else {
		switch (tipo_colisao) {
			case 'l':
				f = fopen("files/lisch.dat", "r+b");
				break;
			case 'e':
				f = fopen("files/eisch.dat", "r+b");
				break;
		}

		fseek(f, TAMANHO_ARQUIVO*sizeof(Registro), SEEK_SET);
		fread(&r, sizeof(int), 1, f);

		if (r == -1) {
			return true;
		}
		else {
			hash = hashing(chave);
			fseek(f, hash*sizeof(Registro), SEEK_SET);
			fread(&reg, sizeof(Registro), 1, f);
			if (reg.marcador) {
				fclose(f);
				switch (tipo_colisao) {
					case 'l':
						lisch(chave, nome, idade, hash, r);
						break;
					case 'e':
						eisch(chave, nome, idade, hash, r);
						break;
				}
			}
			else {
				reg.chave = chave;
				strcpy(reg.nome, nome);
				reg.idade = idade;
				reg.prox = -1;
				reg.marcador = true;
				fwrite(&reg, sizeof(Registro), 1, f);
				fclose(f);
				printf("Inseriu!\n");
			}

			return true;
		}
	}
}

int main() {
	FILE *f;
	Registro reg;
	char tipo_colisao, opcao, nome[20];
	int r, chave, idade, result;

	scanf(" %c", &tipo_colisao);
	switch (tipo_colisao) {
		case 'l':
			f = fopen("files/lisch.dat", "r+b");
			if (f == NULL) {
				f = fopen("files/lisch.dat", "w+b");
			}
			break;
		case 'e':
			f = fopen("files/eisch.dat", "r+b");
			if (f == NULL) {
				f = fopen("files/eisch.dat", "w+b");
			}
			break;
	}
	fclose(f);
	popular(tipo_colisao);


// Testes sobre o funcionamento da função popular
/*	fopen("files/lisch.dat", "r+b");
	fseek(f, TAMANHO_ARQUIVO*sizeof(Registro), SEEK_SET);
	fread(&r, sizeof(int), 1, f);
	printf("%d\n", r);
	fclose(f);
*/

	scanf(" %c", &opcao);
	while (opcao != 'e') {
		switch (opcao) {
			case 'i':
				// Fluxo para inserção
				scanf(" %d ", &chave);
				gets(nome);
				scanf(" %d", &idade);

				printf("Chamando funcao p inserir: %d %s %d\n", chave, nome, idade);
				result = inserir(chave, nome, idade, tipo_colisao);
				if (!result) {
					printf("chave ja existente: %d\n", chave);
				}
				printf("Saiu da inserção\n");
				break;
			case 'c':
				// Fluxo para consulta
				scanf(" %d", &chave);
				result = consulta(chave, tipo_colisao);
				if (result) {
					printf("chave: %d\n%s\n%d\n", chave, nome, idade);
				}
				else {
					printf("chave nao encontrada: %d\n", chave);
				}
				break;
		}
		scanf(" %c", &opcao);
	}
	return 0;
}