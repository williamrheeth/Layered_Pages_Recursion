#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include "page.h"

using std::endl;
using std::ofstream;
using std::vector;

class Board {
    public:
        Board(int num_jobs, int width, int height, ofstream& output_stream);
        ~Board();

        void print_board();
        void print_job(int job_idx, char job_type, int id);

        //job functions
        void insert_page(int x, int y, int width, int height, int id, char content);
        void insert_page_again(int x, int y, int width, int height, int id, char content);
        void delete_page(int id);
        void delete_recursion(int id, vector<int>& index);
        void modify_content(int id, char content);
        void modify_content_recursion(int pagenum, vector<int>& index, char content);
        void modify_position(int id, int x, int y);
        void modify_position_recursion(int pagenum, vector<int>& index, int newx, int newy);

        vector<int> pagesOnTop(int id);
        vector<int> checkIfFull(vector<int> onTop);
        bool boolCheckIfFull(int pagenum, vector<int> onTop);
        void clear_board();

    private:
        int num_jobs, width, height; 
        ofstream& output; 
        char* board; 
        static vector<Page> vectorPages;
        static vector<int> indexvec;
        static vector<int> indexvec2;
        static vector<int> onTop;
};

vector<int> Board::indexvec = {};
vector<int> Board::indexvec2 = {};
vector<int> Board::onTop = {};
vector<Page> Board::vectorPages = {};

Board::Board(int num_jobs, int width, int height, ofstream& output_stream): output(output_stream) {
    this->width = width;
    this->height = height;
    this->num_jobs = num_jobs;

    board = new char[width*height];

    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            board[h*width + w] = ' ';
        }
    }

}

Board::~Board() {
    delete board;
}


void Board::print_board() {
    int h, w;
    for (w = 0; w < width+2; w++) output << "- ";
    output << endl;
    
    for (h = 0; h < height; h++) {
        output << "| ";
        for (w = 0; w < width; w++) {
            output << board[h*width + w] << " ";
        }
        output << "| " << endl;
    }

    for (w = 0; w < width+2; w++) output << "- ";
    output << endl;
}

void Board::print_job(int job_idx, char job_type, int id) {
    output << ">> (" << job_idx <<") ";
    switch(job_type) {
        
        case 'i':
            output << "Insert ";
            break;
        case 'd':
            output << "Delete ";
            break;
        case 'm':
            output << "Modify ";
            break;
    }

    output << id << endl;
}


void Board::insert_page(int x, int y, int width, int height, int id, char content) {
    Page page = Page(x,y,width,height,id,content); // Save the page to vector
    vectorPages.push_back(page);    // Add to vector

    for (int h = y; h < y+height; h++) {
        for (int w = x; w < x+width; w++) {
            board[h*this->width+w] = content;
        }
    }
    this->print_board();
}

void Board::insert_page_again(int x, int y, int width, int height, int id, char content) {
    for (int h = y; h < y+height; h++) {
        for (int w = x; w < x+width; w++) {
            this->board[h*this->width+w] = content;
        }
    }
}

void Board::delete_page(int id) {
    indexvec.clear();
    indexvec2.clear();
    onTop.clear();

    for(int i = 0; i < vectorPages.size(); i++) {
        indexvec.push_back(i);
    }

    this->onTop = pagesOnTop(id);
    vector<int> index = onTop;
     
    // Find position of page of such id
    int pageNum = 0;
    while(vectorPages[pageNum].getPageid() != id) {
        pageNum++;
    }

    this->delete_recursion(pageNum, index);

    auto iter = vectorPages.begin() + pageNum;
    vectorPages.erase(iter);

    indexvec2.clear();
}

