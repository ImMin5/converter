#include "ssu_convert.h"


void ssu_runtime(struct timeval* begin_t, struct timeval* end_t) {
	end_t -> tv_sec -= begin_t -> tv_sec;

	if (end_t -> tv_usec < begin_t ->tv_usec) {
		end_t -> tv_usec--;
		end_t -> tv_usec += SECOND_TO_MICRO;
	}

	end_t -> tv_usec -= begin_t -> tv_usec;
	printf("Runtime: %ld:%06ld(sec:usec)\n", end_t -> tv_sec, end_t -> tv_usec);
}

int main(int argc, char *argv[])
{
	int skip_line;
	int i;
	int pid = -1;
	int status;
	int class_count = 0;
	FILE *fp = NULL;
	struct stat statbuf;
	char read_line[MAX_LINE] = {};
	char read_tmp[MAX_LINE] = {};

	struct timeval begin_t, end_t;

	size_t java_size = 0;
	int c_use[MAX_LINE]; //해당 파일이 

	gettimeofday(&begin_t, NULL);
	//인자를 잘못 입력했을 때
	if(argc < 2) {
		fprintf(stderr,"Usage : <filename> <filename.java> <option>\n");
		exit(1);
	}

	memcpy(j_info.java_file_name, argv[1], sizeof(char) * FILE_NAME);
	option_error(argc,argv);
	get_option(argc,argv); //옵셔 처리 함수 기능 실행은 main함수 끝에서 이루어 진다.

	//코드 변환할 자바파일 오픈
	if((fp = fopen(j_info.java_file_name, "r")) == NULL) {
		fprintf(stderr,"fopen error for %s\n",j_info.java_file_name);
		exit(1);
	}

	//개발자가 설정한 헤더파일 오픈
	if((header_info.fp_header = fopen(HEADER, "r")) == NULL) {
		fprintf(stderr, "header file doesn't exist!\n");
		exit(1);
	}

	//파일의 정보를 불러온다.
	if(stat(j_info.java_file_name, &statbuf)  < 0) {
		fprintf(stderr,"stat error\n");
		exit(1);
	}


	//Headr_info 구조체에 정보를 저장한다.
	save_header();

	//class별로 파일을 만들어준다.
	make_c_file(fp, statbuf);

	//첫번째 파일 오픈
	f_info[class_count].c_file_fp = fopen(f_info[class_count].c_file_name, "w+");

	//public class가 아닐때
	if(strstr(read_line,"public class") == NULL) {
		class_flag = 1;
	}
	else
		class_flag = 0;

	if(flag_r == 1) {
		fseek(fp,0,SEEK_END);
		java_size = ftell(fp);
		fseek(fp,0,SEEK_SET);
	}
	/* ----------------fork--------------------------------*/
	while(1) {
		fgets(read_line, sizeof(char) * MAX_LINE ,fp);

		if(feof(fp)) {
			//이전파일에 헤더추가
			find_header(f_info[class_count].c_file_name, class_count);
			break;
		}
		memcpy(parent_j_read[j_info.line_count], read_line, sizeof(char) * strlen(read_line));
		j_info.line_count++; //자바 파일 라인수를 구한다.
		//extern 함수, 변수 저장
		if(class_flag == 1) save_extern(read_line, check_var_func(read_line));

		skip_line = skip(read_line,class_count);
		//0 : skip 
		//1 : 변환
		//2 : class
		//else : 그대로 출력
		if(skip_line == 0)
			continue;
		else if( skip_line == 1) {
			//변환
		}
		//class 파일을 발견했을 때
		else if( skip_line == 2) {
			if(strstr(read_line,f_info[class_count].file_name) == NULL) {
				//이전파일에 헤더추가
				find_header(f_info[class_count].c_file_name, class_count);
				class_count+=1;
				brace_open = 0;
				brace_close = 0;
				init_flag = 0;

				if((f_info[class_count].c_file_fp = fopen(f_info[class_count].c_file_name, "w")) == NULL) {
					fprintf(stderr,"fopen error for %s\n",f_info[class_count].c_file_name);
					exit(1);
				}
				//public class가 아닐때
				if(strstr(read_line,"public class") == NULL) {
					class_flag = 1;
				}
				else {
					class_flag = 0;
				}

			}
		}
		else{
			count_brace(read_line);

			//public을 삭제한다.
			if(strstr(read_line, "public") != NULL) {
				memset(read_tmp,0x00, sizeof(char) * MAX_LINE);
				memcpy(read_tmp, read_line, sizeof(char) * strlen(read_line));
				remove_space_front(read_tmp);
				remain_space(read_line);
				strcat(read_line, &read_tmp[7]);
			}

			//생성자를 초기화 함수로 바꾸는 과정
			if(class_flag == 1 && init_flag == 0) {
				convert_init(read_line,class_count);
			}

			if(brace_open > brace_close){
				fputs(read_line,f_info[class_count].c_file_fp);
				if(flag_r == 1) {
					memcpy(parent_c_read[c_line], read_line,sizeof(char) * strlen(read_line));
					c_use[c_line] = j_line_count;
					printf("parent_c_read[%d] : %s\n",c_line,parent_c_read[c_line]);
					printf("c_use[%d] : %d\n",c_line,c_use[c_line]);
					c_line++;
				}
			}
			else if(brace_open == 0 && brace_close == 0) {
				fputs(read_line,f_info[class_count].c_file_fp);
			}
			else if((brace_open == brace_close) && (brace_open > 0)) {

				if(main_flag == 1)remove_space(read_line);

				fputs(read_line,f_info[class_count].c_file_fp);
			}
		}

		//			sleep(1);
		c_line++;
	}//while
	/*
	 */

	int tmp_class = 0;
	int c = 0;
	if(flag_r ==1) {
		if((pid = fork()) < 0){
			fprintf(stderr,"fork error\n");
			exit(1);
		}
		else if(pid == 0) {
			int j = 0;
			system("clear");
			for(i = 0; i < j_info.line_count; i++) {
				printf("-----------------------------------\n");
				printf("%s Converting...\n",j_info.java_file_name);
				printf("-----------------------------------\n");
				while(j <i){
					printf("%d %s",j+1,parent_j_read[j]);
					j++;
				}
				sleep(1);

				printf("--------------------------------\n");
				printf("%s Converting...\n",f_info[tmp_class].c_file_name);
				printf("--------------------------------\n");
				while(c < i) {
					if(c_use[c] >= i) break;
					printf("%d %s",c+1,parent_c_read[c]);
					c++;
				}
				c =0;
				j =0;
				sleep(3);
				system("clear");
			}
		}
		else {
			wait(&status);
		}
	}
	if(pid != 0) {
		for( i = 0; i < file_count; i++) {
			printf("%s convert Success!\n",f_info[i].c_file_name);
			if(flag_p == 1) {
				for(int j = 0; j < p_info[i].count; j++) {
					printf("%s\n",p_info[i].trans[j]);
				}
			}
		}

		if(flag_j == 1) option_j(fp);
		if(flag_c == 1) option_c();
		if(flag_f == 1) option_f(fp);
		if(flag_l == 1) option_l();

		gettimeofday(&end_t, NULL);
		ssu_runtime(&begin_t, &end_t);

		fclose(header_info.fp_header);
		close_fp();
		makefile();
		exit(0);
	}
}
//옵션을 판별해 주는 함수
void get_option(int argc , char**argv)
{
	int c;// 옵션을 처리할 변수
	opterr = 0;

	while((c = getopt(argc, argv,"jcpflr")) != -1) {
		switch(c) {
			case 'j' :
				flag_j = 1;
				break;
			case 'c' :
				flag_c = 1;
				break;
			case 'p' :
				flag_p = 1;
				break;
			case 'f' :
				flag_f = 1;
				break;
			case 'l' :
				flag_l = 1;
				break;
			case 'r' :
				flag_r = 1;
				break;
			case '?' :
				option_error(argc,argv);
				exit(1);
		}
	}
}
//j옵션 실행 함수
void option_j(FILE *fp)
{
	int i = 1;
	char read_line[MAX_LINE] = {};
	fseek(fp, 0, SEEK_SET);

	while(1) {

		fgets(read_line, sizeof(char) * MAX_LINE ,fp);
		if(feof(fp)) 
			break;
		printf("%d ",i);
		printf("%s",read_line);
		i++;
	}
}
//c옵션 실행 함수
void option_c()
{
	int i,j;
	char read_line[MAX_LINE] = {};
	for(i = 0; i < file_count; i++) {
		j = 1;
		fseek(f_info[i].c_file_fp, 0, SEEK_SET);
		while(1) {
			if(feof(f_info[i].c_file_fp))break;
			fgets(read_line, sizeof(char) * MAX_LINE, f_info[i].c_file_fp);
			printf("%d ",j);
			printf("%s",read_line);
			j++;
		}
			printf("\n");
	}
	printf("\n");
}
//f옵션 실행 함수
void option_f(FILE *fp) 
{
	size_t fsize;
	int i;

	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	printf("%s file size is %d bytes\n",j_info.java_file_name, fsize);
	for(i = 0;i < file_count; i ++) {
		fseek(f_info[i].c_file_fp, 0, SEEK_END);
		fsize = ftell(f_info[i].c_file_fp);
		printf("%s file size is %d bytes\n",f_info[i].c_file_name, fsize);
	}
}
//p옵션 실행함수
void option_p(char *buf, int class_count)
{
	int i;
	for(i = 0; i < p_info[class_count].count; i++) {
		if(strncmp(buf,p_info[class_count].trans[i], sizeof(char) *strlen(buf)) == 0)
			return ;
	}
	memcpy(p_info[class_count].trans[i], buf, sizeof(char) * strlen(buf));
	p_info[class_count].count++;
}

