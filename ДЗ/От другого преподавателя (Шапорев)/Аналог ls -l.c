#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <libgen.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

char* current_color = ANSI_COLOR_RESET;

void show_it(char* file, struct stat *info, int recursive);

// Вывести тип файла и права доступа к нему
void list_rights(char* file, struct stat* info) {

	// Тип файла
	if (S_ISDIR(info->st_mode)) // Директория
		printf("d");
	else if (S_ISREG(info->st_mode)) // Обычный файл
		printf("-");
	else if (S_ISBLK(info->st_mode)) // Блочный файл (файл устройства хранения, например, диск)
		printf("b");
	else if (S_ISCHR(info->st_mode)) // "Символьный" файл (например, файл звуковой карты, клавиатуры)
		printf("c");
	else if (S_ISSOCK(info->st_mode)) // Файл сокета (мдля межпроцессного взаимодействия)
		printf("s");
	else if (S_ISLNK(info->st_mode)) // Символьная ссылка
		printf("l");
	else if (S_ISFIFO(info->st_mode)) // Файл именованного канала (FIFO) или неименованного (pipe) (для межпроцессного взаимодействия)
		printf("l");
	else
		printf("?");

	// Права доступа на:
	printf((info->st_mode & S_IRUSR) ? "r" : "-"); // чтение у владельца
	printf((info->st_mode & S_IWUSR) ? "w" : "-"); // запись у владельца
	printf((info->st_mode & S_IXUSR) ? "x" : "-"); // выполнение у владельца
	printf((info->st_mode & S_IRGRP) ? "r" : "-"); // чтение у группы-владельца
	printf((info->st_mode & S_IWGRP) ? "w" : "-"); // запись у группы-владельца
	printf((info->st_mode & S_IXGRP) ? "x" : "-"); // выполнение у группы-владельца
	printf((info->st_mode & S_IROTH) ? "r" : "-"); // чтение у остальных
	printf((info->st_mode & S_IWOTH) ? "w" : "-"); // запись у остальных
	printf((info->st_mode & S_IXOTH) ? "x" : "-"); // выполнение у остальных
}

// Вывести всю информацию об одном файле
// file - путь к файлу
// info - информация о файле
// color - цвет вывода
void list_file(char* file, struct stat* info, char* color) {

	list_rights(file, info);
	char* b = basename(file); // Получение имени файла из всего пути до него

	// Количество ссылок на файл
	printf(" %4ld ", (long)info->st_nlink);

	// Размер файла (в байтах)
	printf("%8lld ", (long long) info->st_size);

	// UID - id владельца, GID - id группы владельца
	printf("UID %4ld GID %4ld ", (long) info->st_uid, (long) info->st_gid);

	// isatty() - проверяет, связан ли файл с терминалом, принтером, последовательным портом
	// 1 - дескриптор вывода программы (т.е. если вывод осуществляется в терминал, то он раскрашивается)
	if (isatty(1))
		printf(color);

	printf("%s", b);

	if (isatty(1))
		printf(ANSI_COLOR_RESET);
}

// Вывести информацию о блочном файле (пурпурного цвета)
void list_blkdev(char* file, struct stat* info) {
	list_file(file, info, ANSI_COLOR_MAGENTA);
}

// Вывести информацию о "символьном" файле (красного цвета)
void list_chardev(char* file, struct stat* info) {
	list_file(file, info, ANSI_COLOR_RED);
}

// Вывести информацию о файле сокета (зелёного цвета)
void list_socket(char* file, struct stat* info) {
	list_file(file, info, ANSI_COLOR_GREEN);
}

// Вывести информацию о символьной ссылке (голубого цвета)
void list_link(char* link, struct stat* info) {
	list_file(link, info, ANSI_COLOR_CYAN);
	char buf[256];

	// readlink() возвращает кол-во байт, на которые ссылается ссылка, или -1, если произошла ошибка
	// В buf записывает то, что на что ссылается ссылка
	// 256 - максимальный размер, который запишется в buf
	int ans = readlink(link, buf, 256);
	if (ans == -1)
		printf("<broken link>");
	else {
		// isatty() - проверяет, связан ли файл с терминалом, принтером, последовательным портом
		// 1 - дескриптор вывода программы (т.е. если вывод осуществляется в терминал, то он раскрашивается)
		if (isatty(1))
			printf(" -> %s%s%s", ANSI_COLOR_CYAN, buf, ANSI_COLOR_RESET);
		else
			printf(" -> %s", buf);
	}
}