void Board::delete_recursion(int pagenum, vector<int>& index) {
    if(this->indexvec.size() > this->vectorPages.size() - this->onTop.size()) {
        for(int i = 0; i < index.size(); i++) {
            if(index.size() == 0 || index.size() == 1) {
                // do nothing
            } else if(i < index.size()-1 && vectorPages[index[i]].getPageid() > vectorPages[index[i+1]].getPageid()) {
                iter_swap(index.begin()+i, index.begin()+i+1);
                // organize in ascending id order
            }
        }

        for(int i = 0; i < index.size(); ) {
            
            if(boolCheckIfFull(index[i],index)) {
                auto pos = std::find(this->indexvec.begin(), this->indexvec.end(), index[i]);
                this->indexvec.erase(pos);

                std::reverse(indexvec2.begin(), indexvec2.end());
                indexvec2.push_back(index[i]);
                std::reverse(indexvec2.begin(), indexvec2.end());

                index.erase(index.begin()+i);
                this->clear_board();
                for(int j = 0; j < this->indexvec.size(); j++) {
                    int x = vectorPages[this->indexvec[j]].getx();
                    int y = vectorPages[this->indexvec[j]].gety();
                    int width = vectorPages[this->indexvec[j]].getwidth();
                    int height = vectorPages[this->indexvec[j]].getheight();
                    int id = vectorPages[this->indexvec[j]].getPageid();
                    char content = vectorPages[this->indexvec[j]].getcontent();
                    this->insert_page_again(x, y, width, height, id, content);
                }
                this->print_board();
            } else {
                vector<int> onTop1 = pagesOnTop(vectorPages[index[i]].getPageid());

                if(onTop1.size() == 0 || onTop1.size() == 1) {
                    // do nothing
                    } else if(i < onTop1.size()-1 && vectorPages[onTop1[i]].getPageid() > vectorPages[onTop1[i+1]].getPageid()) {
                    int temp = onTop1[i];
                    onTop1[i] = onTop1[i+1];
                    onTop1[i+1] = temp;
                    // organize in ascending id order
                }
                int a = index[i];

                for(int j = 0; j < onTop1.size(); j++) {
                    vector<int> b = onTop1;
                    if(std::find(indexvec.begin(), indexvec.end(), onTop1[j]) != indexvec.end()) {

                        delete_recursion(onTop1[j], onTop1);

                    }

                    std::reverse(indexvec2.begin(), indexvec2.end());
                    indexvec2.push_back(index[i]);
                    std::reverse(indexvec2.begin(), indexvec2.end());

                    vector<int>::iterator pos = std::find(this->indexvec.begin(), this->indexvec.end(), a);
                    if(pos != this->indexvec.end())
                        this->indexvec.erase(pos);

                    vector<int>::iterator pos2 = std::find(index.begin(), index.end(), a);
                    if(pos2 != index.end())
                        index.erase(pos2);
                    for(int l = 0; l < b.size(); l++) {
                        vector<int>::iterator pos3 = std::find(index.begin(), index.end(), b[l]);
                        if(pos3 != index.end())
                            index.erase(pos3);
                    }
                    

                    this->clear_board();
                    for(int k = 0; k < this->indexvec.size(); k++) {
                        int x = vectorPages[this->indexvec[k]].getx();
                        int y = vectorPages[this->indexvec[k]].gety();
                        int width = vectorPages[this->indexvec[k]].getwidth();
                        int height = vectorPages[this->indexvec[k]].getheight();
                        int id = vectorPages[this->indexvec[k]].getPageid();
                        char content = vectorPages[this->indexvec[k]].getcontent();
                        this->insert_page_again(x, y, width, height, id, content);
                    }
                    this->print_board();
                                        
                    
                }
            }
            
            
        }
    }

    if(this->indexvec.size() == this->vectorPages.size() - this->onTop.size()) {
        vector<int>::iterator pos3 = std::find(this->indexvec.begin(), this->indexvec.end(), pagenum);
        this->indexvec.erase(pos3);
        this->clear_board();
        for(int i = 0; i < this->indexvec.size(); i++) {
            int x = vectorPages[this->indexvec[i]].getx();
            int y = vectorPages[this->indexvec[i]].gety();
            int width = vectorPages[this->indexvec[i]].getwidth();
            int height = vectorPages[this->indexvec[i]].getheight();
            int id = vectorPages[this->indexvec[i]].getPageid();
            char content = vectorPages[this->indexvec[i]].getcontent();
            insert_page_again(x, y, width, height, id, content);
        }
        this->print_board();

        for(int i = 0; i < this->indexvec2.size(); i++) {
            int x = vectorPages[this->indexvec2[i]].getx();
            int y = vectorPages[this->indexvec2[i]].gety();
            int width = vectorPages[this->indexvec2[i]].getwidth();
            int height = vectorPages[this->indexvec2[i]].getheight();
            int id = vectorPages[this->indexvec2[i]].getPageid();
            char content = vectorPages[this->indexvec2[i]].getcontent();
            insert_page_again(x, y, width, height, id, content);
            this->print_board();
        }
    }
}

