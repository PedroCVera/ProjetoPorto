#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

typedef	struct	_contentor
{
	int			pilha_id;	//N da pilha em que se encontra
	int			peso;
	char		*p_id;		//personal identification
}				s_cont;

typedef	struct	_pilha
{
	s_cont		*cont;			//array de contentores nesta pilha
	int			peso_total;		//qual e o peso total da pilha
	int			contentores;	//quantos contentores tem a pilha
}				s_pilha;


typedef struct	_embarcacao
{
	int			n_porto;		//em que lugar do porto esta
	s_pilha		pilha[6];		//quantos contentores tem em cada pilha (+1 para o \0)
	char		*nome;
}				s_emb;

typedef	struct	_porto
{
	int			postos[10];		//Diz se ha ou nao embarcacoes em cada lugar
	s_emb		_emb[10];
}				s_port;


// debug function to see if the content was well set

void	print_content(s_port *porto)
{
	int	ind = 1;
	int	jnd = 0;
	int	gnd = 0;

	printf ("N_porto:%d Nome:%s Pilha:%d\n", porto->_emb[ind].n_porto, porto->_emb[ind].nome, jnd);
	while (jnd < 4)
	{
		gnd = 0;
		printf("Peso_total:%d, N_contentores:%d\n", porto->_emb[ind].pilha[jnd].peso_total, porto->_emb[ind].pilha[jnd].contentores);
		while (gnd < porto->_emb[ind].pilha[jnd].contentores)
		{
			printf("Id:%d, peso:%d, personal_id:%s\n", porto->_emb[ind].pilha[jnd].cont[gnd].pilha_id, porto->_emb[ind].pilha[jnd].cont[gnd].peso,porto->_emb[ind].pilha[jnd].cont[gnd].p_id);
			gnd++;
		}
		jnd++;
	}
}

size_t	_strlen(const char *str)
{
	size_t	size = 0;

	while (str && str[size] != '\0')
		size++;
	return (size);
}

int	_atoi(const char *str)
{
	signed int	result;
	int			sign;

	result = 0;
	sign = 1;
	while (*str == '\t' || *str == '\n' || *str == '\v' || *str == '\f' || *str == '\r' || *str == 32)
		str++;
	if (*str == '+' || *str == '-')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	while (*str && *str >= '0' && *str <= '9')
	{
		result = result * 10 + *str - '0';
		str++;
	}
	return (result * sign);
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

void	set_ship(s_port *porto, char *ocurrence, int *buffer) //mete o nome e numero do barco na respetiva estrutura embarcacao
{
	int		index = 0;

	while (ocurrence[index] != *ocurrence && ocurrence[index])
		index++;
	*buffer = ocurrence[index + 1] - 48; // 48 = 0 em decimal (por isso verifica qual e o numero a partir de 0)
	porto->postos[*buffer] = 1;
	ocurrence += 3;
	printf("%d\n", *buffer);
	porto->_emb[index].n_porto = *buffer;
	printf("%d\n", porto->_emb[index].n_porto);
	name_ship(porto, *buffer, ocurrence);
}

int	create_contentor(s_port *porto, int buffer, int n_pilha)
{
	int	index = 0;

	porto->_emb[buffer].pilha[n_pilha].cont = calloc (sizeof(s_cont), porto->_emb[buffer].pilha[n_pilha].contentores);
	if (!porto->_emb[buffer].pilha[n_pilha].cont)
		return (1);
	while (index < porto->_emb[buffer].pilha[n_pilha].contentores)
	{
		porto->_emb[buffer].pilha[n_pilha].cont[index].p_id = calloc (sizeof(char), 4);
		if (!porto->_emb[buffer].pilha[n_pilha].cont[index].p_id)
			return (1);
		index++;
	}
	return (0);
}



int	set_piles(s_port *porto, char *ocurrence, int buffer)	//occurence e a linha que estamos a ler e aponta para o P
{
	int		index = 0;
	int		jndex = 0;
	int		gndex = 0;
	int		index_contentor = 0;
	int		n_pilha;
	char	*buff;

	if (buffer == 30)
		return (3);
	while (ocurrence[index] != *ocurrence && ocurrence[index])
		index++;
	n_pilha = ocurrence[index + 1] - 48; // 48 = 0 em decimal (por isso verifica qual e o numero a partir de 0)    65
	index += 3;
	porto->_emb[buffer].pilha[n_pilha].contentores = ocurrence[index] - 48;
	porto->_emb[buffer].pilha[n_pilha].peso_total = 0;
	if (create_contentor(porto, buffer, n_pilha) == 1)
		return (1);
	index++;
	while (ocurrence[index])
	{
		if (!ocurrence[index + 1])
			break;
		gndex = 0;
		index++;

		if (ocurrence[index] >= 'A' && ocurrence[index] <= 'Z')
		{
			porto->_emb[buffer].pilha[n_pilha].cont[index_contentor].pilha_id = index_contentor;
			while (gndex < 3)
			{
				porto->_emb[buffer].pilha[n_pilha].cont[index_contentor].p_id[gndex] = ocurrence[index];
				index++;
				gndex++;
			}
			index++;
			jndex = index - 1;
			gndex = 0;
			while (ocurrence[index] >= 48 && ocurrence[index] <= 57)
				index++;
			buff = calloc (sizeof(char), (index - jndex));
			if (!buff)
				return (1);
			while (jndex < index)
			{
				buff[gndex] = ocurrence[jndex + 1];
				jndex++;
				gndex++;
			}
			porto->_emb[buffer].pilha[n_pilha].cont[index_contentor].peso = _atoi(buff);
			if (porto->_emb[buffer].pilha[n_pilha].cont[index_contentor].peso < 500)
			{
				printf("contentor numero:%d da pilha:%d da embarcacao:%s e demasiado leve:%d\n",index_contentor, n_pilha, porto->_emb[buffer].nome, porto->_emb[buffer].pilha[n_pilha].cont[index_contentor].peso);
				return (1);
			}
			porto->_emb[buffer].pilha[n_pilha].peso_total += porto->_emb[buffer].pilha[n_pilha].cont[index_contentor].peso;
			porto->_emb[buffer].pilha[n_pilha].cont[index_contentor].pilha_id = n_pilha;
			free (buff);
			index_contentor++;
		}
	}
	return (0);
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
			while (line && line[0] != '\n')
			{
				line = get_next_line(fd);
				ocurrence = strchr(line, 'p');
				if(ocurrence == NULL)
					break ;
				else
					if (set_piles(porto, ocurrence, buffer) == 1)
						return (1);
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
		print_content(&porto);
	}
	else //sem ficheiro
	{
		s_port	porto;
		init_program_without_file(&porto);
			return (1);
	}
}