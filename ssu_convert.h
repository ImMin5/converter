#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

#include <sys/time.h>
#define SECOND_TO_MICRO 1000000 

#define MAX_FILE 100 //한번에 변환가능한 c파일 수
#define FILE_NAME 256 //파일이름의 길이
#define MAX_LINE 1024 //한번에 읽을 수 있는 최대 문자수
#define HEADER_MAX 1000 // header의 저장가능한 최대 함수명 수
#define FUNC_MAX 200 // extern 할 함수 프로토타입의 수
#define FUNC_NAME 128 //이름의 최대 글자수
#define VAR_MAX 100//파일하나의 최대 변수의 수
#define VAR_NAME 128//변수의 최대 글자수 
#define HEADER "header"

#define EXT_VAR 0 //extern할 변수
#define EXT_FUNC 1 //extern할 함수
//option 처리 변수
int flag_j;
int flag_c;
int flag_p;
int flag_f;
int flag_l;
int flag_r;

int exit_flag; //0 이면 exit 1이면 return
int main_flag; //
int class_flag; //0이면 public class 1:class
int init_flag; //0이면 생성자 변환x 1이면 생성자 변환0
int var_flag; //변수를 읽었을 때
int file_count; //생성된 c파일의 수
int brace_open;
int brace_close;

int j_line_count;
int c_line;
int use_count;
int c_use[MAX_LINE];
char parent_j_read[MAX_LINE][MAX_LINE];
char parent_c_read[MAX_LINE][MAX_LINE];

//main.c
void get_option(int, char**);//옵션처리해주는 함수 
void option_j(FILE *);//옵션 j를 실행하는 함수
void option_c(); //옵션c를 실행하는 함수
void option_f(FILE *); //옵션 f를 실행하는 함수
void option_p(char*,int); //옵션 p를 실행하는 함수
void option_l(); //옵션 l을 실행하는 함수
void option_error(int, char**); //옵션에러를 처리하는 함수
int skip(char *,int); //문자를 어떻게 처리할지 판단 및 실행하는 함수
void close_fp(); // 열린 c파일을 마지막에 모두 닫아주는 함수
void convert_main(char *, int); //main을 c에 맞게 변환해 주는 함수
void convert_print(char *,int);//printf를 c에 맞게 변환해 주는 함수
void convert_scan(char*, int);//scanf를 c에 맞게 변환해 주는 함수
void convert_return(char*, int);//return을 c에 맞게 변환해 주는 함수
void convert_null(char *);//null문자를  c에 맞게 변환해 주는 함수
void convert_arry(char *,int);//배열을 c에 맞게 변환해 주는 함수
void convert_static(char *,int);//static 변수를 매크로로  변환해 주는 함수
void convert_init(char *, int);//자바 생성자를 을 c에 맞게 변환해 주는 함수
void convert_init_func(char *,char *, int);//main을 c에 맞게 변환해 주는 함수
void convert_alloc(char *,int);//자바 배열을 c에 맞게 변환해 주는 함수
void convert_method(char*, int);//자바 메서드를  c에 맞게 변환해 주는 함수
void count_brace(char *);//중괄호의 수를 세는함수
void make_c_file(FILE*, struct stat); // class별로 구분하여 c파일을 생성해준다.
void remove_space(char*); //문자열의 공백을 모두 지워주는 함수 
void remove_space_front(char *);// 문자열의 첫글자 앞의 공백을 모두 지워주는 함수
void remain_space(char*);//문자열의 첫글자 전까지의 공백만 남기고 모두 지워주는 함수
void remove_char(char*);//매개변수의 문자열터 전부 삭제한다.
void find_msg(char *); //출력할 메세지를 찾아서 저장

//ssu_header.c
void save_header();//헤더파일을 읽어서 저장하는 함수
void find_header(char *, int); //c파일에서 어떤 헤더파일인지 찾아주는 함수

//ssu_file.c
void convert_file(char *, int); //file 관련함수를 변환해주는 함수
void convert_Filewriter(char *, int); // file 변수를 판별해주는 함수
void convert_write(char *, int,int);// write를 변환해주는 함수


//ssu_extern.c
void save_extern(char *,int); //extern 변수 ,함수를 저장한다.
int check_var_func(char *);// 변수인지 함수인지 체크한다.
void insert_extern(int); //extern 변수를 삽입한다.

//ssu_makefile.c
void makefile();
//파일의 정보를 저장하기위한 구조체
struct file_info 
{
	char file_name[FILE_NAME]; 
	char c_file_name[FILE_NAME];
	FILE *c_file_fp;
	int _open;
	int c_size;
	int line_count;
};
//header파일의 정보와 추가할 헤더를 저장하기 위한 구조체
struct Header_info 
{
	FILE *fp_header; //header 파일의 파일포인터저장
	char header[HEADER_MAX][FUNC_NAME]; //header파일을 읽어서 함수명을 저장한다.
	int use[MAX_FILE][HEADER_MAX]; //헤더파일의 사용 유무를 저장한다.
	int count;
};

struct Java_info
{
	char java_file_name[FILE_NAME];
	int line_count;
};

struct FILE_info
{
	char var_name[VAR_MAX][VAR_NAME];
	char var_j_name[VAR_MAX][VAR_NAME];
	char open_file_name[VAR_MAX][VAR_NAME];

	int var_count;
	int open_count;
	int line_count;
};

struct Extern_info
{
	char var_name[VAR_MAX][VAR_NAME];
	char func_name[FUNC_MAX][FUNC_NAME];
	int func_use[FUNC_MAX];
	int var_count;
	int func_count;

	char c_var_name[VAR_NAME];
};
//옵션p사용시 정보를 저장
struct Print_info
{
	char trans[FUNC_MAX][MAX_LINE];
	int use[FUNC_MAX];
	int count;
};

struct Header_info header_info;
struct file_info f_info[MAX_FILE];
struct Java_info j_info;
struct FILE_info file_info;
struct Extern_info e_info;
struct Print_info p_info[MAX_FILE];
