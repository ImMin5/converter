#include "ssu_convert.h"


void save_header()
{
	char read_header[MAX_LINE] = {};
	char *result = NULL;

	//파일의 크기를 구한다.
	if(fseek(header_info.fp_header, 0,SEEK_END) != 0) {
		fprintf(stderr,"fseek error in save_header()\n");
		exit(1);
	}
	else {
		fseek(header_info.fp_header,0, SEEK_SET);
	}


	//헤더파일을 한줄 씩 읽는다.
	while(1) {
		fgets(read_header, sizeof(char) * MAX_LINE, header_info.fp_header);

		if(feof(header_info.fp_header))
			break;

		result = strtok(read_header," ");

		if(result != NULL){
			//헤더파일을 Header_info 구조체 변수 header_info에 저장한다.
			memcpy(header_info.header[header_info.count], result, sizeof(char)  * strlen(result));
			header_info.count++; //읽은 헤더파일의 총 수를 저장
		}
		result = NULL;

	}
}
void find_header(char *c_filename, int class_count)
{
	char *buf = NULL;
	char buf_header[MAX_LINE] = {};
	char *result = NULL;
	char *result_tmp = NULL;
	char *result_tok = NULL;
	char save_include[HEADER_MAX][FILE_NAME] = {};//삽입할 헤더를 저장하는 변수
	int save_count = 0;
	int fd, i ,j;
	size_t fsize;
	FILE *fp;

	char *copy_c = NULL; //헤더를 삽입하기전 내용을 복사해서 저장할 변수

	//
	if((fd = open(c_filename, O_RDWR)) < 0) {
		fprintf(stderr,"open error for %s\n",c_filename);
		exit(1);
	}

	//파일별 사용 헤더 사용 유무 초기화
	for(i = 0; i < MAX_FILE; i++)
		header_info.use[class_count][i] = 0;

	fseek(f_info[class_count].c_file_fp,0, SEEK_END);
	fsize = ftell(f_info[class_count].c_file_fp); //c파일의 파일 크기 저장
	buf = (char *)malloc(sizeof(char) * fsize);
	memset(buf, 0x00, sizeof(char) * fsize);

	fseek(f_info[class_count].c_file_fp,0,SEEK_SET);
	
	if(read(fd,buf, sizeof(char) * (fsize-1)) < 0) {
		fprintf(stderr,"read error in find header\n" );
		exit(1);
	}


	
	//c파일에서 해당 단어아 있는 지 검색
	for(i = 0 ;i<header_info.count;i++) {
		if((result=strstr(buf, header_info.header[i])) != NULL) {
			fseek(header_info.fp_header, 0, SEEK_SET);
			
			//해당 단어를 찾았으면 사용자가 설정해둔 해더파일에서 단어를 읽어서 헤더파일을 구분한다.
			while(1) {
				fgets(buf_header, sizeof(char) * MAX_LINE, header_info.fp_header);

				if(feof(header_info.fp_header))
					break;

				if((result_tmp = strstr(buf_header,header_info.header[i])) != NULL) {
					result_tok = strtok(result_tmp,"#");
					while( result_tok != NULL) {

						result_tok = strtok(NULL,"#");

						if(result_tok != NULL) {
							save_include[save_count][0] = '#';
							strncpy(&save_include[save_count][1], result_tok, sizeof(char) *(strlen(result_tok)-1));
							save_count++;
						}
					}
				}
				else
					continue;

			}//WHILE(1)

		}//IF

	} //FOR

	fseek(f_info[class_count].c_file_fp, 0, SEEK_END);
	copy_c = (char *)malloc(sizeof(char) * fsize);
	memset(copy_c, 0x00, sizeof(char) * fsize);

	fclose(f_info[class_count].c_file_fp);
	fp = fopen(f_info[class_count].c_file_name, "w+");
	f_info[class_count].c_file_fp = fp;
	for(i = 0; i< save_count; i++) {
		for(j = 0; j< save_count; j++){
			if(strncmp(save_include[i], save_include[j] ,sizeof(char) *strlen(save_include[j])) == 0){
				if(header_info.use[class_count][j] == 1){
					break;
				}
			}
		}
		if(save_count<=j){

		fputs(save_include[i],f_info[class_count].c_file_fp);
		fputs("\n",f_info[class_count].c_file_fp);
		header_info.use[class_count][i] = 1;
		}
	}


	//extern 함수 및 변수 삽입
	if(class_count == 1) {
		insert_extern(class_count);
	}
	//c파일 다시 쓰기
	fputs(buf,f_info[class_count].c_file_fp);
	free(buf);
	free(copy_c);
	close(fd);
}


