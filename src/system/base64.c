/*
Copyright (C) 2019 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "base64.h"

static char encodingTable[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 
	                           'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 
							   'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 
							   'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 
							   '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};
static char *decodingTable = NULL;
static int modTable[] = {0, 2, 1};

void initBase64(void)
{
	int i;
	
	decodingTable = malloc(256);
	
	for (i = 0; i < 64; i++)
	{
		decodingTable[(unsigned char) encodingTable[i]] = i;
	}
}

char *base64Encode(const unsigned char *data, unsigned long inputLength, unsigned long *outputLength)
{
	char *encodedData;
	int i, j;
	unsigned long octetA, octetB, octetC, triple;
	
	*outputLength = 4 * ((inputLength + 2) / 3);
	
	encodedData = malloc(*outputLength + 1);
	
	if (encodedData != NULL)
	{
		for (i = 0, j = 0 ; i < inputLength ; )
		{
			octetA = i < inputLength ? (unsigned char)data[i++] : 0;
			octetB = i < inputLength ? (unsigned char)data[i++] : 0;
			octetC = i < inputLength ? (unsigned char)data[i++] : 0;

			triple = (octetA << 0x10) + (octetB << 0x08) + octetC;

			encodedData[j++] = encodingTable[(triple >> 3 * 6) & 0x3F];
			encodedData[j++] = encodingTable[(triple >> 2 * 6) & 0x3F];
			encodedData[j++] = encodingTable[(triple >> 1 * 6) & 0x3F];
			encodedData[j++] = encodingTable[(triple >> 0 * 6) & 0x3F];
		}
		
		for (i = 0; i < modTable[inputLength % 3]; i++)
		{
			encodedData[*outputLength - 1 - i] = '=';
		}
		
		encodedData[*outputLength - 1] = '\0';
	}
	
	return encodedData;
}

unsigned char *base64Decode(const char *data, unsigned long inputLength)
{
	int i, j;
	unsigned long sextetA, sextetB, sextetC, sextetD, triple, outputLength;
	unsigned char *decodedData;
	
	decodedData = NULL;
	
	if (inputLength % 4 == 0)
	{
		outputLength = inputLength / 4 * 3;
		
		if (data[inputLength - 1] == '=')
		{
			outputLength--;
		}
		
		if (data[inputLength - 2] == '=')
		{
			outputLength--;
		}
		
		decodedData = malloc(outputLength);
		
		if (decodedData != NULL)
		{
			for (i = 0, j = 0; i < inputLength;)
			{
				sextetA = data[i] == '=' ? 0 & i++ : decodingTable[(unsigned char)data[i++]];
				sextetB = data[i] == '=' ? 0 & i++ : decodingTable[(unsigned char)data[i++]];
				sextetC = data[i] == '=' ? 0 & i++ : decodingTable[(unsigned char)data[i++]];
				sextetD = data[i] == '=' ? 0 & i++ : decodingTable[(unsigned char)data[i++]];

				triple = (sextetA << 3 * 6) + (sextetB << 2 * 6)+ (sextetC << 1 * 6) + (sextetD << 0 * 6);

				if (j < outputLength)
				{
					decodedData[j++] = (triple >> 2 * 8) & 0xFF;
					decodedData[j++] = (triple >> 1 * 8) & 0xFF;
					decodedData[j++] = (triple >> 0 * 8) & 0xFF;
				}
			}
			
			decodedData[outputLength] = '\0';
		}
	}
	
	return decodedData;
}
