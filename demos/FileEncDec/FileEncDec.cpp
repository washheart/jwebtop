#include<stdio.h>   
#include<stdlib.h>   
#include<string.h>   

#define FLAG 10

void encfile(char *in_filename, char *out_filename);/*对文件进行加密的具体函数*/
void decryptfile(char* in_filename, char *out_filename); /*对文件解密的具体函数*/

int main(int argc, char *argv[])/*定义main()函数的命令行参数*/
{
	if (argc != 4) {// argv[0]是exe自身
		printf("欢迎使用，本程序需要三个参数：\n");
		printf("  参数1：指示要进行的操作。1=加密；2=解密。\n");
		printf("  参数2：需要进行加密或解密的文件。\n");
		printf("  参数3：加密或解密后的文件保存路径。\n");
		system("pause");
		return -1;
	}
	int option=argv[1][0]; /*功能选择*/

	switch (option) {
	case '1': //加密      
		encfile(argv[2], argv[3]);/*加密函数调用*/
		break;
	case '2'://解密  
		decryptfile(argv[2], argv[3]);/*解密函数调用*/
		break;
	default:
		printf("第一个参数只能是1或者2：1=加密；2=解密。\n");
		return -1;
	}
	printf("处理完成。");
	return 0;
}

/*加密子函数开始*/
void encfile(char *in_filename, char *out_file) {
	FILE *fp1, *fp2;
	int j = 0;
	fp1 = fopen(in_filename, "rb");/*打开要加密的文件*/
	if (fp1 == NULL) {
		printf("cannot open in-file.\n");
		exit(1);/*如果不能打开要加密的文件,便退出程序*/
	}
	fp2 = fopen(out_file, "wb");
	if (fp2 == NULL) {
		printf("cannot open or create out-file.\n");
		exit(1);/*如果不能建立加密后的文件,便退出*/
	}
	register int ch;
	for (ch = fgetc(fp1); ch != EOF; ch = fgetc(fp1)) {// 执行加密
		ch += FLAG;// 加密算法：直接加上一个数值
		fputc(ch, fp2); 
	}
	fclose(fp1);/*关闭源文件*/
	fclose(fp2);/*关闭目标文件*/
}

/*解密子函数开始*/
void decryptfile(char *in_filename, char *out_file) {
	FILE *fp1, *fp2;
	int j = 0;
	fp1 = fopen(in_filename, "rb");/*打开要解密的文件*/
	if (fp1 == NULL) {
		printf("cannot open in-file.\n");
		exit(1);/*如果不能打开要解密的文件,便退出程序*/
	}
	fp2 = fopen(out_file, "wb");
	if (fp2 == NULL) {
		printf("cannot open or create out-file.\n");
		exit(1);/*如果不能建立解密后的文件,便退出*/
	}
	register int ch;
	for (ch = fgetc(fp1); ch != EOF; ch = fgetc(fp1)) {// 执行解密
		ch -= FLAG;
		fputc(ch, fp2);/*我的解密算法*/
	}
	fclose(fp1);/*关闭源文件*/
	fclose(fp2);/*关闭目标文件*/
}