void Board::modify_content(int id, char content) {
    indexvec.clear();
    indexvec2.clear();
    indexvec2 = {};
    for(int i = 0; i < vectorPages.size(); i++) {
        this->indexvec.push_back(i);
    }

    // Find position of page of such id
    int pageNum = 0;
    while(vectorPages[pageNum].getPageid() != id) {
        pageNum++;
    }

    onTop.clear();
    this->onTop = pagesOnTop(id);
    vector<int> index = onTop;

    if(boolCheckIfFull(pageNum,index)) {
        auto iter = indexvec.begin() + pageNum;
        indexvec.erase(iter);

        this->clear_board();
        for(int k = 0; k < this->indexvec.size(); k++) {
            int x = vectorPages[this->indexvec[k]].getx();
            int y = vectorPages[this->indexvec[k]].gety();
            int width = vectorPages[this->indexvec[k]].getwidth();
            int height = vectorPages[this->indexvec[k]].getheight();
            int id = vectorPages[this->indexvec[k]].getPageid();
            char content = vectorPages[this->indexvec[k]].getcontent();
            this->insert_page_again(x, y, width, height, id, content);
        }
        this->print_board();

        vectorPages[pageNum].setcontent(content);

        this->clear_board();
        for(int k = 0; k < this->vectorPages.size(); k++) {
            int x = vectorPages[k].getx();
            int y = vectorPages[k].gety();
            int width = vectorPages[k].getwidth();
            int height = vectorPages[k].getheight();
            int id = vectorPages[k].getPageid();
            char content = vectorPages[k].getcontent();
            this->insert_page_again(x, y, width, height, id, content);
        }
        this->print_board();
    } else {
        this->modify_content_recursion(pageNum, index, content);
    }
}

