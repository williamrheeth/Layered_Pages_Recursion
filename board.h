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
        void delete_recursion(int id, vector<int>& onTop);
        void modify_content(int id, char content);
        void modify_position(int id, int x, int y);

        vector<Page> vectorPages;
        vector<int> pagesOnTop(int id);
        vector<int> checkIfFull(vector<int> onTop);
        void clear_board();

    private:
        int num_jobs, width, height; 
        ofstream& output; 
        char* board; 
        static vector<int> indexvec;
};

vector<int> Board::indexvec = {};

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
            board[h*this->width+w] = content;
        }
    }
}

void Board::delete_page(int id) {
    vector<int> onTop = pagesOnTop(id);

    // Initialize this->indexvec which contains all the page numbers of vectorPages (like 0,1,2,3,...)
    this->indexvec.clear();
    for(int i = 0; i < vectorPages.size(); i++) {
        this->indexvec.push_back(i);
    }
        
    // Find position of page of such id
    int pageNum = 0;
    while(vectorPages[pageNum].getPageid() != id) {
        pageNum++;
    }

    delete_recursion(pageNum, onTop);
}

void Board::delete_recursion(int pagenum, vector<int>& onTop) {
    while(this->indexvec.size() > vectorPages.size() - onTop.size()) {
        vector<int> check = checkIfFull(onTop);
        for(int i = 0; i < check.size(); i++) {
            if(check.size() == 0 || check.size() == 1) {
                // do nothing
            } else if(i < check.size()-1 && vectorPages[check[i]].getPageid() > vectorPages[check[i+1]].getPageid()) {
                int temp = check[i];
                check[i] = check[i+1];
                check[i+1] = temp;
                // organize in ascending id order
            }
        }
        for(int i = 0; i < check.size(); i++) {
            vector<int>::iterator pos = std::find(this->indexvec.begin(), this->indexvec.end(), check[i]);
            this->indexvec.erase(pos);
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
        }
        
        delete_recursion(pagenum, onTop);

        for(int i = check.size()-1; i >= 0; i--) {
            int x = vectorPages[check[i]].getx();
            int y = vectorPages[check[i]].gety();
            int width = vectorPages[check[i]].getwidth();
            int height = vectorPages[check[i]].getheight();
            int id = vectorPages[check[i]].getPageid();
            char content = vectorPages[check[i]].getcontent();
            insert_page_again(x, y, width, height, id, content);
            this->print_board();
        }
    }

    if(this->indexvec.size() == vectorPages.size() - onTop.size()) {
        vector<int>::iterator pos = std::find(this->indexvec.begin(), this->indexvec.end(), pagenum);
        this->indexvec.erase(pos);
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

        return;
    }
}

void Board::modify_content(int id, char content) {
   
}
void Board::modify_position(int id, int x, int y) {
   
    
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
        if(k!=i) {
            if(vectorPages[k].getx()+vectorPages[k].getwidth() < x ||
            vectorPages[k].getx() > x + width ||
            vectorPages[k].gety() + vectorPages[k].getheight() < y ||
            vectorPages[k].gety() > y + height) {
                
            } else {
                index.push_back(k);
            }
        }
    }

    return index;
}

vector<int> Board::checkIfFull(vector<int> onTop) {
    vector<int> checkIndex;
    for(int i = 0; i < onTop.size(); i++){
        if(std::count(this->indexvec.begin(), this->indexvec.end(), onTop[i])) { // See if ith onTop value (pagenumber) is in this->indexvec
            bool isItFull = true;
            for (int h = vectorPages[onTop[i]].gety() ; h < vectorPages[onTop[i]].gety()+vectorPages[onTop[i]].getheight(); h++) {
                for (int w = vectorPages[onTop[i]].getx(); w < vectorPages[onTop[i]].getx()+vectorPages[onTop[i]].getwidth(); w++) {
                    if(board[h*this->width+w] != vectorPages[onTop[i]].getcontent()) {
                        isItFull = false;
                    }
                }
            }
            if(isItFull) {
                checkIndex.push_back(onTop[i]);
            }
        }
    }

    return checkIndex;
}

void Board::clear_board() {
    for(int i = 0; i < width*height; i++) {
        this->board[i] = ' ';
    }
}