/*
	O endereço para os arquivos são:
		lisch: files/lisch.dat
		eisch: files/eisch.dat
*/

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
	int i, r = TAMANHO_ARQUIVO-1;

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
		reg.prox = -1;
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
	int i=0, hash;

	switch (tipo_colisao) {
		case 'l':
			f = fopen("files/lisch.dat", "r+b");
			break;
		case 'e':
			f = fopen("files/eisch.dat", "r+b");
			break;
	}
/*
	while ((!result)&&(i<TAMANHO_ARQUIVO)) {
		fseek(f, i*sizeof(Registro), SEEK_SET);
		fread(&reg, sizeof(Registro), 1, f);
		if (reg.chave == chave) {
			result = true;
		}
		i++;
	}
*/
	hash = hashing(chave);
	fseek(f, hash*sizeof(Registro), SEEK_SET);
	fread(&reg, sizeof(Registro), 1, f);
	while ((reg.chave != chave)&&(reg.prox != -1)) {
		fseek(f, reg.prox*sizeof(Registro), SEEK_SET);
		fread(&reg, sizeof(Registro), 1, f);
	}
	fclose(f);
	if (reg.chave == chave)
		return true;
	return false;
}

void reposicionar(char tipo_colisao) {
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
	prox_reg = hash;
	while (reg.prox != -1) {
		prox_reg = reg.prox;
		fseek(f, prox_reg*sizeof(Registro), SEEK_SET);
		fread(&reg, sizeof(Registro), 1, f);
	}

	reg.prox = r;
	fseek(f, prox_reg*sizeof(Registro), SEEK_SET);
	fwrite(&reg, sizeof(Registro), 1, f);

	novo_reg.chave = chave;
	strcpy(novo_reg.nome, nome);
	novo_reg.idade = idade;
	novo_reg.prox = -1;
	novo_reg.marcador = true;

	fseek(f, r*sizeof(Registro), SEEK_SET);
	fwrite(&novo_reg, sizeof(Registro), 1, f);
	fseek(f, r*sizeof(Registro), SEEK_SET);
	fread(&novo_reg, sizeof(Registro), 1, f);

	fseek(f, TAMANHO_ARQUIVO*sizeof(Registro), SEEK_SET);
	r = -1;
	fwrite(&r, sizeof(int), 1, f);
	fclose(f);
	reposicionar('l');

/*
	Teste r´apido
	f = fopen("files/lisch.dat", "r+b");
	fseek(f, 10*sizeof(Registro), SEEK_SET);
	fread(&novo_reg, sizeof(Registro), 1, f);
	fseek(f, TAMANHO_ARQUIVO*sizeof(Registro), SEEK_SET);
	fread(&r, sizeof(int), 1, f);
	printf("Depois de reposicionar o R. %d %s %d e foi para a pos: %d\n", novo_reg.chave, novo_reg.nome, novo_reg.idade, r);
	fclose(f);
*/
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
	fseek(f, hash*sizeof(Registro), SEEK_SET);
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
				fseek(f, hash*sizeof(Registro), SEEK_SET);
				fwrite(&reg, sizeof(Registro), 1, f);
				fclose(f);
			}

			return true;
		}
	}
}

void imprime_chave(int chave, char tipo_colisao) {
	FILE *f;
	Registro reg;
	bool result = false;
	int i=0;

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
			printf("chave: %d\n%s\n%d\n", reg.chave, reg.nome, reg.idade);
		}
		i++;
	}

	fclose(f);	
}

void imprimir(char tipo_colisao) {
	FILE *f;
	Registro reg;
	int i;

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
		fread(&reg, sizeof(Registro), 1, f);
		if (reg.marcador) {
			if (reg.prox == -1)
				printf("%d: %d %s %d nulo\n", i, reg.chave, reg.nome, reg.idade);
			else
				printf("%d: %d %s %d %d\n", i, reg.chave, reg.nome, reg.idade, reg.prox);
		}
		else {
			printf("%d: vazio\n", i);
		}
	}

	fclose(f);
}

