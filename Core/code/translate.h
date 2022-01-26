#ifndef _translate_h
#define _translate_h
#include "config.h"
#include "display.h"

class CTranslate{
public:
	long data;
	char n_after_dot;

	struct COEFF{
		long binary;
		char n_after_dot;

		COEFF(){
			binary = 1;
			n_after_dot = 0;
		};
		COEFF(char *src, char size_src){
			binary = 1;
			n_after_dot = 0;
			int c = display.strnlen_lir(src, size_src);
			if (c > 0) {
				for (c--; src[c] != '.'; c--) {
					if (src[c] == '0')
						src[c] = '\0';
					else
						break;
				}
			}
			for (int i = 0; src[i] != '\0'; i++) {
				if (src[i] == '.') {
					i++;
					for (int j = i; src[j] != '\0'; j++) {
						n_after_dot++;
						i++;
					}
				}
			}
			display.delete_sym((unsigned char*)src, size_src, '.');
			display.delete_sym((unsigned char*)src, size_src, '-');
			binary = strtol(src,nullptr,10);
			if(binary <= 0){
				binary = 1;
				n_after_dot = 0;
			}
		}
		bool operator==(const COEFF& obj){
			if(binary == obj.binary && n_after_dot == obj.n_after_dot)
				return true;
			else
				return false;
		}
		COEFF& operator=(const COEFF& obj){
			binary = obj.binary;
			n_after_dot = obj.n_after_dot;
			return *this;
		}
		long operator*(int64_t data){
			long znam = 1;
			for(int c = 0; c < n_after_dot;c++)
				znam *= 10;
			data *= binary; //умножаем на числитель			
			data /= znam;// делим на знаменатель
			return data;
		}
		long operator/(int64_t data){
			long znam = 1;		
			for(int c = 0; c < n_after_dot;c++)
				znam *= 10;
			data *= znam; //умножаем на знаменатель
			data /= binary; // делим на числитель
			return data;
		}
	};

	COEFF new_coeff_[N_AXIS];
	COEFF correction;
	COEFF coeff_corr[N_AXIS];
	

	void set_axis_coeff(unsigned char n_channel,long bin,unsigned char dot){
 		switch (n_channel){
			case 0:new_coeff_[0].binary=bin;new_coeff_[0].n_after_dot=dot;break;
			case 1:new_coeff_[1].binary=bin;new_coeff_[1].n_after_dot=dot;break;
			case 2:new_coeff_[2].binary=bin;new_coeff_[2].n_after_dot=dot;break;
			case 3:new_coeff_[3].binary=bin;new_coeff_[3].n_after_dot=dot;break;
			case 4:correction.binary=bin;correction.n_after_dot=dot;break;
		 }
	}

	struct GAP_NEW{
		long binary;
	} new_gap[N_AXIS];
	void set_axis_gap(unsigned char n_channel,long bin){
		switch (n_channel){
			case 0:new_gap[0].binary=bin;break;
			case 1:new_gap[1].binary=bin;break;
			case 2:new_gap[2].binary=bin;break;
			case 3:new_gap[3].binary=bin;break;
		}
	}