//l옵션 실행함수
void option_l()
{
	char read_line[MAX_LINE] = {};
	printf("%s line number %d lines\n",j_info.java_file_name, j_info.line_count);
	for(int i = 0; i < file_count; i++) {
		fseek(f_info[i].c_file_fp, 0, SEEK_SET);
		while(1) {

			if(feof(f_info[i].c_file_fp))
				break;
			fgets(read_line, sizeof(char) * MAX_LINE,f_info[i].c_file_fp);
			f_info[i].line_count++;
		}
		printf("%s line number %d lines\n",f_info[i].c_file_name,f_info[i].line_count);
	}
}
//존재하지 않는 에러를 처리하는 함수
void option_error(int argc, char **argv)
{
	int i;
	for (i = 2; i < argc; i++) {
		if(strcmp("-j",argv[i]) == 0) continue;
		else if(strcmp("-c",argv[i]) == 0) continue;
		else if(strcmp("-p",argv[i]) == 0) continue;
		else if(strcmp("-f",argv[i]) == 0) continue;
		else if(strcmp("-l",argv[i]) == 0) continue;
		else if(strcmp("-r",argv[i]) == 0) continue;
		else{

			fprintf(stderr,"Unknown option %s\n", argv[i]);
			exit(1);
		}
	}
}
//읽은 문자열을 어떻게 처리 할지 판결해주는 함수
int skip(char *buf,int class_count)
{
	int i;
	char buf_tmp[MAX_LINE] = {};
	char init[MAX_LINE] = {};

	if(strstr(buf,"import") != NULL)
		return 0;
	else if(strstr(buf,"class") != NULL) {
		return 2;
	}
	else if(strstr(buf, "Scanner") != NULL)
		return 0;
	else if(strstr(buf, "System.out.printf") != NULL) {
		convert_print(buf,class_count);
		if(flag_p == 1) option_p("System.out.printf() -> printf()",class_count);
		return 1;
	}
	else if(strstr(buf, "nextInt()") != NULL){
		convert_scan(buf, class_count);
		return 1;
	}
	else if(strstr(buf, "public static void main") != NULL) {
		main_flag = 1;
		convert_main(buf, class_count);
		return 1;
	}
	else if(strstr(buf, "return") != NULL) {
		convert_return(buf, class_count);
		return 1;
	}
	else if(strstr(buf, "int[]") != NULL) {
		convert_arry(buf, class_count);
		return 1;
	}
	else if(strstr(buf, "public static final") != NULL) {
		convert_static(buf, class_count);
		return 1;
	}
	else if(strstr(buf, "File") != NULL) {
		if(strstr(buf,"FileWriter") != NULL) {
			convert_Filewriter(buf, class_count);
		}
		else
			convert_file(buf, class_count);
		return 1;
	}
	//생성자를 초기화 함수로 바꿔주는 과정
	else if(init_flag == 0 && strstr(buf, f_info[class_count].file_name) != NULL)
	{
		count_brace(buf);
		convert_init(buf,class_count);
		return 1;
	}
	//자바 배열을 씨언어 배열로 바꿔주는 과정
	else if(strstr(buf, "new int") != NULL)
	{
		convert_alloc(buf, class_count);
		return 1;
	}
	else{

		memcpy(buf_tmp, buf, sizeof(char) * strlen(buf));
		//File 관련 메소드 변환
		for(i = 0; i < file_info.var_count; i++) {
			if((int)strlen(file_info.var_j_name[i]) != 0 && strstr(buf,file_info.var_j_name[i]) != NULL) {
				convert_write(buf_tmp, class_count, i);
				return 1;
			}
		}

		memcpy(buf_tmp, buf, sizeof(char) * strlen(buf));
		remove_space_front(buf_tmp);
		//클래스 변수가 선언되었는지 확인
		for(i = 0; i < file_count; i++) {
			if(strncmp(buf_tmp, f_info[i].file_name,sizeof(char) * strlen(f_info[i].file_name)) == 0) {
				memset(init, 0x00, sizeof(char)  * MAX_LINE);
				strcat(init,"new ");
				strcat(init,f_info[i].file_name);
				strcat(init,"()");
				//java 파일에서 생성자를 사용 했을 경우
				if(strstr(buf_tmp, init) != NULL) {
					convert_init_func(buf,init,class_count);
				}
				return 1;
			}
		}

		//메서드를 함수로 바꿔주는 과정
		memcpy(buf_tmp, buf, sizeof(char) * strlen(buf));
		if(class_flag == 0 && var_flag == 1 && strstr(buf,e_info.c_var_name) != NULL) {
			convert_method(buf,class_count);
			return 1;
		}
		return 3;
	}

}
//c파일을 생성해 주는 함수
void make_c_file(FILE *fp, struct stat statbuf) {
	char *result = NULL;
	char result_tmp[FILE_NAME] = {};
	char *buf = (char*)malloc(sizeof(char) * statbuf.st_size);
	int i;
	int flag_space = 0;


	if(fread(buf, statbuf.st_size,1, fp) != 1) {
		fprintf(stderr,"fread error in make_c_file\n");
	}

	result = strstr(buf,"class");

	while(result != NULL) {
		memset(result_tmp, 0x00, sizeof(char) * FILE_NAME);
		memcpy(result_tmp, &result[6], sizeof(char) * FILE_NAME);

		for(i = 0; i < (int)strlen(result_tmp); i++) {
			if(((flag_space == 0) && isspace(result_tmp[i] != 0)) || (result_tmp[i] == '{')) {
				result_tmp[i] = 0x00;
				flag_space =1;
			}
			if(flag_space == 1)
				result_tmp[i] = 0x00;
		}

		memcpy(f_info[file_count].file_name, result_tmp, sizeof(char) * strlen(result_tmp));
		strcat(result_tmp,".c");
		memcpy(f_info[file_count].c_file_name, result_tmp, sizeof(char) * strlen(result_tmp));
		file_count++;

		result = strstr(result +1,"class");
		flag_space = 0;
	}
	fseek(fp, 0, SEEK_SET);

	free(buf);//buf할당 해제
}
//자바의 메인함수를 c언어의 메인함수로 바꿔주는 함수
void convert_main(char *buf,int class_count)
{
	char _main[MAX_LINE] = "public static void main";
	if(strstr(buf,_main)!= NULL) {
		fputs("int main(void){", f_info[class_count].c_file_fp);
		brace_open++;

		if(flag_r == 1) {
			memcpy(parent_c_read[use_count], "int main(void){\n", sizeof(char) * strlen("int main(void){\n"));
			c_use[use_count] = j_line_count;
			printf("parent_c_read[%d] : %s\n", use_count, parent_c_read[use_count]);
			printf("c_use[%d] : %d\n",use_count,j_line_count);
			use_count++;
		}
	}

}
//scan함수를 바꿔주는 함수
void convert_scan(char *buf, int class_count)
{

	char buf_tmp[FILE_NAME] = {};
	char *result_name = NULL;
	char result_tmp[FILE_NAME] = {};
	int i,k;

	char var[MAX_LINE] = {};
	char *var_tmp;

	memcpy(buf_tmp, buf, sizeof(char) * strlen(buf));
	result_name = strtok(buf_tmp,"=");

	for(i = 0; i< (int)strlen(result_name) ; i++) {
		if(isspace(result_name[i]) == 0) {
			memcpy(result_name,&result_name[i],(int)strlen(result_name));
			for(k = i+1; k <(int)strlen(result_name); k++) {
				if(isspace(result_name[k]) != 0)
					result_name[k] = 0x00;
			}
			break;
		}
	}


	for(i = 0; i< (int)strlen(buf) ; i++) {
		if(isspace(buf[i]) != 0) {
			result_tmp[i] = buf[i];
		}
		else
			break;
	}
	strcat(result_tmp, "scanf(\"%d\", &");
	strcat(result_tmp, result_name);
	strcat(result_tmp, ");\n");
	fputs(result_tmp, f_info[class_count].c_file_fp);

	count_brace(result_tmp);

		if(flag_r == 1) {
			memcpy(parent_c_read[use_count], result_tmp, sizeof(char) * strlen(result_tmp));
			c_use[use_count] = j_line_count;
			printf("parent_c_read[%d] : %s\n", use_count, parent_c_read[use_count]);
			printf("c_use[%d] : %d\n",use_count,j_line_count);
			use_count++;
		}
	if(flag_p == 1) {
		memcpy(buf_tmp, buf, sizeof(char) * strlen(buf));
		var_tmp = strstr(buf_tmp,"=");
		memcpy(var, &var_tmp[1], sizeof(char) * strlen(&var_tmp[1]));
		remove_space_front(var);
		for(i =0 ; i < (int)strlen(var) ; i++) {
			if(var[i] ==';'){
				var[i] = 0x00;
				break;
			}
		}
		strcat(var, " -> scanf()");
		option_p(var, class_count);
	}
}
//printf를 변환해 주는 함수
void convert_print(char *buf, int class_count)
{
	char *result;
	char result_tmp[FILE_NAME] = {};
	char var[MAX_LINE] = {};
	char *var_tmp;
	char var_tmp_tmp[MAX_LINE] = {};
	int i = 0;
	char div_tmp[MAX_LINE] = {};
	char msg[MAX_LINE] = {};
	char *msg_tmp;
	char buf_tmp[MAX_LINE] = {};

	for(i = 0; i< (int)strlen(buf) ; i++) {
		if(isspace(buf[i]) != 0) {
			result_tmp[i] = buf[i];
		}
		else
			break;
	}

	//printf 문 안에서 메소드만 남기기
	if((result = strstr(buf,"printf")) != NULL) {
		strncat(result_tmp,&result[0],sizeof(char) *  strlen(&result[0]));
		if(e_info.c_var_name != NULL && (int)strlen(e_info.c_var_name) > 0) {
			strncat(var, e_info.c_var_name, sizeof(char) * strlen(e_info.c_var_name));
			strcat(var,".");
			var_tmp = strstr(result_tmp,var);
			memcpy(var_tmp_tmp, &var_tmp[(int)strlen(var)], sizeof(char) * (strlen(var_tmp) - strlen(var)));
			remove_char(var_tmp);
			strcat(var_tmp, var_tmp_tmp);//결국엔 result_tmp값이 변경됨
		}
	}
	//printf(변수 + ""); 형식을 변환해주는 과정
	if((result = strstr(result_tmp,"(")) != NULL) {
		memcpy(div_tmp,result, sizeof(char)  * strlen(result));
		remove_space(div_tmp);
		if(div_tmp[1] !='\"'){
			msg_tmp = strstr(div_tmp,"\"");
			memcpy(msg, &msg_tmp[1], sizeof(char) * (strlen(msg_tmp)-1));
			for(i =0; i < (int)strlen(msg); i++) {
				if(msg[i] == '\"') {
					remove_char(&msg[i]);
					break;
				}
			}

			for(i = 0; i< (int)strlen(div_tmp); i++) {
				if(div_tmp[i] == '+'){
					memcpy(var, &div_tmp[1], sizeof(char) * (i-1));
					break;
				}
			}

			memcpy(buf_tmp,buf, sizeof(char) * strlen(buf));
			remain_space(buf_tmp);
			strcat(buf_tmp,"printf(\"%d ");
			strcat(buf_tmp, msg);
			strcat(buf_tmp, "\", ");
			strcat(buf_tmp, var);
			strcat(buf_tmp, ");\n");
			memset(result_tmp, 0x00, MAX_LINE);
			memcpy(result_tmp, buf_tmp, sizeof(char)  * strlen(buf_tmp));
		}//if
	}//if

	fputs(result_tmp, f_info[class_count].c_file_fp);
	count_brace(result_tmp);

				if(flag_r == 1) {
					memcpy(parent_c_read[c_line], result_tmp,sizeof(char) * strlen(result_tmp));
					c_use[c_line] = j_line_count;
					printf("parent_c_read[%d] : %s\n",c_line,parent_c_read[c_line]);
					printf("c_use[%d] : %d\n",c_line,c_use[c_line]);
					c_line++;
				}
}
//return을 변환해 주는 함수
void convert_return(char *buf, int class_count)
{

	if(exit_flag == 0 && brace_open-1 == brace_close) {
		remain_space(buf);
		strcat(buf, "exit(0);\n");
		fputs(buf, f_info[class_count].c_file_fp);
	}
	else if(exit_flag == 1) {
		fputs(buf, f_info[class_count].c_file_fp);
	}
		if(flag_r == 1) {
			memcpy(parent_c_read[use_count], buf,sizeof(char) * strlen(buf));
			c_use[use_count] = j_line_count;
			printf("parent_c_read[%d] : %s\n", use_count, parent_c_read[use_count]);
			printf("c_use[%d] : %d\n",use_count,j_line_count);
			use_count++;
		}
}
//null -> NULL로 바꿔주는 함수
void convert_null(char * buf)
{
	char *result;
	if((result = strstr(buf,"null")) != NULL)
	{
		result[0] = 'N';
		result[1] = 'U';
		result[2] = 'L';
		result[3] = 'L';
	}
}

