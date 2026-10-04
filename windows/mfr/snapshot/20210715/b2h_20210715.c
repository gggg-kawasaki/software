#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <time.h>

void main(int argc, char **argv){
	int data[256];
	int i,j,k;
	int form;
	int shownum,num;

	char OutFileName[256];
	FILE *InFile,*OutFile;
 
	if(argc<3){
		printf("B2H : binary to hex\n");
		printf("b2h infile outfile [ -f0/-f1/-f2 -n ]\n");
		printf("	-f0 : 16 characters per a line\n");
		printf("	-f1 : no line split\n");
		printf("	-f2 : split line by LF or CR\n");
		printf("	-n  : display character number of line (available only -f2)\n");
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
//		printf("1\n"); 
	// initialize array
	for(i=0;i<256;i++) data[i]=0x00;

	form=0;
	shownum=0;
	num=0;

	if(argc>3){
		for(i=3;i<argc;i++) {
			if (strncmp(argv[i],"-f0",3) == 0)
				form=0;
			else if (strncmp(argv[i],"-f1",3) == 0)
				form=1;
			else if (strncmp(argv[i],"-f2",3) == 0)
				form=2;
			else if (strncmp(argv[i],"-n",2) == 0)
				shownum=1;
			else {
				fprintf(stderr,"Unknown %s\n",argv[i]);
				exit(-1);
			}
		}
	}
//		printf("2\n"); 
	k=0;
	while(k==0){ 
		if(form==0){
			for(j=0;j<16;j++){
				data[0] = fgetc(InFile);
				if(data[0]==EOF) exit(0);
				if(j!=0) fprintf(OutFile," ");
				fprintf(OutFile,"%02X",data[0]);
			}
			fprintf(OutFile,"\n");
		}
		else if(form==1){
			data[0] = fgetc(InFile);
			if(data[0]==EOF) exit(0);
			fprintf(OutFile,"%02X",data[0]);
		}
		else if(form==2){
			data[0] = fgetc(InFile);num++;
			if((data[0]==0x0A)||(data[0]==0x0D)){
				fprintf(OutFile," ");
				fprintf(OutFile,"%02X",data[0]);
				if(data[0]==0x0D){
					data[0] = fgetc(InFile);
					if(data[0]==EOF) exit(0);
					if(data[0]==0x0A){
						fprintf(OutFile,"%02X",data[0]);
						if(shownum==1){
							fprintf(OutFile," ( %d )\n",num-1);
							num=0;
						} else {
							fprintf(OutFile,"\n");
							num=0;
						}
					} else {
						if(shownum==1){
							fprintf(OutFile," ( %d )\n",num-1);
							num=0;
						} else {
							fprintf(OutFile,"\n");
							num=0;
						}
						fprintf(OutFile,"%02X",data[0]);
						num++;
					}
				} else {
					if(shownum==1){
						fprintf(OutFile," ( %d )\n",num-1);
						num=0;
					} else {
						fprintf(OutFile,"\n");
						num=0;
					}
				}
			} else {
				if(data[0]==EOF) {
					if(shownum==1) fprintf(OutFile," ( %d )\n",num-1);
					exit(0);
				}
				fprintf(OutFile,"%02X",data[0]);
			}
		}
	}
	fclose(InFile);
	fclose(OutFile);
}
