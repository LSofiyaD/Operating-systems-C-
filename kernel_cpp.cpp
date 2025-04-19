__asm("jmp kmain");

#define VIDEO_BUF_PTR (0xb8000)
#define IDT_TYPE_INTR (0x0E)
#define IDT_TYPE_TRAP (0x0F)
#define GDT_CS (0x8)
#define CURSOR_PORT (0x3D4)
#define VIDEO_WIDTH (80)
#define PIC1_PORT (0x20)
#define NULL 0
#define VIDEO_HIGH 25
#include <stdint.h>

unsigned int pos=0;
unsigned int counter=0;
int c=0x0A;
int number = 0;
char str[41];

void calculator (char* new_str);
void cursor_moveto(unsigned int strnum, unsigned int pos);

const char alphabet[58]={0 , 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '+', 0, 0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i','o', 'p', '[', ']', 0, 0, 'a', 's', 'd', 'f', 'g', 'h','j', 'k', 'l', ':', '\'', '`', 0, '\\', 'z', 'x', 'c','v', 'b', 'n', 'm', ',', '.', '/', 0,'*',0,' '};




struct idt_entry
{
	unsigned short base_lo;
	unsigned short segm_sel;
	unsigned char always0;
	unsigned char flags;
	unsigned short base_hi;
} __attribute__((packed));

struct idt_ptr
{
	unsigned short limit;
	unsigned int base;
} __attribute__((packed));

struct idt_entry g_idt[256];
struct idt_ptr g_idtp;




extern "C" void keyb_handler(void);
extern "C" void keyb_process_keys(void);
extern "C" void default_intr_handler(void);

__asm__(".global default_intr_handler\n"
	"default_intr_handler:\n\t"
	"iret");

/* Define a keyboard handler stub that makes a call to a C function */
__asm__(".global keyb_handler\n"
	"keyb_handler:\n\t"
	"cld\n\t"                    /* Set direction flag forward for C functions */
	"pusha\n\t"                  /* Save all the registers */
	"call keyb_process_keys\n\t"
	"popa\n\t"                   /* Restore all the registers */
	"iret");
typedef void (*intr_handler)();

void intr_reg_handler(int num, unsigned short segm_sel, unsigned char flags, intr_handler hndlr) {
	unsigned int hndlr_addr = (unsigned int)hndlr;
	g_idt[num].base_lo = (unsigned short)(hndlr_addr & 0xFFFF);
	g_idt[num].segm_sel = segm_sel;
	g_idt[num].always0 = 0;
	g_idt[num].flags = flags;
	g_idt[num].base_hi = (unsigned short)(hndlr_addr >> 16);
}

void intr_init() {
	int i;
	int idt_count = sizeof(g_idt) / sizeof(g_idt[0]);
	for (i = 0; i < idt_count; i++)
		intr_reg_handler(i, GDT_CS, 0x80 | IDT_TYPE_INTR,
			default_intr_handler);
}

void intr_start() {
	int idt_count = sizeof(g_idt) / sizeof(g_idt[0]);
	g_idtp.base = (unsigned int)(&g_idt[0]);
	g_idtp.limit = (sizeof(struct idt_entry) * idt_count) - 1;
	asm("lidt %0" : : "m" (g_idtp));
}

void intr_enable() {
	asm("sti");
}

void intr_disable() {
	asm("cli");
}