void Board::modify_content_recursion(int pagenum, vector<int>& index, char content) {
    if(this->indexvec.size() > this->vectorPages.size() - this->onTop.size()) {
        for(int i = 0; i < index.size(); i++) {
            if(index.size() == 0 || index.size() == 1) {
                // do nothing
            } else if(i < index.size()-1 && vectorPages[index[i]].getPageid() > vectorPages[index[i+1]].getPageid()) {
                iter_swap(index.begin()+i, index.begin()+i+1);
                // organize in ascending id order
            }
        }


        for(int i = 0; i < index.size(); ) {
            if(boolCheckIfFull(index[i],index)) {
                auto pos = std::find(this->indexvec.begin(), this->indexvec.end(), index[i]);
                this->indexvec.erase(pos);

                std::reverse(indexvec2.begin(), indexvec2.end());
                indexvec2.push_back(index[i]);
                std::reverse(indexvec2.begin(), indexvec2.end());

                auto pos2 = std::find(index.begin(), index.end(), index[i]);
                index.erase(pos2);
                this->clear_board();
                for(int j = 0; j < this->indexvec.size(); j++) {
                    int x = vectorPages[this->indexvec[j]].getx();
                    int y = vectorPages[this->indexvec[j]].gety();
                    int width = vectorPages[this->indexvec[j]].getwidth();
                    int height = vectorPages[this->indexvec[j]].getheight();
                    int id = vectorPages[this->indexvec[j]].getPageid();
                    char content = vectorPages[this->indexvec[j]].getcontent();
                    this->insert_page_again(x, y, width, height, id, content);
                }
                this->print_board();
            } else {
                vector<int> onTop1 = pagesOnTop(vectorPages[index[i]].getPageid());
                if(onTop1.size() == 0 || onTop1.size() == 1) {
                    // do nothing
                    } else if(i < onTop1.size()-1 && vectorPages[onTop1[i]].getPageid() > vectorPages[onTop1[i+1]].getPageid()) {
                    int temp = onTop1[i];
                    onTop1[i] = onTop1[i+1];
                    onTop1[i+1] = temp;
                    // organize in ascending id order
                }
                int a = index[i];

                for(int j = 0; j < onTop1.size(); j++) {
                    vector<int> b = onTop1;
                    if(std::find(indexvec.begin(), indexvec.end(), onTop1[j]) != indexvec.end()) {

                        modify_content_recursion(pagenum, onTop1, content);

                    }

                    std::reverse(indexvec2.begin(), indexvec2.end());
                    indexvec2.push_back(index[i]);
                    std::reverse(indexvec2.begin(), indexvec2.end());

                    vector<int>::iterator pos = std::find(this->indexvec.begin(), this->indexvec.end(), a);
                    if(pos != this->indexvec.end())
                        this->indexvec.erase(pos);

                    vector<int>::iterator pos2 = std::find(index.begin(), index.end(), a);
                    if(pos2 != index.end())
                        index.erase(pos2);
                    for(int l = 0; l < b.size(); l++) {
                        vector<int>::iterator pos3 = std::find(index.begin(), index.end(), b[l]);
                        if(pos3 != index.end())
                            index.erase(pos3);
                    }

                    this->clear_board();
                    for(int k = 0; k < this->indexvec.size(); k++) {
                        int x = vectorPages[this->indexvec[k]].getx();
                        int y = vectorPages[this->indexvec[k]].gety();
                        int width = vectorPages[this->indexvec[k]].getwidth();
                        int height = vectorPages[this->indexvec[k]].getheight();
                        int id = vectorPages[this->indexvec[k]].getPageid();
                        char content = vectorPages[this->indexvec[k]].getcontent();
                        this->insert_page_again(x, y, width, height, id, content);
                    }
                    this->print_board();
                    
                }
            }
        }
    }

    if(this->indexvec.size() == this->vectorPages.size() - this->onTop.size()) {
        vector<int>::iterator pos3 = std::find(this->indexvec.begin(), this->indexvec.end(), pagenum);
        this->indexvec.erase(pos3);
        this->clear_board();
        for(int i = 0; i < this->indexvec.size(); i++) {
            int x = vectorPages[this->indexvec[i]].getx();
            int y = vectorPages[this->indexvec[i]].gety();
            int width = vectorPages[this->indexvec[i]].getwidth();
            int height = vectorPages[this->indexvec[i]].getheight();
            int id = vectorPages[this->indexvec[i]].getPageid();
            char content = vectorPages[this->indexvec[i]].getcontent();
            insert_page_again(x, y, width, height, id, content);
        }
        this->print_board();

        this->vectorPages[pagenum].setcontent(content);
        insert_page_again(vectorPages[pagenum].getx(), vectorPages[pagenum].gety(), vectorPages[pagenum].getwidth(), vectorPages[pagenum].getheight(), vectorPages[pagenum].getPageid(), vectorPages[pagenum].getcontent());
        this->print_board();

        for(int i = 0; i < this->indexvec2.size(); i++) {
            int x = vectorPages[this->indexvec2[i]].getx();
            int y = vectorPages[this->indexvec2[i]].gety();
            int width = vectorPages[this->indexvec2[i]].getwidth();
            int height = vectorPages[this->indexvec2[i]].getheight();
            int id = vectorPages[this->indexvec2[i]].getPageid();
            char content = vectorPages[this->indexvec2[i]].getcontent();
            insert_page_again(x, y, width, height, id, content);
            this->print_board();
        }
    }
}


