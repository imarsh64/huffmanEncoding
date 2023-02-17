/**
 * Author                : Isaac Marsh
 * Assignment Title      : Huffman Encoding
 * Assignment Description: Program either compresses a text file
 *                         or decompresses a file compressed using the program
 * Due Date              : 2/17/2023
 * Date Created          : 2/9/2023
 * Date Last Modified    : 2/17/2023
 */

#include <iostream>
#include <fstream>
#include <map>
#include <queue>

using namespace std;

const char EOD          = 13;//end of data in the compressed file
const int MAGIC_NUMBER  = string::npos;//unique int at top of compressed file

struct node{
    int count;
    char c;
    node *left, *right;

    /**
     * description: constructor for node object
     * return: none
     * precondition: none
     * postcondition: node is created with left and right pointing
     *                to the null or the nodes passed into the function.
     *                If there is a right and left child, count becomes
     *                the sum of the count of the left and right nodes
     */
    explicit node(node *lptr = nullptr, node *rptr = nullptr);

    /**
     * description: destroys the node and all its children
     * return: none
     * precondition: Node to be destroyed exists
     * postcondition: node and all of its children are destroyed and
     *                set to null
     */
    ~node();
};

class cmp_node{
public:
    /**
     * description: comparator for 2 nodes
     * return: true if a > b and false if a < b
     * precondition: nodes a and b exist
     * postcondition: nodes a and b are compared returning
     *                whether a > b
     */
    bool operator()(node *a, node *b);
};

class writeToBinary{
private:
    unsigned char buffer;
    unsigned short bufferLength;
    fstream out;

public:
    /**
     * description: constructor for writeToBinary object
     * return: none
     * precondition: none
     * postcondition: fileName file opens.
     *                sets buffer and bufferLength to 0.
     *                writes magic number and number of char int pairs
     *                followed by the char int pairs.
     */
    writeToBinary(const string& fileName, map<char, int> frequency);

    /**
     * description: destructor for writeToBinary object
     * return: none
     * precondition: writeToBinary object exists
     * postcondition: buffer is padded with zeros then written to the file.
     *                closes file.
     */
    ~writeToBinary();

    /**
     * description: writes a given path in binary to a file
     * return: none
     * precondition: path of 1s and 0s is passed.
     * postcondition: buffer is updated with path passed in.
     *                when buffer is full, it writes to the file.
     */
    void write(string);
};


/**
 * description: creates a path to a character in the huffman tree
 * return: none
 * precondition: huffman tree is constructed
 * postcondition: m is filled with characters and their path in
 *                the huffman tree
 */
void createPath(node* root, map<char, string> *m, string path ="");

/**
 * description: creates a path to a character in the huffman tree
 * return: none
 * precondition: huffman tree is constructed
 * postcondition: m is filled with paths in the huffman tree and
 *                the character at the end of the path
 */
void createPath(node* root, map<string, char> *m, string path);

/**
 * description: creates huffman tree from map of characters and
 *              their frequencies
 * return: root of the huffman tree
 * precondition: map passed through is filled with character and
 *               frequency pairs
 * postcondition: huffman tree is constructed
 */
node* createHuffmanTree(map<char, int> frequency);



/**
 * description: driver for the program
 *              huffs or unhuffs an input file to an output file
 * return: 0
 * precondition: correct flags and arguments are passed
 * postcondition: huffs or unhuffs the input file into the output file
 */

