/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
#include <stdarg.h>

#define UDATAREG        0x00
#define UFLAGREG        0x18
#define UBAUDREG        0x24
#define ULINECNTRLREG   0x2c
#define UINTERRUPTREG   0x38

#define BUSY    0x08 // Device Busy
#define RXFE    0x10 // RX Buffer Empty
#define TXFF    0x20 // TX Buffer Empty
#define RXFF    0x40 // RX Buffer Full
#define TXFE    0x80 // TX Buffer Full

typedef volatile struct uart
{
    char *base;
    int n;
}
UART;

char* numeralTable = "0123456789ABCDEF";

UART uart[4];

int uart_init()
{
    int i;
    UART *pUART;

    for (i=0; i<4; i++)
    {
        pUART = &uart[i];
        pUART->base = (char *)(0x101F1000 + i*0x1000);
        pUART->n = i;
    }
    uart[3].base = (char *)(0x10009000); // uart3 at 0x10009000
}

int ugetc(UART *pUART)
{
    while (*(pUART->base + UFLAGREG) & RXFE);
    return *(pUART->base + UDATAREG);
}

int uputc(UART *pUART, char c)
{
    while(*(pUART->base + UFLAGREG) & TXFF);
    *(pUART->base + UDATAREG) = c;
}

int ugets(UART *pUART, char *s)
{
    while ((*s = (char) ugetc(pUART)) != '\r') {
        uputc(pUART, *s);
        s++;
    }
    *s = 0;
}

int uprints(UART *pUART, char *s)
{
    while(*s)
    uputc(pUART, *s++);
}

void urpu(UART *pUART, u32 num, int base)
{
    char c;

    if (num != 0)
    {
        c = numeralTable[num % base];
        urpu(pUART, num / base, base);
        uputc(pUART, num);
    }
}

void uprintnum(UART* pUART, int num, int base)
{
    if (num < 0)
    {
        uputc(pUART, '-');
        num *= -1;
    }

    if (num == 0) 
    {
        uputc(pUART, '0');
    }
    else 
    {
        urpu(pUART, num, base);
    }
}

void uprintu(UART *pUART, u32 num)
{
    uprintnum(pUART, num, 10);
}

void uprintd(UART *pUART, int num)
{
    uprintnum(pUART, num, 10);
}

void uprintx(UART *pUART, u32 num)
{
    uprintnum(pUART, num, 16);
}

void uprinto(UART *pUART, u32 num)
{
    uprintnum(pUART, num, 8);
}

int ufprint(UART* pUART, char* formattedString, ...)
{
    va_list variadicParams;
    int escapeSequences = 0;
    char* s = formattedString;

    while(*s != '\0')
    {
        if (*s == '\\' && *(s+1) != '\\')
        {
            escapeSequences++;
        }
        s++;
    }

    va_start(variadicParams, escapeSequences);

    while (*formattedString != '\0')
    {
        if (*formattedString != '\\')
        {
            uputc(pUART, *formattedString);
        }
        else
        {
            s = (formattedString + 1);
            switch (*s)
            {
                case('\\'):
                {
                    uputc(pUART, '\\');
                    formattedString++;
                } break;

                // Character
                case ('c'):
                {
                    uputc(pUART, va_arg(variadicParams, char));
                    formattedString++;
                } break;

                // String
                case ('s'):
                {
                    uprints(pUART, va_arg(variadicParams, char*));
                    formattedString++;
                } break;

                // Unsigned Integer
                case ('u'):
                {
                    uprintu(pUART, va_arg(variadicParams, u32));
                    formattedString++;
                } break;

                // Signed Integer
                case ('d'):
                {
                    uprintd(pUART, va_arg(variadicParams, int));
                    formattedString++;
                } break;

                // Hexidecimal
                case ('x'):
                {
                    uprintx(pUART, va_arg(variadicParams, u32));
                    formattedString++;
                } break;

                // Octal
                case ('o'):
                {
                    uprinto(pUART, va_arg(variadicParams, u32));
                    formattedString++;
                } break;

                default: {

                } break;
            }
        }

        formattedString++;
    }
}