void out_str(int color, const char* ptr, unsigned int strnum) {
	
	unsigned char* video_buf = (unsigned char*)VIDEO_BUF_PTR + 80 * 2 * strnum;
        
	while (*ptr)
	{
		video_buf[0] = (unsigned char)*ptr;
		video_buf[1] = color;
		video_buf += 2;
		ptr++;
	}
	if(counter>24){
	    unsigned char* video_buf = (unsigned char*)VIDEO_BUF_PTR;
	    for(int i = 0; i < VIDEO_WIDTH*25; i++) {  
		video_buf[0]=' ';
		video_buf[1]=c;
		video_buf+=2;
		//*(video_buf + i*2) = '\0';
            }
            counter=0;
            pos=0;
            cursor_moveto(counter,pos);
	    const char* hello = "Welcome to CalcOS (gcc edition)!";
	    out_str(c, hello, 0);
	    if (counter==25){counter=1;}
	    
	    //const char* new_line = "# ";
	    //out_str(c, new_line, counter);
            //cursor_moveto(counter,pos);
	}
}
void char_out(const char* ptr) {
	
	unsigned char* video_buf = (unsigned char*)VIDEO_BUF_PTR + 2 * (80 * counter) + pos * 2;
	video_buf[0] = (unsigned char)(*ptr);
	video_buf[1] = c;
}
static inline unsigned char inb(unsigned short port) {
	unsigned char data;
	asm volatile ("inb %w1, %b0" : "=a" (data) : "Nd" (port));
	return data;
}

static inline void outb(unsigned short port, unsigned char data) {
	asm volatile ("outb %b0, %w1" : : "a" (data), "Nd" (port));
}
static inline void outw(unsigned short port, unsigned short data) {
	asm volatile ("outw %w0, %w1" : : "a" (data), "Nd" (port));
}

void cursor_moveto(unsigned int strnum, unsigned int pos) {
	unsigned short new_pos = (strnum * VIDEO_WIDTH) + pos;
	outb(CURSOR_PORT, 0x0F);
	outb(CURSOR_PORT + 1, (unsigned char)(new_pos & 0xFF));
	outb(CURSOR_PORT, 0x0E);
	outb(CURSOR_PORT + 1, (unsigned char)((new_pos >> 8) & 0xFF));
}


void shutdown(void) {
	outw(0xB004, (unsigned short)0x2000); // qemu < 1.7, ex. 1.6.2
	outw(0x604, (unsigned short)0x2000);  // qemu >= 1.7  
}


int size(const char* size_str){
    int i=0;
    while(size_str[i]!='\0'|| size_str[i]!=' '){
       i++;
    }
    return i;
}
char i_t_c(int num){
    if(num==1){
        return '1'; 
    }
    else if(num==2){
        return '2'; 
    } 
    else if(num==3){
        return '3'; 
    }
    else if(num==4){
        return '4'; 
    }
    else if(num==5){
        return '5'; 
    }
    else if(num==6){
        return '6'; 
    }
    else if(num==7){
        return '7'; 
    }
    else if(num==8){
        return '8'; 
    }
    else if(num==9){
        return '9'; 
    }
    else {return '0'; }
}

int c_t_i(char ch) {
    if (ch == '1') { return 1; }
    else if (ch == '2') { return 2; }
    else if (ch == '3') { return 3; }
    else if (ch == '4') { return 4; }
    else if (ch == '5') { return 5; }
    else if (ch == '6') { return 6; }
    else if (ch == '7') { return 7; }
    else if (ch == '8') { return 8; }
    else if (ch == '9') { return 9; }
    else { return 0; }
}
int char_to_int(int start, int end, char* work_str) {

    int mn = 10;
    int help = end - start-1 ;
    if(help==-1){mn=1;}
    while (help > 0) {
        mn = mn * 10;
        help--;
        
    }
    int rezult = 0;
    int number = 0;
    while (start != end + 1) {
        number = c_t_i(work_str[start]);
        rezult = rezult + (mn * number);
        if(mn>=10){mn = mn / 10;}
        
        start++;
    }
    return rezult;
}
int over(int k, char* new_str){  //chek for overflow number
    int number=char_to_int(k,k+9,new_str);
    if(number>2147483647){
       counter++;
       out_str(c,"numbermore",counter);
       return 0;}
    else {return 1;}
}