void Board::modify_position(int id, int x, int y) {
    indexvec.clear();
    indexvec2.clear();
    indexvec2 = {};
    for(int i = 0; i < vectorPages.size(); i++) {
        this->indexvec.push_back(i);
    }

    // Find position of page of such id
    int pageNum = 0;
    while(vectorPages[pageNum].getPageid() != id) {
        pageNum++;
    }

    onTop.clear();
    this->onTop = pagesOnTop(id);
    vector<int> index = pagesOnTop(id);

    if(boolCheckIfFull(pageNum,index)) {
        auto iter = indexvec.begin() + pageNum;
        indexvec.erase(iter);

        this->clear_board();
        for(int k = 0; k < this->indexvec.size(); k++) {
            int x = vectorPages[this->indexvec[k]].getx();
            int y = vectorPages[this->indexvec[k]].gety();
            int width = vectorPages[this->indexvec[k]].getwidth();
            int height = vectorPages[this->indexvec[k]].getheight();
            int id = vectorPages[this->indexvec[k]].getPageid();
            char content = vectorPages[this->indexvec[k]].getcontent();
            this->insert_page_again(x, y, width, height, id, content);
        }
        this->print_board();

        vectorPages[pageNum].setx(x);
        vectorPages[pageNum].sety(y);

        for(int i = indexvec.size()-1; i >= 0; i--) {
            int x1 = vectorPages[pageNum].getx();
            int y1 = vectorPages[pageNum].gety();
            int width1 = vectorPages[pageNum].getwidth();
            int height1 = vectorPages[pageNum].getheight();
            int x2 = vectorPages[indexvec[i]].getx();
            int y2 = vectorPages[indexvec[i]].gety();
            int width2 = vectorPages[indexvec[i]].getwidth();
            int height2 = vectorPages[indexvec[i]].getheight();

            if(x2+width2 <= x1 || x2 >= x1 + width1 || y2 + height2 <= y1 || y2 >= y1 + height1) {

            } else if (indexvec[i] != pageNum && indexvec[i] > pageNum) {
                Page temp = vectorPages[pageNum];
                auto ins = vectorPages.insert(vectorPages.begin()+indexvec[i]+1, temp);
                auto itr = vectorPages.begin()+pageNum;
                if (itr != vectorPages.end()) {
                    vectorPages.erase(itr);
                    pageNum = indexvec[i]+1;
                }
            }
        }

        this->clear_board();
        for(int k = 0; k < this->vectorPages.size(); k++) {
            int x = vectorPages[k].getx();
            int y = vectorPages[k].gety();
            int width = vectorPages[k].getwidth();
            int height = vectorPages[k].getheight();
            int id = vectorPages[k].getPageid();
            char content = vectorPages[k].getcontent();
            this->insert_page_again(x, y, width, height, id, content); 
        }
        this->print_board();
        } else {
        this->modify_position_recursion(pageNum, index, x, y);
    
        for(int i = indexvec.size()-1; i >= 0; i--) {
            int x1 = vectorPages[pageNum].getx();
            int y1 = vectorPages[pageNum].gety();
            int width1 = vectorPages[pageNum].getwidth();
            int height1 = vectorPages[pageNum].getheight();
            int x2 = vectorPages[indexvec[i]].getx();
            int y2 = vectorPages[indexvec[i]].gety();
            int width2 = vectorPages[indexvec[i]].getwidth();
            int height2 = vectorPages[indexvec[i]].getheight();

            if(x2+width2 <= x1 || x2 >= x1 + width1 || y2 + height2 <= y1 || y2 >= y1 + height1) {

            } else if (indexvec[i] != pageNum && indexvec[i] > pageNum) {
                Page temp = vectorPages[pageNum];
                auto ins = vectorPages.insert(vectorPages.begin()+indexvec[i]+1, temp);
                auto itr = vectorPages.begin()+pageNum;
                if (itr != vectorPages.end()) {
                    vectorPages.erase(itr);
                    pageNum = indexvec[i]+1;
                }
            }
        }
    }

    
}

