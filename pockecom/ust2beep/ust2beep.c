#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <time.h>

int convlylic(unsigned char *src, unsigned char *dst);
int convnotenum(int src);
int convlength(int tempo, int length, int notenum, unsigned char *lylic);

int main(int argc, char **argv){
	int i,j,k,l;
	int line_n;
    char width[4];
    char height[4];
	int w,h;
	int rot,comp;
    unsigned char src0[2048];
    unsigned char s_tempo[2048];
    unsigned char s_length[2048];
    unsigned char s_lylic[2048];
    unsigned char s_notenum[2048];
	int tempo,length,notenum;
    unsigned char s_lylic2[2048];
    unsigned char src6[2048];
    unsigned char src7[2048];
    char InFileName[64];
    unsigned char img[72];
	time_t timer;
    unsigned char schr[2];
    unsigned char simg[144];
    unsigned char mimg[144];
	int note_max,note_min;
    unsigned char s_nof[4];
	int note_ofs;

    FILE *InFile,*OutFile;
 
	time(&timer);
	//printf("%s\n", ctime(&timer));
	note_ofs=0;
	if(argc<3){
		printf("UST2BEEP :  UST -> PC-G850 BASIC source code converter\n");
		printf("ust2beep infile outfile [-n num]\n");
		printf("infile : ust file\n");
		printf("outfile : bas file\n");
		printf("[-n num] : adding num note offset\n");
		exit(0);
	}
    InFile = fopen(argv[1],"rb" );
    if((InFile==NULL)||(ferror(InFile))){
		printf(" Input File Error\n"); 
		exit(0);
	}
	strncpy(InFileName,argv[1],64);
    OutFile = fopen(argv[2],"wb" );
    if((OutFile==NULL)||(ferror(OutFile))){
		printf(" Output File Error\n"); 
		exit(0);
	}
	if(argc>3){
		for(i=3;i<argc;i++) {
			if (strncmp(argv[i],"-HELP",3) == 0) {
				printf("UST2BEEP :  UST -> PC-G850 BASIC source code converter\n");
				printf("ust2beep infile outfile [-n num]\n");
				printf("infile : ust file\n");
				printf("outfile : bas file\n");
				printf("[-n num] : adding num note offset\n");
				exit(-1);
			}
			else if (strncmp(argv[i],"-n",2) == 0){
				strncpy(s_nof,argv[++i],4);
				note_ofs=atoi(s_nof);
			}
			else {
				fprintf(stderr,"Don't know how to do %s\n",argv[i]);
				exit(-1);
			}
		}
	}
	note_max=0;note_min=255;
	line_n=10;k=0;
	fprintf(OutFile,"%1d REM UST2BEEP Generated from %s %s",line_n,InFileName,ctime(&timer)); line_n+=10;
	for(j=0;k==0;){
		fscanf(InFile,"%s",src0);
		if(strcmp(src0,"[#TRACKEND]")==0){
			if(j%4==0) { fprintf(OutFile,"%1d DATA ",line_n); line_n+=10; }
			fprintf(OutFile,"\" \",25,25\n");
			printf("note count = %d\n",j);
			printf("min note num = %d\n",note_min);
			printf("max note num = %d\n",note_max);
			printf("note offset = %d\n",note_ofs);
			if(note_min<46 || note_max>107) printf("Available note range is 46-107.\n");
			k=1;
			//	exit(0);
		}
		if(strncmp(src0,"Tempo",5)==0){
			for(i=0;i<8;i++) s_tempo[i]=src0[i+6];
			tempo=atoi(s_tempo);
		}

		if(strncmp(src0,"Length",6)==0){
			for(i=0;i<8;i++) s_length[i]=src0[i+7];
			length=atoi(s_length);
		}
		if(strncmp(src0,"Lyric",5)==0){
			for(i=0;i<8;i++) s_lylic[i]=src0[i+6];
			convlylic(s_lylic,s_lylic2);
			if(j%4==0) { fprintf(OutFile,"%1d DATA ",line_n); line_n+=10; }
			fprintf(OutFile,"\"%s\", ",s_lylic2);
		}
		if(strncmp(src0,"NoteNum",7)==0){
			for(i=0;i<8;i++) s_notenum[i]=src0[i+8];
			notenum=atoi(s_notenum)+note_ofs;
			if(notenum<note_min) note_min=notenum;
			if(notenum>note_max) note_max=notenum;
			fprintf(OutFile,"%d, ",convnotenum(notenum));
			fprintf(OutFile,"%d",convlength(tempo,length,notenum,s_lylic2));
			if(j%4==3){
				fprintf(OutFile,"\n");
			}else{
				fprintf(OutFile,", ");
			}
			j++;
		}
	}

	fprintf(OutFile,"%1d FOR I=0 TO %d STEP 1\n",line_n,j); line_n+=10;
	fprintf(OutFile,"%1d READ A$,B,C\n",line_n); line_n+=10;
	fprintf(OutFile,"%1d IF A$=\" \" THEN PRINT \" \": FOR J=0 TO C STEP 1: NEXT J\n",line_n); line_n+=10;
	fprintf(OutFile,"%1d IF A$<>\" \" THEN PRINT A$; : BEEP 1,B,C\n",line_n); line_n+=10;
	fprintf(OutFile,"%1d NEXT I\n",line_n); line_n+=10;
	fprintf(OutFile,"%1d END\n",line_n); line_n+=10;

}


