#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <assert.h>
/*
This program encodes and decodes text into Base64 format.
*/
/*name of this program.*/
const char* pgm_name;

/*BASE64 code page values.*/
const char base64_codepage[64] = 
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
prints usage information for this program to stream
(typically stdout or stderr), and exit the program with EXIT_CODE. 
does not return
*/
void print_usage (FILE* stream, int exit_code)
{
	fprintf (stream, "Usage: %s options <data>\n", pgm_name);
	fprintf (stream, 
		" -h	--help			Display this usage information\n"
		" -e	--encode		Encode data to Base64 format\n"
		" -d	--decode		Decode data from Base64 format\n");
	exit (exit_code);
}

unsigned char* encode_b64 (unsigned char* input_txt,size_t length,size_t* out_length)
{
	unsigned char* output_txt;
	unsigned char c1,c2,c3,temp;
	size_t offset,out_offset;
	int flag = 0;
	*out_length = length * (4/3 + 1);
	output_txt = (unsigned char*) malloc (*out_length);
	offset = 0;
	out_offset = 0;
	while (offset < length) {
		c1 = input_txt[offset++];
		temp = c1 >> 2;
		output_txt[out_offset++] = base64_codepage[temp];
		
		if (offset >= length) {
			flag = 1;
		}
		c2 = input_txt[offset++];
		temp = (c1 << 6) | (c2 >> 2);
		temp = temp >> 2;
		output_txt[out_offset++] = base64_codepage[temp];

		if (flag) {
			output_txt[out_offset++] = '=';
			output_txt[out_offset++] = '=';
			break;
		}
		if (offset >= length) {
			flag = 1;
		}
		c3 = input_txt[offset++];
		temp = (c2 << 4) | (c3 >> 4);
		temp = temp >> 2;
		output_txt[out_offset++] = base64_codepage[temp];

		if (flag) {
			output_txt[out_offset++] = '=';
			break;
		}		
		temp = c3 & 0x3F;
		output_txt[out_offset++] = base64_codepage[temp];
	}
	output_txt[out_offset] = '\0';
	return output_txt;
}

unsigned char* decode_b64 (unsigned char* input_txt,size_t length,size_t* out_length)
{
	unsigned char* output_txt;
	int index;
	unsigned char c1,c2,c3,temp;
	size_t offset,out_offset;
	assert (length % 4 == 0);
	*out_length = length * (3/4.0) + 1;
	output_txt = (unsigned char*) malloc (*out_length);
	offset = 0;
	out_offset = 0;
	while (offset < length) {
		temp = input_txt[offset++];
		for (index=0;base64_codepage[index] != temp && index < 64;index++);
		c1 = index << 2;
		temp = input_txt[offset++];
		for (index=0;base64_codepage[index] != temp && index < 64;index++);
		c1 |= ((index & 0x30) >> 4);
		c2 = (index & 0x0F) << 4;
		output_txt[out_offset++] = c1;

		temp = input_txt[offset++];
		if ('=' != temp) {
			for (index=0;base64_codepage[index] != temp && index < 64;index++);
			c2 |= ((index & 0x3C) >> 2);
			c3 = (index & 0x03) << 6;
		}
		output_txt[out_offset++] = c2;
		
		temp = input_txt[offset++];
		if ('=' != temp) {
			for (index=0;base64_codepage[index] != temp && index < 64;index++);
			c3 |= index;
			output_txt[out_offset++] = c3;
		}
	}
	output_txt[out_offset] = '\0';
	return output_txt;
}

int main (int argc,char* argv[])
{
	unsigned char* input_txt;
	size_t length;
	size_t out_length;
	unsigned char* output_txt;
	size_t i;
	
	int next_option;
	/*a string listing valid short options letters*/
	const char* const short_options = "he:d:";
	/* an array describing valid long options.*/
	const struct option long_options[] = {
		{ "help",  no_argument,       NULL, 'h'},
		{"encode", required_argument, NULL, 'e'},
		{"decode", required_argument, NULL, 'd'},
		{0 , 0, 0, 0 } /*required at end of array*/
	};
	
	/*remember the name of pgm to incorporate in messages.*/
	pgm_name = argv[0];
	
	do {
		next_option = getopt_long (argc, argv, short_options, long_options, NULL);
		switch (next_option)
		{
		case 'h' : /*-h or --help*/
			/*user has requested usage info. print it to standard output and exit with exit code zero(normal termination*/
			print_usage (stdout, 0);
			break;
		case 'e' : /* -e or --encode */
			/*this option takes an argument, the data to encode*/
			input_txt = (unsigned char*) optarg;
			length = strlen (optarg);
			output_txt = encode_b64(input_txt,length,&out_length);
			if (output_txt != NULL)
				printf ("%s",output_txt);
			break;
		case 'd' : /* -d or --decode */
			/*this option takes an argument, the data to decode*/
			input_txt = (unsigned char*) optarg;
			length = strlen (optarg);
			output_txt = decode_b64(input_txt,length,&out_length);
			if (output_txt != NULL) {
				for (i = 0;i <= out_length;i++) {
					printf ("%c",output_txt[i]);
				}
			}
			break;
		case '?' : /*use specified an invalid option. */
			/*print usage information to stderr, and exit with exit_code 1(indicating abormal termination*/
			print_usage (stderr, 1);
		case -1 : /*done parsing options*/
			break;
		default : /*something else: unexpected */
			abort();
		}
	} while (next_option != -1);
	
	return 0;
}
