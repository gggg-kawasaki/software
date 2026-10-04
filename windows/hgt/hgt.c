
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
int main(int argc, char **argv){
    int data[130];
	int i,j;
	int k;
    char title[256];
    char artist[256];
    char album[256];
    char year[256];
    char comment[256];
    char truck;
    char genre;

	char OutFileName[256];
    FILE *InFile,*OutFile;
 
	if(argc<3){
		printf("HGT : ごみヘッダ削除、タグ取りかえ\n");
		printf("hgt infile outfile [ -t title -a artist -b album -y year -c comment -g genre -n truck ]\n");
		exit(0);
	}
    InFile = fopen(argv[1],"rb" );
    if((InFile==NULL)||(ferror(InFile))){
		printf(" Input File Error\n"); 
		exit(0);
	}
    OutFile = fopen(argv[2],"wb" );
    if((OutFile==NULL)||(ferror(OutFile))){
		printf(" Output File Error\n"); 
		exit(0);
	}
	// initialize array
	for(i=0;i<130;i++) data[i]=0x00;
	for(i=0;i<256;i++) title[i]=0x00;
	for(i=0;i<256;i++) artist[i]=0x00;
	for(i=0;i<256;i++) album[i]=0x00;
	for(i=0;i<256;i++) year[i]=0x00;
	for(i=0;i<256;i++) comment[i]=0x00;
	truck=0x0;
	genre=0xFF;

	if(argc>3){
		for(i=3;i<argc;i++) {
			if (strncmp(argv[i],"-HELP",3) == 0) {
				printf("HGT : ごみヘッダ削除、タグ取りかえ\n");
				printf("hgt infile outfile [ -t title -a artist -b album -y year -c comment -g genre -n truck ]\n");
				exit(-1);
			}
			else if (strncmp(argv[i],"-t",2) == 0)
				strncpy(title,argv[++i],30);
			else if (strncmp(argv[i],"-a",2) == 0)
				strncpy(artist,argv[++i],30);
			else if (strncmp(argv[i],"-b",2) == 0)
				strncpy(album,argv[++i],30);
			else if (strncmp(argv[i],"-y",2) == 0)
				strncpy(year,argv[++i],4);
			else if (strncmp(argv[i],"-c",2) == 0)
				strncpy(comment,argv[++i],4);
			else if (strncmp(argv[i],"-g",2) == 0)
				genre = atoi(argv[++i]);
			else if (strncmp(argv[i],"-n",2) == 0)
				truck = atoi(argv[++i]);
			else {
				fprintf(stderr,"Don't know how to do %s\n",argv[i]);
				exit(-1);
			}
		}
	}
 
//	for(i=3;i<argc;i++) {
//		printf("%d : %s\n",i,argv[i]);
//	}
//	printf("%d\n",i++);

	// remove bytes to header
	while(((data[0]&0xFF)!=0xFF)||((data[1]&0xF0)!=0xF0)){ 
		data[0]=data[1];
		data[1] = fgetc(InFile);
		if(data[1]==EOF) exit(0);
	}
	fprintf(OutFile,"%c",data[0]);  
	fprintf(OutFile,"%c",data[1]);  
//		data[2]=data[1];
//		data[1]=data[0];

	// remove remaining TAG bytes
	data[1] = fgetc(InFile);
	data[2] = fgetc(InFile);
	k=0;
//	printf("%d\n",k);
    while(k==0){ 
		data[0]=data[1];
		data[1]=data[2];
		data[2] = fgetc(InFile);
		if(data[0]==0x54 && data[1]==0x41 && data[2]==0x47){
			for(i=3;i<128;i++){
				data[i] = fgetc(InFile);
				if(data[i]==EOF){
					for(j=0;j<i;j++){
						fprintf(OutFile,"%c",data[j]);  
					}
					printf("1\n");
				//	exit(0);
					k=1;
				}
			}
			data[128] = fgetc(InFile);
			if(data[128]==EOF){
				printf("4\n");
			//	exit(0);
				k=1;
			}else{
				for(j=0;j<127;j++){
					fprintf(OutFile,"%c",data[j]);  
				}
				data[1]=data[127];
				data[2]=data[128];
				printf("2\n");
			}
		}else{
			fprintf(OutFile,"%c",data[0]);  
			if(data[2]==EOF){
				fprintf(OutFile,"%c",data[1]);  
				printf("3\n");
			//	exit(0);
				k=1;
			}
		}
	}
	// add new TAG bytes
	if(argc>3){
		fprintf(OutFile,"TAG");  
		for(i=0;i<30;i++) fprintf(OutFile,"%c",title[i]);  
		for(i=0;i<30;i++) fprintf(OutFile,"%c",artist[i]);  
		for(i=0;i<30;i++) fprintf(OutFile,"%c",album[i]);  
		for(i=0;i< 4;i++) fprintf(OutFile,"%c",year[i]);  
		for(i=0;i<28;i++) fprintf(OutFile,"%c",comment[i]);  
		for(i=0;i<1;i++) fprintf(OutFile,"%c",0x00);  
		for(i=0;i<1;i++) fprintf(OutFile,"%c",truck);  
		fprintf(OutFile,"%c",genre);  
	}
}