void Board::modify_position_recursion(int pagenum, vector<int>& index, int newx, int newy) {
    if(this->indexvec.size() > this->vectorPages.size() - this->onTop.size()) {
        for(int i = 0; i < index.size(); i++) {
            if(index.size() == 0 || index.size() == 1) {
                // do nothing
            } else if(i < index.size()-1 && vectorPages[index[i]].getPageid() > vectorPages[index[i+1]].getPageid()) {
                iter_swap(index.begin()+i, index.begin()+i+1);
                // organize in ascending id order
            }
        }

        for(int i = 0; i < index.size(); ) {
            if(boolCheckIfFull(index[i],index)) {
                auto pos = std::find(this->indexvec.begin(), this->indexvec.end(), index[i]);
                this->indexvec.erase(pos);

                std::reverse(indexvec2.begin(), indexvec2.end());
                indexvec2.push_back(index[i]);
                std::reverse(indexvec2.begin(), indexvec2.end());

                auto pos2 = std::find(index.begin(), index.end(), index[i]);
                index.erase(pos2);
                this->clear_board();
                for(int j = 0; j < this->indexvec.size(); j++) {
                    int x = vectorPages[this->indexvec[j]].getx();
                    int y = vectorPages[this->indexvec[j]].gety();
                    int width = vectorPages[this->indexvec[j]].getwidth();
                    int height = vectorPages[this->indexvec[j]].getheight();
                    int id = vectorPages[this->indexvec[j]].getPageid();
                    char content = vectorPages[this->indexvec[j]].getcontent();
                    this->insert_page_again(x, y, width, height, id, content);
                }
                this->print_board();
            } else {
                vector<int> onTop1 = pagesOnTop(vectorPages[index[i]].getPageid());
                if(onTop1.size() == 0 || onTop1.size() == 1) {
                    // do nothing
                    } else if(i < onTop1.size()-1 && vectorPages[onTop1[i]].getPageid() > vectorPages[onTop1[i+1]].getPageid()) {
                    int temp = onTop1[i];
                    onTop1[i] = onTop1[i+1];
                    onTop1[i+1] = temp;
                    // organize in ascending id order
                }
                int a = index[i];
                for(int j = 0; j < onTop1.size(); j++) {

                    vector<int> b = onTop1;
                    if(std::find(indexvec.begin(), indexvec.end(), onTop1[j]) != indexvec.end()) {

                        modify_position_recursion(pagenum, onTop1, newx, newy);

                    }
                    
                    std::reverse(indexvec2.begin(), indexvec2.end());
                    indexvec2.push_back(index[i]);
                    std::reverse(indexvec2.begin(), indexvec2.end());

                    vector<int>::iterator pos = std::find(this->indexvec.begin(), this->indexvec.end(), a);
                    if(pos != this->indexvec.end())
                        this->indexvec.erase(pos);

                    vector<int>::iterator pos2 = std::find(index.begin(), index.end(), a);
                    if(pos2 != index.end())
                        index.erase(pos2);
                    for(int l = 0; l < b.size(); l++) {
                        vector<int>::iterator pos3 = std::find(index.begin(), index.end(), b[l]);
                        if(pos3 != index.end())
                            index.erase(pos3);
                    }

                    this->clear_board();
                    for(int k = 0; k < this->indexvec.size(); k++) {
                        int x = vectorPages[this->indexvec[k]].getx();
                        int y = vectorPages[this->indexvec[k]].gety();
                        int width = vectorPages[this->indexvec[k]].getwidth();
                        int height = vectorPages[this->indexvec[k]].getheight();
                        int id = vectorPages[this->indexvec[k]].getPageid();
                        char content = vectorPages[this->indexvec[k]].getcontent();
                        this->insert_page_again(x, y, width, height, id, content);
                    }
                    this->print_board();
                    
                }
            }
        }
    }

    if(this->indexvec.size() == this->vectorPages.size() - this->onTop.size()) {
        vector<int>::iterator pos3 = std::find(this->indexvec.begin(), this->indexvec.end(), pagenum);
        this->indexvec.erase(pos3);
        this->clear_board();
        for(int i = 0; i < this->indexvec.size(); i++) {
            int x = vectorPages[this->indexvec[i]].getx();
            int y = vectorPages[this->indexvec[i]].gety();
            int width = vectorPages[this->indexvec[i]].getwidth();
            int height = vectorPages[this->indexvec[i]].getheight();
            int id = vectorPages[this->indexvec[i]].getPageid();
            char content = vectorPages[this->indexvec[i]].getcontent();
            insert_page_again(x, y, width, height, id, content);
        }
        this->print_board();

        this->vectorPages[pagenum].setx(newx);
        this->vectorPages[pagenum].sety(newy);
        insert_page_again(vectorPages[pagenum].getx(), vectorPages[pagenum].gety(), vectorPages[pagenum].getwidth(), vectorPages[pagenum].getheight(), vectorPages[pagenum].getPageid(), vectorPages[pagenum].getcontent());
        this->print_board();

        for(int i = 0; i < this->indexvec2.size(); i++) {
            int x = vectorPages[this->indexvec2[i]].getx();
            int y = vectorPages[this->indexvec2[i]].gety();
            int width = vectorPages[this->indexvec2[i]].getwidth();
            int height = vectorPages[this->indexvec2[i]].getheight();
            int id = vectorPages[this->indexvec2[i]].getPageid();
            char content = vectorPages[this->indexvec2[i]].getcontent();
            insert_page_again(x, y, width, height, id, content);
            this->print_board();
        }
    }
}