int over1(int k, char* new_str){
     if(c_t_i(new_str[k])<2){return 1;}
     else if(c_t_i(new_str[k])>2){return 0;}
     else if(c_t_i(new_str[k+1])<1){return 1;}
     else if(c_t_i(new_str[k+1])>1){return 0;}
     else if(c_t_i(new_str[k+2])<4){return 1;}
     else if(c_t_i(new_str[k+2])>4){return 0;}
     else if(c_t_i(new_str[k+3])<7){return 1;}
     else if(c_t_i(new_str[k+3])>7){return 0;}
     else if(c_t_i(new_str[k+4])<4){return 1;}
     else if(c_t_i(new_str[k+4])>4){return 0;}
     else if(c_t_i(new_str[k+5])<8){return 1;}
     else if(c_t_i(new_str[k+5])>8){return 0;}
     else if(c_t_i(new_str[k+6])<3){return 1;}
     else if(c_t_i(new_str[k+6])>3){return 0;}
     else if(c_t_i(new_str[k+7])<6){return 1;}
     else if(c_t_i(new_str[k+7])>6){return 0;}
     else if(c_t_i(new_str[k+8])<4){return 1;}
     else if(c_t_i(new_str[k+8])>4){return 0;}
     else if(c_t_i(new_str[k+9])<7){return 1;}
     else if(c_t_i(new_str[k+9])>7){return 0;}
     else {return 1;}
}
int correct(char* new_str){
     int array[100];
     int c_ar=0;
     int i=0;
     int c_numb=0;
     for(i;new_str[i]!='\0';i++){
         if(new_str[i]=='/' && new_str[i+1]=='0'){   //situation /0
             counter++;
             out_str(c,"Error:division by 0",counter);
             return 0;
         }
         if(((new_str[i]=='/' || new_str[i]=='+' || new_str[i]=='*' || new_str[i]=='-') && new_str[i+1]=='\0')|| ((new_str[i]=='/' || new_str[i]=='*') && i==0) ){ //situation where +-+/ first or last
             counter++;
             out_str(c,"Invalid expression",counter);
             return 0;
         }                   //we rite index of numbers in array
         if(new_str[i]=='1' || new_str[i]=='2' || new_str[i]=='3' || new_str[i]=='4' || new_str[i]=='5' || new_str[i]=='6' || new_str[i]=='7' || new_str[i]=='8' || new_str[i]=='9' || new_str[i]=='0'){
             array[c_ar]=i;
             c_ar++;
         }    
     }
     for(int j=0;j<c_ar-1;j++){                         //situation:5647++857574
          if((array[j+1]-array[j])>2){
              counter++;
              out_str(c,"Error:expression is incorrect",counter);
              return 0;
          }
          if((array[j+1]-array[j])==1){          //situation overflow
                
                if(c_numb==8){
                     int k=j-8+1;    //start of big number
                     if(k==1){k=0;}
                     int fl2=over1(k,new_str);
                     if(fl2==0){
                          counter++;
                          out_str(c,"Error: integer overflow",counter);
                          return 0;
                     }
                }
               
                c_numb++;
          }
          else{c_numb=0;}
     }
     return 1;
}
int sr2(int array[10],char* new_str,int ww){   //array with index if operation; str;number of operation
     char second[21]={};
     int start=0;
     //counter++;
     int midl=array[ww];
     //char_out(&new_str[midl]);
     midl++;
     for(midl;new_str[midl]!='+' && new_str[midl]!='-'&& new_str[midl]!='*'&& new_str[midl]!='/'&& new_str[midl]!='\0';midl++){
     }
     int i=array[ww]+1;
     int j=0;
     for(i;i!=midl;i++){
         second[j]=new_str[i];
         j++;
     }
     second[j]='\0';
     
     int output2=char_to_int(0,j-1,second);
     return output2;        //i make second number from str tupe to int tupe
}
void binary (){counter++; out_str(c,"10",counter);}
int sr1(int array[10],char* new_str,int ww){   //array with index if operation; str;number of operation
     char first[21]={};
     int start=0;
     //counter++;
     int midl=array[ww];
     //char_out(&new_str[midl]);
     midl--;
     for(midl;new_str[midl]!='+' && new_str[midl]!='-'&& new_str[midl]!='*'&& new_str[midl]!='/'&& midl!=0;midl--){
          if(new_str[midl]=='-'&& midl==0){midl++;}
          
     }
     int i=0;
     for(midl;midl!=array[ww];midl++){
         first[i]=new_str[midl];
         i++;
     }
     first[i]='\0';
     int output1=char_to_int(0,i-1,first);
    
     return output1;        //i make first number from str tupe to int tupe
}

