#include"reader.hpp"

void Reader::clear_status(){
    type=COMMENT;
    cmd.clear();
    label.clear();
    action.clear();
    action_old.clear();
    data.clear();
    comment.clear();
    if(this->version=="SIC")ni=0;
    else ni=3;
};

Reader::Reader(string FileName,int TabSpace,string version){
    this->file.open(FileName,std::ios::in);
    this->lines=0;
    this->TabSpace = TabSpace;
    this->version = version;
}
Reader::~Reader(){
    this->file.close();
}
string Reader::toUpper(string from){
    for(unsigned int i=0;i<from.length();i++){
        from.at(i)=toupper(from.at(i));
    }
    return from;
}
string Reader::replaceTab(string from){
    string ans,tmp;
    int txtLong=0;
    for(unsigned int i=0;i<from.length();i++){
        if(from.at(i)=='\t'){
            for(int j=0;j< this->TabSpace-(txtLong% this->TabSpace ) ;j++)ans.append(" ");

            txtLong+= this->TabSpace - (txtLong % this->TabSpace);
        }
        else{
            tmp=from.at(i);
            ans.append(tmp.c_str());
            txtLong++;
        }
    }
    return ans;
}
string Reader::removeHeaderSpace(string str){
    unsigned int i=0;
    for(i=0;i<str.length();i++)if(str.at(i)!=' ')break;
    return str.substr(i,str.length()-i);
}
bool Reader::nextLine(){
    string tmp,old;             //設定暫存
    int SpaceCounter=0;         //設定空白分段計數器為0
    bool SpaceFlag=false;       //設定空白旗標為false
    this->clear_status();       //清除buffer資料
    this->lines++;              //行數+1
    if(getline(this->file,old)){//獲取一行的資料
        old = this->replaceTab(old);//替換tab為空白鍵
        tmp = this->toUpper(old);   //轉成大寫

        for(unsigned int i=0;i<tmp.length();i++){               //逐字掃描
            if(i>=8&&i<15&&SpaceCounter<1)SpaceCounter=1;       //如果字數到了 自動把分段計數器加一
            else if(i>=18&&i<35&&SpaceCounter<2)SpaceCounter=2; //如果字數到了 自動把分段計數器加一
            else if(i>=35&&SpaceCounter<3)SpaceCounter=3;       //如果字數到了 自動把分段計數器加一
            if(tmp.at(i)==' '){                                 //如果遇到空白
                if(SpaceFlag==false){                               //若前面都沒遇過空白
                    SpaceFlag=true;                                     //設空白旗標為true
                    if(SpaceCounter<3)SpaceCounter++;                   //空白計數器+1
                }
                else{                                               //如果前面有遇過空白
                    if(SpaceCounter==3)this->comment+=old.at(i);    //但是這空白是在註解部分 直接存入buffer
                }
            }
            else{                                               //如果遇到的不是空白
                if(tmp.at(i)=='.'){                                 //是點
                    SpaceCounter = 3;                                   //直接把分段計數器設成comment
                    SpaceFlag=true;                                     //並且允許空白鍵
                }
                if(SpaceCounter==0){                                //如果SpaceCounter是label
                    this->label+=old.at(i);                             //存入label
                    SpaceFlag=false;                                    //空白旗標設成false
                }
                else if(SpaceCounter==1){                           //如果SpaceCounter是Opcode
                    this->action+=tmp.at(i);                            //存入指令(大寫判斷用)
                    this->action_old+=old.at(i);                        //存入指令(原始樣貌)
                    SpaceFlag=false;                                    //空白旗標設成false
                }
                else if(SpaceCounter==2){                           //如果SpaceCounter是data
                    this->data+=old.at(i);                              //存入data
                    SpaceFlag=false;                                    //空白旗標設成false
                    if((this->data.at(0)=='C'||this->data.at(0)=='c')&&this->action=="BYTE"){
                        string tmp;
                        tmp=old.substr(i,i+19);
                        int place = tmp.rfind("\' ");
                        tmp = tmp.substr(0,place+1);
                        this->data=tmp;
                        i+=tmp.length()-1;
                        SpaceCounter = 3;
                    }
                }
                else{                                               //如果SpaceCounter是comment
                    this->comment+=old.at(i);                           //存入comment
                }
            }
        }
        if(this->data!=""&&this->version!="SIC"){
            if(this->data.at(0)=='@'){
                this->data=this->data.substr(1);
                this->ni=2;
            }
            else if(this->data.at(0)=='#'){
                this->data=this->data.substr(1);
                this->ni=1;
            }
            ni=0;
        }
        else ni=3;
        //cout<<"Line="<<this->lines<<" ni="<<ni<<endl;

        this->comment = this->removeHeaderSpace(this->comment);             //把註解去除開頭的空白
        if(this->action=="START")this->type=START;                          //設定type
        else if(this->action=="END")type = END;                             //設定type
        else if(this->action=="RESW"||this->action=="RESB")this->type=RES;  //設定type
        else if(this->action=="WORD"||this->action=="BYTE")this->type=CONST;//設定type
        else if(this->action=="ORG")this->type=ORG;                         //設定type
        else if(this->action=="")this->type=COMMENT;                        //設定type
        else if(this->action=="BASE")this->type=BASE;
        else this->type = OPERATOR;                                         //設定type
        return true;                                                 //回傳true代表成功讀取資料
    }
    else return false;                                               //回傳false代表讀取資料失敗
}

AsmCode* Reader::get_AsmCode(){
    return new AsmCode(this->type,this->label,this->action,this->data,this->comment,this->lines,this->action_old,this->ni);
}
