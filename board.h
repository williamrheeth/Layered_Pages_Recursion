#include <fstream>
using std::ofstream;

class Board{
    public:
        Board(int num_jobs, int width, int height, ofstream& output_stream);
        ~Board();

        void print_board();
        void print_job(int job_idx, char job_type, int id);

        // job functions
        void insert_page(int x, int y, int width, int height, int id, int content);
        void delete_page(int id);
        void modify_content(int id, char content); 
        void modify_position(int id, int x, int y);

    private:
        int num_jobs, width, height;
        ofstream& output;
        char* board;
};