void rezult_in_str(int symb, int array[10], char* new_str, int rezult,int full,int binflag){

        int midl=array[symb];
        int i=midl-1;

        //if(rezult==23){
          // counter++;
           //out_str(c,"rez23",counter);
        //}
        char output_str[40];
        int j=0;
        int tail=0;
        int t=midl+1;
        if(full>1){
            for(i;new_str[i]!='-' && new_str[i]!='+' && new_str[i]!='*' && new_str[i]!='/' && i!=0;i--){}
            
            for(t;new_str[t]!='-' && new_str[t]!='+' && new_str[t]!='*' && new_str[t]!='/' && new_str[t]!='\0';t++){
                if(new_str[t]!='\0'){tail=1;}
            }
            for(j; j!=i+1 && i!=0;j++){
                output_str[j]=new_str[j];
            }
        }
        if(binflag==1){
            output_str[j]='-';
            j++;
        }
        int k=10;
        if(rezult/k>9){
           k=k*10;
        }
        if(rezult<10){k=1;}
        while(k>9){
            char help=i_t_c(rezult/k);
            output_str[j]=help;
            rezult=rezult-((rezult/k)*k);
            j++;
            k=k/10;
        }
        output_str[j]=i_t_c(rezult);
        j++;
        if(tail==0){output_str[j]='\0';}
        else{
            while(new_str[t]!='\0'){
                 output_str[j]=new_str[t];
                 j++;
                 t++;
            }
            output_str[j]='\0';
        }
        full--;
        if(full>0){
            //counter++;
            //out_str(c,"fuul>0",counter);
            calculator(output_str);
        }
        else {
            counter++;
            out_str(c,output_str,counter);
            
        }
}

