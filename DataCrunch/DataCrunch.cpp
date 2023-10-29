#include <iostream>
#include<fstream>
#include<queue>
#include<vector>
#include<map>

using namespace std;

struct Node {
    char data;
    int frequency;
    Node* left, * right;
};

Node* newNode(char data, int frequency, Node* left, Node* right)
{
    Node* newNode = new Node;
    newNode->data = data;
    newNode->frequency = frequency;
    newNode->left = left;
    newNode->right = right;
    return newNode;
}

struct comparator {
    bool operator()(Node* left, Node* right) {
        return (left->frequency > right->frequency);
    }
};


//Build the Huffman tree 

Node* buildHuffmanTree(map<char, int> freqs)
{
    //compare func to make it minHeap, default max heap
    priority_queue<Node*, vector<Node*>, comparator>minHeap;

    //make leaf node, push in min heap
    for (auto pair : freqs) minHeap.push(newNode(pair.first, pair.second, nullptr, nullptr));

    //to generate tree 
    while (minHeap.size() != 1)
    {
        Node* left = minHeap.top();
        minHeap.pop();
        Node* right = minHeap.top();
        minHeap.pop();

        //create a new node merging left and right frequencies and push it back
        int mergeFreq = left->frequency + right->frequency;
        minHeap.push(newNode('\0', mergeFreq, left, right));
    }

    // last node remaining in the minheap is the root node
    return minHeap.top();

}

//recursive function to generate code using Huffman tree
void generateHuffmanCode(Node* root, string codes, map<char, string>& huffmanCodes)
{
    //root get null
    if (!root) return;

    //if root is a leaf node
    if (root->data != '\0') huffmanCodes[root->data] = codes;

    //recursive call moving left
    generateHuffmanCode(root->left, codes + "0", huffmanCodes);

    //recursive call moving right
    generateHuffmanCode(root->right, codes + "1", huffmanCodes);
}

// Encode the Data
string encodeInputFile(string inputFile, map<char, string>& huffmanCode)
{
    string encodedData;
    ifstream file(inputFile);
    char c;

    if (file.is_open())
    {
        while (file.get(c)) encodedData += huffmanCode[c];
        file.close();
    }
    return encodedData;
}

// put the compressed data in output file
void outputEncodedFile(string outputFile, string encodedData)
{
    ofstream file(outputFile);

    if (file.is_open())
    {
        //write the huffman code for decoding process
        //  If we don't have the tree then we have to pass the huffman codes in the output file for decoding process without the tree
        //for (auto pair : huffmanCodes) file << pair.first << ' ' << pair.second << endl;


        // transfer encoded data to output file
        file << encodedData;
        file.close();
    }
}

void decodeHuffman(ifstream& encodedFile, ofstream& outputFile, Node* root) {
    char bit;
    Node* currentNode = root;

    while (encodedFile.get(bit)) {
        if (bit == '0') {
            currentNode = currentNode->left;
        }
        else if (bit == '1') {
            currentNode = currentNode->right;
        }

        if (currentNode->left == nullptr && currentNode->right == nullptr) {
            // Found a leaf node, which corresponds to a decoded symbol.
            outputFile << currentNode->data;
            currentNode = root;  // Reset to the root for the next symbol.
        }
    }
}


int main()
{
    cout << "---------------Data Cruncher!---------------";
    cout << endl << endl;

    string inputFile = "input.txt";
    string outputFile = "output.txt";

    // map to store character of file and its frequency
    map<char, int> frequencies;

    ifstream file(inputFile);
    char c;

    if (file.is_open())
    {
        while (file.get(c)) { 
            cout << c << ": ";
            frequencies[c]++; 
            cout << frequencies[c];
            cout << endl;
        }
        file.close();
    }
    else cout << "file not found";

    //printing frequencies
    // const to make sure we are just iterating map key-values not modifying any of them
    cout << "frequencies: " << endl;
    for (const auto& pair : frequencies) {
        cout << " " << pair.first << " : " << pair.second << endl;
    }
    cout << endl;

    // build huffman tree
    Node* root = buildHuffmanTree(frequencies);

    // Generate huffman codes

    map<char, string> huffmanCodes;
    generateHuffmanCode(root, "", huffmanCodes);

    cout << "Huffman Code:" << endl;
    for (const auto& pair : huffmanCodes) {
        cout << " " << pair.first << " : " << pair.second << endl;
    }
    cout << endl;

    // Encode the input file and put the data in string
    string encoded = encodeInputFile(inputFile, huffmanCodes);

    // put the encoded string into output file
    outputEncodedFile(outputFile, encoded);


    ifstream outputEncodedFile("output.txt");
    ofstream decodedFile("decoded.txt");
    //decode file
    decodeHuffman(outputEncodedFile, decodedFile, root);

}

