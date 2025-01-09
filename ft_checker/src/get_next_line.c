/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ezeppa <ezeppa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 01:01:42 by ezeppa            #+#    #+#             */
/*   Updated: 2024/12/10 22:15:03 by ezeppa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*ft_strjoin(const char *s1, const char *s2)
{
	char	*ptr;
	size_t	s1_len;
	size_t	s2_len;

	s1_len = ft_strlen(s1);
	s2_len = ft_strlen(s2);
	ptr = malloc(sizeof(char) * (s1_len + s2_len + 1));
	if (!ptr)
		return (ptr);
	ft_strlcpy(ptr, s1, s1_len + 1);
	ft_strlcat(ptr, s2, s1_len + s2_len + 1);
	return (ptr);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*ptr;

	if (!s)
		return (NULL);
	if (start > ft_strlen(s))
		return (ft_strldup("", 1));
	if (len > ft_strlen(s) - start)
		len = ft_strlen(s) - start;
	ptr = malloc(sizeof(char) * (len + 1));
	if (!ptr)
		return (ptr);
	ft_strlcpy(ptr, &s[start], (len + 1));
	return (ptr);
}

char	*process_line(char **text)
{
	char	*tmp;
	char	*line;
	int		pos_n;

	if (!text || !*text)
		return (free(*text), *text = NULL, NULL);
	if (!ft_strchr(*text, '\n'))
	{
		if (**text == '\0')
			line = NULL;
		else
			line = ft_strldup(*text, ft_strlen(*text) + 1);
		return (free(*text), *text = NULL, line);
	}
	pos_n = ft_strchr(*text, '\n') - *text;
	line = ft_strldup(*text, pos_n + 2);
	if (!line)
		return (free(*text), *text = NULL, NULL);
	tmp = ft_substr(*text, pos_n + 1, ft_strlen(*text) - pos_n);
	return (free(*text), *text = tmp, line);
}

char	*get_next_line(int fd)
{
	static char	*text[1024] = {NULL};
	char		*tmp;
	char		*buffer;
	int			br;

	if (fd < 0)
		return (free(text[fd]), text[fd] = NULL, NULL);
	buffer = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!buffer)
		return (free(text[fd]), text[fd] = NULL, NULL);
	while (!ft_strchr(text[fd], '\n'))
	{
		br = read(fd, buffer, BUFFER_SIZE);
		if (br == -1)
			return (free(buffer), free(text[fd]), text[fd] = NULL, NULL);
		if (br == 0)
			return (free(buffer), process_line(&text[fd]));
		buffer[br] = '\0';
		tmp = text[fd];
		text[fd] = ft_strjoin(text[fd], buffer);
		free(tmp);
		tmp = NULL;
	}
	return (free(buffer), process_line(&text[fd]));
}
