#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <expat.h>

#define BUFFSIZE 8192

char Buff[BUFFSIZE];
FILE *input, *output;

static void XMLCALL
default_hndl(void *data, const char *s, int len) {
	fwrite(s, len, sizeof(char), output);
}

static void XMLCALL
printcurrent(XML_Parser p) {
	XML_SetDefaultHandler(p, default_hndl);
	XML_DefaultCurrent(p);
	XML_SetDefaultHandler(p, (XML_DefaultHandler) 0);
}

static void XMLCALL
start_hndl(void *data, const char *el, const char **attr) {
	printcurrent((XML_Parser) data);
}

static void XMLCALL
end_hndl(void *data, const char *el) {
	printcurrent((XML_Parser) data);
}

static void XMLCALL
cdata_start_hndl(void *data) {
	printcurrent((XML_Parser) data);
}

static void XMLCALL
cdata_end_hndl(void *data) {
	printcurrent((XML_Parser) data);
}

static void XMLCALL
char_hndl(void *data, const char *txt, int txtlen) {
	char mask[txtlen];
	strncpy(mask, txt, txtlen);
	int i;
	for (i = 0; i < txtlen; i++) {
		if (islower(mask[i]))
			mask[i] = (char)'x';
		else if (isupper(mask[i]))
			mask[i] = (char)'X';
		else if (isdigit(mask[i]))
			mask[i] = (char)'0';
	}

	// escape '&' and '<' characters
	char escaped_text[txtlen*5];
	int escaped_position = 0;
	for (i = 0; i < txtlen; i++) {
		if (mask[i] == '<') {
			escaped_text[escaped_position++] = '&';
			escaped_text[escaped_position++] = 'l';
			escaped_text[escaped_position++] = 't';
			escaped_text[escaped_position++] = ';';
		} else if (mask[i] == '&') {
			escaped_text[escaped_position++] = '&';
			escaped_text[escaped_position++] = 'a';
			escaped_text[escaped_position++] = 'm';
			escaped_text[escaped_position++] = 'p';
			escaped_text[escaped_position++] = ';';
		} else
			escaped_text[escaped_position++] = mask[i];
	}
	escaped_text[escaped_position++] = '\0';

	fwrite(escaped_text, sizeof(char), escaped_position - 1, output);
}

static void XMLCALL
proc_hndl(void *data, const char *target, const char *pidata) {
	printcurrent((XML_Parser) data);
}

void print_usage(char *message) {
	fprintf(stderr, "Usage: xmlscrambler <input file> <output file>\n");
	fprintf(stderr, "\tEither parameter can be '-' to indicate standard input or output.\n");
	if (strcmp(message, "")!=0)
		fprintf(stderr, message);
	exit(1);
}

int
main(int argc, char **argv) {
	if (argc != 3)
		print_usage("");

	if (strcmp(argv[1], "-")==0)
		input = stdin;
	else {
		if (!(input = fopen(argv[1], "r")))
			print_usage("\nError: Could not open input file.\n");
	}

	if (strcmp(argv[2], "-")==0)
		output = stdout;
	else {
		if (!(output = fopen(argv[2], "w")))
			print_usage("\nError: Could not open output file.\n");
	}

	XML_Parser p = XML_ParserCreate(NULL);
	if (!p) {
		fprintf(stderr, "Couldn't allocate memory for parser\n");
		exit(-1);
	}

	XML_UseParserAsHandlerArg(p);
	XML_SetCharacterDataHandler(p, char_hndl);
	XML_SetElementHandler(p, start_hndl, end_hndl);
	XML_SetProcessingInstructionHandler(p, proc_hndl);
	XML_SetCdataSectionHandler(p, cdata_start_hndl, cdata_end_hndl);
	
	fprintf(output, "<?xml version=\"1.0\"?>\n");

	for (;;) {
		int done;
		int len;
		len = fread(Buff, 1, sizeof(Buff), input);
		done = feof(input);
		if (! XML_Parse(p, Buff, len, done)) {
			fprintf(stderr, "len: %d\n", len);
			fprintf(stderr, "Buff: %s\n", Buff);
			fprintf(stderr, "Parse error at line %d:\n%s\n:",
					(int)XML_GetCurrentLineNumber(p),
					XML_ErrorString(XML_GetErrorCode(p)));
			exit(-1);
		}

		if (done)
			break;
	}
	fprintf(output, "\n");
	XML_ParserFree(p);

	fclose(input);
	fclose(output);

	return EXIT_SUCCESS;
}