/**
 * Data Abstraction:
 *          Creates file stream for input file
 *          Creates a character integer map to hold the frequencies of
 *              each character
 *          Creates character to hold the current character of the input file
 *          Creates long long to hold the size of the input file
 *          Creates long long to hold the size of the output file
 *          Creates node pointer to the root of the huffman tree
 *          Creates character string map to hold the path to each character
 *          Creates writeToBinary object to write to the output file
 *
 *          Creates integer to hold number of character integer pairs in input
 *              file
 *          Creates character to hold the character of the char/int pair in
 *              the input file
 *          Creates integer to hold the frequency of the char/int pair in
 *              the input file
 *          Creates file stream for the output file
 *          Creates a string character map for the path and their associated
 *              character
 *          Creates integer to hold the bit value from input file
 *
 * Input:
 *          Inputs command to either huff or unhuff
 *          Inputs input and output file
 * Process:
 *          If the first argument passed is "-huff", open the input file.
 *          Then read through the input file character by character creating
 *          a map with each character and it's frequencies. The program then
 *          calculates the size of the input file and of the header of the
 *          compressed file. If the compressed header is smaller than the input
 *          file, then create a huffman tree. Then create a map where every
 *          character is paired with it's path on the huffman tree. Then write
 *          the encoding to the output file.
 *          If the first argument is "-unhuff", open the input file and check
 *          if the magic number matches. If the magic numbers match, read in
 *          the number of character/frequency pairs. Then it reads in all the
 *          pairs. A huffman tree is then constructed. Each path is then
 *          mapped with their corresponding character. Then it reads in a byte.
 *          That byte is then parsed through by bit. If the path matches a
 *          character in the map it is written to the output file, unless
 *          the character was the EOD character. In that case, the program
 *          stops reading from the file and closes both the input and output
 *          file.
 * Output:
 *          If huff command is entered, then the output file is the
 *          huffman encoded binary file. If unhuff command is entered,
 *          then the output file is the decoded version of the huffed
 *          file.
 * Assumptions:
 *          Assumes the user enters a valid command and an existing
 *          input and output file
 */
int main(int argc, char *argv[]) {
    //Huffs input file to output file
    if((string)argv[1] == "-huff"){
        //Open input file
        ifstream input;
        input.open(argv[2]);
        input.unsetf(ios_base::skipws); //allow spaces to be read
        if(!input.is_open()){
            cout << "File failed to open!" << endl;
            return -1;
        }

        //Checks frequency of all characters
        map<char, int> frequency;

        char c;
        long long fileSize = 0;
        while(input >> c){
            frequency[c]++;
            fileSize++;
        }

        frequency[EOD]++;

        //Size of input file
        fileSize *= sizeof(char);

        //Size of header of compressed file
        //size of magic number + # of pairs(size of char + size of int)
        long long compressedHeader = sizeof(int) + sizeof(int)
                + frequency.size()*(sizeof(char) + sizeof(int));

        //Checks if compressed file will be a smaller file
        if(compressedHeader < fileSize) {
            //Create huffman tree
            node *root = createHuffmanTree(frequency);

            //Create paths
            map<char, string> paths;
            createPath(root, &paths);

            //Delete tree
            delete root;

            //Write into compressed file
            writeToBinary *w = new writeToBinary(argv[3], frequency);
            input.clear();
            input.seekg(0);
            while (input >> c) {
                w->write(paths[c]);
            }

            w->write(paths[EOD]);

            delete w;
        }
        else{
            cout << "File will not compress" << endl;
        }

        input.close();
    }
        //Unhuffs input file into output file
    else if((string)argv[1] == "-unhuff"){
        //Open input file
        fstream input;
        input.open((string)argv[2], ios::binary | ios::in);

        //Test if opened correctly
        if(!input.is_open()){
            cout << "ERROR" << endl;
            return -1;
        }

        //Read in magic number
        int magicNum;
        input.read((char*)&magicNum, sizeof(int));

        //Checks if file was compressed using same program
        if(magicNum == MAGIC_NUMBER) {
            int numChar;
            input.read((char *) &numChar, sizeof(int));

            //Create frequency map for each character
            map<char, int> frequency;
            for (int i = 0; i < numChar; i++) {
                char c;
                input.read(&c, sizeof(char));

                int f;
                input.read((char *) &f, sizeof(int));

                frequency[c] = f;
            }

            //Open output file
            fstream out;
            out.open(argv[3], ios::out | ios::trunc);

            //Create huffman tree
            node *root = createHuffmanTree(frequency);

            //Create paths for each character
            map<string, char> paths;
            createPath(root, &paths, "");

            delete root;

            char currChar;
            bool isEOD = false;
            string currPath = "";
            //Read in file byte by byte
            while (!isEOD && input.read(&currChar, sizeof(char))) {
                //Create a path from each bit
                for (int i = 7; i >= 0 && !isEOD; i--) {
                    int temp = ((currChar >> i) & 1);
                    if (temp == 1) {
                        currPath += "1";
                    } else {
                        currPath += "0";
                    }

                    //Checks is path is in map
                    if (paths.count(currPath)) {
                        //Checks if currPath leads to EOD character
                        if (paths[currPath] == EOD) {
                            isEOD = true;
                        }
                        else {
                            //Writes character from path
                            out << paths[currPath];
                        }
                        currPath = "";
                    }
                }
            }
            //Close files
            out.close();
            input.close();
        }
        else{
            cout << "Input file was not Huffman Endoded." << endl;
        }

    }
    else{
        cout << "INVALID ARGUMENT" << endl;
    }


    return 0;
}


