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
        void modify_content(int id, char content);
        void modify_position(int id, int x, int y);

        vector<Page> vectorPages;
        void pagesOnTop(int id);
        bool isInArray(int k, vector<int> vect);
        vector<int> checkOverlap(int pagenum);
        void organizeOnTop(int id);

    private:
        int num_jobs, width, height; 
        ofstream& output; 
        char* board; 
        static int pagesOnTopIndex;
        static vector<int> indexVec;
};

int Board::pagesOnTopIndex = 0;
vector<int> Board::indexVec = {};

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
    vector<int> onTop = {};
    vector<int> notTop = {};
    
    pagesOnTop(id);
    organizeOnTop(vectorPages[indexVec[0]].getPageid());



    // Find position of page of such id
    int pageNum = 0;
    while(vectorPages[pageNum].getPageid() != id) {
        pageNum++;
    }

    // for(int i = 0; i < onTop.size(); i++){
    //     if(pagesOnTop(onTop[i]).size() > 0) {
    //         moveElementToFront(onTop,pagesOnTop(onTop[i])[0],i);
    //     }
    // }

    // Delete pages above and rewrite one by one
    for(int ind = 0; ind < onTop.size(); ind++) {
        for (int h = vectorPages[onTop[ind]].gety(); h < vectorPages[onTop[ind]].gety() + vectorPages[onTop[ind]].getheight(); h++) {
            for (int w = vectorPages[onTop[ind]].getx(); w < vectorPages[onTop[ind]].getx() + vectorPages[onTop[ind]].getwidth(); w++) {
                board[h*this->width+w] = ' ';
            }
        }

        // Make a vector for number of pages that are not on top and not the specific page
        int val = 0;
        for (int i = 0; i < vectorPages.size(); i++) {
            while (std::find(onTop.begin(), onTop.begin()+ind+1, val) != onTop.begin()+ind+1) {
                val++; // Increment val until it is not in onTop
            }
            if(val >= vectorPages.size()) {
                i = vectorPages.size();
            } 
            // else if(val == pageNum) {
            //     val++;
            // } 
            else {
                notTop.push_back(val);
                val++;
            }
        }

        std::cout << "onTop: "<< endl;
        for(int i = 0; i < onTop.size(); i++) {
            std::cout << onTop[i] << endl;
        }

        
        for(int k = 0; k < vectorPages.size() ; k++) {
            if(isInArray(k,notTop)) {
                int x = vectorPages[k].getx();
                int y = vectorPages[k].gety();
                int width = vectorPages[k].getwidth();
                int height = vectorPages[k].getheight();
                int id = vectorPages[k].getPageid();
                char content = vectorPages[k].getcontent();
                insert_page_again(x,y,width,height,id,content);
            }
        }
        this->print_board();
        notTop.clear();
    }

    // Delete the certain page
    for (int h = vectorPages[pageNum].gety(); h < vectorPages[pageNum].gety() + vectorPages[pageNum].getheight(); h++) {
        for (int w = vectorPages[pageNum].getx(); w < vectorPages[pageNum].getx() + vectorPages[pageNum].getwidth(); w++) {
            board[h*this->width+w] = ' ';
        }
    }

    this->print_board();

    // Restore the pages above
    int i = 1;
    for(int k = 0; k < vectorPages.size() ; k++) {
        if (k!=pageNum && isInArray(k,onTop)) {
            int j = onTop[onTop.size()-i];

            int x = vectorPages[j].getx();
            int y = vectorPages[j].gety();
            int width = vectorPages[j].getwidth();
            int height = vectorPages[j].getheight();
            int id = vectorPages[j].getPageid();
            char content = vectorPages[j].getcontent();
            insert_page_again(x,y,width,height,id,content);
            this->print_board();

            i++;
        }
    }
}

void Board::modify_content(int id, char content) {
   
}
void Board::modify_position(int id, int x, int y) {
   
    
}

void Board::pagesOnTop(int id) {
    int i = 0;
    // Find which page has such id
    while(vectorPages[i].getPageid() != id) {
        i++;
    }

    int x = vectorPages[i].getx();
    int y = vectorPages[i].gety();
    int width = vectorPages[i].getwidth();
    int height = vectorPages[i].getheight();
    int pushbackindex = 0;

    // Check if overlap
    for(int k = 0; k < vectorPages.size(); k++) {
        if(k!=i) {
            if(vectorPages[k].getx()+vectorPages[k].getwidth() < x ||
            vectorPages[k].getx() > x + width ||
            vectorPages[k].gety() + vectorPages[k].getheight() < y ||
            vectorPages[k].gety() > y + height) {
                
            } else {
                indexVec.push_back(k);
                pushbackindex++;
            }
        }
    }

    // Gives page numbers of vectorPages

    // Organize with smallest id
    for(int ind = pagesOnTopIndex; ind < pagesOnTopIndex + pushbackindex; ind++) {
        if(vectorPages[indexVec[ind]].getPageid() > vectorPages[indexVec[ind+1]].getPageid() && ind+1<indexVec.size()) {
            int temp = indexVec[ind];
            indexVec[ind] = indexVec[ind+1];
            indexVec[ind+1] = temp;
        }
    }

    pagesOnTopIndex = pushbackindex;
}

void Board::organizeOnTop(int id) {
    int i = 0;
    while(vectorPages[i].getPageid() != id) {
        i++;
    }
    if(!checkOverlap(i).empty()) {
        int k = pagesOnTopIndex;
        pagesOnTop(vectorPages[i].getPageid());
        for(int l = k; l < pagesOnTopIndex; l++) {
            organizeOnTop(vectorPages[l].getPageid());
        }
    }
}

vector<int> Board::checkOverlap(int pagenum) {
    vector<int> index;
    int x = vectorPages[pagenum].getx();
    int y = vectorPages[pagenum].gety();
    int width = vectorPages[pagenum].getwidth();
    int height = vectorPages[pagenum].getheight();
    for(int k = 0; k < vectorPages.size(); k++) {
        if(k!=pagenum) {
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

bool Board::isInArray(int k, vector<int> vect) {
    bool a = false;
    for(int i = 0; i < vect.size(); i++) {
        if(k == vect[i]) {
            a = true;
        }
    }
    return a;
}
