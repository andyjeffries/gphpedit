/* This file is part of gPHPEdit, a GNOME2 PHP Editor.
 
   Copyright (C) 2003, 2004, 2005 Andy Jeffries <andy at gphpedit.org>
   Copyright (C) 2009 Anoop John <anoop dot john at zyxware.com>
	  
   For more information or to find the latest release, visit our 
   website at http://www.gphpedit.org/
 
   gPHPEdit is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   gPHPEdit is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with gPHPEdit.  If not, see <http://www.gnu.org/licenses/>.
 
   The GNU General Public License is contained in the file COPYING.
*/

#include <stdio.h>
#include "calltip.h"
#include "tab.h"
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
//#define DEBUGCALLTIP
GSList *api_list;
GSList *css_api_list;
GString *calltip=NULL;
#define MAX_API_LINE_LENGTH 16384

gchar *cobol_keywords[] = {"ACCEPT","ACCESS","ACQUIRE","ACTUAL","ADD","ADDRESS","ADVANCING","AFTER","ALL","ALLOWING","ALPHABET","ALPHABETIC","ALPHABETIC-LOWER","ALPHABETIC-UPPER","ALPHANUMERIC","ALPHANUMERIC-EDITED","ALSO","ALTER","ALTERNATE","AND","ANY","APPLY","ARE","AREA","AREAS","ASCENDING","ASSIGN","ATTRIBUTE","AUTHOR","AUTO","AUTO-HYPHEN-SKIP","AUTO-SKIP","AUTOMATIC","AUTOTERMINATE","BACKGROUND-COLOR","BACKGROUND-COLOUR","BACKWARD","BASIS","BEEP","BEFORE","BEGINNING","BELL","BINARY","BLANK","BLINK","BLINKING","BLOCK","BOLD","BOTTOM","BOX","BOXED","CALL","CANCEL","CBL","CENTERED","CHAIN","CHAINING","CHANGED","CHARACTER","CHARACTERS","CHART","CLASS","CLOCK-UNITS","CLOSE","COBOL","CODE","CODE-SET","COL","COLLATING","COLOR","COLOUR","COLUMN","COM-REG","COMMA","COMMAND-LINE","COMMIT","COMMITMENT","COMMON","COMMUNICATION","COMP","COMP-0","COMP-1","COMP-2","COMP-3","COMP-4","COMP-5","COMP-6","COMP-X","COMPRESSION","COMPUTATIONAL","COMPUTATIONAL","COMPUTATIONAL-1","COMPUTATIONAL-2","COMPUTATIONAL-3","COMPUTATIONAL-4","COMPUTATIONAL-5","COMPUTATIONAL-6","COMPUTATIONAL-X","COMPUTE","CONFIGURATION","CONSOLE","CONTAINS","CONTENT","CONTINUE","CONTROL","CONTROL-AREA","CONTROLS","CONVERSION","CONVERT","CONVERTING","COPY","CORE-INDEX","CORR","CORRESPONDING","COUNT","CRT","CRT-UNDER","CSP","CURRENCY","CURRENT-DATE","CURSOR","CYCLE","CYL-INDEX","CYL-OVERFLOW","DATA","DATE","DATE-COMPILED","DATE-WRITTEN","DAY","DAY-OF-WEEK","DBCS","DEBUG","DEBUG-CONTENTS","DEBUG-ITEM","DEBUG-LINE","DEBUG-NAME","DEBUG-SUB-1","DEBUG-SUB-2","DEBUG-SUB-3","DEBUGGING","DECIMAL-POINT","DECLARATIVES","DEFAULT","DELETE","DELIMITED","DELIMITER","DEPENDING","DESCENDING","DESTINATION","DETAIL","DISABLE","DISK","DISP","DISPLAY","DISPLAY-1","DISPLAY-ST","DIVIDE","DIVISION","DOWN","DRAW","DROP","DUPLICATES","DYNAMIC","ECHO","EGCS","EGI","EJECT","ELSE","EMI","EMPTY-CHECK","ENABLE","ENCRYPTION","END","END-ACCEPT","END-ADD","END-CALL","END-CHAIN","END-COMPUTE","END-DELETE","END-DISPLAY","END-DIVIDE","END-EVALUATE","END-IF","END-INVOKE","END-MULTIPLY","END-OF-PAGE","END-PERFORM","END-READ","END-RECEIVE","END-RETURN","END-REWRITE","END-SEARCH","END-START","END-STRING","END-SUBTRACT","END-UNSTRING","END-WRITE","ENDING","ENTER","ENTRY","ENVIRONMENT","ENVIRONMENT-DIVISION","EOL","EOP","EOS","EQUAL","EQUALS","ERASE","ERROR","ESCAPE","ESI","EVALUATE","EVERY","EXAMINE","EXCEEDS","EXCEPTION","EXCESS-3","EXCLUSIVE","EXEC","EXECUTE","EXHIBIT","EXIT","EXTEND","EXTENDED-SEARCH","EXTERNAL","EXTERNALLY-DESCRIBED-KEY","FACTORY","FALSE","FH--FCD","FH--KEYDEF","FILE","FILE-CONTROL","FILE-ID","FILE-LIMIT","FILE-LIMITS","FILE-PREFIX","FILLER","FINAL","FIRST","FIXED","FOOTING","FOOTING","FOR","FOREGROUND-COLOR","FOREGROUND-COLOUR","FORMAT","FROM","FULL","FUNCTION","GENERATE","GIVING","GLOBAL","GO","GOBACK","GREATER","GRID","GROUP","HEADING","HIGH","HIGH-VALUE","HIGH-VALUES","HIGHLIGHT","I-O-CONTROL","IDENTIFICATION","IGNORE","INDEX","INDEXED","INDIC","INDICATE","INDICATOR","INDICATORS","INHERITING","INITIAL","INITIALIZE","INITIATE","INPUT","INPUT-OUTPUT","INSERT","INSPECT","INSTALLATION","INTO","INVALID","INVOKE","INVOKED","JAPANESE","JUST","JUSTIFIED","KANJI","KEPT","KEY","KEYBOARD","LABEL","LAST","LEADING","LEAVE","LEFT","LEFT-JUSTIFY","LEFTLINE","LENGTH","LENGTH-CHECK","LESS","LIMIT","LIMITS","LIN","LINAGE","LINAGE-COUNTER","LINE","LINE-COUNTER","LINES","LINKAGE","LOCAL-STORAGE","LOCK","LOCK-HOLDING","LOCKING","LOW","LOW-VALUE","LOW-VALUES","LOWER","LOWLIGHT","MANUAL","MASS-UPDATE","MASTER-INDEX","MEMORY","MERGE","MESSAGE","METHOD","MODE","MODIFIED","MODULES","MORE-LABELS","MOVE","MULTIPLE","MULTIPLY","NAME","NAMED","NATIONAL","NATIONAL-EDITED","NATIVE","NCHAR","NEGATIVE","NEXT","NO-ECHO","NOMINAL","NOT","NOTE","NSTD-REELS","NULL","NULLS","NUMBER","NUMERIC","NUMERIC-EDITED","NUMERIC-FILL","O-FILL","OBJECT","OBJECT-COMPUTER","OBJECT-STORAGE","OCCURS","OFF","OMITTED","OOSTACKPTR","OPEN","OPTIONAL","ORDER","ORGANIZATION","OTHER","OTHERS","OTHERWISE","OUTPUT","OVERFLOW","OVERLINE","PACKED-DECIMAL","PACKED-DECIMAL","PADDING","PAGE","PAGE-COUNTER","PARAGRAPH","PASSWORD","PERFORM","PIC","PICTURE","PLUS","POINTER","POP-UP","POS","POSITION","POSITIONING","POSITIVE","PREVIOUS","PRINT","PRINT-CONTROL","PRINT-SWITCH","PRINTER","PRINTER-1","PRINTING","PRIOR","PRIVATE","PROCEDURE","PROCEDURE-POINTER","PROCEDURES","PROCEED","PROCESS","PROCESSING","PROGRAM","PROGRAM-ID","PROMPT","PROTECTED","PUBLIC","PURGE","QUEUE","QUOTE","QUOTES","RANDOM","RANGE","READ","READERS","READY","RECEIVE","RECEIVE-CONTROL","RECORD","RECORD-OVERFLOW","RECORDING","RECORDS","REDEFINES","REEL","REFERENCE","REFERENCES","RELATIVE","RELEASE","RELOAD","REMAINDER","REMARKS","REMOVAL","RENAMES","REORG-CRITERIA","REPEATED","REPLACE","REPLACING","REPORT","REPORTING","REPORTS","REQUIRED","REREAD","RERUN","RESERVE","RESET","RESIDENT","RETURN","RETURN-CODE","RETURNING","REVERSE","REVERSE-VIDEO","REVERSED","REWIND","REWRITE","RIGHT","RIGHT-JUSTIFY","ROLLBACK","ROLLING","ROUNDED","RUN","S01","S02","S03","S04","S05","SAME","SCREEN","SCROLL","SEARCH","SECTION","SECURE","SECURITY","SEEK","SEGMENT","SEGMENT-LIMIT","SELECT","SELECTIVE","SELF","SELFCLASS","SEND","SENTENCE","SEPARATE","SEQUENCE","SEQUENTIAL","SERVICE","SET","SETSHADOW","SHIFT-IN","SHIFT-OUT","SIGN","SIZE","SKIP1","SKIP2","SKIP3","SORT","SORT-CONTROL","SORT-CORE-SIZE","SORT-FILE-SIZE","SORT-MERGE","SORT-MESSAGE","SORT-MODE-SIZE","SORT-OPTION","SORT-RETURN","SOURCE","SOURCE-COMPUTER","SPACE","SPACE-FILL","SPACES","SPACES","SPECIAL-NAMES","STANDARD","STANDARD-1","STANDARD-2","START","STARTING","STATUS","STOP","STORE","STRING","SUB-QUEUE-1","SUB-QUEUE-2","SUB-QUEUE-3","SUBFILE","SUBTRACT","SUM","SUPER","SUPPRESS","SYMBOLIC","SYNC","SYNCHRONIZED","SYSIN","SYSIPT","SYSLST","SYSOUT","SYSPCH","SYSPUNCH","SYSTEM-INFO","TAB","TALLYING","TAPE","TERMINAL","TERMINAL-INFO","TERMINATE","TEST","TEXT","THAN","THEN","THROUGH","THRU","TIME","TIME-OF-DAY","TIME-OUT","TIMEOUT","TIMES","TITLE","TOP","TOTALED","TOTALING","TRACE","TRACK-AREA","TRACK-LIMIT","TRACKS","TRAILING","TRAILING-SIGN","TRANSACTION","TRANSFORM","TRUE","TYPE","TYPEDEF","UNDERLINE","UNDERLINED","UNEQUAL","UNIT","UNLOCK","UNSTRING","UNTIL","UPDATE","UPDATERS","UPON","UPPER","UPSI-0","UPSI-1","UPSI-2","UPSI-3","UPSI-4","UPSI-5","UPSI-6","UPSI-7","USAGE","USE","USER","USING","VALUE","VALUES","VARIABLE","VARYING","WAIT","WHEN","WHEN-COMPILED","WINDOW","WITH","WORDS","WORKING-STORAGE","WRAP","WRITE","WRITE-ONLY","WRITE-VERIFY","WRITERSZERO","ZERO","ZERO-FILL","ZEROES","ZEROS",NULL};

