/*
    @Desc
        LL1�ķ�ʵ��
    @Include:
        sytax_c_minus.h
*/

#include "syntax_c_minus.h"
#include <stack>

// ���캯��
LL_1::LL_1(LexList _lex_list) : Syntax_Analysis(_lex_list) {
    __isLL_1 = true;
    __first_set = __getFirstSet();
    __follow_set = __getFollowSet();
    __analysis_table = __getAnalysisTable();
}


void LL_1::analysis() {
    std::pair<int, ProductionRight> res = __match();
    SEE(res.first);
    SEE(__lex_list.size());
    if (res.first == __lex_list.size()) {
        std::cout << "ƥ��ɹ�\n";
        printSyntaxTree(res.second, __lex_list);
    }
    else {
        std::cout << "ƥ��ʧ��\n";
    }
}

void LL_1::printFirstSet() {
    printf("\nFirst Set:\n");
    for (auto item : __first_set) {
        printf("%-25s", item.first.c_str());
        for (auto i : item.second) {
            printf("%-10s", i.val.c_str());
        }
        printf("\n");
    }
}

void LL_1::printFollowSet() {
    printf("\nFollow Set:\n");
    for (auto item : __follow_set) {
        printf("%-25s", item.first.c_str());
        for (auto i : item.second) {
            printf("%-10s", i.val.c_str());
        }
        printf("\n");
    }
}

void LL_1::printAnalysisTable() {
    printf("\nAnalysis Table\n");
    for (auto item : __analysis_table) {
        printf("%-20s", item.first.c_str());
        for (auto i : item.second) {
            printf("<%s,", i.first.val.c_str());
            for (auto j : i.second) {
                printf("%s ", j.val.c_str());
            }
            printf("%s\t", ">");
        }
        printf("\n");
    }
}

FirstSet LL_1::__getFirstSet() {
    FirstSet first_set;
    // ��ʼ��ÿ�����ս��
    for (auto item : Grammar) {
        first_set.insert({ item.first, std::set<Token>()});
    }
    bool isupdate = true;
    while (isupdate) {
        isupdate = false;
        for (auto item : Grammar) {
            int src_size = first_set[item.first].size();
            for (auto produc : item.second) {
                for (int i = 0; i < produc.size(); i++) {
                    // �����һ���ս��
                    if (produc[i].type != 4) {
                        first_set[item.first].insert(produc[i]);
                        //std::cout << item.first << " : " << produc[i].val << "\n";
                        break;
                    }
                    // ����ǲ�����epsilon�ķ��ս��
                    if (first_set[produc[i].val].count({ 5, epsilon }) == 0) {
                        first_set[item.first].insert(first_set[produc[i].val].begin(), first_set[produc[i].val].end());
                        break;
                    }
                    // ����epsilon�ķ��ս��
                    else {
                        std::set<Token> mid = first_set[produc[i].val];
                        mid.erase(mid.find({ 5, epsilon }));
                        first_set[item.first].insert(mid.begin(), mid.end());
                        if (i == produc.size() - 1) {
                            first_set[item.first].insert({ 5, epsilon });
                        }
                    }
                }   
            }
            // ���ڸ���
            if (src_size != first_set[item.first].size()) {
                isupdate = true;
            }
        }
    }
    return first_set;
}

FollowSet LL_1::__getFollowSet() {
    FollowSet follow_set;
    // ��ʼ��ÿ�����ս��
    for (auto item : Grammar) {
        if (item.first != __begin) {
            follow_set.insert({ item.first, std::set<Token>()});
        }
        else {
            // ��ʼ���Ų���epsilon
            follow_set.insert({ item.first, {{6, "$"}} });
        }
    }
    bool isupdate = true;
    while (isupdate) {
        isupdate = false;
        // ����ÿһ�����ս��
        for (auto B : Grammar) {
            // ����ÿһ������ʽ
            int src_size = follow_set[B.first].size();
            for (auto item : Grammar) {
                for (auto produc : item.second) {
                    for (int i = 0; i < produc.size(); i++) {
                        if (produc[i].val == B.first) {
                            // ����2
                            for (int j = i + 1; j < produc.size(); j++) {
                                // ���ս�� �� ����First - epsilon
                                if (produc[j].type == 4) {
                                    std::set<Token> mid = __first_set[produc[j].val];
                                    if (mid.count({ 5,epsilon }) == 1) {
                                        mid.erase(mid.find({ 5, epsilon }));
                                        follow_set[B.first].insert(mid.begin(), mid.end());
                                    }
                                    else {
                                        follow_set[B.first].insert(mid.begin(), mid.end());
                                        break;
                                    }
                                }
                                // �ս��ֱ�Ӽ���
                                else if(produc[j].type != 5){
                                    follow_set[B.first].insert(produc[j]);
                                    break;
                                }
                            }
                            // ����3
                            if (i == produc.size() - 1) {
                                std::set<Token> A = follow_set[item.first];
                                follow_set[B.first].insert(A.begin(), A.end());
                            }
                            for (int j = i + 1; j < produc.size(); j++) {
                                if (produc[j].type == 4) {
                                    std::set<Token> mid = __first_set[produc[j].val];
                                    if (mid.count({ 5,epsilon }) == 0) {
                                        break;
                                    }
                                }
                                else if (produc[j].type != 5) {
                                    break;
                                }
                                if (j == produc.size() - 1) {
                                    std::set<Token> A = follow_set[item.first];
                                    follow_set[B.first].insert(A.begin(), A.end());
                                }
                            }
                        }
                    }
                }
            }
            // ���ڸ���
            if (src_size != follow_set[B.first].size()) {
                isupdate = true;
            }
        }
    }
    return follow_set;
}