int convlylic(unsigned char *src, unsigned char *dst){
	int i;
	int flag;
	i=0;flag=0;
// match 6chars
	if(flag==0){
	if(strncmp((const char *)src,"ÉLÅKÉF",6)==0) {flag=1; dst[i++]='∑';dst[i++]='ﬁ';dst[i++]='™';}
	if(strncmp((const char *)src,"ÉLÅKÉÉ",6)==0) {flag=1; dst[i++]='∑';dst[i++]='ﬁ';dst[i++]='¨';}
	if(strncmp((const char *)src,"ÉLÅKÉÖ",6)==0) {flag=1; dst[i++]='∑';dst[i++]='ﬁ';dst[i++]='≠';}
	if(strncmp((const char *)src,"ÉLÅKÉá",6)==0) {flag=1; dst[i++]='∑';dst[i++]='ﬁ';dst[i++]='Æ';}
	}
// match 4chars
	if(flag==0){
	if(strncmp((const char *)src,"Ç¢Ç•",4)==0) {flag=1; dst[i++]='≤';dst[i++]='™';}
	if(strncmp((const char *)src,"Ç§Ç°",4)==0) {flag=1; dst[i++]='≥';dst[i++]='®';}
	if(strncmp((const char *)src,"Ç§Ç•",4)==0) {flag=1; dst[i++]='≥';dst[i++]='™';}
	if(strncmp((const char *)src,"Ç§Çß",4)==0) {flag=1; dst[i++]='≥';dst[i++]='´';}
	if(strncmp((const char *)src,"Ç´Ç•",4)==0) {flag=1; dst[i++]='∑';dst[i++]='™';}
	if(strncmp((const char *)src,"Ç´Ç·",4)==0) {flag=1; dst[i++]='∑';dst[i++]='¨';}
	if(strncmp((const char *)src,"Ç´Ç„",4)==0) {flag=1; dst[i++]='∑';dst[i++]='≠';}
	if(strncmp((const char *)src,"Ç´ÇÂ",4)==0) {flag=1; dst[i++]='∑';dst[i++]='Æ';}
	if(strncmp((const char *)src,"Ç¨Ç•",4)==0) {flag=1; dst[i++]='∑';dst[i++]='ﬁ';dst[i++]='™';}
	if(strncmp((const char *)src,"Ç¨Ç·",4)==0) {flag=1; dst[i++]='∑';dst[i++]='ﬁ';dst[i++]='¨';}
	if(strncmp((const char *)src,"Ç¨Ç„",4)==0) {flag=1; dst[i++]='∑';dst[i++]='ﬁ';dst[i++]='≠';}
	if(strncmp((const char *)src,"Ç¨ÇÂ",4)==0) {flag=1; dst[i++]='∑';dst[i++]='ﬁ';dst[i++]='Æ';}
	if(strncmp((const char *)src,"ÇµÇ•",4)==0) {flag=1; dst[i++]='º';dst[i++]='™';}
	if(strncmp((const char *)src,"ÇµÇ·",4)==0) {flag=1; dst[i++]='º';dst[i++]='¨';}
	if(strncmp((const char *)src,"ÇµÇ„",4)==0) {flag=1; dst[i++]='º';dst[i++]='≠';}
	if(strncmp((const char *)src,"ÇµÇÂ",4)==0) {flag=1; dst[i++]='º';dst[i++]='Æ';}
	if(strncmp((const char *)src,"Ç∂Ç•",4)==0) {flag=1; dst[i++]='º';dst[i++]='ﬁ';dst[i++]='™';}
	if(strncmp((const char *)src,"Ç∂Ç·",4)==0) {flag=1; dst[i++]='º';dst[i++]='ﬁ';dst[i++]='¨';}
	if(strncmp((const char *)src,"Ç∂Ç„",4)==0) {flag=1; dst[i++]='º';dst[i++]='ﬁ';dst[i++]='≠';}
	if(strncmp((const char *)src,"Ç∂ÇÂ",4)==0) {flag=1; dst[i++]='º';dst[i++]='ﬁ';dst[i++]='Æ';}
	if(strncmp((const char *)src,"Ç∑Ç°",4)==0) {flag=1; dst[i++]='Ω';dst[i++]='®';}
	if(strncmp((const char *)src,"Ç∏Ç°",4)==0) {flag=1; dst[i++]='Ω';dst[i++]='ﬁ';dst[i++]='®';}
	if(strncmp((const char *)src,"ÇøÇ•",4)==0) {flag=1; dst[i++]='¡';dst[i++]='™';}
	if(strncmp((const char *)src,"ÇøÇ·",4)==0) {flag=1; dst[i++]='¡';dst[i++]='¨';}
	if(strncmp((const char *)src,"ÇøÇ„",4)==0) {flag=1; dst[i++]='¡';dst[i++]='≠';}
	if(strncmp((const char *)src,"ÇøÇÂ",4)==0) {flag=1; dst[i++]='¡';dst[i++]='Æ';}
	if(strncmp((const char *)src,"Ç¬Çü",4)==0) {flag=1; dst[i++]='¬';dst[i++]='ß';}
	if(strncmp((const char *)src,"Ç¬Ç°",4)==0) {flag=1; dst[i++]='¬';dst[i++]='®';}
	if(strncmp((const char *)src,"Ç¬Ç•",4)==0) {flag=1; dst[i++]='¬';dst[i++]='™';}
	if(strncmp((const char *)src,"Ç¬Çß",4)==0) {flag=1; dst[i++]='¬';dst[i++]='´';}
	if(strncmp((const char *)src,"ÇƒÇ°",4)==0) {flag=1; dst[i++]='√';dst[i++]='®';}
	if(strncmp((const char *)src,"ÇƒÇ„",4)==0) {flag=1; dst[i++]='√';dst[i++]='≠';}
	if(strncmp((const char *)src,"Ç≈Ç°",4)==0) {flag=1; dst[i++]='√';dst[i++]='ﬁ';dst[i++]='®';}
	if(strncmp((const char *)src,"Ç≈Ç„",4)==0) {flag=1; dst[i++]='√';dst[i++]='ﬁ';dst[i++]='≠';}
	if(strncmp((const char *)src,"Ç∆Ç£",4)==0) {flag=1; dst[i++]='ƒ';dst[i++]='©';}
	if(strncmp((const char *)src,"Ç«Ç£",4)==0) {flag=1; dst[i++]='ƒ';dst[i++]='ﬁ';dst[i++]='©';}
	if(strncmp((const char *)src,"Ç…Ç•",4)==0) {flag=1; dst[i++]='∆';dst[i++]='™';}
	if(strncmp((const char *)src,"Ç…Ç·",4)==0) {flag=1; dst[i++]='∆';dst[i++]='¨';}
	if(strncmp((const char *)src,"Ç…Ç„",4)==0) {flag=1; dst[i++]='∆';dst[i++]='≠';}
	if(strncmp((const char *)src,"Ç…ÇÂ",4)==0) {flag=1; dst[i++]='∆';dst[i++]='Æ';}
	if(strncmp((const char *)src,"Ç–Ç•",4)==0) {flag=1; dst[i++]='À';dst[i++]='™';}
	if(strncmp((const char *)src,"Ç–Ç·",4)==0) {flag=1; dst[i++]='À';dst[i++]='¨';}
	if(strncmp((const char *)src,"Ç–Ç„",4)==0) {flag=1; dst[i++]='À';dst[i++]='≠';}
	if(strncmp((const char *)src,"Ç–ÇÂ",4)==0) {flag=1; dst[i++]='À';dst[i++]='Æ';}
	if(strncmp((const char *)src,"Ç—Ç•",4)==0) {flag=1; dst[i++]='À';dst[i++]='ﬁ';dst[i++]='™';}
	if(strncmp((const char *)src,"Ç—Ç·",4)==0) {flag=1; dst[i++]='À';dst[i++]='ﬁ';dst[i++]='¨';}
	if(strncmp((const char *)src,"Ç—Ç„",4)==0) {flag=1; dst[i++]='À';dst[i++]='ﬁ';dst[i++]='≠';}
	if(strncmp((const char *)src,"Ç—ÇÂ",4)==0) {flag=1; dst[i++]='À';dst[i++]='ﬁ';dst[i++]='Æ';}
	if(strncmp((const char *)src,"Ç“Ç•",4)==0) {flag=1; dst[i++]='À';dst[i++]='ﬂ';dst[i++]='™';}
	if(strncmp((const char *)src,"Ç“Ç·",4)==0) {flag=1; dst[i++]='À';dst[i++]='ﬂ';dst[i++]='¨';}
	if(strncmp((const char *)src,"Ç“Ç„",4)==0) {flag=1; dst[i++]='À';dst[i++]='ﬂ';dst[i++]='≠';}
	if(strncmp((const char *)src,"Ç“ÇÂ",4)==0) {flag=1; dst[i++]='À';dst[i++]='ﬂ';dst[i++]='Æ';}
	if(strncmp((const char *)src,"Ç”Çü",4)==0) {flag=1; dst[i++]='Ã';dst[i++]='ß';}
	if(strncmp((const char *)src,"Ç”Ç°",4)==0) {flag=1; dst[i++]='Ã';dst[i++]='®';}
	if(strncmp((const char *)src,"Ç”Ç•",4)==0) {flag=1; dst[i++]='Ã';dst[i++]='™';}
	if(strncmp((const char *)src,"Ç”Çß",4)==0) {flag=1; dst[i++]='Ã';dst[i++]='´';}
	if(strncmp((const char *)src,"Ç›Ç•",4)==0) {flag=1; dst[i++]='–';dst[i++]='™';}
	if(strncmp((const char *)src,"Ç›Ç·",4)==0) {flag=1; dst[i++]='–';dst[i++]='¨';}
	if(strncmp((const char *)src,"Ç›Ç„",4)==0) {flag=1; dst[i++]='–';dst[i++]='≠';}
	if(strncmp((const char *)src,"Ç›ÇÂ",4)==0) {flag=1; dst[i++]='–';dst[i++]='Æ';}
	if(strncmp((const char *)src,"ÇËÇ•",4)==0) {flag=1; dst[i++]='ÿ';dst[i++]='™';}
	if(strncmp((const char *)src,"ÇËÇ·",4)==0) {flag=1; dst[i++]='ÿ';dst[i++]='¨';}
	if(strncmp((const char *)src,"ÇËÇ„",4)==0) {flag=1; dst[i++]='ÿ';dst[i++]='≠';}
	if(strncmp((const char *)src,"ÇËÇÂ",4)==0) {flag=1; dst[i++]='ÿ';dst[i++]='Æ';}
	if(strncmp((const char *)src,"ÉJÅK",4)==0) {flag=1; dst[i++]='∂';dst[i++]='ﬁ';}
	if(strncmp((const char *)src,"ÉLÅK",4)==0) {flag=1; dst[i++]='∑';dst[i++]='ﬁ';}
	if(strncmp((const char *)src,"ÉNÅK",4)==0) {flag=1; dst[i++]='∏';dst[i++]='ﬁ';}
	if(strncmp((const char *)src,"ÉPÅK",4)==0) {flag=1; dst[i++]='π';dst[i++]='ﬁ';}
	if(strncmp((const char *)src,"ÉRÅK",4)==0) {flag=1; dst[i++]='∫';dst[i++]='ﬁ';}
	}

// match 2chars
	if(flag==0){
	if(strncmp((const char *)src,"Ç†",2)==0) {flag=1; dst[i++]='±';}
	if(strncmp((const char *)src,"Ç¢",2)==0) {flag=1; dst[i++]='≤';}
	if(strncmp((const char *)src,"Ç§",2)==0) {flag=1; dst[i++]='≥';}
	if(strncmp((const char *)src,"Ç¶",2)==0) {flag=1; dst[i++]='¥';}
	if(strncmp((const char *)src,"Ç®",2)==0) {flag=1; dst[i++]='µ';}
	if(strncmp((const char *)src,"Ç©",2)==0) {flag=1; dst[i++]='∂';}
	if(strncmp((const char *)src,"Ç´",2)==0) {flag=1; dst[i++]='∑';}
	if(strncmp((const char *)src,"Ç≠",2)==0) {flag=1; dst[i++]='∏';}
	if(strncmp((const char *)src,"ÇØ",2)==0) {flag=1; dst[i++]='π';}
	if(strncmp((const char *)src,"Ç±",2)==0) {flag=1; dst[i++]='∫';}
	if(strncmp((const char *)src,"Ç≥",2)==0) {flag=1; dst[i++]='ª';}
	if(strncmp((const char *)src,"Çµ",2)==0) {flag=1; dst[i++]='º';}
	if(strncmp((const char *)src,"Ç∑",2)==0) {flag=1; dst[i++]='Ω';}
	if(strncmp((const char *)src,"Çπ",2)==0) {flag=1; dst[i++]='æ';}
	if(strncmp((const char *)src,"Çª",2)==0) {flag=1; dst[i++]='ø';}
	if(strncmp((const char *)src,"ÇΩ",2)==0) {flag=1; dst[i++]='¿';}
	if(strncmp((const char *)src,"Çø",2)==0) {flag=1; dst[i++]='¡';}
	if(strncmp((const char *)src,"Ç¬",2)==0) {flag=1; dst[i++]='¬';}
	if(strncmp((const char *)src,"Çƒ",2)==0) {flag=1; dst[i++]='√';}
	if(strncmp((const char *)src,"Ç∆",2)==0) {flag=1; dst[i++]='ƒ';}
	if(strncmp((const char *)src,"Ç»",2)==0) {flag=1; dst[i++]='≈';}
	if(strncmp((const char *)src,"Ç…",2)==0) {flag=1; dst[i++]='∆';}
	if(strncmp((const char *)src,"Ç ",2)==0) {flag=1; dst[i++]='«';}
	if(strncmp((const char *)src,"ÇÀ",2)==0) {flag=1; dst[i++]='»';}
	if(strncmp((const char *)src,"ÇÃ",2)==0) {flag=1; dst[i++]='…';}
	if(strncmp((const char *)src,"ÇÕ",2)==0) {flag=1; dst[i++]=' ';}
	if(strncmp((const char *)src,"Ç–",2)==0) {flag=1; dst[i++]='À';}
	if(strncmp((const char *)src,"Ç”",2)==0) {flag=1; dst[i++]='Ã';}
	if(strncmp((const char *)src,"Ç÷",2)==0) {flag=1; dst[i++]='Õ';}
	if(strncmp((const char *)src,"ÇŸ",2)==0) {flag=1; dst[i++]='Œ';}
	if(strncmp((const char *)src,"Ç‹",2)==0) {flag=1; dst[i++]='œ';}
	if(strncmp((const char *)src,"Ç›",2)==0) {flag=1; dst[i++]='–';}
	if(strncmp((const char *)src,"Çﬁ",2)==0) {flag=1; dst[i++]='—';}
	if(strncmp((const char *)src,"Çﬂ",2)==0) {flag=1; dst[i++]='“';}
	if(strncmp((const char *)src,"Ç‡",2)==0) {flag=1; dst[i++]='”';}
	if(strncmp((const char *)src,"Ç‚",2)==0) {flag=1; dst[i++]='‘';}
	if(strncmp((const char *)src,"Ç‰",2)==0) {flag=1; dst[i++]='’';}
	if(strncmp((const char *)src,"ÇÊ",2)==0) {flag=1; dst[i++]='÷';}
	if(strncmp((const char *)src,"ÇÁ",2)==0) {flag=1; dst[i++]='◊';}
	if(strncmp((const char *)src,"ÇË",2)==0) {flag=1; dst[i++]='ÿ';}
	if(strncmp((const char *)src,"ÇÈ",2)==0) {flag=1; dst[i++]='Ÿ';}
	if(strncmp((const char *)src,"ÇÍ",2)==0) {flag=1; dst[i++]='⁄';}
	if(strncmp((const char *)src,"ÇÎ",2)==0) {flag=1; dst[i++]='€';}
	if(strncmp((const char *)src,"ÇÌ",2)==0) {flag=1; dst[i++]='‹';}
	if(strncmp((const char *)src,"Ç",2)==0) {flag=1; dst[i++]='¶';}
	if(strncmp((const char *)src,"ÇÒ",2)==0) {flag=1; dst[i++]='›';}
	if(strncmp((const char *)src,"Ç™",2)==0) {flag=1; dst[i++]='∂'; dst[i++]='ﬁ'; }
	if(strncmp((const char *)src,"Ç¨",2)==0) {flag=1; dst[i++]='∑'; dst[i++]='ﬁ'; }
	if(strncmp((const char *)src,"ÇÆ",2)==0) {flag=1; dst[i++]='∏'; dst[i++]='ﬁ'; }
	if(strncmp((const char *)src,"Ç∞",2)==0) {flag=1; dst[i++]='π'; dst[i++]='ﬁ'; }
	if(strncmp((const char *)src,"Ç≤",2)==0) {flag=1; dst[i++]='∫'; dst[i++]='ﬁ'; }
	if(strncmp((const char *)src,"Ç¥",2)==0) {flag=1; dst[i++]='ª'; dst[i++]='ﬁ'; }
	if(strncmp((const char *)src,"Ç∂",2)==0) {flag=1; dst[i++]='º'; dst[i++]='ﬁ'; }
	if(strncmp((const char *)src,"Ç∏",2)==0) {flag=1; dst[i++]='Ω'; dst[i++]='ﬁ'; }
	if(strncmp((const char *)src,"Ç∫",2)==0) {flag=1; dst[i++]='æ'; dst[i++]='ﬁ'; }
	if(strncmp((const char *)src,"Çº",2)==0) {flag=1; dst[i++]='ø'; dst[i++]='ﬁ'; }
	if(strncmp((const char *)src,"Çæ",2)==0) {flag=1; dst[i++]='¿'; dst[i++]='ﬁ'; }
	if(strncmp((const char *)src,"Ç¿",2)==0) {flag=1; dst[i++]='¡'; dst[i++]='ﬁ'; }
	if(strncmp((const char *)src,"Ç√",2)==0) {flag=1; dst[i++]='¬'; dst[i++]='ﬁ'; }
	if(strncmp((const char *)src,"Ç≈",2)==0) {flag=1; dst[i++]='√'; dst[i++]='ﬁ'; }
	if(strncmp((const char *)src,"Ç«",2)==0) {flag=1; dst[i++]='ƒ'; dst[i++]='ﬁ'; }
	if(strncmp((const char *)src,"ÇŒ",2)==0) {flag=1; dst[i++]=' '; dst[i++]='ﬁ'; }
	if(strncmp((const char *)src,"Ç—",2)==0) {flag=1; dst[i++]='À'; dst[i++]='ﬁ'; }
	if(strncmp((const char *)src,"Ç‘",2)==0) {flag=1; dst[i++]='Ã'; dst[i++]='ﬁ'; }
	if(strncmp((const char *)src,"Ç◊",2)==0) {flag=1; dst[i++]='Õ'; dst[i++]='ﬁ'; }
	if(strncmp((const char *)src,"Ç⁄",2)==0) {flag=1; dst[i++]='Œ'; dst[i++]='ﬁ'; }
	if(strncmp((const char *)src,"Çœ",2)==0) {flag=1; dst[i++]=' '; dst[i++]='ﬂ'; }
	if(strncmp((const char *)src,"Ç“",2)==0) {flag=1; dst[i++]='À'; dst[i++]='ﬂ'; }
	if(strncmp((const char *)src,"Ç’",2)==0) {flag=1; dst[i++]='Ã'; dst[i++]='ﬂ'; }
	if(strncmp((const char *)src,"Çÿ",2)==0) {flag=1; dst[i++]='Õ'; dst[i++]='ﬂ'; }
	if(strncmp((const char *)src,"Ç€",2)==0) {flag=1; dst[i++]='Œ'; dst[i++]='ﬂ'; }
	if(strncmp((const char *)src,"Çü",2)==0) {flag=1; dst[i++]='ß';}
	if(strncmp((const char *)src,"Ç°",2)==0) {flag=1; dst[i++]='®';}
	if(strncmp((const char *)src,"Ç£",2)==0) {flag=1; dst[i++]='©';}
	if(strncmp((const char *)src,"Ç•",2)==0) {flag=1; dst[i++]='™';}
	if(strncmp((const char *)src,"Çß",2)==0) {flag=1; dst[i++]='´';}
	if(strncmp((const char *)src,"Ç¡",2)==0) {flag=1; dst[i++]='Ø';}
	if(strncmp((const char *)src,"Ç·",2)==0) {flag=1; dst[i++]='¨';}
	if(strncmp((const char *)src,"Ç„",2)==0) {flag=1; dst[i++]='≠';}
	if(strncmp((const char *)src,"ÇÂ",2)==0) {flag=1; dst[i++]='Æ';}
	}
// match 1char
	if(flag==0){
	if(strncmp((const char *)src,"R",1)==0) {flag=1; dst[i++]=' ';}
	}
// no match
	if(flag==0){
		flag=1;
		dst[i++]='?';
	}

	dst[i]='\0';

}

