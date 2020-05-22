#include "ssu_convert.h"

void makefile()
{
	FILE *fp;
	char run_file[MAX_LINE] ={};
	char obj_file[MAX_LINE] = {};
	char *result;
	int i;
	fp = fopen("./_Makefile", "w+");

	memcpy(run_file,j_info.java_file_name,sizeof(char) * strlen(j_info.java_file_name));
	result = strrchr(run_file,'.');
	remove_char(result);
	
/*----------------------------실행 파일 -------------------*/
	fputs(run_file, fp);
	fputs(" : ", fp);

	for(i = 0; i < file_count;i++) {
		memset(obj_file, 0x00, MAX_LINE);
		fputs(f_info[i].file_name,fp);
		fputs(".o ",fp);
	}

	fputs("\n",fp);

	fputs("	gcc -o ",fp);
	fputs(run_file, fp);
	fputs(" ",fp);
	for(i = 0; i< file_count; i++) {
		fputs(f_info[i].file_name,fp);
		fputs(".o ",fp);
	}
	fputs("\n",fp);

	/*---------------------오브젝트 파일 ------------------------*/
	for(i = 0 ; i< file_count; i++) {
		fputs(f_info[i].file_name,fp);
		fputs(".o : ",fp);
		fputs(f_info[i].file_name,fp);
		fputs(".c\n",fp);

		fputs("	gcc -c -Wall -W -o ",fp);
		fputs(f_info[i].file_name,fp);
		fputs(".o ",fp);
		fputs(f_info[i].file_name,fp);
		fputs(".c\n",fp);
	}
}