gchar *sql_keywords[] = {"ADD", "ALL", "ALTER", "ANALYZE", "AND", "AS", "ASC", "ASENSITIVE", "AUTO_INCREMENT", 
						 "BDB", "BEFORE", "BERKELEYDB", "BETWEEN", "BIGINT", "BINARY", "BLOB", "BOTH", "BTREE",
						 "BY", "CALL", "CASCADE", "CASE", "CHANGE", "CHAR", "CHARACTER", "CHECK", "COLLATE", 
						 "COLUMN", "COLUMNS", "CONNECTION", "CONSTRAINT", "CREATE", "CROSS", "CURRENT_DATE", 
						 "CURRENT_TIME", "CURRENT_TIMESTAMP", "CURSOR", "DATABASE", "DATABASES", "DAY_HOUR", 
						 "DAY_MINUTE", "DAY_SECOND", "DEC", "DECIMAL", "DECLARE", "DEFAULT", "DELAYED", 
						 "DELETE", "DESC", "DESCRIBE", "DISTINCT", "DISTINCTROW", "DIV", "DOUBLE", "DROP", 
						 "ELSE", "ELSEIF", "ENCLOSED", "ERRORS", "ESCAPED", "EXISTS", "EXPLAIN", "FALSE", 
						 "FIELDS", "FLOAT", "FOR", "FORCE", "FOREIGN", "FROM", "FULLTEXT", "GRANT", "GROUP", 
						 "HASH", "HAVING", "HIGH_PRIORITY", "HOUR_MINUTE", "HOUR_SECOND", "IF", "IGNORE", 
						 "IN", "INDEX", "INFILE", "INNER", "INNODB", "INOUT", "INSENSITIVE", "INSERT", "INT", 
						 "INTEGER", "INTERVAL", "INTO", "IS", "ITERATE", "JOIN", "KEY", "KEYS", "KILL", 
						 "LEADING", "LEAVE", "LEFT", "LIKE", "LIMIT", "LINES", "LOAD", "LOCALTIME", 
						 "LOCALTIMESTAMP", "LOCK", "LONG", "LONGBLOB", "LONGTEXT", "LOOP", "LOW_PRIORITY", 
						 "MASTER_SERVER_ID", "MATCH", "MEDIUMBLOB", "MEDIUMINT", "MEDIUMTEXT", "MIDDLEINT", 
						 "MINUTE_SECOND", "MOD", "MRG_MYISAM", "NATURAL", "NOT", "NULL", "NUMERIC", "ON",
						 "OPTIMIZE", "OPTION", "OPTIONALLY", "OR", "ORDER", "OUT", "OUTER", "OUTFILE", 
						 "PRECISION", "PRIMARY", "PRIVILEGES", "PROCEDURE", "PURGE", "READ", "REAL", 
						 "REFERENCES", "REGEXP", "RENAME", "REPEAT", "REPLACE", "REQUIRE", "RESTRICT", 
						 "RETURN", "RETURNS", "REVOKE", "RIGHT", "RLIKE", "RTREE", "SELECT", "SENSITIVE", 
						 "SEPARATOR", "SET", "SHOW", "SMALLINT", "SOME", "SONAME", "SPATIAL", "SPECIFIC", 
						 "SQL_BIG_RESULT", "SQL_CALC_FOUND_ROWS", "SQL_SMALL_RESULT", "SSL", "STARTING", 
						 "STRAIGHT_JOIN STRIPED", "TABLE", "TABLES", "TERMINATED", "THEN", "TINYBLOB", 
						 "TINYINT", "TINYTEXT", "TO", "TRAILING", "TRUE", "TYPES", "UNION", "UNIQUE", 
						 "UNLOCK", "UNSIGNED", "UNTIL", "UPDATE", "USAGE", "USE", "USER_RESOURCES", 
						 "USING", "VALUES", "VARBINARY", "VARCHAR", "VARCHARACTER", "VARYING", "WARNINGS", 
						 "WHEN", "WHERE", "WHILE", "WITH", "WRITE", "XOR", "YEAR_MONTH", "ZEROFILL", NULL};

