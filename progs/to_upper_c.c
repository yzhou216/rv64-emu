void to_upper_c(char *src, char *dst)
{
	char ch;
	while ((ch = *src++)) {
		if (ch >= 'a' && ch <= 'z')
			ch -= 32;
		*dst++ = ch;
	}
	*dst = '\0';
}
