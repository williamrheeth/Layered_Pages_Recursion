#include <fstream>
#include <vector>

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

    private:
        int num_jobs, width, height; 
        ofstream& output; 
        char* board; 
};

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
    int i = vectorPages.size();

    // Delete pages above and rewrite one by one
    while(vectorPages[i-1].getPageid() != id) {
        for (int h = vectorPages[i-1].gety(); h < vectorPages[i-1].gety() + vectorPages[i-1].getheight(); h++) {
            for (int w = vectorPages[i-1].getx(); w < vectorPages[i-1].getx() + vectorPages[i-1].getwidth(); w++) {
                board[h*this->width+w] = ' ';
            }
        }

    // Rewrite the remaining
        for(int k = 0; k < i-1 ; k++) {
            int x = vectorPages[k].getx();
            int y = vectorPages[k].gety();
            int width = vectorPages[k].getwidth();
            int height = vectorPages[k].getheight();
            int id = vectorPages[k].getPageid();
            char content = vectorPages[k].getcontent();
            insert_page_again(x,y,width,height,id,content);
        }
        this->print_board();
        i--;
    }

    // Delete the certain page
    for (int h = vectorPages[i-1].gety(); h < vectorPages[i-1].gety() + vectorPages[i-1].getheight(); h++) {
        for (int w = vectorPages[i-1].getx(); w < vectorPages[i-1].getx() + vectorPages[i-1].getwidth(); w++) {
            board[h*this->width+w] = ' ';
        }
    }

    this->print_board();

    // Restore the pages above
    for(; i < vectorPages.size() ; i++) {
        int x = vectorPages[i].getx();
        int y = vectorPages[i].gety();
        int width = vectorPages[i].getwidth();
        int height = vectorPages[i].getheight();
        int id = vectorPages[i].getPageid();
        char content = vectorPages[i].getcontent();
        insert_page_again(x,y,width,height,id,content);
    }
    this->print_board();
}

void Board::modify_content(int id, char content) {
   

}
void Board::modify_position(int id, int x, int y) {
   
    
}