float media(char tipo_colisao) {
	FILE *f;
	Registro reg, reg_apoio;
	int i, hash;
	float acesso, qtd=0, cont=0;

	switch (tipo_colisao) {
		case 'l':
			f = fopen("files/lisch.dat", "r+b");
			break;
		case 'e':
			f = fopen("files/eisch.dat", "r+b");
			break;
	}

	for (i = 0; i < TAMANHO_ARQUIVO; i++) {
		fseek(f, i*sizeof(Registro), SEEK_SET);
		fread(&reg, sizeof(Registro), 1, f);
		if (reg.marcador) {
			qtd += 1;
			hash = hashing(reg.chave);
			fseek(f, hash*sizeof(Registro), SEEK_SET);
			fread(&reg_apoio, sizeof(Registro), 1, f);
			while (reg.chave != reg_apoio.chave) {
				fseek(f, reg_apoio.prox*sizeof(Registro), SEEK_SET);
				fread(&reg_apoio, sizeof(Registro), 1, f);
				cont += 1;
			}
			cont += 1;
		}
	}
	fclose(f);
	if (qtd >0) {
		acesso = cont/qtd;
		return acesso;
	}
	return 0;
}

void reinicializar(int pos, char tipo_colisao) {
	FILE *f;
	Registro reg;

	switch (tipo_colisao) {
		case 'l':
			f = fopen("files/lisch.dat", "r+b");
			break;
		case 'e':
			f = fopen("files/eisch.dat", "r+b");
			break;
	}

	fseek(f, pos*sizeof(Registro), SEEK_SET);
	fread(&reg, sizeof(Registro), 1, f);
	reg.prox = -1;
	reg.marcador = false;
	fwrite(&reg, sizeof(Registro), 1, f);
	fclose(f);
}

_Bool remover(int chave, char tipo_colisao) {
	FILE *f;
	Registro reg_linkador, reg_buraco, reg, reg1, reg2, reg3;
	int i, hash, buraco, caso, substituto = -1, provisorio, linkador, linkado;
	bool result = false;

	if (consulta(chave, tipo_colisao)) {
		switch (tipo_colisao) {
			case 'l':
				f = fopen("files/lisch.dat", "r+b");
				break;
			case 'e':
				f = fopen("files/eisch.dat", "r+b");
				break;
		}
		hash = hashing(chave);
		buraco = hash;
		fseek(f, buraco*sizeof(Registro), SEEK_SET);
		fread(&reg, sizeof(Registro), 1, f);
		if (reg.chave == chave)
			result = true;		

		// acha o registro pra remover
		while ((!result)&&(reg.prox != -1)) {
			buraco = reg.prox;
			fseek(f, buraco*sizeof(Registro), SEEK_SET);
			fread(&reg, sizeof(Registro), 1, f);
			if (reg.chave == chave)
				result = true;
		}

		while (buraco != -1) {
			// sei que a posição q tenho q remover esta sendo apontada pela var buraco
			fseek(f, buraco*sizeof(Registro), SEEK_SET);
			fread(&reg, sizeof(Registro), 1, f);
			strcpy(reg.nome, "aqui");

			substituto = -1;
			// anunciando qual registro tenho q substituir

			// variavel substituto informa qual registro subsittuirá
			result = false;
			fseek(f, buraco*sizeof(Registro), SEEK_SET);
			fread(&reg1, sizeof(Registro), 1, f);
			while ((!result)&&(reg1.prox != -1)) {
				fseek(f, reg1.prox*sizeof(Registro), SEEK_SET);
				provisorio = reg1.prox;
				fread(&reg1, sizeof(Registro), 1, f);
				if (hashing(reg1.chave) == buraco) {
					result = true;
					substituto = provisorio;
				}
			}
			if (substituto == -1) {
				// fazer a linkagem
				caso = buraco;
				i=0;
				result = false;
				while ((i<TAMANHO_ARQUIVO)&&(!result)) {
					fseek(f, i*sizeof(Registro), SEEK_SET);
					fread(&reg_linkador, sizeof(Registro), 1, f);
					if (reg_linkador.prox == buraco) {
						linkador = i;
						result = true;
					}
					i++;
				}
				if (result) {
					fseek(f, buraco*sizeof(Registro), SEEK_SET);
					fread(&reg_buraco, sizeof(Registro), 1, f);

					if (reg_buraco.prox != -1) {
						linkado = reg_buraco.prox;

						fseek(f, linkador*sizeof(Registro), SEEK_SET);
						fread(&reg_linkador, sizeof(Registro), 1, f);
						reg_linkador.prox = linkado;

						fseek(f, linkador*sizeof(Registro), SEEK_SET);
						fwrite(&reg_linkador, sizeof(Registro), 1, f);
					}
					else {
						fseek(f, linkador*sizeof(Registro), SEEK_SET);
						fread(&reg_linkador, sizeof(Registro), 1, f);

						reg_linkador.prox = -1;

						fwrite(&reg_linkador, sizeof(Registro), 1, f);	
					}
				}
				fclose(f);
				reinicializar(buraco-1, tipo_colisao);

				buraco = -1;
			}
			else {
				fseek(f, buraco*sizeof(Registro), SEEK_SET);
				fread(&reg2, sizeof(Registro), 1, f);

				fseek(f, substituto*sizeof(Registro), SEEK_SET);
				fread(&reg3, sizeof(Registro), 1, f);


				reg2.chave = reg3.chave;
				strcpy(reg2.nome, reg3.nome);
				reg2.idade = reg3.idade;

				fseek(f, buraco*sizeof(Registro), SEEK_SET);
				fwrite(&reg2, sizeof(Registro), 1, f);

				buraco = substituto;
			}
		}
	}
	else {
		fclose(f);
		return false;
	}
	reposicionar(tipo_colisao);
	return true;
}

