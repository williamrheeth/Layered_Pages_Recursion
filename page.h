class Page {
    public:
        Page(int x, int y, int width, int height, int id, char content);
        int getPageid();
        int getx();
        int gety();
        int getwidth();
        int getheight();
        char getcontent();
    private:
        int x, y; // position of the page on the board
        int width, height; // width and height of the page 
        int id; // unique id for each page
        char content; 
};

Page::Page(int x, int y, int width, int height, int id, char content){
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->id = id;
    this->content = content;
}

int Page::getPageid() {
    return this->id;
}

int Page::getx() {
    return this->x;
}

int Page::gety() {
    return this->y;
}

int Page::getwidth() {
    return this->width;
}

int Page::getheight() {
    return this->height;
}

char Page::getcontent() {
    return this->content;
}