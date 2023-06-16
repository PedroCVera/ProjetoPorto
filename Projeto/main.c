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
	while (ind < 6)
	{
		jnd = 0;
		printf ("\n\nN_porto:%d Nome:%s Pilha:%d\n", porto->_emb[ind].n_porto, porto->_emb[ind].nome, jnd);
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
		ind++;
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
	porto->_emb[index].nome[4] = '\0';
}

int	set_ship(s_port *porto, char *ocurrence, int *buffer) //mete o nome e numero do barco na respetiva estrutura embarcacao
{
	int		index = 0;

	while (ocurrence[index] != *ocurrence && ocurrence[index])
		index++;
	*buffer = ocurrence[index + 1] - 48; // 48 = 0 em decimal (por isso verifica qual e o numero a partir de 0)
	if (porto->postos[*buffer] == 1)
	{
		printf("ERROR:Dois ou mais navious no mesmo espaco do porto:%d\n", *buffer);
		return (1);
	}
	porto->postos[*buffer] = 1;
	ocurrence += 3;
	name_ship(porto, *buffer, ocurrence);
	return (0);
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
			if (set_ship(porto, ocurrence, &buffer) == 1)
				return (1);
			porto->_emb[buffer].n_porto = buffer;
			while (line && line[0] != '\n')
			{
				line = get_next_line(fd);
				if (!line)
					break ;
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

void	starting(s_port *porto)
{
	int	ind = 0;
	int	jnd = 0;

	while (ind <= 9)
	{
		porto->postos[ind] = 0;
		ind++;
	}
	ind = 0;
	while (ind <= 9)
	{
		jnd = 0;
		while (jnd <= 5)
		{
			porto->_emb[ind].pilha[jnd].peso_total = 0;
			jnd++;
		}
		ind++;
	}
}

int	init_program_with_file(s_port *porto, char *av1) //vai ler o ficheiro de configuracao
{
	int	fd;

	starting(porto);
	fd = open(av1, O_RDONLY);
	if (parsing(porto, fd) == 1)
		return (1);
	close (fd);
	return (0);
}

int	weighting(s_port *porto, int ind)
{
	int	weight = 0;
	int	jnd = 0;

	while (jnd <= 5)
	{
		weight += porto->_emb[ind].pilha[jnd].peso_total;
		jnd++;
	}
	return (weight);
}

int	show_weight(s_port *porto, char *prompt)
{
	int		ind = 0;
	int		jnd = 6;
	char	*name;

	while (prompt[jnd + 1] && (prompt[jnd] <='A' || prompt[jnd] >= 'Z'))
		jnd++;
	name = calloc (sizeof(char), 5);
	while (ind < 4)
		name[ind++] = prompt[jnd++];
	ind = 0;
	while (ind <= 9)
	{
		if (porto->postos[ind] == 1)
		{
			if (strcmp(porto->_emb[ind].nome, name) != 0)
				;
			else
			{
				printf("%s %d\n", name, weighting(porto, ind));
				return (0);
			}
		}
		ind++;
	}
	printf("ERROR:invalid command\n");
	return (1);
}

int	where_is_it(s_port *porto, char *prompt, int opt, int jnd)
{
	int		ind = 0;
	// int		jnd = 6;
	char	*name;

	while (prompt[jnd + 1] && (prompt[jnd] <='A' || prompt[jnd] >= 'Z'))
		jnd++;
	name = calloc (sizeof(char), 5);
	while (ind < 4)
		name[ind++] = prompt[jnd++];
	ind = 0;
	while (ind <= 9)
	{
		if (porto->postos[ind] == 1)
		{
			if (strcmp(porto->_emb[ind].nome, name) != 0)
				;
			else
			{
				if (opt == 1)
					printf("%d %s\n", porto->_emb[ind].n_porto, name);
				return (ind);
			}
		}
		ind++;
	}
	if (opt != 100)
		printf("ERROR:invalid command\n");
	return (-1);
}

void	print_menu(void)
{
	printf("+---- MENU\n| move        [-g grua] [-d ponto] [-p pilha] [-D ponto] [-P pilha] [-n numero_de_contentores]\n| show        [-d ponto] [-e embarc]\n| where        [embarc]\n| navigate    [-e embarc] [-d ponto]\n| load        [-e embarc] [-p pilha] [-c contentor:peso]\n| weight    [embarc]\n| save        [filename]\n| help\n| quit\n+----\n");
}

void	imprimir_fila(s_emb emb, int ind)
{
	int	total = 0;

	printf("		p%d %d", ind, emb.pilha[ind].contentores);
	while (total < emb.pilha[ind].contentores)
	{
		printf(" %s:%d", emb.pilha[ind].cont[total].p_id, emb.pilha[ind].cont[total].peso);
		total++;
	}
}

void	imprimir(s_emb emb)
{
	int	ind = 0;

	printf("d%d %s\n", emb.n_porto, emb.nome);
	while (ind < 6)
	{
		if (emb.pilha[ind].peso_total != 0)
		{
			imprimir_fila(emb, ind);
			printf("\n");
		}
		ind++;
	}
}

void	d_show(s_port *porto, char *prompt, int ind)
{
	int	pos;

	while (prompt[ind] != '\0' && (prompt[ind] > '9' || prompt[ind] < '0'))
		ind++;
	pos = prompt[ind] - '0';
	if (porto->postos[pos])
		imprimir(porto->_emb[pos]);
	else
		printf("ERROR:invalid command\n");
}

void	e_show(s_port *porto, char *prompt, int ind)
{
	int	pos;

	pos = where_is_it(porto, prompt, 0, ind + 1);
	if (pos == -1)
		return ;
	if (porto->postos[pos])
		imprimir(porto->_emb[pos]);
	else
		printf("ERROR:invalid command\n");
}

void	show_all(s_port *porto)
{
	int	ind = 0;

	while (ind < 10)
	{
		if (porto->postos[ind] == 1)
			imprimir(porto->_emb[ind]);
		ind++;
	}
}

int	show(s_port *porto, char *prompt)
{
	int		ind = 4;
	char	flag;

	while (prompt[ind] != '\0' && (prompt[ind] > 'z' || prompt[ind] < 'a'))
		ind++;
	if (prompt[ind] == '\0')
		show_all(porto);
	flag = prompt[ind];
	if (flag == 'd')
		d_show(porto, prompt, ind);
	if (flag == 'e')
		e_show(porto, prompt, ind);
	return (1);
}

void	contentor_shenanigans(s_port *porto, char *contentor, int n_contentor, int pos, int pilha)
{
	s_pilha	_pilha;
	int	index = 0;
	int	jndex;

	_pilha.cont = calloc (sizeof(s_cont), porto->_emb[pos].pilha[pilha].contentores + 1);
	if (!_pilha.cont)
		return ;
	while (index < porto->_emb[pos].pilha[pilha].contentores + 1)
	{
		_pilha.cont[index].p_id = calloc (sizeof(char), 4);
		if (!_pilha.cont[index].p_id)
			return ;
		index++;
	}
//	porto->_emb[pos].pilha[pilha].cont[index]
	index = 0;
	while (index < porto->_emb[pos].pilha[pilha].contentores)
	{
		jndex = 0;
		while (jndex < 4)
		{
			_pilha.cont[index].p_id[jndex] = porto->_emb[pos].pilha[pilha].cont[index].p_id[jndex];
			jndex++;
		}
		_pilha.cont[index].p_id[jndex] = '\0';
		_pilha.cont[index].pilha_id = index;
		_pilha.cont[index].peso = porto->_emb[pos].pilha[pilha].cont[index].peso;
		index++;
	}
	jndex = 0;
	index = porto->_emb[pos].pilha[pilha].contentores;
	while (jndex < 4)
	{
		_pilha.cont[index].p_id[jndex] = contentor[jndex];
		jndex++;
	}
	_pilha.cont[index].p_id[jndex] = '\0';
	_pilha.cont[index].pilha_id = pilha;
	_pilha.cont[index].peso = n_contentor;
	_pilha.contentores = porto->_emb[pos].pilha[pilha].contentores;
	_pilha.peso_total = porto->_emb[pos].pilha[pilha].peso_total;
	// if (porto->_emb[pos].pilha[pilha])
	// 	free(porto->_emb[pos].pilha[pilha]);
	porto->_emb[pos].pilha[pilha] = _pilha;
}

void	add_contentor(s_port *porto, int pos, int pilha, char *brutocontentor)
{
	int		jndex = porto->_emb[pos].pilha[pilha].contentores;
	char	*contentor;
	char	buff[15];
	int		n_contentor = 0;
	int		jnd = 0;

	jndex++;
	while (brutocontentor[n_contentor] != ':' && brutocontentor[n_contentor] != '\0')
		n_contentor++;
	n_contentor++;
	while (brutocontentor[n_contentor] != '\0')
		buff[jnd++] = brutocontentor[n_contentor++];
	buff[jnd] = '\0';
	if (jnd < 3)
	{
		printf("ERROR:invalid command\n");
		return ;
	}
	n_contentor = atoi(buff);
	if (n_contentor < 500)
	{
		printf("ERROR:invalid command\n");
		return ;
	}
	contentor = strndup(brutocontentor, 3);
	jnd = 0;
	while ((jnd < porto->_emb[pos].pilha[pilha].contentores) && porto->_emb[pos].pilha[pilha].peso_total != 0)
	{
		if (strncmp(porto->_emb[pos].pilha[pilha].cont[jnd].p_id, contentor, 4) == 0)
		{
			printf("ERROR:invalid command\n");
			return ;
		}
		jnd++;
	}
	jnd = 0;
	if (porto->_emb[pos].pilha[pilha].peso_total == 0)
	{
		porto->_emb[pos].pilha[pilha].contentores = 1;
		porto->_emb[pos].pilha[pilha].peso_total = n_contentor;
		create_contentor(porto,pos,pilha);
		while (jnd < 4)
		{
			porto->_emb[pos].pilha[pilha].cont[0].p_id[jnd] = contentor[jnd];
			jnd++;
		}
		porto->_emb[pos].pilha[pilha].cont[0].p_id[jnd] = '\0';
		porto->_emb[pos].pilha[pilha].cont[0].peso = n_contentor;
		porto->_emb[pos].pilha[pilha].cont[0].pilha_id = pilha;
		if (!porto->_emb[pos].pilha[pilha].cont || porto->_emb[pos].pilha[pilha].cont->p_id == NULL)
		{
			printf("ERROR:invalid command\n");
			return ;
		}
	}
	else
	{
		contentor_shenanigans(porto, contentor, n_contentor, pos, pilha);
		porto->_emb[pos].pilha[pilha].peso_total += n_contentor;
		porto->_emb[pos].pilha[pilha].contentores += 1;
	}
	printf("SUCESS: operation concluded\n");
}

void	load(s_port *porto, char *prompt)
{
	int 	ind = 4;
	int		pos;
	int		pilha;
	char	contentor[15];
	int		jnd = 0;

	while (prompt[ind] != '\0' && prompt[ind] != 'e')
		ind++;
	pos = where_is_it(porto, prompt, 0, ind);
	if (pos == -1)
		return ;
	ind = 4;
	while (prompt[ind] != '\0' && prompt[ind] != 'p')
		ind++;
	while (prompt[ind] != '\0' && (prompt[ind] > '9' || prompt[ind] < '0'))
		ind++;
	if (prompt[ind] == '\0')
	{
		printf("ERROR:invalid command\n");
		return ;
	}
	pilha = prompt[ind] - '0';
	if (pilha > 5)
	{
		printf("ERROR:invalid command\n");
		return ;
	}
	ind = 4;
	while (prompt[ind] != '\0' && prompt[ind] != 'c')
		ind++;
	while (prompt[ind] != '\0' && (prompt[ind] > 'Z' || prompt[ind] < 'A'))
		ind++;
	if (prompt[ind] == '\0')
	{
		printf("ERROR:invalid command\n");
		return ;
	}
	while (prompt[ind] != '\0' && prompt[ind] != ' ')
		contentor[jnd++] = prompt[ind++];
	contentor[jnd] = '\0';
	add_contentor(porto, pos, pilha, contentor);
}

void	create_barco(s_port *porto, int dest, char *prompt)
{
	int		ind = 8;
	char	name[4];
	int		jnd = 0;

	while(prompt[ind] != 'e' && prompt[ind] != '\0')
		ind++;
	while((prompt[ind] >= 'Z' || prompt[ind] <= 'A') && prompt[ind] != '\0')
		ind++;
	if (prompt[ind] == '\0')
	{
		printf("ERROR:invalid command\n");
		return ;
	}
	while (jnd < 4 && prompt[ind] != '\0')
		name[jnd++] = prompt[ind++];
	if (prompt[ind] != '\0')
	{
		if ((prompt[ind] <= 'Z' && prompt[ind] >= 'A') || (name[ind] <= 'z' && name[ind] >= 'a'))
		{
			printf("ERROR:invalid command\n");
			return ;
		}
	}
	name[4] = '\0';
	jnd = 0;
	while (name[jnd++] != '\0')
	{
		if (name[jnd] <= 'z' && name[jnd] >= 'a')
		{
			printf("ERROR:invalid command\n");
			return ;
		}
	}
	porto->postos[dest] = 1;
	porto->_emb[dest].nome = strdup(name);
	printf("SUCCESS: operation concluded\n");
	porto->_emb[dest].n_porto = dest;
	jnd = 0;
	while (jnd < 6)
	{
		porto->_emb[dest].pilha[jnd].peso_total = 0;
		jnd++;
	}
}

void	barcos_handle(s_port *porto, int dest, int pos)
{
	porto->_emb[dest] = porto->_emb[pos];
	porto->_emb[dest].n_porto = dest;
	porto->postos[pos] = 0;
	porto->postos[dest] = 1;
	printf("SUCCESS: operation concluded\n");
}

void	navigate(s_port *porto, char *prompt)
{
	int	ind = 8;
	int	pos;
	int	dest;

	while(prompt[ind] != 'e' && prompt[ind] != '\0')
		ind++;
	if (prompt[ind] == '\0')
	{
		printf("ERROR:invalid command\n");
		return ;
	}
	pos = where_is_it(porto, prompt, 100, ind);
	ind = 8;
	while (prompt[ind] != 'd' && prompt[ind] != '\0')
		ind++;
	while (prompt[ind] != '\0' && (prompt[ind] > '9' || prompt[ind] < '0'))
		ind++;
	if (prompt[ind] == '\0')
	{
		printf("ERROR:invalid command\n");
		return ;
	}
	if (prompt[ind + 1] <= '9' && prompt[ind + 1] >= '0')
	{
		printf("ERROR:invalid command\n");
		return ;
	}
	dest = prompt[ind] - '0';
	if (porto->postos[dest] == 1)
	{
		printf("ERROR:invalid command\n");
		return ;
	}
	if (pos != -1)
		barcos_handle(porto, dest, pos);
	else
		create_barco(porto, dest, prompt);
}

int	actual_program(s_port *porto)
{
	char	*prompt;
	int		len = 0;

	prompt = calloc(sizeof(char), 50);
	// printf("+---- MENU\n| move        [-g grua] [-d ponto] [-p pilha] [-D ponto] [-P pilha] [-n numero_de_contentores]\n| show        [-d ponto] [-e embarc]\n| where        [embarc]\n| navigate    [-e embarc] [-d ponto]\n| load        [-e embarc] [-p pilha] [-c contentor:peso]\n| weight    [embarc]\n| save        [filename]\n| help\n| quit\n+----\n");
	print_menu();
	while (1)
	{
		printf(">");
		fgets(prompt, 50, stdin);
		len = strlen(prompt);
		if (len > 0 && prompt[len - 1] == '\n')
			prompt[--len] = '\0';
		if (strncmp(prompt, "weight", 6) == 0)
			show_weight(porto, prompt);
		if (strncmp(prompt, "quit", 4) == 0)
			return (0);
		if (strncmp(prompt, "help", 4) == 0)
			print_menu();
		if (strncmp(prompt, "where", 5) == 0)
			where_is_it(porto, prompt, 1, 6);
		if (strncmp(prompt, "show", 4) == 0)
			show(porto, prompt);
		if (strncmp(prompt, "navigate", 8) == 0)
			navigate(porto, prompt);
		if (strncmp(prompt, "load", 4) == 0)
			load(porto, prompt);
	}
	return (1);
}

/* 4 entries: demasiados args, args certos com ficheiro (+ficheiro pode tar mal), args certos sem ficheiro*/

int	main(int argc, char *argv[])
{
	s_port	porto;

	argv[1] = "config.txt";
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
		if (init_program_with_file(&porto, argv[1]) == 1)
			return (1);
		// print_content(&porto);
	}
	else //sem ficheiro
		starting(&porto);;
	if (actual_program(&porto) == 1)
	{
		// free_all;
		return (1);
	}
	else
		//free_all(porto);
	return (0);
}