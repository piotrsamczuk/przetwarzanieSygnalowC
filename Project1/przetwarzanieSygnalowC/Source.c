#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <conio.h>
#define COUNT 500 //liczba elementow tablicy
#define	PARAM_NUM 3 //liczba parametrów, tu: a,b,c
#define FORMULA (param[0] + (param[1] * sin(z)) - (cos(z) / param[2])) //z to inaczej delta x, do funkcji sin i cos wpisujemy kat w radianach
#define NOISEPERCENT 0.1 // z jakiego procenta ekstremum mamy losowac szum
#define MAXCHARS 32 // maksymalna ilosc znakow spisywana z linijki we wczytywanym pliku
#define MAXUSERINPUT 256
#define LINENUM 500 //maksymalna liczba linijek w odczytywanym pliku

bool asciiDblCheck(char* input) //funkcja zwraca jeden gdy: na poczatku jest minus(lub nie), wpisana jest liczba skladajaca sie z cyfr od 0 do 9 w jednym miejscu oddzielona przecinkiem
{
	if (input[strlen(input) - 1] != '\n') //odpowiednia dlugosc
		return 0;

	if (input[0] == '\n')
		return 0;

	int len = strlen(input);
	if (len > 255)
		return 0;

	int i = 0;
	if (input[i] == '-')
		i++;

	int commaNum = 0; //liczba przecinkow
	while (i < len && commaNum < 2)
	{
		if (input[i] > 47 && input[i] < 58)
			i++;
		else if (input[i] == '\n')
			return 1;
		else if (input[i] == ',')
		{
			commaNum++;
			i++;
		}
		else
			return 0;
	}
	return 0;
}

int rearrangeDomains(double domain[])
{
	if (domain[1] == domain[0])
	{
		printf("\nGranice dziedziny musza sie roznic.\n");
		return 0;
	}
	else if (domain[0] > domain[1]) //jesli wartosc dolnej granicy jest wieksza niz gornej to zamieniamy je miejscami w tablicy
	{
		double domainTemp;
		domainTemp = domain[0];
		domain[0] = domain[1];
		domain[1] = domainTemp;
	}
	return 1;
}

void getExtremes(double arr[], double extreme[])
{
	extreme[0] = arr[0];
	extreme[1] = arr[0];

	for (int i = 0; i < COUNT; i++)
	{
		if (extreme[0] > arr[i])
		{
			extreme[0] = arr[i];
		}
		if (extreme[1] < arr[i])
		{
			extreme[1] = arr[i];
		}
	}
	rearrangeDomains(extreme);
}

void writeFormula(double param[], double res[], double domain[], double dx, double noiseRes[], double extreme[])
{
	double z = domain[0];
	extreme[0] = FORMULA;
	extreme[1] = FORMULA;
	for (int i = 0; i < COUNT; i++)
	{
		z = domain[0] + (dx * i); // dla jakiego x'a wypisywana jest wartosc
		res[i] = FORMULA;
		noiseRes[i] = FORMULA; //przyda sie aby zaszumic pozniej tablice 
		printf("\n%d wynik dla f(%.7g)= %lf", i + 1, domain[0] + (dx * i), res[i]); // zamiast 'z' mozna od razu domain[0] + (dx * i)
	}
}

void writeNoise(double noiseRes[], double extreme[])
{
	double noiseRange = fabs((extreme[1] - extreme[0])) * NOISEPERCENT;
	double noiseMin = -(noiseRange / 2);

	for (int i = 0; i < COUNT; i++)
	{
		if ((rand() % 2) == 1)
		{
			noiseRes[i] += noiseMin + (rand() / (RAND_MAX / noiseRange)); // 50% szans na wygenerowanie szumu o dziedzinie o amplitudzie noiserange
		}

	}
}

int readDomain(double domain[])
{
	char* buffer = calloc(MAXUSERINPUT, sizeof(char)); //uzytkownik moze wprowadzic maksymalnie 255 znakow + \n
	if (buffer == NULL)
	{
		printf("\nBlad alokacji pamieci.\n");
		return -1;
	}

	for (int i = 0; i < 2; i++)
	{
		printf("Wpisz %d czesc dziedziny:", i + 1);
		fgets(buffer, MAXUSERINPUT - 1, stdin);
		if (asciiDblCheck(buffer) == 0)
		{
			i--;
			continue;
		}
		else
			(void)sscanf(buffer, "%lf", &domain[i]);
	}
	free(buffer);
	if (rearrangeDomains(domain) == 0)
		readDomain(domain);
	return 1;
}