vector<int> Board::pagesOnTop(int id) {
    int i = 0;
    // Find which page has such id
    while(vectorPages[i].getPageid() != id) {
        i++;
    }

    int x = vectorPages[i].getx();
    int y = vectorPages[i].gety();
    int width = vectorPages[i].getwidth();
    int height = vectorPages[i].getheight();

    vector<int> index = {};
    // Check if overlap
    for(int k = 0; k < vectorPages.size(); k++) {
        if(std::find(indexvec.begin(), indexvec.end(), k) != indexvec.end() && std::find(indexvec.begin(), indexvec.end(), i) != indexvec.end())  {
            if(k!=i) {
                if(vectorPages[k].getx()+vectorPages[k].getwidth() <= x ||
                vectorPages[k].getx() >= x + width ||
                vectorPages[k].gety() + vectorPages[k].getheight() <= y ||
                vectorPages[k].gety() >= y + height) {
                
                } else {
                    auto pos1 = std::find(indexvec.begin(), indexvec.end(), i);
                    auto pos2 = std::find(indexvec.begin(), indexvec.end(), k);
                    if(pos1 < pos2) {
                        index.push_back(k);
                    }
                }
            }
        } 
    }

    for(int i = 0; i < index.size(); i++) {
        if(!pagesOnTop(vectorPages[index[i]].getPageid()).empty()) {
            vector<int> temp = pagesOnTop(vectorPages[index[i]].getPageid());
            for(int j = 0; j < temp.size(); j++) {
                if(std::find(index.begin(), index.end(), temp[j]) == index.end()) {
                    index.push_back(temp[j]);
                }
            }
        }
    }

    return index;
}

// vector<int> Board::checkIfFull(vector<int> onTop) {
//     vector<int> checkIndex;
//     for(int i = 0; i < onTop.size(); i++){
//         if(std::count(this->indexvec.begin(), this->indexvec.end(), onTop[i])) { // See if ith onTop value (pagenumber) is in this->indexvec
//             bool isItFull = true;
//             for (int h = vectorPages[onTop[i]].gety() ; h < vectorPages[onTop[i]].gety()+vectorPages[onTop[i]].getheight(); h++) {
//                 for (int w = vectorPages[onTop[i]].getx(); w < vectorPages[onTop[i]].getx()+vectorPages[onTop[i]].getwidth(); w++) {
//                     if(board[h*this->width+w] != vectorPages[onTop[i]].getcontent()) {
//                         isItFull = false;
//                     }
//                 }
//             }
//             if(isItFull) {
//                 checkIndex.push_back(onTop[i]);
//             }
//         }
//     }

//     return checkIndex;
// }

bool Board::boolCheckIfFull(int pagenum, vector<int> onTop1) {
    if(std::count(this->indexvec.begin(), this->indexvec.end(), pagenum)) { // See if ith onTop value (pagenumber) is in this->onTop
        for (int h = vectorPages[pagenum].gety() ; h < vectorPages[pagenum].gety()+vectorPages[pagenum].getheight(); h++) {
            for (int w = vectorPages[pagenum].getx(); w < vectorPages[pagenum].getx()+vectorPages[pagenum].getwidth(); w++) {
                if(board[h*this->width+w] != vectorPages[pagenum].getcontent()) {
                    return false;
                }
            }
        }
        for(int j = 0; j < vectorPages.size(); j++) {
            if(std::count(this->indexvec.begin(), this->indexvec.end(), j)) {
                if(j!=pagenum && vectorPages[pagenum].getcontent() == vectorPages[j].getcontent()) {
                    int x1 = vectorPages[pagenum].getx();
                    int y1 = vectorPages[pagenum].gety();
                    int width1 = vectorPages[pagenum].getwidth();
                    int height1 = vectorPages[pagenum].getheight();
                    int x2 = vectorPages[j].getx();
                    int y2 = vectorPages[j].gety();
                    int width2 = vectorPages[j].getwidth();
                    int height2 = vectorPages[j].getheight();

                    if(x2+width2 <= x1 || x2 >= x1 + width1 || y2 + height2 <= y1 || y2 >= y1 + height1) {

                    } else {
                        if(pagenum < j) {
                            return false;
                        }
                    }
                }
            }
        }
    }    
    return true;
}

void Board::clear_board() {
    for(int i = 0; i < width*height; i++) {
        this->board[i] = ' ';
    }
}

