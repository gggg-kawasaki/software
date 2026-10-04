#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <time.h>

	int is_debug;

int main(int argc, char **argv){
    int data[1024];
	int i,j;
	int k;
    char title[256];
    char artist[256];
    char album[256];
    char year[256];
    char comment[256];
    char genre;
	int header;
	int track;
	int ch;
	int chunksize;
	int deltatime;
	int datalength;
	int ev_prev;
	int ev_cnt;
	int ev_typ;

	char OutFileName[256];
    FILE *InFile,*OutFile;

	is_debug=1;

	if(argc<3){
		printf("MFR: SMF Retouch\n");
		printf("mfr <infile> <outfile> <options...>\n");
		if ((argc>1) && strncmp(argv[1],"-HELP",3) == 0) {
			printf("option:\n");
			printf("mfr <infile> <outfile> <options...>\n");
			exit(-1);
		}else{
			printf("\"mfr -HELP\" in option detail.\n");
		}
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
	for(i=0;i<256;i++) data[i]=0x00;
	for(i=0;i<256;i++) title[i]=0x00;
	for(i=0;i<256;i++) artist[i]=0x00;
	for(i=0;i<256;i++) album[i]=0x00;
	for(i=0;i<256;i++) year[i]=0x00;
	for(i=0;i<256;i++) comment[i]=0x00;
	genre=0xFF;

	if(argc>3){
		for(i=3;i<argc;i++) {
			if (strncmp(argv[i],"-HELP",3) == 0) {
				printf("MFR: SMF Retouch\n");
				printf("mfr <infile> <outfile> <options...>\n");
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
	i=0;j=0;k=0;
	track=0;
	ch=0;
	
    while(k==0){ 
		//ChunkID
		// remove bytes to header
		while((!(((data[0]&0xFF)==0x4D)&&((data[1]&0xFF)==0x54)&&((data[2]&0xFF)==0x68)&&((data[3]&0xFF)==0x64)))&& 
			  (!(((data[0]&0xFF)==0x4D)&&((data[1]&0xFF)==0x54)&&((data[2]&0xFF)==0x72)&&((data[3]&0xFF)==0x6B)))){ 
			data[0]=data[1];
			data[1]=data[2];
			data[2]=data[3];
			data[3] = fgetc(InFile);
			if(data[3]==EOF){
				printf("EOF is detected.\n");
				exit(0);
			}

//			if(is_debug){
//				printf("  SearchStr:%X%X%X%X\n",data[0]&0xFF,data[1]&0xFF,data[2]&0xFF,data[3]&0xFF);
//			}

		}
		header=0;
		if((((data[0]&0xFF)==0x4D)&&((data[1]&0xFF)==0x54)&&((data[2]&0xFF)==0x68)&&((data[3]&0xFF)==0x64))){
			header=1;
		}
		if(header==1){
			//----------------------------------------------------------
			//Header Chunk
			//----------------------------------------------------------
			if(is_debug){
				printf("//----------------------------------------------------------\n");
				printf("//Header Chunk\n");
			}
			if(is_debug){
				printf("  Chunk ID:%c%c%c%c\n",data[0],data[1],data[2],data[3]);
			}
			fprintf(OutFile,"%c",data[0]);
			fprintf(OutFile,"%c",data[1]);
			fprintf(OutFile,"%c",data[2]);
			fprintf(OutFile,"%c",data[3]);
			//ChunkSize
			data[0] = fgetc(InFile);
			data[1] = fgetc(InFile);
			data[2] = fgetc(InFile);
			data[3] = fgetc(InFile);
			if(is_debug){
				printf("  Chunk Size:%x%x%x%x\n",data[0]&0xFF,data[1]&0xFF,data[2]&0xFF,data[3]&0xFF);
			}
			fprintf(OutFile,"%c",data[0]);
			fprintf(OutFile,"%c",data[1]);
			fprintf(OutFile,"%c",data[2]);
			fprintf(OutFile,"%c",data[3]);
			//FormatType
			data[0] = fgetc(InFile);
			data[1] = fgetc(InFile);
			if(is_debug){
				printf("  Format Type:%x%x\n",data[0]&0xFF,data[1]&0xFF);
			}
			fprintf(OutFile,"%c",data[0]);
			fprintf(OutFile,"%c",data[1]);
			//Number of Track
			data[0] = fgetc(InFile);
			data[1] = fgetc(InFile);
			if(is_debug){
				printf("  Number of Track:%x%x\n",data[0]&0xFF,data[1]&0xFF);
			}
			fprintf(OutFile,"%c",data[0]);
			fprintf(OutFile,"%c",data[1]);
			track=(data[0]&0xFF)*256+(data[1]&0xFF);
			//Time Division
			data[0] = fgetc(InFile);
			data[1] = fgetc(InFile);
			if(is_debug){
				printf("  Time Division:%x%x\n",data[0]&0xFF,data[1]&0xFF);
			}
			fprintf(OutFile,"%c",data[0]);
			fprintf(OutFile,"%c",data[1]);
			if(is_debug){
				printf("//----------------------------------------------------------\n");
			}
		}//End header
		else {
			i++;
			if(is_debug){
				printf("//----------------------------------------------------------\n");
				printf("//Header Chunk (Tr:%2d)\n",i);
			}
			if(is_debug){
				printf("  Chunk ID:%c%c%c%c\n",data[0],data[1],data[2],data[3]);
			}
			fprintf(OutFile,"%c",data[0]);
			fprintf(OutFile,"%c",data[1]);
			fprintf(OutFile,"%c",data[2]);
			fprintf(OutFile,"%c",data[3]);
			//ChunkSize
			data[0] = fgetc(InFile);
			data[1] = fgetc(InFile);
			data[2] = fgetc(InFile);
			data[3] = fgetc(InFile);
			chunksize=(data[0]&0xFF)*256*256*256+(data[1]&0xFF)*256*256+(data[2]&0xFF)*256+(data[3]&0xFF);
			if(is_debug){
				printf("  Chunk Size:%02x%02x%02x%02x(%08x)\n",data[0]&0xFF,data[1]&0xFF,data[2]&0xFF,data[3]&0xFF,chunksize);
			}
			fprintf(OutFile,"%c",data[0]);
			fprintf(OutFile,"%c",data[1]);
			fprintf(OutFile,"%c",data[2]);
			fprintf(OutFile,"%c",data[3]);
			while(chunksize>0){
				//Delta Time
				data[0] = 0x00;
				data[1] = 0x00;
				data[2] = 0x00;
				data[3] = 0x00;
				data[0] = fgetc(InFile);chunksize--;
				deltatime=data[0]&0x7F;
				fprintf(OutFile,"%c",data[0]);
				if((data[0]&0x80)==0x80){
					deltatime=deltatime*128;
					data[1] = fgetc(InFile);chunksize--;
					deltatime=deltatime+(data[1]&0x7F);
					fprintf(OutFile,"%c",data[1]);
				}
				if((data[1]&0x80)==0x80){
					deltatime=deltatime*128;
					data[2] = fgetc(InFile);chunksize--;
					deltatime=deltatime+(data[2]&0x7F);
					fprintf(OutFile,"%c",data[2]);
				}
				if((data[2]&0x80)==0x80){
					deltatime=deltatime*128;
					data[3] = fgetc(InFile);chunksize--;
					deltatime=deltatime+(data[3]&0x7F);
					fprintf(OutFile,"%c",data[3]);
				}
				if(is_debug){
					printf("  Delta Time:%02X%02X%02X%02X(%07x)\n",data[0]&0xFF,data[1]&0xFF,data[2]&0xFF,data[3]&0xFF,deltatime);
				}
				//Event
				data[0] = fgetc(InFile);chunksize--;
				if(data[0]==EOF){
					printf("EOF is detected.\n");
					exit(0);
				}
				ev_cnt=0;
				if((data[0]&0xFF)<0x80){
					ev_cnt=1;
					data[1]=data[0];
					data[0]=ev_prev;
				}
				ev_prev=(data[0]&0xFF);
				switch(data[0]&0xFF){
					case 0x80:
					case 0x81:
					case 0x82:
					case 0x83:
					case 0x84:
					case 0x85:
					case 0x86:
					case 0x87:
					case 0x88:
					case 0x89:
					case 0x8a:
					case 0x8b:
					case 0x8c:
					case 0x8d:
					case 0x8e:
					case 0x8f:
						if(ev_cnt==0){
							data[1] = fgetc(InFile);chunksize--;
						}
						data[2] = fgetc(InFile);chunksize--;
						if(is_debug){
							if(ev_cnt==0) printf("  Note Off:%02X%02X%02X\n",data[0]&0xFF,data[1]&0xFF,data[2]&0xFF);
							if(ev_cnt==1) printf("  Note Off:  %02X%02X\n",data[1]&0xFF,data[2]&0xFF);
						}
						if(ev_cnt==0) fprintf(OutFile,"%c",data[0]);
						fprintf(OutFile,"%c",data[1]);
						fprintf(OutFile,"%c",data[2]);
					break;
					case 0x90:
					case 0x91:
					case 0x92:
					case 0x93:
					case 0x94:
					case 0x95:
					case 0x96:
					case 0x97:
					case 0x98:
					case 0x99:
					case 0x9a:
					case 0x9b:
					case 0x9c:
					case 0x9d:
					case 0x9e:
					case 0x9f:
						if(ev_cnt==0){
							data[1] = fgetc(InFile);chunksize--;
						}
						data[2] = fgetc(InFile);chunksize--;
						if(is_debug){
							if(ev_cnt==0) printf("  Note On:%02X%02X%02X\n",data[0]&0xFF,data[1]&0xFF,data[2]&0xFF);
							if(ev_cnt==1) printf("  Note On:  %02X%02X\n",data[1]&0xFF,data[2]&0xFF);
						}
						if(ev_cnt==0) fprintf(OutFile,"%c",data[0]);
						fprintf(OutFile,"%c",data[1]);
						fprintf(OutFile,"%c",data[2]);
					break;
					case 0xa0:
					case 0xa1:
					case 0xa2:
					case 0xa3:
					case 0xa4:
					case 0xa5:
					case 0xa6:
					case 0xa7:
					case 0xa8:
					case 0xa9:
					case 0xaa:
					case 0xab:
					case 0xac:
					case 0xad:
					case 0xae:
					case 0xaf:
						if(ev_cnt==0){
							data[1] = fgetc(InFile);chunksize--;
						}
						data[2] = fgetc(InFile);chunksize--;
						if(is_debug){
							if(ev_cnt==0) printf("  Key After Touch:%02X%02X%02X\n",data[0]&0xFF,data[1]&0xFF,data[2]&0xFF);
							if(ev_cnt==1) printf("  Key After Touch:  %02X%02X\n",data[1]&0xFF,data[2]&0xFF);
						}
						if(ev_cnt==0) fprintf(OutFile,"%c",data[0]);
						fprintf(OutFile,"%c",data[1]);
						fprintf(OutFile,"%c",data[2]);
					break;
					case 0xb0:
					case 0xb1:
					case 0xb2:
					case 0xb3:
					case 0xb4:
					case 0xb5:
					case 0xb6:
					case 0xb7:
					case 0xb8:
					case 0xb9:
					case 0xba:
					case 0xbb:
					case 0xbc:
					case 0xbd:
					case 0xbe:
					case 0xbf:
						if(ev_cnt==0){
							data[1] = fgetc(InFile);chunksize--;
						}
						data[2] = fgetc(InFile);chunksize--;
						if(is_debug){
							if(ev_cnt==0) printf("  Control Change/Channel Mode:%02X%02X%02X\n",data[0]&0xFF,data[1]&0xFF,data[2]&0xFF);
							if(ev_cnt==1) printf("  Control Change/Channel Mode:  %02X%02X\n",data[1]&0xFF,data[2]&0xFF);
						}
						if(ev_cnt==0) fprintf(OutFile,"%c",data[0]);
						fprintf(OutFile,"%c",data[1]);
						fprintf(OutFile,"%c",data[2]);
					break;
					case 0xc0:
					case 0xc1:
					case 0xc2:
					case 0xc3:
					case 0xc4:
					case 0xc5:
					case 0xc6:
					case 0xc7:
					case 0xc8:
					case 0xc9:
					case 0xca:
					case 0xcb:
					case 0xcc:
					case 0xcd:
					case 0xce:
					case 0xcf:
						data[1] = fgetc(InFile);chunksize--;
						if(is_debug){
							printf("  Program Change:%02X%02X\n",data[0]&0xFF,data[1]&0xFF);
						}
						fprintf(OutFile,"%c",data[0]);
						fprintf(OutFile,"%c",data[1]);
					break;
					case 0xd0:
					case 0xd1:
					case 0xd2:
					case 0xd3:
					case 0xd4:
					case 0xd5:
					case 0xd6:
					case 0xd7:
					case 0xd8:
					case 0xd9:
					case 0xda:
					case 0xdb:
					case 0xdc:
					case 0xdd:
					case 0xde:
					case 0xdf:
						data[1] = fgetc(InFile);chunksize--;
						if(is_debug){
							printf("  Channel After Touch:%02X%02X\n",data[0]&0xFF,data[1]&0xFF);
						}
						fprintf(OutFile,"%c",data[0]);
						fprintf(OutFile,"%c",data[1]);
					break;
					case 0xe0:
					case 0xe1:
					case 0xe2:
					case 0xe3:
					case 0xe4:
					case 0xe5:
					case 0xe6:
					case 0xe7:
					case 0xe8:
					case 0xe9:
					case 0xea:
					case 0xeb:
					case 0xec:
					case 0xed:
					case 0xee:
					case 0xef:
						if(ev_cnt==0){
							data[1] = fgetc(InFile);chunksize--;
						}
						data[2] = fgetc(InFile);chunksize--;
						if(is_debug){
							if(ev_cnt==0) printf("  Pitch Bend:%02X%02X%02X\n",data[0]&0xFF,data[1]&0xFF,data[2]&0xFF);
							if(ev_cnt==1) printf("  Pitch Bend:  %02X%02X\n",data[1]&0xFF,data[2]&0xFF);
						}
						if(ev_cnt==0) fprintf(OutFile,"%c",data[0]);
						fprintf(OutFile,"%c",data[1]);
						fprintf(OutFile,"%c",data[2]);
					break;
					case 0xf1:
						data[1] = fgetc(InFile);chunksize--;
						if(is_debug){
							printf("  MIDI TIme Code:%02X%02X\n",data[0]&0xFF,data[1]&0xFF);
						}
						fprintf(OutFile,"%c",data[0]);
						fprintf(OutFile,"%c",data[1]);
					break;
					case 0xf2:
						data[1] = fgetc(InFile);chunksize--;
						data[2] = fgetc(InFile);chunksize--;
						if(is_debug){
							printf("  Song Position:%02X%02X%02X\n",data[0]&0xFF,data[1]&0xFF,data[2]&0xFF);
						}
						fprintf(OutFile,"%c",data[0]);
						fprintf(OutFile,"%c",data[1]);
						fprintf(OutFile,"%c",data[2]);
					break;
					case 0xf3:
						data[1] = fgetc(InFile);chunksize--;
						if(is_debug){
							printf("  Song Number:%02X%02X\n",data[0]&0xFF,data[1]&0xFF);
						}
						fprintf(OutFile,"%c",data[0]);
						fprintf(OutFile,"%c",data[1]);
					break;
					case 0xf6:
						if(is_debug){
							printf("  Tune Request:%02X%02X%02X\n",data[0]&0xFF);
						}
						fprintf(OutFile,"%c",data[0]);
					break;
					case 0xf8:
						if(is_debug){
							printf("  MIDI Clock:%02X\n",data[0]&0xFF);
						}
						fprintf(OutFile,"%c",data[0]);
					break;
					case 0xfa:
						if(is_debug){
							printf("  MIDI Start:%02X\n",data[0]&0xFF);
						}
						fprintf(OutFile,"%c",data[0]);
					break;
					case 0xfb:
						if(is_debug){
							printf("  MIDI Continue:%02X\n",data[0]&0xFF);
						}
						fprintf(OutFile,"%c",data[0]);
					break;
					case 0xfc:
						if(is_debug){
							printf("  MIDI Stop:%02X\n",data[0]&0xFF);
						}
						fprintf(OutFile,"%c",data[0]);
					break;
					case 0xfe:
						if(is_debug){
							printf("  Active Sencing:%02X\n",data[0]&0xFF);
						}
						fprintf(OutFile,"%c",data[0]);
					break;
					case 0xf0:
					case 0xf7:
						if(is_debug){
							printf("  System Exclusive:%02X\n",data[0]&0xFF);
						}
						fprintf(OutFile,"%c",data[0]);
						//Data Length
						data[0] = 0x00;
						data[1] = 0x00;
						data[2] = 0x00;
						data[3] = 0x00;
						data[0] = fgetc(InFile);chunksize--;
						datalength=data[0]&0x7F;
						fprintf(OutFile,"%c",data[0]);
						if((data[0]&0x80)==0x80){
							datalength=datalength*128;
							data[1] = fgetc(InFile);chunksize--;
							datalength=datalength+(data[1]&0x7F);
							fprintf(OutFile,"%c",data[1]);
						}
						if((data[1]&0x80)==0x80){
							datalength=datalength*128;
							data[2] = fgetc(InFile);chunksize--;
							datalength=datalength+(data[2]&0x7F);
							fprintf(OutFile,"%c",data[2]);
						}
						if((data[2]&0x80)==0x80){
							datalength=datalength*128;
							data[3] = fgetc(InFile);chunksize--;
							datalength=datalength+(data[3]&0x7F);
							fprintf(OutFile,"%c",data[3]);
						}
						if(is_debug){
							printf("   Data Length:%02X%02X%02X%02X(%07x)\n",data[0]&0xFF,data[1]&0xFF,data[2]&0xFF,data[3]&0xFF,datalength);
						}
						if(is_debug){
							printf("   Data:");
						}
						while(datalength>0){
							data[0] = fgetc(InFile);chunksize--;
							fprintf(OutFile,"%c",data[0]);
							datalength--;
							if(is_debug){
								printf("%02X",data[0]&0xFF);
							}
						}
						if(is_debug){
							printf("\n");
						}
					break;
					case 0xff:
						data[1] = fgetc(InFile);chunksize--;
						ev_typ=(data[1]&0xFF);
						if(is_debug){
							printf("  Meta Event:%02X%02X",data[0]&0xFF,data[1]&0xFF);
							switch(ev_typ){
								case 0x00:printf("[Sequence Number]\n");
								break;
								case 0x01:printf("[Text Event]\n");
								break;
								case 0x02:printf("[Copyright Notice]\n");
								break;
								case 0x03:printf("[Sequence/Track Name]\n");
								break;
								case 0x04:printf("[Instrument Name]\n");
								break;
								case 0x05:printf("[Lyric]\n");
								break;
								case 0x06:printf("[Marker]\n");
								break;
								case 0x07:printf("[Cue Point]\n");
								break;
								case 0x20:printf("[Channel Prefix]\n");
								break;
								case 0x21:printf("[Port Prefix]\n");
								break;
								case 0x2f:printf("[End of Track]\n");
								break;
								case 0x51:printf("[Set Tempo]\n");
								break;
								case 0x54:printf("[SMPTE Offset]\n");
								break;
								case 0x58:printf("[Time Signature]\n");
								break;
								case 0x59:printf("[Key Signature]\n");
								break;
								case 0x7f:printf("[Sequencer-Specific Meta-Event]\n");
								break;
								default:printf("\n");
								break;
							}
						}
						fprintf(OutFile,"%c",data[0]);
						fprintf(OutFile,"%c",data[1]);
						//Data Length
						data[0] = 0x00;
						data[1] = 0x00;
						data[2] = 0x00;
						data[3] = 0x00;
						data[0] = fgetc(InFile);chunksize--;
						datalength=data[0]&0x7F;
						fprintf(OutFile,"%c",data[0]);
						if((data[0]&0x80)==0x80){
							datalength=datalength*128;
							data[1] = fgetc(InFile);chunksize--;
							datalength=datalength+(data[1]&0x7F);
							fprintf(OutFile,"%c",data[1]);
						}
						if((data[1]&0x80)==0x80){
							datalength=datalength*128;
							data[2] = fgetc(InFile);chunksize--;
							datalength=datalength+(data[2]&0x7F);
							fprintf(OutFile,"%c",data[2]);
						}
						if((data[2]&0x80)==0x80){
							datalength=datalength*128;
							data[3] = fgetc(InFile);chunksize--;
							datalength=datalength+(data[3]&0x7F);
							fprintf(OutFile,"%c",data[3]);
						}
						if(is_debug){
							printf("   Data Length:%02X%02X%02X%02X(%07x)\n",data[0]&0xFF,data[1]&0xFF,data[2]&0xFF,data[3]&0xFF,datalength);
						}
						if(is_debug){
							printf("   Data:");
						}
						while(datalength>0){
							data[0] = fgetc(InFile);chunksize--;
							fprintf(OutFile,"%c",data[0]);
							datalength--;
							if(is_debug){
								if(ev_typ<8) printf("%c",data[0]&0xFF);
								if(ev_typ>=8) printf("%02X",data[0]&0xFF);
							}
						}
						if(is_debug){
							printf("\n");
						}
					break;
					default:
						if(is_debug){
							printf("  Unknown:%02X\n",data[0]&0xFF);
						}
						fprintf(OutFile,"%c",data[0]);
					break;
				}
			}
			if(is_debug){
				printf("//----------------------------------------------------------\n");
			}

			if(i >= track){
				k=1;
			}
		}
	}
//		data[2]=data[1];
//		data[1]=data[0];
exit(0);

}