int convnotenum(int src){
	int dst;
	switch(src){
		case 46: dst=246 ; break;
		case 47: dst=232 ; break;
		case 48: dst=218 ; break;
		case 49: dst=206 ; break;
		case 50: dst=194 ; break;
		case 51: dst=182 ; break;
		case 52: dst=172 ; break;
		case 53: dst=162 ; break;
		case 54: dst=152 ; break;
		case 55: dst=143 ; break;
		case 56: dst=135 ; break;
		case 57: dst=127 ; break;
		case 58: dst=119 ; break;
		case 59: dst=112 ; break;
		case 60: dst=105 ; break;
		case 61: dst=99 ; break;
		case 62: dst=93 ; break;
		case 63: dst=87 ; break;
		case 64: dst=82 ; break;
		case 65: dst=77 ; break;
		case 66: dst=72 ; break;
		case 67: dst=68 ; break;
		case 68: dst=64 ; break;
		case 69: dst=60 ; break;
		case 70: dst=56 ; break;
		case 71: dst=52 ; break;
		case 72: dst=49 ; break;
		case 73: dst=46 ; break;
		case 74: dst=43 ; break;
		case 75: dst=40 ; break;
		case 76: dst=37 ; break;
		case 77: dst=35 ; break;
		case 78: dst=32 ; break;
		case 79: dst=30 ; break;
		case 80: dst=28 ; break;
		case 81: dst=26 ; break;
		case 82: dst=24 ; break;
		case 83: dst=22 ; break;
		case 84: dst=21 ; break;
		case 85: dst=19 ; break;
		case 86: dst=18 ; break;
		case 87: dst=16 ; break;
		case 88: dst=15 ; break;
		case 89: dst=14 ; break;
		case 90: dst=12 ; break;
		case 91: dst=11 ; break;
		case 92: dst=10 ; break;
		case 93: dst=9 ; break;
		case 94: dst=8 ; break;
		case 95: dst=7 ; break;
		case 96: dst=7 ; break;
		case 97: dst=6 ; break;
		case 98: dst=5 ; break;
		case 99: dst=4 ; break;
		case 100: dst=4 ; break;
		case 101: dst=3 ; break;
		case 102: dst=2 ; break;
		case 103: dst=2 ; break;
		case 104: dst=1 ; break;
		case 105: dst=1 ; break;
		case 106: dst=0 ; break;
		case 107: dst=0 ; break;
		default: dst=255 ; break;
	}

	return dst;

}