//자바의 배열을 포인터로 바꿔주는 함수
void convert_arry(char *buf, int class_count)
{
	char *result;
	char result_tmp[MAX_LINE] = {};

	result = strtok(buf," ");
	result = strtok(NULL, " ");
	memcpy(result_tmp, buf, sizeof(char) *strlen(buf));
	remain_space(result_tmp);
	strcat(result_tmp, "int *");
	strcat(result_tmp, result);

	//extern 할 함수 또는 변수를 구조체에 저장한다.
	if(class_flag == 1) save_extern(result_tmp,check_var_func(result_tmp));

	fputs(result_tmp, f_info[class_count].c_file_fp);
}

//static 을 #define으로 바꿔주는 함수
void convert_static(char *buf, int class_count)
{
	int i;
	char *result;
	char result_tmp[MAX_LINE] = {};
	char var[MAX_LINE] ={};
	char val[MAX_LINE] = {};
	memcpy(result_tmp, buf, sizeof(char) * strlen(buf)); //원래 내용 복사
	remain_space(result_tmp);//앞에 공백만 남기고 뒤에는 초기화

	result = strstr(buf, "int");

	for(i = 4 ; i < (int)strlen(result) ;i++) {
		if(result[i] == '=')
			break;
	}
	memcpy(var, &result[4], sizeof(char) * (i-4));

	result = strstr(buf, "=");
	for(i = 2 ; i < (int)strlen(result) ; i++) {
		if(result[i] ==';')
			break;
	}
	memcpy(val, &result[2], sizeof(char) * (i-2));
	strcat(result_tmp, "#define ");
	strcat(result_tmp, var);
	strcat(result_tmp, " ");
	strcat(result_tmp, val);
	strcat(result_tmp, "\n");

	fputs(result_tmp, f_info[class_count].c_file_fp);
}