void calculator (char* new_str){
    
    if((new_str[0]=='-'&& new_str[1]=='-')||(new_str[0]=='+'&& new_str[1]=='+')){
        int i=0;
        int j=0;
        for(i;(new_str[i]=='-')|| (new_str[i]=='+');i++){}
        if(i%2==0){
            while(new_str[i]!='\0'){
                 new_str[j]=new_str[i];
                 i++;
                 j++;
            }
            new_str[j]='\0';
        }
        else{
            i--;
            if(new_str[i]=='+'){i++;}
            while(new_str[i]!='\0'){
                 new_str[j]=new_str[i];
                 i++;
                 j++;
            }
            new_str[j]='\0';
        }
    }
    int mult[10];
    int c_m=0;
    int sum[10];
    int s_m=0;
    int del[10];
    int d_m=0;
    int razn[10];
    int r_m=0;
    for(int i=0;new_str[i]!='\0';i++){
          if(new_str[i]=='*'){
               mult[c_m]=i;
               c_m++;
          }
          if(new_str[i]=='/'){
               del[d_m]=i;
               d_m++;
          }
          if(new_str[i]=='+'){
               sum[s_m]=i;
               s_m++;
          }
          if(new_str[i]=='-'&& i!=0){
               razn[r_m]=i;
               r_m++;
          }
    }
    int full=r_m+s_m+d_m+c_m;
    int num_m=0;
    int binflag=0;
    if(c_m>0){
         if(new_str[0]=='-'){              //vork with bin minus? if it is, we make flag ON and remember if; we work with as usual
            binflag=1;
            int j=0;
            int i=1;
            while(new_str[i]!='\0'){
                 new_str[j]=new_str[i];
                 i++;
                 j++;
            }
            new_str[j]='\0';
            mult[num_m]=mult[num_m]-1;
         }
        
         int num1=sr1(mult,new_str,num_m);  //how many;array;str;with whitch we work
         int num2=sr2(mult,new_str,num_m);
        
         int rezult=num1*num2;
         rezult_in_str(num_m,mult,new_str,rezult,full,binflag);
         num_m++;
         c_m--;
    }
    int dum_m=0;
    if (d_m>0 && c_m==0 && num_m==0){
         //counter++;
         //out_str(c,new_str,counter);
         int num1=sr1(del,new_str,dum_m); 
         int num2=sr2(del,new_str,dum_m);
         int rezult=num1/num2;
         //full=0;
         rezult_in_str(dum_m,del,new_str,rezult,full,0);
         dum_m++;
         d_m--;  
    }
    int sum_m=0;
    if(s_m>0 && new_str[0]!='-'&& d_m==0 && dum_m==0 && num_m==0 && c_m==0){
        int num1=sr1(sum,new_str,sum_m); 
        int num2=sr2(sum,new_str,sum_m); 
        int rezult=num1+num2;
        rezult_in_str(sum_m,sum,new_str,rezult,0,0);
        sum_m++;
        s_m--;  
    }
    int ruzn_m=0;
    if((r_m>0 || (s_m>0 && new_str[0]=='-'))&& d_m==0 && dum_m==0 && num_m==0 && c_m==0){
        //counter++;
        //out_str(c,new_str,counter);
        if(s_m>0 && new_str[0]=='-'){
          int num1=sr1(sum,new_str,sum_m); 
          int num2=sr2(sum,new_str,sum_m); 
          int rezult=0;
          if(num1>num2){
              rezult=num1-num2;
              binflag=1;
          } 
          else {
              rezult=num2-num1;
              binflag=0;
          } 
          rezult_in_str(sum_m,sum,new_str,rezult,0,binflag); 
          sum_m++;
          s_m--;  
        }
        else {
          int num1=sr1(razn,new_str,ruzn_m); 
          int num2=sr2(razn,new_str,ruzn_m); 
          int rezult=0;
          if(num1>num2){
              rezult=num1-num2;
              binflag=0;
          } 
          else {
              rezult=num2-num1;
              binflag=1;
          } 
          rezult_in_str(ruzn_m,razn,new_str,rezult,0,binflag); 
        }
        ruzn_m++;
        r_m--;
    }
    
}
void expr(){
     
     char new_str[37];
     int h=0;
     if(str[4]==' '){h=5;}
     else{h=6;}
     int l=0;
     for(h;str[h]!='\0';h++){
          new_str[l]=str[h];
          l++;
     }
     new_str[l]='\0';
     //out_str(c,new_str,counter);
     int f=correct(new_str);   //str-good or we have mistakes
     if (f==1){
         //counter++;
         //out_str(c,"all is good",counter);
         calculator(new_str);
     }
     
}

