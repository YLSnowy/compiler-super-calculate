#include "valuechange_.h"
int tovalue_(char a[])
{
	int val = 0;
	if (strlen(a) == 1)
	{
		val = a[0] - 48;
		return val;
	}
	if (a[1] == 'x' || a[1] == 'X')
	{
		for (int i = 2; a[i]; i++)
		{
			int temp = 0;
			if (a[i] >= '0' && a[i] <= '9')
				temp = a[i] - 48;
			else if (a[i] == 'a' || a[i] == 'A')
				temp = 10;
			else if (a[i] == 'b' || a[i] == 'B')
				temp = 11;
			else if (a[i] == 'c' || a[i] == 'C')
				temp = 12;
			else if (a[i] == 'd' || a[i] == 'D')
				temp = 13;
			else if (a[i] == 'e' || a[i] == 'E')
				temp = 14;
			else if (a[i] == 'f' || a[i] == 'F')
				temp = 15;
			val = val * 16 + temp;
		}
		return val;
	}
	if (a[0] == '0') //�?进制
	{
		for (int i = 0; a[i]; i++)
		{
			val = val * 8 + a[i] - 48;
		}
		return val;
	}

	for (int i = 0; a[i]; i++)
		val = val * 10 + a[i] - 48;
	return val;
}