int main() {
	FILE *f;
	Registro reg;
	char tipo_colisao, opcao, nome[20];
	int r, chave, idade, result;
	bool fechado = false;
	float media_acesso;

	scanf(" %c", &tipo_colisao);
	switch (tipo_colisao) {
		case 'l':
			f = fopen("files/lisch.dat", "r+b");
			if (f == NULL) {
				f = fopen("files/lisch.dat", "w+b");
				fclose(f);
				popular('l');
				fechado = true;
			}
			if (!fechado)
				fclose(f);

			break;
		case 'e':
			f = fopen("files/eisch.dat", "r+b");
			if (f == NULL) {
				f = fopen("files/eisch.dat", "w+b");
				fclose(f);
				popular('e');
				fechado = true;
			}
			if (!fechado)
				fclose(f);
			break;
	}
	


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

				result = inserir(chave, nome, idade, tipo_colisao);
				if (!result) {
					printf("chave ja existente: %d\n", chave);
				}
				break;
			case 'c':
				// Fluxo para consulta
				scanf(" %d", &chave);
				result = consulta(chave, tipo_colisao);
				if (result) {
					imprime_chave(chave, tipo_colisao);
				}
				else {
					printf("chave nao encontrada: %d\n", chave);
				}
				break;
			case 'p':
				// Fluxo para impressão do arquivo
				imprimir(tipo_colisao);
				break;
			case 'm':
				// Fluxo para média de acesso
				media_acesso = media(tipo_colisao);
				printf("%.1f\n", media_acesso);
				break;
			case 'r':
				// Fluxo para remoção
				scanf(" %d", &chave);
				result = remover(chave, tipo_colisao);
				if (!result) {
					printf("chave nao encontrada: %d\n", chave);
				}
				break;
		}
		scanf(" %c", &opcao);
	}

/*
	Outro teste rápido
	Registro teste;
	f = fopen("files/lisch.dat", "r+b");
	fseek(f, (TAMANHO_ARQUIVO-1)*sizeof(Registro), SEEK_SET);
	fread(&teste, sizeof(Registro), 1, f);
	printf("----- %d %s\n", teste.chave, teste.nome);
	fclose(f);
*/
	return 0;
}