#include "ssu_convert.h"

void save_extern(char *buf,int var_func)
{

	int i;
	char buf_tmp[MAX_LINE] = {};
	char tmp[MAX_LINE] = {};
	char *remove_var = NULL;
	char *remove_tmp = NULL;

	//var_func 값이 0이면 변수 1이면 함수이다.
	if(var_func == EXT_VAR) {
		for(i = 0; i < (int)strlen(buf); i++){
			if(buf[i] == ';')
				break;
		}
		memcpy(tmp, buf, sizeof(char) * (i+1));
		remove_space_front(tmp);
		memcpy(e_info.var_name[e_info.var_count],tmp, sizeof(char) * strlen(tmp));
		e_info.var_count++;
	}
	else if (var_func == EXT_FUNC) {
		memcpy(buf_tmp, buf, sizeof(char)  * strlen(buf));

		remove_space_front(buf_tmp);

		for(i = 6; i < (int)strlen(buf_tmp); i++) {
			if(buf_tmp[i] == '{')
				break;
		}
		memcpy(tmp, &buf_tmp[6], sizeof(char) * (i-6));
		remove_space_front(tmp);
		if((remove_var = strrchr(tmp,'(')) != NULL) {
			if((remove_tmp = strstr(remove_var,"int")) != NULL) {
				remove_char(&remove_tmp[3]);
				strcat(tmp,")");
			}
		}
		memcpy(e_info.func_name[e_info.func_count],tmp, sizeof(char) * strlen(tmp));
		e_info.func_count++;
	}
	else
		return ;
}


int check_var_func(char *buf)
{

	char buf_tmp[MAX_LINE] = {};
	char *result;
	char result_cmp[MAX_LINE] = {};

	memcpy(buf_tmp, buf, sizeof(char) * strlen(buf));

	remove_space_front(buf_tmp);
	//함수일 경우
	if(strncmp(buf_tmp,"public",sizeof(char) * strlen("public")) == 0) {

		result = strtok(buf_tmp," ");
		result = strtok(NULL," ");
		memcpy(result_cmp,result, sizeof(char) *strlen("void"));
		if(strncmp("void", result_cmp, sizeof(char) * strlen(result_cmp)) == 0) {
			return 1;
		}
		else if(strncmp("int", result_cmp, sizeof(char) * strlen("int")) == 0) {
			return 1;
		}
		else return -1;
	}
	//변수 혹은 아무것도 아닐때
	else {

		remove_space_front(buf_tmp);
		memcpy(result_cmp, buf_tmp, sizeof(char) * 4);
		if(strncmp("int", result_cmp, sizeof(char) *3) == 0) {
			if(result_cmp[3] =='[')
				return -1;
			else
				return 0;
		}
		else 
			return -1;
	}

	return -1;

}
void convert_method(char *buf, int class_count)
{
	char buf_tmp[MAX_LINE] = {};
	char *result;
	memcpy(buf_tmp, buf, sizeof(char) * strlen(buf));
	result = strrchr(buf,'.');

	remain_space(buf_tmp);
	strncat(buf_tmp,&result[1], sizeof(char) * strlen(&result[1]));

	fputs(buf_tmp, f_info[class_count].c_file_fp);
}

void insert_extern(int class_count)
{
	int i;
	char buf_extern[MAX_LINE] = {};

	fputs("\n", f_info[class_count].c_file_fp);

	for(i = 0; i < e_info.var_count; i++)
	{
		memset(buf_extern, 0x00, sizeof(char) * MAX_LINE);
		strcat(buf_extern , "extern ");
		strcat(buf_extern ,e_info.var_name[i]);
		strcat(buf_extern ,"\n");
		fputs(buf_extern, f_info[class_count].c_file_fp);
	}

	for(i = 0;i < e_info.func_count; i++)
	{
		memset(buf_extern, 0x00, sizeof(char) * MAX_LINE);
		strcat(buf_extern , "extern ");
		strcat(buf_extern ,e_info.func_name[i]);
		strcat(buf_extern ,";\n");
		fputs(buf_extern, f_info[class_count].c_file_fp);

	}
}
