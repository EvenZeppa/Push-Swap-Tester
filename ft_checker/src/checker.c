#include <limits.h>
#include "libft.h"
#include "get_next_line.h"

typedef struct	s_stack
{
	int				nb;
	struct s_stack	*prev;
	struct s_stack	*next;
}	t_stack;

t_stack	*create_stack(int nb)
{
	t_stack	*stack;

	stack = malloc(sizeof(t_stack));
	if (!stack)
		return (NULL);
	stack->nb = nb;
	stack->prev = NULL;
	stack->next = NULL;
	return (stack);
}

void	free_lstack(t_stack **lstack)
{
	t_stack	*prev;
	t_stack	*current;

	if (!lstack)
		return ;
	current = *lstack;
	while (current)
	{
		prev = current;
		current = prev->next;
		free(prev);
	}
}

t_stack	*get_last_lstack(t_stack *lstack)
{
	t_stack	*current;

	if (!lstack)
		return (NULL);
	current = lstack;
	while (current && current->next)
		current = current->next;
	return (current);
}

int	get_size_lstack(t_stack *lstack)
{
	t_stack	*current;
	int		size;

	current = lstack;
	size = 0;
	while (current)
	{
		current = current->next;
		size++;
	}
	return (size);
}

void	add_back_lstack(t_stack **lstack, t_stack *new)
{
	t_stack	*last;

	if (!new)
		return ;
	last = get_last_lstack(*lstack);
	if (!last)
		*lstack = new;
	else
	{
		last->next = new;
		new->prev = last;
	}
}

void	init_lstack(t_stack **lstack, char **nbrs, int size_nbrs)
{
	t_stack	*new;
	int		i;

	i = 0;
	while (i < size_nbrs)
	{
		new = create_stack(ft_atoi(nbrs[i]));
		if (!new)
		{
			free_lstack(lstack);
			return ;
		}
		add_back_lstack(lstack, new);
		i++;
	}
}

int	has_lstack_duplicate_nb(t_stack *lstack)
{
	t_stack	*current;
	t_stack	*duplicate_stack;

	current = lstack;
	while (current)
	{
		duplicate_stack = current->next;
		while (duplicate_stack)
		{
			if (current->nb == duplicate_stack->nb)
				return (1);
			duplicate_stack = duplicate_stack->next;
		}
		current = current->next;
	}
	return (0);
}

void	print_lstacks(t_stack *lstack_a, t_stack *lstack_b)
{
	t_stack	*current_a;
	t_stack	*current_b;
	int		size_a;
	int		size_b;

	ft_printf(
		"--------------------------------------\n"
		"|     Stack A     ||     Stack B     |\n"
		"--------------------------------------\n"
		);
	current_a = lstack_a;
	current_b = lstack_b;
	size_a = get_size_lstack(lstack_a);
	size_b = get_size_lstack(lstack_b);
	while (size_a || size_b)
	{
		if (size_a && size_a >= size_b)
		{
			ft_printf("|   % -12d  |", current_a->nb);
			current_a = current_a->next;
			size_a--;
		}
		else
			ft_printf("|                 |");
		if (size_b && size_b > size_a)
		{
			ft_printf("|   % -12d  |\n", current_b->nb);
			current_b = current_b->next;
			size_b--;
		}
		else
			ft_printf("|                 |\n");
	}
	ft_printf("--------------------------------------\n");
}

int	check_buffer(char *buffer)
{
	if (!buffer || !ft_strncmp(buffer, "\n", 1))
		return (0);
	if (!ft_strncmp(buffer, "sa\n", 3)
		|| !ft_strncmp(buffer, "sb\n", 3)
		|| !ft_strncmp(buffer, "ss\n", 3)
		|| !ft_strncmp(buffer, "pa\n", 3)
		|| !ft_strncmp(buffer, "pb\n", 3)
		|| !ft_strncmp(buffer, "ra\n", 3)
		|| !ft_strncmp(buffer, "rb\n", 3)
		|| !ft_strncmp(buffer, "rr\n", 3)
		|| !ft_strncmp(buffer, "rra\n", 4)
		|| !ft_strncmp(buffer, "rrb\n", 4)
		|| !ft_strncmp(buffer, "rrr\n", 4))
		return (1);
	return (-1);
}

int	is_lstack_sorted(t_stack *lstack)
{
	t_stack	*current;

	current = lstack;
	while (current && current->next)
	{
		if (current->nb > current->next->nb)
			return (0);
		current = current->next;
	}
	return (1);
}

void	lstack_swap(t_stack **lstack)
{
	t_stack	*tmp;

	if (!lstack || !(*lstack) || !(*lstack)->next)
		return ;
	tmp = (*lstack)->next;
	(*lstack)->prev = tmp;
	(*lstack)->next = tmp->next;
	tmp->prev = NULL;
	tmp->next = (*lstack);
	*lstack = tmp;
}