void function_list_prepare(void)
{
	FILE *apifile;
	char buffer[MAX_API_LINE_LENGTH];
        gchar *api_dir = NULL;
	/* use autoconf macro to build api file path */
	api_dir = g_build_path (G_DIR_SEPARATOR_S, API_DIR, "php-gphpedit.api", NULL);
	#ifdef DEBUGCALLTIP
	g_print("DEBUG::API PATH:'%s'\n",api_dir);
	#endif
	apifile = fopen(api_dir, "r");
	if( apifile != NULL ) {
		while( fgets( buffer, MAX_API_LINE_LENGTH, apifile ) != NULL ) {
			gchar *line=g_strdup(buffer);
			/* From glib docs. Prepend and reverse list it's more eficient */
			api_list = g_slist_prepend(api_list, line);
		}
		api_list= g_slist_reverse (api_list);
		fclose( apifile );
	}
	else {
		g_print(_("WARNING: Could not open php-gphpedit.api file\n"));
	}
	g_free(api_dir);
}


void css_function_list_prepare(void)
{
	FILE *apifile;
	char buffer[MAX_API_LINE_LENGTH];
        gchar *api_dir = NULL;
	/* use autoconf macro to build api file path */
	api_dir = g_build_path (G_DIR_SEPARATOR_S, API_DIR, "css.api", NULL);
	#ifdef DEBUGCALLTIP
	g_print("DEBUG::CSS API PATH:'%s'\n",api_dir);
	#endif
	apifile = fopen(api_dir, "r");
	if( apifile != NULL ) {
		while( fgets( buffer, MAX_API_LINE_LENGTH, apifile ) != NULL ) {
			gchar *line=g_strdup(buffer);
			/* From glib docs. Prepend and reverse list it's more eficient */
			css_api_list = g_slist_prepend(css_api_list, line);
		}
		css_api_list= g_slist_reverse (css_api_list);
		fclose( apifile );
	}
	else {
		g_print(_("WARNING: Could not open php-gphpedit.api file\n"));
	}
	g_free(api_dir);
}