int readParam(double param[])
{
	char* buffer = calloc(MAXUSERINPUT, sizeof(char)); //uzytkownik moze wprowadzic maksymalnie 255 znakow + \n
	if (buffer == NULL)
	{
		printf("\nBlad alokacji pamieci.\n");
		return -1;
	}
	for (int i = 0; i < PARAM_NUM; i++)
	{
		printf("Podaj wartosc %d parametru:", i + 1);
		fgets(buffer, MAXUSERINPUT - 1, stdin);
		if (asciiDblCheck(buffer) == 0)
		{
			i--;
			continue;
		}
		else
			(void)sscanf(buffer, "%lf", &param[i]);
	}
	printf("\n");
	free(buffer);
	return 1;
}

void saveGeneratedArr(double arr[], double dx, double domain[], char* nazwa)
{
	FILE* fptr;
	fptr = fopen(nazwa, "w");
	if (fptr == NULL)
	{
		printf("\nBlad zapisywania pliku");
		return;
	}
	else
		printf("\nZapisywanie...\n");

	for (int i = 0; i < COUNT; i++) //wpisywanie wartosci po sredniku
	{
		fprintf(fptr, "%lf;%lf\n", domain[0] + (dx * i), arr[i]);
	}

	fclose(fptr);
}

int countLines(char* nazwa)
{
	int lineNum = 0;
	char c = 0;
	FILE* fptr;
	fptr = fopen(nazwa, "r");
	if (fptr == NULL)
	{
		printf("\nBlad otwierania pliku");
		return -1;
	}

	for (c = fgetc(fptr); c != EOF; c = fgetc(fptr))
	{
		if (c == '\n')
			lineNum++;
	}

	fclose(fptr);
	return lineNum;
}

void readFile(char* nazwa, double* arrX, double* arrY, int lineNum)
{
	FILE* fptr;
	fptr = fopen(nazwa, "r");
	if (fptr == NULL)
	{
		printf("\nBlad otwierania pliku");
		return;
	}
	else
		printf("\nWczytywanie...\n");

	char* line = calloc(MAXUSERINPUT, sizeof(char));
	char* lineptr;

	if (line == NULL)
	{
		printf("\nBlad alokacji pamieci.");
		return;
	}

	int i = 0;
	while (fgets(line, MAXUSERINPUT, fptr) != NULL)
	{
		if (line[strlen(line) - 1] == '\n') //kazda linijka konczy sie znakiem nowej linii
		{
			fputs(line, stdout); //test
			arrX[i] = strtod(line, &lineptr);	//strtod to funkcja przeksztalcajaca string na double
			lineptr++;
			arrY[i] = strtod(lineptr, NULL);
			line[0] = '\0';
		}
		i++;
	}
	free(line);
	fclose(fptr);
}