// Вывести информацию о директории без её наполнения
void list_dir_nocontent(char* link, struct stat* info) {
	list_file(link, info, ANSI_COLOR_YELLOW);
}

// Вывести информацию о FIFO или pipe
void list_pipe(char* link, struct stat* info) {
	list_file(link, info, ANSI_COLOR_BLUE);
}

// Вывести информацию о директории и её наполнении (рекурсивно)
void list_dir(char* dir, struct stat* info) {
	printf("--- Folder %s:\n", dir);
	DIR* d = opendir(dir);
	struct dirent *dp;

	errno = 0;

	do {
		if ((dp = readdir(d)) != NULL) {
			if (!strcmp(dp->d_name, ".")) continue;
			if (!strcmp(dp->d_name, "..")) continue;

			char* fullname = malloc(strlen(dir) + strlen(dp->d_name) + 1);;
			strcpy(fullname, dir);
			strcat(fullname, "/");
			strcat(fullname, dp->d_name);

			struct stat ans;
			int res = lstat(fullname, &ans);
			if (res == -1) {
				if (errno == EACCES)
					printf("Cannot read %s: access denied\n", fullname);
				else if (errno == ENAMETOOLONG)
					printf("Cannot read %s: name is too long\n", fullname);
				else if (errno == ENOENT)
					printf("Cannot read %s: does not exist\n", fullname);
				else {
					printf("Cannot read %s: ", fullname);
					perror("");
					printf("\n");
				}
			} else {
				show_it(fullname, &ans, 0);
			}

			free(fullname);
		}
	} while (dp != NULL);

	if (errno != 0)
		perror("error reading directory");
	closedir(d);
}

// Вывести информацию об объекте, не зная, какого он типа
void show_it(char* file, struct stat *info, int recursive) {
	if (S_ISDIR(info->st_mode)) {
		if (recursive)
			list_dir(file, info);
		else
			list_dir_nocontent(file, info);
	} else if (S_ISREG(info->st_mode))
		list_file(file, info, "");
	else if (S_ISLNK(info->st_mode))
		list_link(file, info);
	else if (S_ISBLK(info->st_mode))
		list_blkdev(file, info);
	else if (S_ISSOCK(info->st_mode))
		list_socket(file, info);
	else if (S_ISCHR(info->st_mode))
		list_chardev(file, info);
	else if (S_ISFIFO(info->st_mode))
		list_pipe(file, info);
	else
		printf("Unrecognized file: %s", file);

	printf("\n");
}

int main( int argc, char *argv[] ) {
	int free_argv = 0; // Была ли программно добавлена текущая директория к аргументам

	// Первый аргумент - это путь до исполняемой программы, так что argc==1 означает, что не переданы аргументы
	if (argc == 1) {
		printf("No pathnames given, considering current directory\n");
		argc++;
		argv[1] = malloc(sizeof(char));
		argv[1][0] = '.';
		free_argv = 1;
	}

	int res; //
	struct stat ans; // Информация о директории, переданной в аргументах

	// Выполняем отображение каждой директории из переданных в аргументах
	for (int i = 1; i < argc; i++) {
		// Вместо stat() используется lstat(), чтобы не переходить по символьным ссылкам, а рассматривать их как файлы сами по себе
		// В структуру ans записывается информация о директории
		// Вовзращает 0, если информация получена, -1, если произошла ошибка
		res = lstat(argv[i], &ans);
		if (res == -1) {
			if (errno == EACCES)
				printf("Cannot read %s: access denied\n", argv[i]);
			else if (errno == ENAMETOOLONG)
				printf("Cannot read %s: name is too long", argv[i]);
			else if (errno == ENOENT)
				printf("Cannot read %s: does not exist", argv[i]);
			else {
				printf("Cannot read %s: ", argv[i]);
				perror(""); // Печатает ошибку (если выше не получилось определить её тип)
			}
		} else {
			show_it(argv[i], &ans, 1);
		}
	}

	// Освобождает память, выделенную для аргумента, отвечающего за текущую папку (если не были переданы аргументы)
	if (free_argv)
		free(argv[1]);
} // main()