GString *get_css_api_line(GtkWidget *scintilla, gint wordStart, gint wordEnd)
{
	GSList *walk;
	gchar *line;
	gchar *buffer = NULL;
	gchar *function_name;
	gchar *params;
	gchar *description;
	gint length;
	gchar *token_line, *copy_line;

	buffer = gtk_scintilla_get_text_range (GTK_SCINTILLA(scintilla), wordStart, wordEnd, &length);

	for (walk = css_api_list; walk != NULL; walk = g_slist_next (walk)) {
		line = walk->data;
		token_line = g_malloc(strlen(line)+1);
		copy_line = token_line;
		strcpy(token_line, line); // Line has trailing \n
		function_name = strtok(token_line, "|");
		params = strtok(NULL, "|");
		description = strtok(NULL, "|");
		/* A full comparison of the function name is required for the tool tip
		a partial match will result in an incorrect tooltip. So we 
		have to use strcmp and not strncasecmp */
		if (strcmp(function_name, buffer)==0) {
			calltip = g_string_new(NULL);
			g_string_printf(calltip, "%s %s\n%s", function_name, params, description);
			g_free (buffer);
			g_free(copy_line);
			return calltip;
		}
		g_free(copy_line);
	}

	g_free (buffer);
	return NULL;
}

GString *complete_css_function_list(gchar *original_list)
{
	GSList *walk;
	gchar *line;
	GString *result;
	gchar *buffer = NULL;
	gchar *function_name;
	gchar *token_line, *copy_line;
	result = g_string_new(original_list);

	for (walk = css_api_list; walk != NULL; walk = g_slist_next (walk)) {
		line = walk->data;
		token_line = g_malloc(strlen(line)+1);
		copy_line = token_line;
		strcpy(token_line, line); // Line has trailing \n
		function_name = strtok(token_line, "|");
		if (result == NULL) {
			if (original_list==NULL) {
				result = g_string_new(function_name);
				result = g_string_append(result, " ");
			}
			else {
				result = g_string_new("");
			}
		}
		else {
			result = g_string_append(result, " ");
		}
		if (function_name)
		result = g_string_append(result, function_name);
		g_free(copy_line);
	}

	g_free (buffer);
	return result;
}