void sortArray(double* arr, int arrsize) //najprostszy bubble (wolny)
{
	double temp; //tymczasowa zmienna do podmiany dwoch wartosci w tablicy
	for (int i = 0; i < arrsize - 1; i++)
	{
		for (int j = 0; j < arrsize - i - 1; j++)
		{
			if (arr[j] > arr[j + 1])
			{
				temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
		}
	}
}


void medianFilter(double* arrY, int linenum, int samplesize, double* median)
{
	int midpoint = samplesize / 2; //srodkowe polozenie okna o rozmiarze samplesize w tablicy
	double* temparr = calloc(samplesize, sizeof(double));
	if (temparr == NULL)
	{
		printf("\nBlad alokacji pamieci.");
		return -1;
	}

	for (int i = 0; i < linenum; i++)
	{
		median[i] = arrY[i];
	}

	double* mirrorArr1 = calloc(midpoint, sizeof(double));
	double* mirrorArr2 = calloc(midpoint, sizeof(double));
	for (int i = 0, j = midpoint - 1; i < midpoint; i++, j--)
	{
		mirrorArr1[i] = arrY[j];
	}
	for (int i = 0, j = linenum - 1; i < midpoint; i++, j--)
	{
		mirrorArr2[i] = arrY[j];
	}

	for (int i = 0; i < midpoint; i++) //filtr gdzie okno filtra uzywa danych odbitych lustrzanie z poczatku tablicy
	{
		for (int j = i; j < i + samplesize; j++)
		{
			if (j < midpoint + i)
				temparr[j - i] = mirrorArr1[j];
			else
				temparr[j - i] = arrY[j - midpoint];
		}
		sortArray(temparr, samplesize);
		median[i] = temparr[samplesize / 2];
	}
	for (int i = linenum - midpoint; i < linenum; i++) //filtr gdzie okno filtra uzywa danych odbitych lustrzanie z konca tablicy
	{
		for (int j = i, k = 0; j < i + samplesize; j++, k++)
		{
			if (j - midpoint > linenum - 1)
				temparr[j - i] = mirrorArr2[k];
			else
				temparr[j - i] = arrY[j - midpoint];
		}
		sortArray(temparr, samplesize);
		median[i] = temparr[samplesize / 2];
	}

	for (int i = 0; i < linenum - samplesize; i++) //wykona sie od tyle razy ile zmiesci sie okno filtra o wielkosci samplesize w tablicy
	{
		for (int j = i; j < i + samplesize; j++) //w petli przydziela sie do okna filtra wartosci odpowiadajace tym miejscom w tablicy wczytanej
		{
			temparr[j - i] = arrY[j]; // zakres temparr to od 0 do dlugosci okna filtra
		}
		sortArray(temparr, samplesize);
		median[i + midpoint] = temparr[samplesize / 2];
	}
	free(temparr);
	free(mirrorArr1);
	free(mirrorArr2);
}

void averageFilter(double* arrY, int linenum, int samplesize, double* average)
{
	int midpoint = samplesize / 2; //srodkowe polozenie okna o rozmiarze samplesize w tablicy
	double sum = 0.0;

	for (int i = 0; i < linenum; i++)
	{
		average[i] = arrY[i];
	}

	double* mirrorArr1 = calloc(midpoint, sizeof(double));
	double* mirrorArr2 = calloc(midpoint, sizeof(double));
	for (int i = 0, j = midpoint - 1; i < midpoint; i++, j--)
	{
		mirrorArr1[i] = arrY[j];
	}
	for (int i = 0, j = linenum - 1; i < midpoint; i++, j--)
	{
		mirrorArr2[i] = arrY[j];
	}

	for (int i = 0; i < midpoint; i++) //filtr gdzie okno filtra uzywa danych odbitych lustrzanie z poczatku tablicy
	{
		for (int j = i; j < i + samplesize; j++)
		{
			if (j < midpoint)
				sum += mirrorArr1[j];
			else
				sum += arrY[j - midpoint];
		}
		average[i] = sum / samplesize;
		sum = 0.0;
	}
	for (int i = linenum - midpoint; i < linenum; i++) //filtr gdzie okno filtra uzywa danych odbitych lustrzanie z konca tablicy
	{
		for (int j = i, k = 0; j < i + samplesize; j++, k++)
		{
			if (j - midpoint > linenum - 1)
				sum += mirrorArr2[k];
			else
				sum += arrY[j - midpoint];
		}
		average[i] = sum / samplesize;
		sum = 0.0;
	}

	for (int i = 0; i < linenum - samplesize; i++) //wykona sie od tyle razy ile zmiesci sie okno filtra o wielkosci samplesize w tablicy
	{
		for (int j = i; j < i + samplesize; j++) //w petli przydziela sie do okna filtra wartosci odpowiadajace tym miejscom w tablicy wczytanej
		{
			sum += arrY[j];
		}
		average[i + midpoint] = sum / samplesize;
		sum = 0;
	}
}

void saveForeignArray(int linenum, char* nazwa, double* arrX, double* arrY)
{
	FILE* fptr;
	fptr = fopen(nazwa, "w");
	if (fptr == NULL)
	{
		printf("\nBlad zapisywania pliku");
		return;
	}
	else
		printf("\nZapisywanie...\n");

	for (int i = 0; i < linenum; i++) //wpisywanie wartosci po sredniku
	{
		fprintf(fptr, "%lf;%lf\n", arrX[i], arrY[i]);
	}

	fclose(fptr);
}

void getFilename(char* nazwa) //zmienic zeby nie bylo przeciekow pamieci
{
	// nazwa pliku moze miec w windowsie maksymalnie 255 znakow + \0
	printf("\nPodaj nazwe pliku(z rozszerzeniem np. csv, txt):");
	(void)scanf("%s", nazwa);

	if (nazwa == NULL)
	{
		printf("\nBlad nazwy pliku.\n");
		return;
	}
}
int main()
{
	setlocale(LC_ALL, "polish_poland");
	printf("1)Wygeneruj tablice %d-elementowa i ja zapisz, nastepnie dodaj szum do wygenerowanej tablicy i ja zapisz.\n2)Wczytaj tablice wynikow z pliku, wykonaj odszumianie wczytanej tablicy filtrem medianowym i zapisz ja.\n3)Wczytaj tablice wynikow z pliku, wykonaj odszumianie wczytanej tablicy filtrem sredniej ruchomej i zapisz ja.\nx)Wyjdz z programu.\n", COUNT);
	double res[COUNT]; //tablica do zapisywania wynikow funkcji generujacej
	double noiseRes[COUNT]; //zaszumiona tablica z wynikami funkcji generujacej
	double param[PARAM_NUM]; //tablica z parametrami A B C
	double domain[2]; //dziedzina [dolna granica, górna granica]
	double extreme[2]; // maksimum i minimum funkcji aby wyznaczyc 10% amplitudy do generowania szumu
	srand((unsigned int)time(NULL));
	double* arrX = NULL;	//tablica do zapisania wynikow
	double* arrY = NULL;	//z obcego pliku
	int samplesize = 0;
	double range;
	double dx;
	char gnaName[MAXUSERINPUT];
	char gnnName[MAXUSERINPUT];
	char agaName[MAXUSERINPUT];
	char mdaName[MAXUSERINPUT];
	char ffile2[MAXUSERINPUT];
	char ffile[MAXUSERINPUT];
	double median[LINENUM];
	double average[LINENUM];

	char choice = 0;
	while (choice != 'x')
	{
		choice = _getch();
		switch (choice)
		{
		case '1':
		{
			if (readParam(param) == -1) //czytanie parametrow wprowadzanych przez uzytkownika
				return 0;
			if (param[2] == 0)
			{
				printf("Nie mozna dzielic przez 0.");
				return;
			}
			if (readDomain(domain) == -1) //czytanie granic dziedziny wprowadzanych przez uzytkownika
				return 0;
			range = domain[1] - domain[0]; //zakres: (górna granica - dolna granica)
			dx = range / COUNT; //delta x: zakres na liczbe probek, czyli co ile bedziemy wyswietlali wyniki funkcji
			printf("\nzakres: %.9g", range);
			printf("\ndelta x: %.9g", dx);
			printf("\nTworzenie tablicy z wynikami:");
			writeFormula(param, res, domain, dx, noiseRes, extreme);
			getExtremes(res, extreme);
			printf("\nEkstrema: %lf; %lf", extreme[0], extreme[1]);
			printf("\nNastapi zapisywanie tablicy z oryginalnymi wynikami.");
			getFilename(gnaName);
			saveGeneratedArr(res, dx, domain, gnaName);
			printf("\nTworzenie tablicy zaszumionej na podstawie wygenerowanej...");
			writeNoise(noiseRes, extreme);
			printf("\nNastapi zapisywanie tablicy z zaszumionymi wynikami.");
			getFilename(gnnName);
			saveGeneratedArr(noiseRes, dx, domain, gnnName);
			printf("\nWybierz inna funkcje programu lub wpisz x aby wyjsc.\n");
			break;
		}
		case '2':
		{
			printf("Nastapi czytanie pliku zewnetrznego...");
			getFilename(ffile);
			int linenum = countLines(ffile);
			if (arrX == NULL)
				arrX = calloc(linenum, sizeof(double));
			if (arrY == NULL)
				arrY = calloc(linenum, sizeof(double));
			if (arrX == NULL || arrY == NULL)
			{
				printf("\nBlad alokacji pamieci.");
				return -1;
			}
			readFile(ffile, arrX, arrY, linenum); //dane odczytane z pliku csv zapisane w arrX i arrY
			samplesize = 0;
			while (samplesize % 2 != 1)
			{
				printf("\nWybierz nieparzysta wielkosc okna filtra:");
				(void)scanf("%d", &samplesize);
			}
			if (median == NULL)
			{
				printf("\nBlad alokacji pamieci.");
				return -1;
			}
			medianFilter(arrY, linenum, samplesize, median);
			printf("\nNastapi zapisywanie tablicy wczytanej po odszumieniu filtrem medianowym...");
			getFilename(mdaName);
			saveForeignArray(linenum, mdaName, arrX, median);
			printf("\nWybierz inna funkcje programu lub wpisz x aby wyjsc.\n");
			break;
		}

		case '3':
		{
			printf("Nastapi czytanie pliku zewnetrznego...");
			getFilename(ffile2);
			int linenum2 = countLines(ffile2);
			if (arrX == NULL)
				arrX = calloc(linenum2, sizeof(double));
			if (arrY == NULL)
				arrY = calloc(linenum2, sizeof(double));
			if (arrX == NULL || arrY == NULL)
			{
				printf("\nBlad alokacji pamieci.");
				return -1;
			}
			readFile(ffile2, arrX, arrY, linenum2); //dane odczytane z pliku csv zapisane w arrX i arrY
			samplesize = 0;
			while (samplesize % 2 != 1)
			{
				printf("\nWybierz nieparzysta wielkosc okna filtra:");
				(void)scanf("%d", &samplesize);
			}
			if (average == NULL)
			{
				printf("\nBlad alokacji pamieci.");
				return -1;
			}
			if (samplesize % 2 != 1)
			{
				printf("\nBlad alokacji pamieci.");
				return -1;
			}
			averageFilter(arrY, linenum2, samplesize, average);
			printf("\nNastapi zapisywanie tablicy wczytanej po odszumieniu filtrem sredniej ruchomej...");
			getFilename(agaName);
			saveForeignArray(linenum2, agaName, arrX, average);
			printf("\nWybierz inna funkcje programu lub wpisz x aby wyjsc.\n");
			break;
		}
		case 'x':
			break;
		default:
			printf("\nTaka funkcja nie istnieje.\n");
		}
	}
	free(arrX);
	free(arrY);
	return 0;
}