//자바의 생성자를 초기화 함수로 바꿔주는 함수
void convert_init(char *buf,int class_count)
{

	char read_init[MAX_LINE] = {};
	char convert_init[MAX_LINE] = {};
	int i;

	strcat(read_init, "public ");
	strcat(read_init, f_info[class_count].file_name);
	for(i = 0 ; i < (int)strlen(buf); i++) {
		if(isspace(buf[i]) == 0)
			break;
	}

	if(strncmp(read_init, &buf[i], sizeof(char) * strlen(read_init)) == 0) {
		memset(read_init, 0x00, sizeof(char) * MAX_LINE);
		memcpy(read_init, buf, sizeof(char) * strlen(buf));
		remain_space(read_init);
		strcat(read_init, "void ");
		strcat(read_init, strstr(buf, f_info[class_count].file_name));
		fputs(read_init, f_info[class_count].c_file_fp);
		strcat(convert_init, "public ");
		strcat(convert_init, "void ");
		strcat(convert_init, strstr(buf, f_info[class_count].file_name));
		save_extern(convert_init,EXT_FUNC);
		init_flag = 1;
		}
}

//java 파일에서 메인함수에서 인스턴스 생성시 c언의 초기화 함수로 바꿔주는 함수
void convert_init_func(char *buf, char *init, int class_count)
{
	char *result;
	char buf_tmp[MAX_LINE] = {};
	char buf_var[MAX_LINE] = {};

	result = strrchr(init, ' ');
	memcpy(buf_tmp, buf, sizeof(char) * strlen(buf));
	remain_space(buf_tmp);
	strcat(buf_tmp, &result[1]);
	strcat(buf_tmp, ";\n");

	memcpy(buf_var, buf, sizeof(char) * strlen(buf));
	result = strtok(buf_var," ");
	result = strtok(NULL," ");
	memcpy(e_info.c_var_name,result, sizeof(char) * strlen(result)); //해당 클래스로 변수로 선언된  변수 명을 저장
	var_flag = 1;
	fputs(buf_tmp, f_info[class_count].c_file_fp);
}
//포인터 변수를 동적할당 시켜주는 함수
void convert_alloc(char *buf, int class_count)
{
	char alloc_size[VAR_NAME] = {};
	char buf_tmp[MAX_LINE] = {};
	char *result;
	int i;

	result = strstr(buf, "[");
	for( i=1 ; i<(int)strlen(result)-1; i++) {
		if(result[i] == ']')
			break;
	}
	memcpy(alloc_size, &result[1], sizeof(char) * (i-1));

	memcpy(buf_tmp, buf, sizeof(char) * strlen(buf));
	result = strstr(buf_tmp ,"=");
	remove_char(&result[1]);
	strcat(buf_tmp, " calloc(");
	strcat(buf_tmp, alloc_size);
	strcat(buf_tmp, ", sizeof(int));\n");
	fputs(buf_tmp, f_info[class_count].c_file_fp);
}
//열려있는 c파일을 닫아준다.
void close_fp()
{
	int i;
	for(i = 0; i < MAX_FILE; i++) {
		if(f_info[i]._open == 1)
			fclose(f_info[i].c_file_fp);
	}
}
//중괄호 수를 세어준다.
void count_brace(char *buf)
{
	if(strstr(buf,"{") != NULL) {
		brace_open++;
		exit_flag++;
	}
	if(strstr(buf,"}") != NULL) {
		brace_close++;
		exit_flag --;
	}
}