GString *get_css_completion_list(GtkWidget *scintilla, gint wordStart, gint wordEnd)
{
	GString *completion_list;
	GSList *walk;
	gchar *line;
	gchar *buffer = NULL;
	gchar *function_name;
	gint length;
	gchar *token_line, *copy_line;
	guint num_in_list;

	buffer = gtk_scintilla_get_text_range (GTK_SCINTILLA(scintilla), wordStart, wordEnd, &length);
	
	completion_list=NULL;
	num_in_list = 0;
	for (walk = css_api_list; (walk != NULL && num_in_list < 50); walk = g_slist_next (walk)) {
		line = walk->data;
		token_line = g_malloc(strlen(line)+1);
		copy_line = token_line;
		strcpy(token_line, line); // Line has trailing \n
		function_name = strtok(token_line, "|");
		if ((g_str_has_prefix(function_name, buffer)) || (wordStart==wordEnd)) {
			num_in_list++;
			if (completion_list == NULL) {
				completion_list = g_string_new(function_name);
			}
			else {
				if (function_name){
				completion_list = g_string_append(completion_list, " ");
				completion_list = g_string_append(completion_list, function_name);
				}
			}
		}
		g_free(copy_line);
	}

	if (completion_list != NULL) {
		completion_list = g_string_append(completion_list, " ");
	}
	g_free (buffer);
	return completion_list;
}