// ���ķ�������elseû�������⴦�� ������Ҫ��Ϊѡ�� ȥ�������� �˴� other->epislon �� else����other->else statement ��ͻ ѡ�����
AnalysisTable LL_1::__getAnalysisTable() {
    AnalysisTable analysis_table;
    // ��ʼ��ÿ�����ս��
    for (auto item : Grammar) {
        analysis_table.insert({item.first, std::map<Token, TokenList>()});
    }
    // �������еĲ���ʽ
    for (auto item : Grammar) {
        for (auto produc : item.second) {
            // ��ò���ʽ��First����
            std::set<Token> first_set;
            for (int i = 0; i < produc.size(); i++) {
                Token token = produc[i];
                if (token.type != 4) {
                    first_set.insert(token);
                    break;
                }
                std::set<Token> mid = __first_set[token.val];
                if (mid.count({ 5,epsilon })) {
                    mid.erase(mid.find({ 5,epsilon }));
                    first_set.insert(mid.begin(), mid.end());
                    if (i == produc.size() - 1) {
                        first_set.insert({ 5,epsilon });
                    }
                }
                else {
                    first_set.insert(mid.begin(), mid.end());
                    break;
                }
            }
            for (auto first : first_set) {
                if (first.type == 5) {
                    continue;
                }
                if (analysis_table[item.first].count(first) == 1) {
                    __isLL_1 = false;
                    std::cout << "��ͻ " << item.first << " " << first.val << " " << ": ";
                    for (auto temp : produc) {
                        std::cout << temp.val << " ";
                    }
                    std::cout << "\n";
                }
                else {
                    analysis_table[item.first].insert({ first, produc });
                }
            }
            if (first_set.count({ 5,epsilon }) == 1) {
                for (auto follow : __follow_set[item.first]) {
                    if (analysis_table[item.first].count(follow) == 1) {
                        __isLL_1 = false;
                        std::cout << "��ͻ " << item.first << " " << follow.val << " "<< ": ";
                        for (auto temp : produc) {
                            std::cout << temp.val << " ";
                        }
                        std::cout << "\n";
                    }
                    else {
                        analysis_table[item.first].insert({ follow, produc });
                    }
                }
                if (__follow_set[item.first].count({ 6, "$" }) == 1) {
                    analysis_table[item.first].insert({ { 6,"$" }, produc });
                }
            }
        }
    }
    return analysis_table;
}

std::pair<int, ProductionRight> LL_1::__match() {
    // ĩβ����$
    TokenList lexlist;
    for (auto lex : __lex_list) {
        lexlist.push_back(Token::LexToToken(lex));
    }
    lexlist.push_back({ 6,"$" });

    std::stack<Token> stac;
    ProductionRight res;    // �������Ƶ��Ĳ���ʽ
    int ip = 0; //�����±�
    // ��ʼ��ջ stac = begin $
    stac.push({ 6, "$" });
    stac.push({ 4, __begin });
    Token top = stac.top();
    // ֻҪû�е�ĩβ��һֱƥ��
    while (top.type != 6) {
        // �ս��
        if (top.type != 4) {
            // ƥ�䵽��󱨴�
            if (ip == lexlist.size()) {
                std::cout << "���\n";
                break;
            }
            if (top == lexlist[ip]) {
                std::cout << "ƥ��: " << lexlist[ip].val << "\n";
                ip++;
                stac.pop();
            }
            else {
                std::cout << "��ƥ��" << lexlist[ip].val << "\n";
                break;  // �˳�
            }
        }
        // ���ս��
        else {
            // ������
            Token tar = lexlist[ip];
            if (__analysis_table[top.val].count(tar) == 0) {
                std::cout << top.val << " : " << tar.val << " ������" << "\n";
                break;
            }
            else {
                stac.pop();
                TokenList produc =  __analysis_table[top.val][tar];
                for (int i = produc.size() - 1; i >= 0; i--) {
                    if (produc[i].type != 5) {
                        stac.push(produc[i]);
                    }
                }
                std::cout << "ƥ�����ʽ: " << top.val << "->";
                for (auto temp : produc) {
                    std::cout << temp.val << " " ;
                }
                std::cout << "\n";
                produc.insert(produc.begin(), { 4, top.val });
                res.push_back(produc);
            }
        }
        top = stac.top();
    }
    return {ip, res};
}
