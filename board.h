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
        void delete_page(int id);
        void modify_content(int id, char content);
        void modify_position(int id, int x, int y);

        vector<Page> vectorPages;
        static void vectorAdd(Page page);

    private:
        int num_jobs, width, height; 
        ofstream& output; 
        char* board; 

        static int pageCount;
};

int Board::pageCount = 0;

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
    //Page page = Page(x,y,width,height,id,content); // Save the page to vector
    //vectorPages[pageCount] = page;    // Save to vector by static int pageCount

    for (int j = y; j < y+height; j++) {
        for (int i = x; i < x+width; i++) {
            board[j*this->width+i] = content;
        }
    }
    this->print_board();
    
}

void Board::delete_page(int id) {
    int i = pageCount;
    while(vectorPages[i+1].getPageid() != id) {
        for (int h = 0; h < vectorPages[i].getheight(); h++) {
            for (int w = 0; w < vectorPages[i].getwidth(); w++) {
                board[h*this->width+w] = ' ';
            }
        }
        i--;
    }
    for(i += 2; i <= pageCount ; i++) {
        int x = vectorPages[i].getx();
        int y = vectorPages[i].gety();
        int width = vectorPages[i].getwidth();
        int height = vectorPages[i].getheight();
        int id = vectorPages[i].getPageid();
        char content = vectorPages[i].getcontent();
        insert_page(x,y,width,height,id,content);
    }
}

void Board::modify_content(int id, char content) {
   

}
void Board::modify_position(int id, int x, int y) {
   
    
}
