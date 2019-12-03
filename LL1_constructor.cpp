#include<iostream>
#include<cctype>
#include<cstring>
#include<vector>
#include<map>
#include<algorithm>

#define maxLength 10240

using namespace std;

// typedef struct Node {
//     string nonterminal;
//     vector<string> container;
// } Node;

// typedef map<string,vector<string>> Node;

struct Grammar {
    // vector<Node> production;
    map<string,vector<string>> production;
    vector<string> ntlist;
    string start;
    // vector<string> tlist;
}g;

char input[maxLength];
vector<string> raw,result;

void print_grammar();
void remove_space();
void transfer();
void remove_left_recursion();
void remove_left_gene();
void tree_shake(string nt,map<string,int> *times_map,int depth,int max_depth);
vector<string>::iterator replace_key(vector<string>::iterator t_it, vector<string>::iterator v_it,vector<string>::iterator new_t_it);


int main() {
    ios::sync_with_stdio(false);
    cout.tie(NULL);

    // freopen("e2data.txt","r",stdin);
    freopen("in.txt","r",stdin); 
    // 输入处理 删除空白
    cin.get(input,maxLength,'\0');
    remove_space();

    // 将输入字符串转化为结构体
    transfer();
    //消除左递归
    remove_left_recursion();
    // 消除左公因子
    remove_left_gene();

    print_grammar();


    return 0;

}

void print_grammar() {
    for (int i = 0; i < g.ntlist.size(); i++)
    {
        cout << g.ntlist[i] << "->";
        cout << g.production[g.ntlist[i]][0];
        for (int j = 1; j < g.production[g.ntlist[i]].size(); j++)
        {
            cout << "|" << g.production[g.ntlist[i]][j];
        }
        cout << ";" << endl;
    }
}

void remove_space() {
    char *s = input,*t = input;
    while ((*t =*s++) && (*s != '\0'))
    {
        if (!isspace(*t))
        {
            ++t;
        }   
    }
    while(t!=s) {
       *t='\0';
       t++;
    }
}

void transfer() {
    int input_len = strlen(input);
    for (int i = 0; i < input_len; i++)
    {
        string oneline = "";
        while (!(input[i] == ';'))
        {
            oneline += input[i++];
        }
        
        int index = oneline.find("->");
        
        
        // 该行的非终结符
        string nt=oneline.substr(0,index);
        // 该行的结果
        string container_string=oneline.substr(index+2,oneline.length()-index-2);

        if (!(g.production.find(nt) != g.production.end())) {
            vector<string> new_vector;
            g.production[nt] = new_vector;
            // g.production.insert(pair<string,vector<string>>(nt,new_vector));
            g.ntlist.push_back(nt);
        }
        vector<string> *container = &g.production[nt];
        int pos = 0;
        while(container_string.find("|",pos)!=string::npos) {
            int next_pos = container_string.find("|",pos);
            container->push_back(container_string.substr(pos,next_pos-pos));
            pos = next_pos + 1;
        }
        container->push_back(container_string.substr(pos));
    }
    g.start = g.ntlist.back() ;
    
}

vector<string>::iterator replace_key(vector<string>::iterator t_it, vector<string>::iterator v_it,vector<string>::iterator new_t_it) {

    string raw = *v_it;
    vector<string>::iterator new_v_it = v_it;
    for (vector<string>::iterator it = g.production[*new_t_it].begin(); it != g.production[*new_t_it].end(); it++)
    {
        string new_string = raw;
        while(true)   {     
            string::size_type   pos(0);     
            if(((pos=new_string.find(*new_t_it))!=string::npos ) && (new_string.find((*new_t_it)+"\'") == string::npos)) {
                new_string.replace(pos,new_t_it->length(),*it);
            }   
            else {
                break;
            }  
        }   
        new_v_it = g.production[*t_it].insert(new_v_it,1,new_string)+1;
    }
    new_v_it = g.production[*t_it].erase(new_v_it);
    return new_v_it;
}