GString *get_api_line(GtkWidget *scintilla, gint wordStart, gint wordEnd)
{
	GSList *walk;
	gchar *line;
	gchar *buffer = NULL;
	gchar *function_name;
	gchar *return_value;
	gchar *params;
	gchar *description;
	gint length;
	gchar *token_line, *copy_line;

	buffer = gtk_scintilla_get_text_range (GTK_SCINTILLA(scintilla), wordStart, wordEnd, &length);

	for (walk = api_list; walk != NULL; walk = g_slist_next (walk)) {
		line = walk->data;
		token_line = g_malloc(strlen(line)+1);
		copy_line = token_line;
		strcpy(token_line, line); // Line has trailing \n
		function_name = strtok(token_line, "|");
		return_value = strtok(NULL, "|");
		params = strtok(NULL, "|");
		description = strtok(NULL, "|");
		/* A full comparison of the function name is required for the tool tip
		a partial match will result in an incorrect tooltip. So we 
		have to use strcmp and not strncasecmp */
		if (strcmp(function_name, buffer)==0) {
			calltip = g_string_new(NULL);
			g_string_printf(calltip, "%s %s %s\n%s", return_value, function_name, params, description);
			g_free (buffer);
			g_free(copy_line);
			return calltip;
		}
		g_free(copy_line);
	}

	g_free (buffer);
	return NULL;
}

GString *complete_function_list(gchar *original_list)
{
	GSList *walk;
	gchar *line;
	GString *result;
	gchar *buffer = NULL;
	gchar *function_name;
	gchar *token_line, *copy_line;

	result = g_string_new(original_list);

	for (walk = api_list; walk != NULL; walk = g_slist_next (walk)) {
		line = walk->data;
		token_line = g_malloc(strlen(line)+1);
		copy_line = token_line;
		strcpy(token_line, line); // Line has trailing \n
		function_name = strtok(token_line, "|");
		if (result == NULL) {
			if (original_list==NULL) {
				result = g_string_new(function_name);
				result = g_string_append(result, " ");
			}
			else {
				result = g_string_new("");
			}
		}
		else {
			result = g_string_append(result, " ");
		}
		result = g_string_append(result, function_name);
		g_free(copy_line);
	}

	g_free (buffer);
	return result;
}


GString *get_completion_list(GtkWidget *scintilla, gint wordStart, gint wordEnd)
{
	GString *completion_list;
	GSList *walk;
	gchar *line;
	gchar *buffer = NULL;
	gchar *function_name;
	gint length;
	gchar *token_line, *copy_line;
	guint num_in_list;

	buffer = gtk_scintilla_get_text_range (GTK_SCINTILLA(scintilla), wordStart, wordEnd, &length);

	completion_list=NULL;
	num_in_list = 0;
	for (walk = api_list; (walk != NULL && num_in_list < 50); walk = g_slist_next (walk)) {
		line = walk->data;
		token_line = g_malloc(strlen(line)+1);
		copy_line = token_line;
		strcpy(token_line, line); // Line has trailing \n
		function_name = strtok(token_line, "|");
		if ((g_str_has_prefix(function_name, buffer)) || (wordStart==wordEnd)) {
			num_in_list++;
			if (completion_list == NULL) {
				completion_list = g_string_new(function_name);
			}
			else {
				completion_list = g_string_append(completion_list, " ");
				completion_list = g_string_append(completion_list, function_name);
			}
		}
		g_free(copy_line);
	}

	if (completion_list != NULL) {
		completion_list = g_string_append(completion_list, " ");
	}
	g_free (buffer);
	return completion_list;
}

void autocomplete_word(GtkWidget *scintilla, gint wordStart, gint wordEnd)
{
	GString *list;

	list = get_completion_list(scintilla, wordStart, wordEnd);

	if (list) {
		gtk_scintilla_autoc_show(GTK_SCINTILLA(scintilla), wordEnd-wordStart, list->str);
		g_string_free(list, FALSE);
	}
}

