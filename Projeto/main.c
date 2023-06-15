#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

typedef	struct _contentor
{
	int			pilha_id;	//N da pilha em que se encontra
	int			peso;
	char		*p_id;		//personal identification
}				s_cont;


typedef struct _embarcacao
{
	int			n_porto;		//em que lugar do porto esta
	int			pilha[6];		//quantos contentores tem em cada pilha (+1 para o \0)
	char		*nome;
	s_cont		*cont;
}				s_emb;

typedef	struct _porto
{
	int			postos[10];		//Diz se ha ou nao embarcacoes em cada lugar
	s_emb		_emb[10];
}				s_port;

size_t	_strlen(const char *str)
{
	size_t	size = 0;

	while (str && str[size] != '\0')
		size++;
	return (size);
}

int	check_name(char *file) // ve a extensao do ficheiro em busca do .txt
{
	int	index;
	int	jndex = 0;

	index = strlen(file) - 1;
	while (file[index] != '.' && file[index])
		index--;
	if (!file[index])
		return (1);
	while (file[index] == ".txt"[jndex] && file[index] && ".txt"[jndex])
	{
		index++;
		jndex++;
	}
	if (jndex == 4)
		return (0);
	return (1);
}

int	check_new_line(char *buff)
{
	int	i = -1;
	int	j = 0;
	int	is_new_line = 0;

	while (buff[++i])
	{
		if (is_new_line)
			buff[j++] = buff[i];
		if (buff[i] == '\n')
			is_new_line = 1;
		buff[i] = 0;
	}
	return (is_new_line);
}

char	*get_line(char *buff, char *line)
{
	int		i;
	int		j;
	int		buff_size = _strlen(buff);
	char	*new_line;

	new_line = malloc((buff_size + _strlen(line) + 1) * sizeof(char));
	if (!new_line)
		return (NULL);
	i = -1 * (line != NULL); // se line for NULL o i = 0;
	while (line && line[++i])
		new_line[i] = line[i];
	j = 0;
	while (j < buff_size)
		new_line[i++] = buff[j++];
	new_line[i] = '\0';
	if (line)
		free(line);
	return (new_line);
}

char	*get_next_line(int fd)
{
	int		byte_red;
	char	buff[1];
	char	*line;

	if (fd < 0 || fd >= FOPEN_MAX)
		return (NULL);
	line = NULL;
	byte_red = 1;
	while (1)
	{
		if (!buff[0])
			byte_red = read(fd, buff, 1);
		if (byte_red > 0)
			line = get_line(buff, line);
		if (check_new_line(buff) || byte_red < 1)
			break ;
	}
	return (line);
}

void	name_ship(s_port *porto, int index, char *start_of_name)
{
	porto->_emb[index].nome = malloc (sizeof(char) * 5);
	strncpy(porto->_emb[index].nome, start_of_name, 4);
}

void	set_ship(s_port *porto, char *ocurrence, int *buffer)
{
	int		index = 0;

	while (ocurrence[index] != *ocurrence && ocurrence[index])
		index++;
	*buffer = ocurrence[index + 1] - 48; // 48 = 0 em decimal (por isso verifica qual e o numero a partir de 0)    65
	porto->postos[*buffer] = 1;
	ocurrence += 3;
	name_ship(porto, *buffer, ocurrence);
}

void	set_piles(s_port *porto, char *ocurrence, int buffer)
{
	int		index = 0;
	int		jndex;

	if (buffer == 30)
	while (ocurrence[index] != *ocurrence && ocurrence[index])
		index++;
	jndex = ocurrence[index + 1] - 48; // 48 = 0 em decimal (por isso verifica qual e o numero a partir de 0)    65
	porto->_emb[buffer].pilha[jndex] = 1;
	ocurrence += 3;
	name_ship(porto, jndex, ocurrence);
}

int	parsing(s_port *porto, int fd)
{
	char	*line;
	char	*ocurrence;
	int		buffer;

	line = get_next_line(fd);
	if (line == NULL)
		return (0);
	while (line != NULL)
	{
		ocurrence = strchr(line, 'd');
		if(ocurrence == NULL)
			;
		else
		{
			set_ship(porto, ocurrence, &buffer);
			while (line[2])
			{
				ocurrence = strchr(line, 'p');
				if(ocurrence == NULL)
					break ;
				else
					set_piles(porto, ocurrence, buffer);
			}
			buffer = 30;
		}
		line = get_next_line(fd);
	}
	return (0);
}

int	init_program_with_file(s_port *porto, char *av1) //vai ler o ficheiro de configuracao
{
	int	fd;

	fd = open(av1, O_RDONLY);
	if (parsing(porto, fd) == 1)
		return (1);
	return (0);
}

int	init_program_without_file(s_port *porto) //marca todos os postos como vazios
{
	int	ind = 0;

	while (ind < 9)
	{
		porto->postos[ind] = 0;
		ind++;
	}
	return (0);
}

/* 4 entries: demasiados args, args certos com ficheiro (+ficheiro pode tar mal), args certos sem ficheiro*/

int	main(int argc, char *argv[])
{
	if (argc > 2) //demasiados argumentos
	{
		printf("Foram introduzidos demasiados argumentos\n");
		return (1);
	}
	else if (argc == 2) //com ficheiro
	{
		if (check_name(argv[1]) == 1)
		{
			printf("ERROR: file format is not recognized\n");
			return (1);
		}
		s_port	porto;
		if (init_program_with_file(&porto, argv[1]) == 1)
			return (1);
	}
	else //sem ficheiro
	{
		s_port	porto;
		init_program_without_file(&porto);
			return (1);
	}
}