void tree_shake(string nt,map<string,int> *times_map,int depth,int max_depth) {
    if (depth>max_depth)
    {
        return;
    }
    else
    {
        for (int i = 0; i < g.production[nt].size(); i++)
        {
            for (int j = 0; j < g.ntlist.size(); j++)
            {
                if (g.production[nt][i].find(g.ntlist[j]) != string::npos)
                {
                    (*times_map)[g.ntlist[j]]++;
                    tree_shake(g.ntlist[j],times_map,depth+1,max_depth);
                }
                
            }
            
        }
    }
    
    
}

void remove_left_recursion() {
    vector<string>::iterator t_it = g.ntlist.begin();
    // 消除间接左递归
    for (;t_it!=g.ntlist.end();t_it++)
    {
        vector<string>::iterator v_it = g.production[*t_it].begin();
        for (;v_it != g.production[*t_it].end();v_it++)
        {
            vector<string>::iterator new_t_it = g.ntlist.begin();
            for (; new_t_it != t_it; new_t_it++)
            {
                if ((v_it->find(*new_t_it) != string::npos) && (v_it->find(*new_t_it+"\'") == string::npos))
                {
                    v_it = replace_key(t_it,v_it,new_t_it);
                }    
            }
            
        }
        // 消除直接左递归
        if (g.production[*t_it][0].find(*t_it) == 0 && g.production[*t_it][0].find(*t_it + '\'') != 0)
        {
            string raw = g.production[*t_it][0];
            for (int i = 1; i < g.production[*t_it].size(); i++)
            {
                g.production[*t_it][i] += (*t_it + '\'' );
            }
            g.production[*t_it].erase(g.production[*t_it].begin());
            raw = raw.substr(raw.find(*t_it)+t_it->length(),raw.length()-(t_it->length()));
            g.production[*t_it+'\''].push_back(raw+*t_it+'\'');
            g.production[*t_it+'\''].push_back("ε");
            g.ntlist.push_back(*t_it+'\'');
        }

    }
    // 化简所得文法
    map<string,int> times_map;
    for (int i = 0; i < g.ntlist.size(); i++)
    {
        times_map[g.ntlist[i]] = 0;
    }
    // 只向下递归三层，比较麻烦就之后再做
    tree_shake(g.start,&times_map,0,3);
    for (map<string,int>::iterator it = times_map.begin(); it != times_map.end(); it++)
    {
        if (it->second == 0)
        {
            g.production.erase(it->first);
            g.ntlist.erase(find(g.ntlist.begin(),g.ntlist.end(),it->first));
        }
        
    }


}

void remove_left_gene() {
    int i,j,x;
    // 每个式子
    for (i = 0; i < g.ntlist.size(); i++)
    {
        vector<string> new_vector = g.production[g.ntlist[i]];
        sort(new_vector.begin(),new_vector.end());
        // 遍历每个候选式
        for (j = 0; j < new_vector.size(); j++)
        {
            string common_string = "";
            common_string += new_vector[j][0];
            int _x=0;
            while(new_vector[j][_x+1] == '\'') {
                common_string += new_vector[j][++_x];
            }

            // 遍历后面的候选式
            for (x = j+1; x < new_vector.size(); x++)
            {
                // 如果不含公共串
                if (!((new_vector[x].find(common_string) == 0) && (new_vector[x].find(common_string+"\'") != 0)))
                {
                    break;
                }
                
            }

            // 替换
            if (x!=j+1)
            {
                string new_nt_name = g.ntlist[i] + '\'';
                while(find(g.ntlist.begin(),g.ntlist.end(),new_nt_name) != g.ntlist.end()) {
                    new_nt_name += '\'';
                }
                g.ntlist.push_back(new_nt_name);
                // 修改原式子
                for (int y = 0; y < x-j; y++)
                {
                    string sub = g.production[g.ntlist[i]][j].substr(common_string.length(),g.production[g.ntlist[i]][j].length());
                    if (sub == "")
                    {
                        g.production[new_nt_name].push_back("ε");
                    }
                    else {
                        g.production[new_nt_name].push_back(sub);
                    }
                    g.production[g.ntlist[i]].erase(g.production[g.ntlist[i]].begin()+j);
                }
                g.production[g.ntlist[i]].insert(g.production[g.ntlist[i]].begin()+j,common_string+new_nt_name);
            }   
        }
    }
}