void same(const char* string){
     const char* inf="info";
     const char* shut="shutdown";
     const char* exp="expr";
     int i=0;
     int flag=0;
     if(string[0]=='i'){
        for(i; inf[i]==string[i];i++)
        if(i==3 && string[4]=='\0'){
            counter++;
            out_str(c,"Calc OS: v.01. Developer: Liasovich Sophia, 5151004/30001, SpbPU, 2025",counter);
            counter++;
            out_str(c,"Compilers: bootloader: GNU, kernel: gcc",counter);
            counter++;
            const char* color_info;
            if(c==0x0A){
                color_info="Bootloader parameters: green color.";
            }
            else if(c==0x0B){
                color_info="Bootloader parameters: blue color.";
            }
            else if(c==0x0C){
                color_info="Bootloader parameters: red color.";
            }
            else if(c==0x0E){
                color_info="Bootloader parameters: yellow color.";
            }
            else if(c==0x07){
                color_info="Bootloader parameters: gray color.";
            }
            else if(c==0x0F){
                color_info="Bootloader parameters: white color.";
            }
            flag=1;
            out_str(c,color_info,counter);
        }
     }
     if(string[0]=='s'){
          for(i=0; shut[i]==string[i];i++)
          if(i==7 && string[8]=='\0'){
            //counter++;
            flag=1;
            //out_str(c,"shutdown",counter);
            shutdown();
           
          }      
     }
     if(string[0]=='e'){
          for(i=0; exp[i]==string[i];i++)
          if(i==3 && (string[4]==':'||string[4]==' ')){
            //counter++;
            flag=1;
            expr();
            
          }
              
     }
     else if (string[0]!='i'&& string[0]!='s'&& string[0]!='e') {
          counter++;
          flag=1;
          out_str(c,"Error: command not recognized.",counter);
     }
     if(flag==0){
         counter++;
         out_str(c,"Error: command not recognized.",counter);
     }
}


void on_key(unsigned char code) {
        char* video_buf = (char*)VIDEO_BUF_PTR;
	
	if (code == 14) {
	       if (number > 0) {
		number--;
		str[number] = '\0';
	       }
	       if (pos > 2) pos--;
	       char space[2] = " ";
	       char_out(space);
	       cursor_moveto(counter, pos);
	}
	else if (code == 28) {   //enter
	        str[number] = '\0';
	       
	        same(str);
		counter++;
		pos=2;
		cursor_moveto(counter,pos);
		
		const char* test= "# ";
	        out_str(c, test, counter);
	        *str={};
	        number=0;
	        return;
	}
	
	else {
	       char symbol;
	       if(code<58){
           	       symbol=alphabet[code];
               }
	       str[number]=symbol;
	       number++;
	       char_out(&symbol);
	       pos++;
	       cursor_moveto(counter,pos);
	}
}

void keyb_process_keys() {
	if (inb(0x64) & 0x01) {
		unsigned char scan_code;
		unsigned char state;
		scan_code = inb(0x60);
		if (scan_code < 128)
			on_key(scan_code);
	}
	outb(PIC1_PORT, 0x20);
}

void keyb_init() {
	intr_reg_handler(0x09, GDT_CS, 0x80 | IDT_TYPE_INTR, keyb_handler);
	outb(PIC1_PORT + 1, 0xFF ^ 0x02);
}
bool is_valid_char(uint8_t symbol)
{
    return (symbol >= '0' && symbol <= '9');  // Проверка, что символ — цифра
}

extern "C" int kmain() {
	unsigned char* video_buf = (unsigned char*)VIDEO_BUF_PTR;
       
        uint8_t symbol = *(uint8_t*)0x8e00;
       // Проверяем корректность символа
        if (is_valid_char(symbol))
        {
            if(symbol=='1'){
                 c=0x0A;
               
            }
            else if(symbol=='2'){
                 c=0x0B;
                 
            }
            else if(symbol=='3'){
                 c=0x0C;
                 
            }
            else if(symbol=='4'){
                 c=0x0E;
                 
            }
            else if(symbol=='5'){
                 c=0x07;
                
            }
            else if(symbol=='6'){
                 c=0x0F;
                
            }
          
        }
        for(int i = 0; i < VIDEO_WIDTH*25; i++)   
		*(video_buf + i*2) = '\0';
	const char* hello = "Welcome to CalcOS (gcc edition)!";
	out_str(c, hello, 0);
	counter=1;
	pos=2;
	const char* new_line = "# ";
	out_str(c, new_line, counter);
        cursor_moveto(counter,pos);
	intr_disable();
	intr_init();
	keyb_init();
	intr_start();
	intr_enable();

	/*while (1) {
		asm("hlt");
	}*/
	return 0;
}