void	lstack_push(t_stack **lstack_r, t_stack **lstack_g)
{
	t_stack	*tmp;

	if (!lstack_g || !(*lstack_g))
		return ;
	tmp = (*lstack_g)->next;
	if (tmp)
		tmp->prev = NULL;
	if (*lstack_r)
	{
		(*lstack_g)->next = *lstack_r;
		(*lstack_r)->prev = (*lstack_g);
	}
	else
		(*lstack_g)->next = NULL;
	(*lstack_g)->prev = NULL;
	*lstack_r = *lstack_g;
	*lstack_g = tmp;
}

void	lstack_rotate(t_stack **lstack)
{
	t_stack	*tmp;
	t_stack	*last;

	if (!lstack || !(*lstack) || !(*lstack)->next)
		return ;
	tmp = (*lstack)->next;
	last = get_last_lstack(*lstack);
	last->next = *lstack;
	(*lstack)->prev = last;
	(*lstack)->next = NULL;
	tmp->prev = NULL;
	*lstack = tmp;
}

void	lstack_rrotate(t_stack **lstack)
{
	t_stack	*tmp;

	if (!lstack || !(*lstack) || !(*lstack)->next)
		return ;
	tmp = get_last_lstack(*lstack);
	tmp->prev->next = NULL;
	tmp->prev = NULL;
	tmp->next = *lstack;
	(*lstack)->prev = tmp;
	*lstack = tmp;
}

int	do_instructions(t_stack **lstack_a, t_stack **lstack_b, char *buffer)
{
	if (!ft_strncmp(buffer, "sa\n", 3))
		return (lstack_swap(lstack_a), 1);
	if (!ft_strncmp(buffer, "sb\n", 3))
		return (lstack_swap(lstack_b), 1);
	if (!ft_strncmp(buffer, "ss\n", 3))
		return (lstack_swap(lstack_a), lstack_swap(lstack_b), 1);
	if (!ft_strncmp(buffer, "pa\n", 3))
		return (lstack_push(lstack_a, lstack_b), 1);
	if (!ft_strncmp(buffer, "pb\n", 3))
		return (lstack_push(lstack_b, lstack_a), 1);
	if (!ft_strncmp(buffer, "ra\n", 3))
		return (lstack_rotate(lstack_a), 1);
	if (!ft_strncmp(buffer, "rb\n", 3))
		return (lstack_rotate(lstack_b), 1);
	if (!ft_strncmp(buffer, "rr\n", 3))
		return (lstack_rotate(lstack_a), lstack_rotate(lstack_b), 1);
	if (!ft_strncmp(buffer, "rra\n", 4))
		return (lstack_rrotate(lstack_a), 1);
	if (!ft_strncmp(buffer, "rrb\n", 4))
		return (lstack_rrotate(lstack_b), 1);
	if (!ft_strncmp(buffer, "rrr\n", 4))
		return (lstack_rrotate(lstack_a), lstack_rrotate(lstack_b), 1);
	return (-1);
}

int	handle_stdin(t_stack **lstack_a, t_stack **lstack_b)
{
	char	*buffer;
	int		check;

	while (TRUE)
	{
		buffer = get_next_line(STDIN_FILENO);
		check = check_buffer(buffer);
		if (check == -1)
			return (ft_printf("Error\n"), -1);
		if (!check)
		{
			if (!get_size_lstack(*lstack_b) && is_lstack_sorted(*lstack_a))
				return (ft_printf("OK\n"), 1);
			return (ft_printf("KO\n"), 1);
		}
		do_instructions(lstack_a, lstack_b, buffer);
	}
}

int	ft_is_int(char *str)
{
	long long	result;
	int			sign;

	sign = 1;
	result = 0;
	if (*str == '+' || *str == '-')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	while (*str)
	{
		if (!ft_isdigit(*str))
			return (0);
		result = result * 10 + (*str - '0');
		if (result * sign < INT_MIN || result * sign > INT_MAX)
			return (0);
		str++;
	}
	return (1);
}

int	check_args(int nb, char *args[])
{
	while (--nb >= 0)
	{
		if (!ft_is_int(args[nb]))
			return (0);
	}
	return (1);
}

int	main(int argc, char *argv[])
{
	t_stack	*lstack_a;
	t_stack	*lstack_b;

	lstack_a = NULL;
	lstack_b = NULL;
	if (argc == 1)
		return (0);
	if (!check_args(argc - 1, &argv[1]))
		return (ft_printf("Error\n"), -1);
	init_lstack(&lstack_a, &argv[1], argc - 1);
	if (has_lstack_duplicate_nb(lstack_a))
		return (ft_printf("Error\n"), -1);
	handle_stdin(&lstack_a, &lstack_b);
	free_lstack(&lstack_a);
	free_lstack(&lstack_b);
	return (0);
}