	enum MULT_OR_DIV_ANSWER{
		OK=(unsigned char)0,
		N_AFTER_DOT_TOO_BIG_MULT=(unsigned char)1,
		RESULT_IS_TOO_BIG_MULT=(unsigned char)2,
		N_AFTER_DOT_TOO_BIG_DIV=(unsigned char)3,
		RESULT_IS_TOO_BIG_DIV=(unsigned char)4,
		COEFF_IS_ZERO_DIV=(unsigned char)5,
		NO_CHANNEL=(unsigned char)6,
	};
	enum BIGDIGIT {
		LONG_MIN = -2147483647L,
		LONG_MAX = 2147483647L,
    };
};
//derive class:base class
class CDouble : public CTranslate{     //
public:

//   static unsigned char  int_error;
   MULT_OR_DIV_ANSWER answer;
   inline bool translate_(char *src, char size_src){
//04.03.2013 it will work only for axis
//if 7 n_a_p : 0.0097789-99.9999999
//if 6 n_a_p : 0.009779-99.999999
//if 5 n_a_p : 0.00978-99.99999
//if 4 n_a_p : 0.0098-99.9999
//if 3 n_a_p : 0.010-99.999
//if 2 n_a_p : 0.01-99.99
//if 1 n_a_p : 0.1-99.9
//if 0 n_a_p : 1-99
//     input check
       long res=0,ch_;
       char ch,inter[11];
       unsigned char i=0,p=0,d=0,s=0,m=0;
       bool dot=false;
       n_after_dot = 0;
       data=1;
       unsigned char chx;
       for (;d<11;d++)inter[d]=0x00;
       d=0x00;
       if (size_src == 0){data=1; return false;}
//string's  parsing
      while( i < size_src) {
        ch= src[i];
        if (ch==0x00)break;
        if (ch==0x2e)dot=true;
        if(dot==false)
        {
//only digit can be input before dot
          if(ch>=0x30 && ch<=0x39){d++;
//all zero left from dot before first digit is ignored
          if (m==0x00){
          if (ch!=0x30)inter[m++]= ch;
          else d--;   }
          else inter[m++]= ch;
          i++;
          }
         else {data=1; return false;}
//only 2 digit can be before dot
         if (d>2){data=1;return false;}
        }
        if (dot==true){
        if(ch>=0x30 && ch<=0x39)
        { inter[m++]= ch;
          i++; p++;
//04.05.2018		  
//only 7 digit after dot is allowed if 10.-99.
//only 8 digit up to 10.
          if (p>=0x09){inter[--m]= 0x00;p--;break;}
        }
        else {
          if (ch==0x2e) {s++; i++;}
//only one dot is allowed
        if (s>=0x02) {data=1; return false;}
//if another letter is coming
        if(ch!=0x2e) {data=1; return false;}
                      }
      }
      }
//d is number of digit before dot
//p is number digit behind dot
// inter[] = ddppppppp
// 99 9999999 = 0x 3b 9a c9 ff
// total string's size
      i=d+p;
//look for last digit to the right
      for (m=0,s=d;s<i+1;s++)
      {
       if (inter[s]==0x30)m++;
       if (inter[s]==0x00)break;
      }
//it means this .0000000
      if (m==p)p=0;
//it must be resolved  .0010100
      if (p>0){p++;
       for (;s!=0x00;)
       { chx=inter[s--];
      //   if(chx!=0x30&&chx!=0x00)break;
      //   else p--;
        if(chx==0x30 || chx==0x00)p--;
        if (chx==0x31||chx==0x32||chx==0x33||chx==0x34||chx==0x35\
            ||chx==0x36||chx==0x37||chx==0x38||chx==0x39) break;
      }
      }
// total string's size
      i=d+p;
      if (d==0x00 && p==0x00)
      { data=1; return false;}
//to take binary from string...
      for (s=0;i!=0x00;i--,s++){
            ch=inter[i-1]-0x30;
            ch_=ch;
      switch (s){
      case 0x00:res=res+ch_*1;break;
      case 0x01:res=res+ch_*10;break;
      case 0x02:res=res+ch_*100;break;
      case 0x03:res=res+ch_*1000;break;
      case 0x04:res=res+ch_*10000;break;
      case 0x05:res=res+ch_*100000;break;
      case 0x06:res=res+ch_*1000000;break;
      case 0x07:res=res+ch_*10000000;break;
      case 0x08:res=res+ch_*100000000;break;
      default:break;
               }
      }
//limitation check
     /* switch(p){
	  case 0x08: if(res<5000001)res=5000001;break;	  
      case 0x07: if (res < 500001)res=500001;break;
      case 0x06: if (res < 50001) res=50001;break;
      case 0x05: if (res < 5001)  res=5001;break;
      case 0x04: if (res < 501)   res=501;break;
      case 0x03: if (res <= 49)  { res=5;p=2;}break;
	  case 0x02: if (res <= 4)  { res=5;p=2;}break;	  
      default: break;
      } */
	 
      data=res;
      n_after_dot=p;
      return true;
      }
//30.01.2018
//#pragma GCC push_options
//#pragma GCC optimize ("O0")
long get_coeff_bin(unsigned char n_axis){
		
		switch (n_axis){
			case 0:
			return new_coeff_[0].binary;
			break;
			case 1:
			return new_coeff_[1].binary;
			break;
			case 2:
			return new_coeff_[2].binary;
			break;
			case 3:
			return new_coeff_[3].binary;
			break;
			default:
			return 1;
		}
	}

char get_coeff_n_after_dot(unsigned char n_axis){

		switch (n_axis){
			case 0:
			return new_coeff_[0].n_after_dot;
			break;
			case 1:
			return new_coeff_[1].n_after_dot;
			break;
			case 2:
			return new_coeff_[2].n_after_dot;
			break;
			case 3:
			return new_coeff_[3].n_after_dot;
			break;
			default:
			return 0;
		}
	}

MULT_OR_DIV_ANSWER mult(unsigned char n_axis,long *_data,unsigned char n_a_d_in = 0,long bin_in = 0){
//19.02.2018 worst case it takes 2423*31.25=75718.75 ns	
 long long result,b,c;
 long coeff_,int_res;
 unsigned char e;
 bool nine=false;	
	switch (n_axis){
		case 0:coeff_ = new_coeff_[0].binary;
		e = new_coeff_[0].n_after_dot;	
		break;
		case 1:coeff_ = new_coeff_[1].binary;
		e = new_coeff_[1].n_after_dot;	
		break;
		case 2:coeff_=new_coeff_[2].binary;
		e=new_coeff_[2].n_after_dot;		
		break;
		case 3:coeff_=new_coeff_[3].binary;
		e=new_coeff_[3].n_after_dot;		
		break;
		case 4:coeff_=correction.binary;
		e=correction.n_after_dot;
		nine=true;
		break;
		case 255:
		coeff_=bin_in;
		e = n_a_d_in;
		break;
		//case 5:coeff_=speed_measure.binary;
		//e=speed_measure.n_after_dot;
		//break;
		default:
		return MULT_OR_DIV_ANSWER::NO_CHANNEL;
	}
//		if(e>7) {answer=MULT_OR_DIV_ANSWER::N_AFTER_DOT_TOO_BIG_MULT;return answer;}
//02.05.2018	
	  if(e>8 && nine==false) {
		answer=MULT_OR_DIV_ANSWER::N_AFTER_DOT_TOO_BIG_MULT;
		return answer;
	  }
	  else if(e>9 && nine==true){
		answer=MULT_OR_DIV_ANSWER::N_AFTER_DOT_TOO_BIG_MULT;
		return answer;
	  }
	  result = *_data;
      result = result * coeff_;
//to make round...
           if(e!=0){
		        e=e-1;
		        if (result>  BIGDIGIT::LONG_MAX || result<  BIGDIGIT::LONG_MIN){
			        switch(e){
				        case 0: break;
				        case 1:result=result/10;break;
				        case 2:result=result/100;break;
				        case 3:result=result/1000;break;
				        case 4:result=result/10000;break;
				        case 5:result=result/100000;break;
				        case 6:result=result/1000000;break;
				        case 7:result=result/10000000;break;
				        case 8:result=result/100000000;break;
				        //  case 9: result=result/1000000000;break;
				        //mistake n_a_p operator
				        default:{answer=MULT_OR_DIV_ANSWER::N_AFTER_DOT_TOO_BIG_MULT;
							     return answer;}
			        }
		        }
		        else{int_res=result;
			        switch(e){
				        case 0: break;
				        case 1:int_res=int_res/10;break;
				        case 2:int_res=int_res/100;break;
				        case 3:int_res=int_res/1000;break;
				        case 4:int_res=int_res/10000;break;
				        case 5:int_res=int_res/100000;break;
				        case 6:int_res=int_res/1000000;break;
				        case 7:int_res=int_res/10000000;break;
				        case 8:int_res=int_res/100000000;break;
				        //  case 9: result=result/1000000000;break;
				        //mistake n_a_p operator
				        default:{answer=MULT_OR_DIV_ANSWER::N_AFTER_DOT_TOO_BIG_MULT;
						         return answer;}
			        }
			        result=int_res;
		        }
		        b=result;
		        if (result> BIGDIGIT::LONG_MAX || result< BIGDIGIT::LONG_MIN)
					result=result/10;
		        else {
					int_res=result;
					int_res=int_res/10; 
					result=int_res;
		        }
		        c=result;
		        c=c*10;
		        b=b-c;
		        //work with negative...
				//первый дискрет в два раза меньше потом нормально, т е происходит смещение 
		       /* if(b<0)
		        {
					b=-b;
			        if(b>4)
					result--;
		        }
		        else {
					if(b>4)
					result++;
				}*/
				//===
				}
	        if (result> BIGDIGIT::LONG_MAX || result< BIGDIGIT::LONG_MIN){
				answer=MULT_OR_DIV_ANSWER::RESULT_IS_TOO_BIG_MULT;
				return answer;
				}
	        else 
				*_data=result;
	        return MULT_OR_DIV_ANSWER::OK;	
		
}

/*
функция для деления
bool hard_round  - грубое округление: при делении 1/2 получается 1 , 0 - включено по умолчанию ,1 выключено
in unsigned char n_axis - номер канала , если этот параметр равен 255 то деление производится на входящий в функцию делитель
in long bin_in - делитель
in unsigned char n_a_d_in - кол - во разрядов делителя после запятой 
out long *_data - указатель на результат деления
*/
MULT_OR_DIV_ANSWER div(unsigned char n_axis,long *_data,unsigned char n_a_d_in = 0,long bin_in = 0,bool hard_round = 0){
//19.02.2018 worst case it takes 3795*31.25=118593.75 ns	
	long long result,b,c;
	long coeff_,int_res;
	unsigned char e;
	switch (n_axis){
		case 0:coeff_=new_coeff_[0].binary;
		e=new_coeff_[0].n_after_dot;
		break;
		case 1:coeff_=new_coeff_[1].binary;
		e=new_coeff_[1].n_after_dot;
		break;
		case 2:coeff_=new_coeff_[2].binary;
		e=new_coeff_[2].n_after_dot;
		break;
		case 3:coeff_=new_coeff_[3].binary;
		e=new_coeff_[3].n_after_dot;
		break;
		case 4:coeff_=correction.binary;
		e=correction.n_after_dot;
		//nine=true;
		break;
		case 255:
		coeff_=bin_in;
		e = n_a_d_in;
		break;
		default:return MULT_OR_DIV_ANSWER::NO_CHANNEL;
	}
//	if(e>7) {answer=MULT_OR_DIV_ANSWER::N_AFTER_DOT_TOO_BIG_DIV;return answer;}
//02.05.2018
	if(e>8) {answer=MULT_OR_DIV_ANSWER::N_AFTER_DOT_TOO_BIG_DIV;return answer;}
	result=*_data;
	        if (coeff_==0) {answer=MULT_OR_DIV_ANSWER::COEFF_IS_ZERO_DIV;return answer;}
 //to make round...
	        e=e+1;
	        switch(e)
	        {
		        case 0: break;
		        case 1: result=result*10;break;
		        case 2: result=result*100;break;
		        case 3: result=result*1000;break;
		        case 4: result=result*10000;break;
		        case 5: result=result*100000;break;
		        case 6: result=result*1000000;break;
		        case 7: result=result*10000000;break;
		        case 8: result=result*100000000;break;
		        case 9: result=result*1000000000;break;
		        //mistake n_a_p operator
		        default:{answer=MULT_OR_DIV_ANSWER::N_AFTER_DOT_TOO_BIG_DIV;return answer;}
	        } 
	        if (result> BIGDIGIT::LONG_MAX || result< BIGDIGIT::LONG_MIN){ 
				result=result/coeff_;
		        c=result;
		        result=result/10;
		        b=result;
		        b=b*10;
	        }
	        else {
				int_res=result;
		        int_res=int_res/coeff_;
		        c=int_res;
		        int_res=int_res/10;
		        result=int_res;
		        int_res=int_res*10;
		        b=int_res;
				}
	        c=c-b;
	        //work with negative...
			// грубое округление: при делении 1/2 получается 1
			if(!hard_round){
				 if(c<0){
					c=-c;
					if(c>4)
					 result--;
				 }
				 else {
					 if(c>4)
					 result++;
				 }
			}
			//====== 			
	        // result is out of range
			if (result> BIGDIGIT::LONG_MAX || result< BIGDIGIT::LONG_MIN){
				answer=MULT_OR_DIV_ANSWER::RESULT_IS_TOO_BIG_DIV;
				return answer; 
				}
			else 
				*_data=result;

	        return MULT_OR_DIV_ANSWER::OK;
	}
};
extern CDouble coeff;

#endif
