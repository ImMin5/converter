#include "ssu_convert.h"

void convert_file(char *read_line, int class_count)
{
	char *result_var = NULL;
	char *result_file_name = NULL;
	char result_tmp[MAX_LINE] = {};
	char read_line_tmp[MAX_LINE] = {};
	int i;
	int flag = 0;

	if((result_var = strstr(read_line," ")) != NULL){

		memcpy(result_tmp, &result_var[1], MAX_LINE);
		for(i = 0; i< (int)strlen(read_line) ; i++) {
			//공백문자일때
			if(flag == 0 && isspace(result_tmp[i]) != 0 ) {
				result_tmp[i] = 0x00;
				flag = 1;
			}
			else if(flag == 1)
				result_tmp[i] = 0x00;
		}
		//변수명을 저장한다.
		memcpy(file_info.var_name[file_info.var_count], result_tmp, sizeof(char) *strlen(result_tmp));
		file_info.var_count++;
	}
	memcpy(read_line_tmp, read_line, sizeof(char) * strlen(read_line));
	remain_space(read_line_tmp);
	strcat(read_line_tmp,"FILE *");
	strcat(read_line_tmp,result_tmp);
	strcat(read_line_tmp," = NULL;\n");
	fputs(read_line_tmp,f_info[class_count].c_file_fp);


	//오픈할 파일이름을 저장하는 과정
	flag = 0;
	if((result_file_name = strstr(read_line,"\"")) != NULL) {
		memset(result_tmp, 0x00, MAX_LINE);
		memcpy(result_tmp, result_file_name, sizeof(char) * MAX_LINE);
		find_msg(result_tmp);

		//변수이름 저장
		memcpy(file_info.open_file_name[file_info.open_count], result_tmp, sizeof(char) * strlen(result_tmp));
		file_info.open_count++;
		memset(result_tmp,0x00, MAX_LINE);

	}
}


void convert_Filewriter(char *read_line, int class_count)
{
	int i,j;
	char *result = NULL;
	char result_tmp[MAX_LINE] = {};

	for(i = 0; i < file_info.var_count; i++) {
		//Filewriter의 변수이름을 저장한다.
		if(strstr(read_line,file_info.var_name[i]) != NULL) {
			result = strstr(read_line," ");
			memcpy(result_tmp, &result[1], sizeof(char) * strlen(&result[1]));
			for(j = 0; j < (int)strlen(result_tmp); j++) {
				if(isspace(result_tmp[j]) != 0) {
					remove_char(&result_tmp[j]);
					memcpy(file_info.var_j_name[i], result_tmp, sizeof(char) * strlen(result_tmp));
				}

			}//for

			memset(result_tmp, 0x00, sizeof(char) * MAX_LINE);
			memcpy(result_tmp, read_line, sizeof(char) * strlen(read_line));

			remain_space(result_tmp);
			strcat(result_tmp,file_info.var_name[i]);
			strcat(result_tmp, " = fopen(");
			strcat(result_tmp, file_info.open_file_name[i]);
			strcat(result_tmp, ",");
			if(strstr(read_line,"false") != NULL)
				strcat(result_tmp,"\"w+\");");
			else if(strstr(read_line, "true") != NULL)
				strcat(result_tmp,"\"a+\");");

			fputs(result_tmp,f_info[class_count].c_file_fp);
		}//if
		break;
	}//for
}

void convert_write(char *buf, int class_count, int v_count)
{
	char result[MAX_LINE] = {};
	char *msg = NULL;
	char var_tmp[MAX_LINE] = {};
	char *replace = NULL;
	char replace_tmp[MAX_LINE] = {};
	int  i;
	char var[MAX_LINE] ={};

	memcpy(var_tmp, file_info.var_j_name[v_count], sizeof(char) * strlen(file_info.var_j_name[v_count]));
	strcat(var_tmp,".");

	if(flag_p == 1) {
		memcpy(var, file_info.var_j_name[v_count], sizeof(char) * strlen(file_info.var_j_name[v_count]));
	}

	if(strstr(buf,var_tmp) != NULL) {

		if(strstr(buf,"flush") != NULL){
			memcpy(result, buf, sizeof(char) * strlen(buf));
			remain_space(result);
			strcat(result,"fflush(");
			strcat(result, file_info.var_name[v_count]);
			strcat(result, ");\n");
			fputs(result, f_info[class_count].c_file_fp);
			if(flag_p == 1){
				strcat(var,".flush() -> fflush()");
				option_p(var,class_count);
			}

		}
		else if(strstr(buf,"close") != NULL) {
			memcpy(result, buf, sizeof(char) * strlen(buf));
			remain_space(result);
			strcat(result,"fclose(");
			strcat(result, file_info.var_name[v_count]);
			strcat(result, ");\n");
			fputs(result, f_info[class_count].c_file_fp);
			if(flag_p == 1){
				strcat(var,".close() -> fclose()");
				option_p(var,class_count);
			}

		}
		else if(strstr(buf,"write") != NULL) {
			memcpy(result, buf, sizeof(char) * strlen(buf));
			remain_space(result);
			strcat(result, "fputs(");

			msg = strstr(buf, "\"");
			find_msg(msg);
			strcat(result, msg);
			strcat(result, ",");
			strcat(result, file_info.var_name[v_count]);
			strcat(result,");\n");

			fputs(result, f_info[class_count].c_file_fp);
			if(flag_p == 1){
				strcat(var,".write() -> fputs()");
				option_p(var,class_count);
			}
		}
	}
	else {
		var_tmp[(int)(strlen(var_tmp))-1] = 0X00;
		memcpy(result, buf, sizeof(char) * strlen(buf));
		replace = strstr(result,var_tmp);

		for( i = 0; i < (int)strlen(replace); i++) {
			if(isspace(replace[i]) != 0) {
				memcpy(replace_tmp, &replace[i], sizeof(char) * MAX_LINE);
				memset(replace,0x00,sizeof(char) *strlen(replace));
				break;
			}
		}

		strcat(result, file_info.var_name[v_count]);
		strcat(result, replace_tmp);

		convert_null(result);
	}
}
