int prints(char *s)
{
	while (*s != '\0')
	{
		putc(*s);
		s++;
	}
}

int gets(char *s)
{
	int index = 0;
	char next;

	while(index < 64)
	{
		next = getc();
		putc(next);

		if (next == '\r')
		{
			index++;
			break;
		}
		s[index] = next;
		index++;
	}
	index--;
	s[index] = '\0';
}

int main()
{
	char ans[64];
	while (1)
	{
		prints("What's your name? ");
		gets(ans);
		prints("\n\r");

		if (ans[0] == 0)
		{
			prints("return to assembly and hang\n\r");
			return;
		}

		prints("Welcome ");
		prints(ans);
		prints("\n\r");
	}
}