int convlength(int tempo, int length, int notenum, unsigned char *lylic){
	float dst;
	int freq;
	if(lylic[0]==' '){
		dst=(60000/(float)tempo)*((float)length/1920)*4;
	}else{
		switch(notenum){
			case 0 : freq=16; break;
			case 1 : freq=17; break;
			case 2 : freq=18; break;
			case 3 : freq=19; break;
			case 4 : freq=21; break;
			case 5 : freq=22; break;
			case 6 : freq=23; break;
			case 7 : freq=24; break;
			case 8 : freq=26; break;
			case 9 : freq=28; break;
			case 10 : freq=29; break;
			case 11 : freq=31; break;
			case 12 : freq=33; break;
			case 13 : freq=35; break;
			case 14 : freq=37; break;
			case 15 : freq=39; break;
			case 16 : freq=41; break;
			case 17 : freq=44; break;
			case 18 : freq=46; break;
			case 19 : freq=49; break;
			case 20 : freq=52; break;
			case 21 : freq=55; break;
			case 22 : freq=58; break;
			case 23 : freq=62; break;
			case 24 : freq=65; break;
			case 25 : freq=69; break;
			case 26 : freq=73; break;
			case 27 : freq=78; break;
			case 28 : freq=82; break;
			case 29 : freq=87; break;
			case 30 : freq=92; break;
			case 31 : freq=98; break;
			case 32 : freq=104; break;
			case 33 : freq=110; break;
			case 34 : freq=117; break;
			case 35 : freq=123; break;
			case 36 : freq=131; break;
			case 37 : freq=139; break;
			case 38 : freq=147; break;
			case 39 : freq=156; break;
			case 40 : freq=165; break;
			case 41 : freq=175; break;
			case 42 : freq=185; break;
			case 43 : freq=196; break;
			case 44 : freq=208; break;
			case 45 : freq=220; break;
			case 46 : freq=233; break;
			case 47 : freq=247; break;
			case 48 : freq=262; break;
			case 49 : freq=277; break;
			case 50 : freq=294; break;
			case 51 : freq=311; break;
			case 52 : freq=330; break;
			case 53 : freq=349; break;
			case 54 : freq=370; break;
			case 55 : freq=392; break;
			case 56 : freq=415; break;
			case 57 : freq=440; break;
			case 58 : freq=466; break;
			case 59 : freq=494; break;
			case 60 : freq=523; break;
			case 61 : freq=554; break;
			case 62 : freq=587; break;
			case 63 : freq=622; break;
			case 64 : freq=659; break;
			case 65 : freq=698; break;
			case 66 : freq=740; break;
			case 67 : freq=784; break;
			case 68 : freq=831; break;
			case 69 : freq=880; break;
			case 70 : freq=932; break;
			case 71 : freq=988; break;
			case 72 : freq=1047; break;
			case 73 : freq=1109; break;
			case 74 : freq=1175; break;
			case 75 : freq=1245; break;
			case 76 : freq=1319; break;
			case 77 : freq=1397; break;
			case 78 : freq=1480; break;
			case 79 : freq=1568; break;
			case 80 : freq=1661; break;
			case 81 : freq=1760; break;
			case 82 : freq=1865; break;
			case 83 : freq=1976; break;
			case 84 : freq=2093; break;
			case 85 : freq=2217; break;
			case 86 : freq=2349; break;
			case 87 : freq=2489; break;
			case 88 : freq=2637; break;
			case 89 : freq=2794; break;
			case 90 : freq=2960; break;
			case 91 : freq=3136; break;
			case 92 : freq=3322; break;
			case 93 : freq=3520; break;
			case 94 : freq=3729; break;
			case 95 : freq=3951; break;
			case 96 : freq=4186; break;
			case 97 : freq=4435; break;
			case 98 : freq=4699; break;
			case 99 : freq=4978; break;
			case 100 : freq=5274; break;
			case 101 : freq=5588; break;
			case 102 : freq=5920; break;
			case 103 : freq=6272; break;
			case 104 : freq=6645; break;
			case 105 : freq=7040; break;
			case 106 : freq=7459; break;
			case 107 : freq=7902; break;
		}
		dst=(float)freq/((float)tempo/60)/(1920/(float)length)*4;
	}
	return (int)dst;

}
