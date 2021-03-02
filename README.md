# converter
유닉스 및 리눅스 시스템에서 제공하는 여러함수들을 사용하여 텍스트 파일을 읽어서 문자열을 비교하여 java로 작성된 코드를 c언어로 변환시켜주는 프로그램


## 파일별 함수 설명

### main.c
+ void get_option(int, char**) : 옵션처리해주는 함수 
+ void option_j(FILE *);= :옵션 j를 실행하는 함수
+ void option_c() : 옵션c를 실행하는 함수
+ void option_f(FILE *) :옵션 f를 실행하는 함수
+ void option_p(char*,int) :옵션 p를 실행하는 함수
+ void option_l() :옵션 l을 실행하는 함수
+ void option_error(int, char**): 옵션에러를 처리하는 함수 argv안에 있는 모든 원소를 검사하여 일치하는 옵션이 없을 경우 에러메세지를 출력한다.
+ int skip(char *,int) : 문자를 어떻게 처리할지 판단 및 실행하는 함수이다. 그대로 변환 없이 번역할 것인지 아니면 c언어로 변환할 것인지 반별한다.
+ void close_fp() : 열린 c파일을 마지막에 모두 닫아주는 함수
+ void convert_main(char *, int) : main을 c에 맞게 변환해 주는 함수이다. 
+ void convert_print(char *,int) : printf를 c에 맞게 변환해 주는 함수이다. 형식이 c언어와 겹치지 않을 경우 변수를 추출해서 c언어 형식에 맞게 변형시켜 준다.
+ void convert_scan(char*, int) : scanf를 c언어에 맞게 변환해 주는 함수
+ void convert_return(char*, int) : return을 c에 맞게 변환해 주는 함수이다. main함수에서는 exit를 사용하게 코드를 구현하였고 나머지는 return 그대로 번역한다.
+ void convert_null(char *) : NULL 문자를  c에 맞게 변환해 주는 함수
+ void convert_arry(char *,int) : 배열을 c에 맞게 변환해 주는 함수이다. 
+ void convert_static(char *,int) : static 변수를 매크로로  변환해 주는 함수
+ void convert_init(char *, int) : 자바 생성자를 을 c에 맞게 변환해 주는 함수
+ void convert_init_func(char *,char *, int) : main을 c에 맞게 변환해 주는 함수
+ void convert_alloc(char *,int) : 자바 배열을 c에 맞게 변환해 주는 함수
+ void convert_method(char*, int) : 자바 메서드를  c에 맞게 변환해 주는 함수
+ void count_brace(char *) : 중괄호의 수를 세는함수
+ void make_c_file(FILE*, struct stat) : public class와 class를 구분하여 c파일을 생성해준다.
+ void remove_space(char*) : 문자열의 공백을 모두 지워주는 함수 
+ void remove_space_front(char *) : 문자열의 첫글자 앞의 공백을 모두 지워주는 함수
+ void remain_space(char*) : 문자열의 첫글자 전까지의 공백만 남기고 모두 지워주는 함수
+ void remove_char(char*) : 매개변수의 문자열터 전부 삭제한다.
+ void find_msg(char *)  : 출력할 메세지를 찾아서 저장

### ssu_header.c
+ void save_header(); : 헤더파일을 열어서 저장하는 함수
+ void find_header(char *, int); : c파일에서 어떤 헤더파일인지 찾아주는 함수

### ssu_file.c
+ void convert_file(char *, int); //file 관련함수를 변환해주는 함수
+ void convert_Filewriter(char *, int); // file 변수를 판별해주는 함수
+ void convert_write(char *, int,int);// write를 변환해주는 함수


### ssu_extern.c
+ void save_extern(char *,int) : extern 변수 ,함수를 저장한다. public class와 class를 구분해서 class일 경우 필드위의 변수와 함수를 저장한다.
+ int check_var_func(char *) : 저장할려는 자료가 변수인지 함수인지 체크한다.
+ void insert_extern(int): 저장했던 extern 변수를 헤더파일 밑에 삽입하는 함수이다.

### ssu_makefile.c
+ void makefile() : 메이크 파일을 만들어 주는 함수이다. 


