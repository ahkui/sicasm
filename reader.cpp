#include"reader.hpp"

void Reader::clear_status(){
    type=-1;
    cmd.clear();
    label.clear();
    action.clear();
    action_old.clear();
    data.clear();
    comment.clear();
};

Reader::Reader(string FileName,AsmTable*Asm){
    this->file.open(FileName,std::ios::in);
    this->AsmTAB = Asm;
    this->lines=0;
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
            for(int j=0;j<4-(txtLong%4);j++)ans.append(" ");

            txtLong+=4-(txtLong%4);
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
    string tmp,old;             //�]�w�Ȧs
    int SpaceCounter=0;         //�]�w�ťդ��q�p�ƾ���0
    bool SpaceFlag=false;       //�]�w�ťպX�Ь�false
    this->clear_status();       //�M��buffer���
    this->lines++;              //���+1
    if(getline(this->file,old)){//����@�檺���
        old = this->replaceTab(old);//����tab���ť���
        tmp = this->toUpper(old);   //�ন�j�g

        for(unsigned int i=0;i<tmp.length();i++){               //�v�r���y
            if(i>=8&&i<15&&SpaceCounter<1)SpaceCounter=1;       //�p�G�r�ƨ�F �۰ʧ���q�p�ƾ��[�@
            else if(i>=18&&i<35&&SpaceCounter<2)SpaceCounter=2; //�p�G�r�ƨ�F �۰ʧ���q�p�ƾ��[�@
            else if(i>=35&&SpaceCounter<3)SpaceCounter=3;       //�p�G�r�ƨ�F �۰ʧ���q�p�ƾ��[�@
            if(tmp.at(i)==' '){                                 //�p�G�J��ť�
                if(SpaceFlag==false){                               //�Y�e�����S�J�L�ť�
                    SpaceFlag=true;                                     //�]�ťպX�Ь�true
                    if(SpaceCounter<3)SpaceCounter++;                   //�ťխp�ƾ�+1
                }
                else{                                               //�p�G�e�����J�L�ť�
                    if(SpaceCounter==3)this->comment+=old.at(i);    //���O�o�ťլO�b���ѳ��� �����s�Jbuffer
                }
            }
            else{                                               //�p�G�J�쪺���O�ť�
                if(tmp.at(i)=='.'){                                 //�O�I
                    SpaceCounter = 3;                                   //��������q�p�ƾ��]��comment
                    SpaceFlag=true;                                     //�åB���\�ť���
                }
                if(SpaceCounter==0){                                //�p�GSpaceCounter�Olabel
                    this->label+=old.at(i);                             //�s�Jlabel
                    SpaceFlag=false;                                    //�ťպX�г]��false
                }
                else if(SpaceCounter==1){                           //�p�GSpaceCounter�OOpcode
                    this->action+=tmp.at(i);                            //�s�J���O(�j�g�P�_��)
                    this->action_old+=old.at(i);                        //�s�J���O(��l�˻�)
                    SpaceFlag=false;                                    //�ťպX�г]��false
                }
                else if(SpaceCounter==2){                           //�p�GSpaceCounter�Odata
                    this->data+=old.at(i);                              //�s�Jdata
                    SpaceFlag=false;                                    //�ťպX�г]��false
                }
                else{                                               //�p�GSpaceCounter�Ocomment
                    this->comment+=old.at(i);                           //�s�Jcomment
                }
            }
        }
        this->comment = this->removeHeaderSpace(this->comment);         //����ѥh���}�Y���ť�
        if(this->action=="START")this->type=0;                          //�]�wtype
        else if(this->action=="END")type = 1;                           //�]�wtype
        else if(this->action=="RESW"||this->action=="RESB")this->type=2;//�]�wtype
        else if(this->action=="WORD"||this->action=="BYTE")this->type=3;//�]�wtype
        else if(this->action=="")this->type=5;                          //�]�wtype
        else this->type = 4;                                            //�]�wtype

        return true;                                                 //�^��true�N���\Ū�����
    }
    else return false;                                               //�^��false�N��Ū����ƥ���
}

AsmCode* Reader::get_AsmCode(){
    return new AsmCode(this->type,this->label,this->action,this->data,this->comment,this->lines,this->action_old);
}