node::node(node *lptr, node *rptr){
    count = 0;
    c     = '\0';
    left  = lptr;
    right = rptr;
    if(left && right){
        count = left->count + right->count;
    }
}

node::~node(){
    if(left){
        delete left;
        left = nullptr;
    }
    if(right){
        delete right;
        right = nullptr;
    }
}

bool cmp_node::operator()(node *a, node *b){
    return a->count > b->count;
}

writeToBinary::writeToBinary(const string& fileName, map<char, int> frequency){
    buffer = 0;
    bufferLength = 0;
    out.open(fileName, ios::binary | ios::out);
    if(!out.is_open()){
        cout << "Invalid output file" << endl;
    }

    int size = frequency.size();

    out.write((char*)&MAGIC_NUMBER, sizeof(int));
    out.write((char*)&size, sizeof(int));
    map<char, int>::iterator it;
    for(it = frequency.begin(); it != frequency.end(); it++){
        out.write((char*)&it->first, sizeof(char));
        out.write((char*)&it->second, sizeof(int));
    }
}

writeToBinary::~writeToBinary() {
    if(bufferLength != 0) {
        while (bufferLength < 7) {
            buffer = buffer << 1;
            bufferLength++;
        }
        out.write((char*)&buffer, sizeof(unsigned char));
    }

    out.close();
}

void writeToBinary::write(string s){
    for(unsigned int i = 0; i < s.length(); i++){
        if(s[i] == '1'){
            buffer += 1;
        }
        bufferLength++;
        if(bufferLength >= 8){
            out.write((char*)&buffer, sizeof(unsigned char));
            bufferLength = 0;
            buffer = 0;
        }
        else {
            buffer = buffer << 1;
        }
    }
}

void createPath(node* root, map<char, string> *m, string path){
    if(root && root->c){
        (*m)[root->c] = path;
    }
    string tmp;
    if(root->left){
        tmp = path + "0";
        createPath(root->left, m, tmp);
    }
    if(root->right){
        tmp = path + "1";
        createPath(root->right, m, tmp);
    }
}

void createPath(node* root, map<string, char> *m, string path){
    if(root && root->c){
        (*m)[path] = root->c;
    }
    string tmp;
    if(root->left){
        tmp = path + "0";
        createPath(root->left, m, tmp);
    }
    if(root->right){
        tmp = path + "1";
        createPath(root->right, m, tmp);
    }
}

node* createHuffmanTree(map<char, int> frequency){
    priority_queue<node*, vector<node*>, cmp_node> nodeQueue;

    //Create nodes
    node *p;
    map<char, int>::iterator it;
    for(it = frequency.begin(); it != frequency.end(); it++){
        p        = new node;
        p->c     = it->first;
        p->count = it->second;

        nodeQueue.push(p);
    }

    //Create tree
    node *n1, *n2;
    while(!nodeQueue.empty()){
        n1 = nodeQueue.top();
        nodeQueue.pop();
        n2 = nodeQueue.top();
        nodeQueue.pop();

        p = new node(n1, n2);
        if(!nodeQueue.empty()){
            nodeQueue.push(p);
        }
    }

    return p;
}