GString *get_cobol_completion_list(GtkWidget *scintilla, gint wordStart, gint wordEnd)
{
	GString *completion_list;
	gchar *buffer = NULL;
	gint length;
	guint n;

	buffer = gtk_scintilla_get_text_range (GTK_SCINTILLA(scintilla), wordStart, wordEnd, &length);

	completion_list=NULL;

	for (n = 0; cobol_keywords[n]!=NULL; n++) {
		if (g_str_has_prefix(cobol_keywords[n], buffer)) {
			if (completion_list == NULL) {
				completion_list = g_string_new(cobol_keywords[n]);
			}
			else {
				completion_list = g_string_append(completion_list, " ");
				completion_list = g_string_append(completion_list, cobol_keywords[n]);
			}
		}
	}

	if (completion_list != NULL) {
		completion_list = g_string_append(completion_list, " ");
	}
	g_free (buffer);
	#ifdef DEBUGCALLTIP
	g_print("Cobol completion list :%s\n",completion_list->str);
	#endif
	return completion_list;
}


GString *get_sql_completion_list(GtkWidget *scintilla, gint wordStart, gint wordEnd)
{
	GString *completion_list;
	gchar *buffer = NULL;
	gint length;
	guint n;

	buffer = gtk_scintilla_get_text_range (GTK_SCINTILLA(scintilla), wordStart, wordEnd, &length);

	completion_list=NULL;

	for (n = 0; sql_keywords[n]!=NULL; n++) {
		if (g_str_has_prefix(sql_keywords[n], buffer)) {
			if (completion_list == NULL) {
				completion_list = g_string_new(sql_keywords[n]);
			}
			else {
				completion_list = g_string_append(completion_list, " ");
				completion_list = g_string_append(completion_list, sql_keywords[n]);
			}
		}
	}

	if (completion_list != NULL) {
		completion_list = g_string_append(completion_list, " ");
	}
	g_free (buffer);
	return completion_list;
}



void css_autocomplete_word(GtkWidget *scintilla, gint wordStart, gint wordEnd)
{
	GString *list;

	list = get_css_completion_list(scintilla, wordStart, wordEnd);
	
	if (list) {
		gtk_scintilla_autoc_show(GTK_SCINTILLA(scintilla), wordEnd-wordStart, list->str);
		g_string_free(list, FALSE);
	}
}

void cobol_autocomplete_word(GtkWidget *scintilla, gint wordStart, gint wordEnd)
{
	GString *list;

	list = get_cobol_completion_list(scintilla, wordStart, wordEnd);

	if (list) {
		gtk_scintilla_autoc_show(GTK_SCINTILLA(scintilla), wordEnd-wordStart, list->str);
		g_string_free(list, TRUE);
	}
}


void sql_autocomplete_word(GtkWidget *scintilla, gint wordStart, gint wordEnd)
{
	GString *list;

	list = get_sql_completion_list(scintilla, wordStart, wordEnd);

	if (list) {
		gtk_scintilla_autoc_show(GTK_SCINTILLA(scintilla), wordEnd-wordStart, list->str);
		g_string_free(list, FALSE);
	}
}

//function to show the tool tip with a short description about the
//php function. The current word at the cursor is used to find the
//corresponding function from the php-gphpedit.api file
void show_call_tip(GtkWidget *scintilla,gint type, gint pos)
{
	gint wordStart;
	gint wordEnd;
	GString *api_line;

	wordStart = gtk_scintilla_word_start_position(GTK_SCINTILLA(scintilla), pos-1, TRUE);
	wordEnd = gtk_scintilla_word_end_position(GTK_SCINTILLA(scintilla), pos-1, TRUE);

        //function returns the global variable calltip. So does not have to free
	if (type==TAB_PHP){
	api_line = get_api_line(scintilla, wordStart, wordEnd);
	} else {
	api_line = get_css_api_line(scintilla, wordStart, wordEnd);
	}

	#ifdef DEBUGCALLTIP
	g_print("DEBUG::Calltip:text->%s,pos%d",api_line->str,pos;
	#endif
	if (api_line != NULL) {
		gtk_scintilla_call_tip_show(GTK_SCINTILLA(scintilla), wordStart, api_line->str);
		// Fix as suggested by "urkle" in bug 55, comment out following line
		//g_string_free(api_line, TRUE);
	}
}
void clean_list_item (gpointer data, gpointer user_data){
     g_free (data);
}

void cleanup_calltip(void){
	if (api_list) {
	g_slist_foreach(api_list, (GFunc)clean_list_item, NULL);
	g_slist_free(api_list);
	}
	if (css_api_list) {
	g_slist_foreach(css_api_list, (GFunc)clean_list_item, NULL);
	g_slist_free(css_api_list);
	}
}