//모든 공백을 제거하고 끝에 개행을 붙여준다.
void remove_space(char *buf) 
{
	int i,j = 0;
	char buf_tmp[FILE_NAME] = {};

	for(i = 0; i < (int)strlen(buf); i++) {
		if(isspace(buf[i]) == 0) {
			buf_tmp[j] = buf[i];		
			j++;
		}
	}
	strcat(buf_tmp,"\n");
	memcpy(buf, buf_tmp, sizeof(char) * FILE_NAME);
}
//매개변수로 들어온 문자열의 첫 공백아닌 문자를 찾아서 그 앞에 공백을 다 지워준다.
void remove_space_front(char *buf)
{
	int i;
	char buf_tmp[MAX_LINE] = {};
	for(i = 0; i < (int)strlen(buf); i++) {
		if(isspace(buf[i]) == 0)
			break;
	}
	memcpy(buf_tmp, &buf[i], sizeof(char) * strlen(&buf[i]));
	memset(buf,0x00, sizeof(char) * MAX_LINE);
	memcpy(buf, buf_tmp, sizeof(char) * strlen(buf_tmp));

}
//매개변수로 들어온 문자열의 첫번째 문자 이전의 공백을 제외한 나머지를 다 지운다.
void remain_space(char *buf)
{
	char result_tmp[MAX_LINE] = {};
	int i;
	for(i = 0; i< (int)strlen(buf) ; i++) {
		if(isspace(buf[i]) != 0) {
			result_tmp[i] = buf[i];
		}
		else
			break;
	}

	memcpy(buf, result_tmp, sizeof(char) * MAX_LINE);
}
//매개변수로 들어온 문자열의 첫문자이후의 문자를 다 지운다.
void remove_char(char *buf)
{
	int i;

	for(i = 0; i < (int)strlen(buf); i++) {
		buf[i] = 0x00;
	}
}

//"" 사이의 문자열을 구한다.
void find_msg(char *msg)
{
	char *result;
	int i;

	if((result = strstr(msg,"\"")) != NULL) {

		for(i = 1;i < (int)strlen(result)-1; i++) {
			if(result[i] == '\"') {
				if(i+1 <= (int)strlen(result) -1)
					remove_char(&result[i+1]);
			}
		}
	}
}
